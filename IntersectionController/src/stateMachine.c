/*
 * stateMachine.c
 *
 *  Created on: 24Sep.,2019
 *      Author: aprab
 */

#include "stateMachine.h"

void initiliseTimer() {

	chid = ChannelCreate(0); // Create a communications channel

	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);

	if(event.sigev_coid != -1) {

		event.sigev_notify = SIGEV_PULSE;

		struct sched_param th_param;
		pthread_getschedparam(pthread_self(), NULL, &th_param);	// Get my own thread parameters
		event.sigev_priority = th_param.sched_curpriority;		// Set sig with same priority as myself

		event.sigev_code = MY_PULSE_CODE;


		// Create State Timer
		if (timer_create(CLOCK_REALTIME, &event, &state_timer_id) == -1) {	// create the timer, binding it to the event
			printf( (char *) stderr, "%s:  couldn't create a timer, errno %d\n", gettid(), errno);
			perror(NULL);
			exit(EXIT_FAILURE);
		}

//		// Create Pedestrian Timer
//		if (timer_create(CLOCK_REALTIME, &event, &ped_timer_id) == -1) {	// create the timer, binding it to the event
//			printf( (char *) stderr, "%s:  couldn't create a timer, errno %d\n", gettid(), errno);
//			perror(NULL);
//			exit(EXIT_FAILURE);
//		}

		// Timer for the state transition of the traffic intersection
		// one-shot timer (default = 0s)
		itime_state_timer.it_value.tv_sec = 0;		// 0 second
		itime_state_timer.it_value.tv_nsec = 0;		// 0 nano seconds
		itime_state_timer.it_interval.tv_sec = 0;	// 0 second 		(reload)
		itime_state_timer.it_interval.tv_nsec = 0;	// 0 nano seconds 	(reload)


		// Timer for the pedestrian crossing at the traffic intersection
		// one-shot timer (default = 0s)
		itime_ped_timer.it_value.tv_sec = 0;		// 0 second
		itime_ped_timer.it_value.tv_nsec = 0;		// 0 nano seconds
		itime_ped_timer.it_interval.tv_sec = 0;		// 0 second 		(reload)
		itime_ped_timer.it_interval.tv_nsec = 0;	// 0 nano seconds 	(reload)

	}
	else {

		printf( (char *) stderr, "%c:  couldn't ConnectAttach to self!\n", gettid());
		perror(NULL);
		exit(EXIT_FAILURE);

	}
}

void changeTrafficState_peak(traffic_state_data_t * state_data, traffic_state_t * previous_state) {

	traffic_state_t next_state = INITIAL;

	pthread_mutex_lock(&state_data->mutex_state);

	while (!state_data->produce_data){
		pthread_cond_wait(&state_data->cond_state_changed, &state_data->mutex_state);
	}
	//printf("State: %d\n", state_data->current_state);

	switch(state_data->current_state) {

		case INITIAL:
			next_state = ALL_RED;
			break;

		case ALL_RED:
			if ((*previous_state == INITIAL) || (*previous_state == NS_R_Y)) {
				next_state = NS_S_G;
			}
			else if (*previous_state == NS_S_Y) {
				next_state = EW_R_G;
			}
			else if (*previous_state == EW_R_Y) {
				next_state = EW_S_G;
			}
			else if (*previous_state == EW_S_Y) {
				next_state = NS_R_G;
			}
			break;

		case FAULT:

			break;

		case NS_S_G:
			next_state = NS_S_Y;
			break;

		case NS_S_Y:
			next_state = ALL_RED;
			break;

		case EW_R_G:
			next_state = EW_R_Y;
			break;

		case EW_R_Y:
			next_state = ALL_RED;
			break;

		case EW_S_G:
			next_state = EW_S_Y;
			break;

		case EW_S_Y:
			next_state = ALL_RED;
			break;

		case NS_R_G:
			next_state = NS_R_Y;
			break;

		case NS_R_Y:
			next_state = ALL_RED;
			break;

		default:
			next_state = FAULT;
			break;
	}


	*previous_state = state_data->current_state;
	state_data->current_state = next_state;

	state_data->produce_data = false;
	pthread_cond_signal(&state_data->cond_state_changed);
	pthread_mutex_unlock(&state_data->mutex_state);
	printf("data produced\n");

}

