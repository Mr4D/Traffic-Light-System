/*
 * stateMachine.c
 *
 *  Created on: 24Sep.,2019
 *      Author: aprab
 */

#include "stateMachine.h"

void initiliseTimer(int chid) {


	state_timer_event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);

	if(state_timer_event.sigev_coid != -1) {

		state_timer_event.sigev_notify = SIGEV_PULSE;

		struct sched_param th_param;
		pthread_getschedparam(pthread_self(), NULL, &th_param);	// Get my own thread parameters
		state_timer_event.sigev_priority = th_param.sched_curpriority;		// Set sig with same priority as myself

		state_timer_event.sigev_code = STATE_TIMER_EXPIRE_PULSE;


		// Create State Timer
		if (timer_create(CLOCK_REALTIME, &state_timer_event, &state_timer_id) == -1) {	// create the timer, binding it to the event
			printf( (char *) stderr, "%s:  couldn't create a timer, errno %d\n", gettid(), errno);
			perror(NULL);
			exit(EXIT_FAILURE);
		}


		// Timer for the state transition of the traffic intersection
		// one-shot timer (default = 0s)
		itime_state_timer.it_value.tv_sec = 0;		// 0 second
		itime_state_timer.it_value.tv_nsec = 0;		// 0 nano seconds
		itime_state_timer.it_interval.tv_sec = 0;	// 0 second 		(reload)
		itime_state_timer.it_interval.tv_nsec = 0;	// 0 nano seconds 	(reload)




	}
	else {

		printf( (char *) stderr, "%c:  couldn't ConnectAttach to self!\n", gettid());
		perror(NULL);
		exit(EXIT_FAILURE);

	}



	ped_timer_event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);

	if(ped_timer_event.sigev_coid != -1){

		ped_timer_event.sigev_notify = SIGEV_PULSE;

		struct sched_param th_param;
		pthread_getschedparam(pthread_self(), NULL, &th_param);	// Get my own thread parameters
		ped_timer_event.sigev_priority = th_param.sched_curpriority;		// Set sig with same priority as myself

		ped_timer_event.sigev_code = PED_TIMER_EXPIRE_PULSE;

		// Create Pedestrian Timer
		if (timer_create(CLOCK_REALTIME, &ped_timer_event, &ped_timer_id) == -1) {	// create the timer, binding it to the event
			printf( (char *) stderr, "%s:  couldn't create a timer, errno %d\n", gettid(), errno);
			perror(NULL);
			exit(EXIT_FAILURE);
		}


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

traffic_state_t changeTrafficState_peak(traffic_state_t current_state, traffic_state_t previous_state, traffic_input_flags_t * input_flags) {

	printf("changeTrafficState_peak\n");
	traffic_state_t next_state = INITIAL;

	switch(current_state) {

		case INITIAL:
			next_state = ALL_RED;
			break;

		case ALL_RED:
			if ((previous_state == INITIAL) || (previous_state == NS_R_Y)) {
				next_state = NS_S_G_SEN;
			}
			else if (previous_state == NS_S_Y) {
				next_state = EW_R_G;
			}
			else if (previous_state == EW_R_Y) {
				next_state = EW_S_G_SEN;
			}
			else if (previous_state == EW_S_Y) {
				next_state = NS_R_G;
			}
			break;

		// North south straight transitions
		case NS_S_G_SEN:
			if(input_flags->ns_p == ON){
				next_state = NS_S_G_P_X;
				input_flags->ns_p = OFF;
			}
			else{
				next_state = NS_S_G;
			}
			break;

		case NS_S_G_P_X:
			next_state = NS_S_G_P_F;
			break;

		case NS_S_G_P_F:
			next_state = NS_S_G;
			break;

		case NS_S_G:
			next_state = NS_S_Y;
			break;

		case NS_S_Y:
			next_state = ALL_RED;
			break;

		// East west straight transitions
		case EW_S_G_SEN:
			if(input_flags->ew_p == ON) {
				next_state = EW_S_G_P_X;
				input_flags->ew_p = OFF;
			}
			else {
				next_state = EW_S_G;
			}
			break;

		case EW_S_G_P_X:
			next_state = EW_S_G_P_F;
			break;
		
		case EW_S_G_P_F:
			next_state = EW_S_G;
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

		// East west right transitions
		case EW_R_G:
			next_state = EW_R_Y;
			break;

		case EW_R_Y:
			next_state = ALL_RED;
			break;
		case FAULT:

			break;
		default:
			next_state = FAULT;
			break;
	}

	return next_state;
}

traffic_state_t changeTrafficState_offPeak(traffic_state_t current_state, traffic_state_t previous_state, traffic_input_flags_t * input_flags) {

	printf("changeTrafficState_offpeak\n");

	traffic_state_t next_state = INITIAL;

	switch(current_state) {

		case INITIAL:
			next_state = ALL_RED;
			break;

		case ALL_RED:
			// do this
			if ((previous_state == INITIAL) || (previous_state == NS_R_Y)) {
				next_state = NS_S_G_SEN;
			}
			else if (previous_state == NS_S_Y) {
				next_state = EW_R_G;
			}
			else if (previous_state == EW_R_Y) {
				next_state = EW_S_G_SEN;
			}
			else if (previous_state == EW_S_Y) {
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

	return next_state;
}




void updateTimers(traffic_state_t current_state){

	switch(current_state) {

			case INITIAL:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_INITIAL;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case ALL_RED:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_ALL_RED;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case NS_S_G_SEN:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_STRAIGHT;
				itime_ped_timer.it_value.tv_sec = TIME_PED_TIME_OUT;

				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case NS_S_G_P_X:
				itime_ped_timer.it_value.tv_sec = TIME_PED_CROSS;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case NS_S_G_P_F:
				itime_ped_timer.it_value.tv_sec = TIME_PED_FLASHING;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case NS_S_G:
				// DO NOT SET TIMER
				break;

			case NS_S_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case EW_S_G_SEN:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_STRAIGHT;
				itime_ped_timer.it_value.tv_sec = TIME_PED_TIME_OUT;

				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case EW_S_G_P_X:
				itime_ped_timer.it_value.tv_sec = TIME_PED_CROSS;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case EW_S_G_P_F:
				itime_ped_timer.it_value.tv_sec = TIME_PED_FLASHING;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case EW_S_G:
				// DO NOT SET TIMER
				break;

			case EW_S_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case NS_R_G:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_TURN;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case NS_R_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case EW_R_G:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_TURN;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case EW_R_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;
			case FAULT:
				// DO NOT SET TIMER
				break;
			default:
				// DO NOT SET TIMER
				break;
		}

}

void * trafficStateMachine(void * data) {

	printf("TrafficStateMachine()\n");

	struct _pulse change_state_msg;
	int change_state_channel_id;
	int rcvid;


	traffic_state_t next_state = INITIAL;
	traffic_state_t current_state = INITIAL;
	traffic_state_t previous_state = INITIAL;
	traffic_input_flags_t input_flags;

	traffic_state_machine_thread_data_t * state_machine_thread_data = (traffic_state_machine_thread_data_t*) data;

	traffic_input_data_t * input_data = state_machine_thread_data->input_data;
	ic_state_data_t * ic_state_data = state_machine_thread_data->ic_state_data;

	traffic_state_data_t * state_data = ic_state_data->traffic_state_data;
	//rail_state_data_t * rail_state_data = ic_state_data->rail_state_data;


	change_state_channel_id = state_machine_thread_data->change_state_channel_id; // Create a communications channel
	initiliseTimer(change_state_channel_id);



	while(1) {

//		// get current state
//		pthread_mutex_lock(&state_data->mutex_state);
//			current_state = state_data->current_state;
//		pthread_mutex_unlock(&state_data->mutex_state);

		// get input flags
		pthread_rwlock_rdlock( &input_data->rwlock_input );
			memcpy(&input_flags, &input_data->input_flags, sizeof(input_flags));
		pthread_rwlock_unlock( &input_data->rwlock_input );


		// get next state
		if (mode == PEAK) {
			next_state = changeTrafficState_peak(current_state, previous_state, &input_flags);
		}
		else if (mode == OFF_PEAK) {
			next_state = changeTrafficState_peak(current_state, previous_state, &input_flags);
		}
		else {
			printf("Encountered an error\n");
		}



		// update input flags
		pthread_rwlock_wrlock( &input_data->rwlock_input );
			memcpy(&input_data->input_flags, &input_flags, sizeof(input_data->input_flags));
		pthread_rwlock_unlock( &input_data->rwlock_input );


		// update previous state
		previous_state = current_state;
		current_state = next_state;

		// update current state
		pthread_mutex_lock(&state_data->mutex_state);
			state_data->current_state = current_state;

			state_data->data_ready_message = true;
			state_data->data_ready_output = true;
			pthread_cond_broadcast(&state_data->cond_state_changed);
		pthread_mutex_unlock(&state_data->mutex_state);



		updateTimers(current_state);


		// Print state info
		printState(state_data->current_state);
		printf("Timer:  %ld\n", itime_state_timer.it_value.tv_sec);




		// wait for timer
		bool change_state = false;
		while(!change_state){
			rcvid = MsgReceivePulse(change_state_channel_id, &change_state_msg, sizeof(change_state_msg), NULL);	// Wait for timer signal

			if(change_state_msg.code == STATE_TIMER_EXPIRE_PULSE){
				change_state = true;
				printf("STATE_TIMER_EXPIRE_PULSE\n");
			}
			else if(change_state_msg.code == PED_TIMER_EXPIRE_PULSE){
				change_state = true;
				printf("PED_TIMER_EXPIRE_PULSE\n");
			}
			else if(change_state_msg.code == NS_P_INPUT_PULSE){
				struct itimerspec ped_itimerspec;
				timer_gettime( ped_timer_id, &ped_itimerspec);
				printf("code: %d\n",change_state_msg.code);

				if(current_state == NS_S_G_SEN && ped_itimerspec.it_value.tv_sec > 1){
					printf("\t\tchange state true\n");
					change_state = true;
				}
				printf("INPUT_PULSE\n");
			}
			else if(change_state_msg.code == EW_P_INPUT_PULSE){
				struct itimerspec ped_itimerspec;
				timer_gettime( ped_timer_id, &ped_itimerspec);
				printf("code: %d\n",change_state_msg.code);

				if(current_state == EW_S_G_SEN && ped_itimerspec.it_value.tv_sec > 1){
					printf("\t\tchange state true\n");
					change_state = true;
				}
				printf("INPUT_PULSE\n");
			}
		}

		printf("\t\tCHANGE STATE\n");

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

		case NS_S_G_SEN:
			printf("\tState: %d [NS_S_G_SEN]\n", state);
			break;

		case NS_S_G_P_X:
			printf("\tState: %d [NS_S_G_P_X]\n", state);
			break;

		case NS_S_G_P_F:
			printf("\tState: %d [NS_S_G_P_F]\n", state);
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

		case EW_S_G_SEN:
			printf("\tState: %d [EW_S_G_SEN]\n", state);
			break;

		case EW_S_G_P_X:
			printf("\tState: %d [EW_S_G_P_X]\n", state);
			break;

		case EW_S_G_P_F:
			printf("\tState: %d [EW_S_G_P_F]\n", state);
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