void changeTrafficState_offPeak(traffic_state_data_t * state_data, traffic_state_t * previous_state) {

	switch(state_data->current_state) {

		case INITIAL:

			break;

		case ALL_RED:

			break;

		case FAULT:

			break;

		case NS_S_G:

			break;

		case NS_S_Y:

			break;

		case EW_R_G:

			break;

		case EW_R_Y:

			break;

		case EW_S_G:

			break;

		case EW_S_Y:

			break;

		case NS_R_G:

			break;

		case NS_R_Y:

			break;

		default:

			break;
	}
}

void *thread_ex(void *data) {

	int d = *((int*)data);
	printf("thread %d with data=%d started. ", pthread_self(), d);
	printf(" val=%d\n", val++);
	sleep(1);

	return EXIT_SUCCESS;

}



void updateStateTimer(traffic_state_t state, struct itimerspec * statetimer){
	long duration = 0;

	if(state == INITIAL){
		duration = TIME_TRAFFIC_INITIAL;
	}
	else if(state == ALL_RED){
		duration = TIME_TRAFFIC_ALL_RED;
	}
	else if(state == NS_S_G || state == EW_S_G ){
		duration = TIME_TRAFFIC_STRAIGHT;
	}
	else if(state == NS_R_G || state == EW_R_G ){
		duration = TIME_TRAFFIC_TURN;
	}
	else if(state ==  NS_S_Y || state == NS_R_Y ||  state ==  EW_S_Y || state == EW_R_Y ){
		duration = TIME_TRAFFIC_YELLOW;
	}
	else{
		duration = TIME_TRAFFIC_NONE;
	}

	statetimer->it_value.tv_sec = duration;
}

void * trafficStateMachine(void * data) {

	traffic_state_t previous_state = INITIAL;

	traffic_state_data_t * state_data = (traffic_state_data_t *) data;

	while(1) {

		if (mode == PEAK) {
			changeTrafficState_peak(state_data, &previous_state);
		}
		else if (mode == OFF_PEAK) {
			changeTrafficState_peak(state_data, &previous_state);
		}
		else {
			printf("Encountered an error\n");
		}


		updateStateTimer(state_data->current_state, &itime_state_timer);

		printState(state_data->current_state);
		printf("Timer:  %ld\n", itime_state_timer.it_value.tv_sec);

		timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!

		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);	// Wait for timer signal
	}

	return EXIT_SUCCESS;
}

void printState(traffic_state_t state){

	switch(state) {

		case INITIAL:
			printf("\tState: %d [INITIAL]\n", state);
			break;

		case ALL_RED:
			printf("\tState: %d [ALLRED]\n", state);
			break;

		case FAULT:
			printf("\tState: %d [FAULT]\n", state);
			break;

		case NS_S_G:
			printf("\tState: %d [NS_S_G]\n", state);
			break;

		case NS_S_Y:
			printf("\tState: %d [NS_S_Y]\n", state);
			break;

		case EW_R_G:
			printf("\tState: %d [EW_R_G]\n", state);
			break;

		case EW_R_Y:
			printf("\tState: %d [EW_R_Y]\n", state);
			break;

		case EW_S_G:
			printf("\tState: %d [EW_S_G]\n", state);
			break;

		case EW_S_Y:
			printf("\tState: %d [EW_S_Y]\n", state);
			break;

		case NS_R_G:
			printf("\tState: %d [NS_R_G]\n", state);
			break;

		case NS_R_Y:
			printf("\tState: %d [NS_R_Y]\n", state);
			break;

		default:

			break;
	}

}
