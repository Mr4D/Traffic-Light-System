/*
 * stateMachine.c
 *
 *  Created on: 24Sep.,2019
 *      Author: aprab
 */

#include "stateMachine.h"

void changeTrafficState_peak() {

	struct itimerspec * itime_to_use;

	printf("State: %d\n", currentState);

	switch(currentState) {

		case INITIAL:

			nextState = ALL_RED;
			previousState = INITIAL;

			itime_to_use = &itime_traffic_all_red;

			break;

		case ALL_RED:

			if ((previousState == INITIAL) || (previousState == NS_R_Y)) {
				nextState = NS_S_G;
			}
			else if (previousState == NS_S_Y) {
				nextState = EW_R_G;
			}
			else if (previousState == EW_R_Y) {
				nextState = EW_S_G;
			}
			else if (previousState == EW_S_Y) {
				nextState = NS_R_G;
			}

			previousState = ALL_RED;

			itime_to_use = &itime_traffic_all_red;

			break;

		case FAULT:

			itime_to_use = &itime_traffic_all_red;

			break;

		case NS_S_G:

			previousState = NS_S_G;
			nextState = NS_S_Y;

			itime_to_use = &itime_traffic_straight;

			break;

		case NS_S_Y:

			previousState = NS_S_Y;
			nextState = ALL_RED;

			itime_to_use = &itime_traffic_yellow;

			break;

		case EW_R_G:

			previousState = EW_R_G;
			nextState = EW_R_Y;

			itime_to_use = &itime_traffic_turn;

			break;

		case EW_R_Y:

			previousState = EW_R_Y;
			nextState = ALL_RED;

			itime_to_use = &itime_traffic_yellow;

			break;

		case EW_S_G:

			previousState = EW_S_G;
			nextState = EW_S_Y;

			itime_to_use = &itime_traffic_straight;

			break;

		case EW_S_Y:

			previousState = EW_S_Y;
			nextState = ALL_RED;

			itime_to_use = &itime_traffic_yellow;

			break;

		case NS_R_G:

			previousState = NS_R_G;
			nextState = NS_R_Y;

			itime_to_use = &itime_traffic_turn;

			break;

		case NS_R_Y:

			previousState = NS_R_Y;
			nextState = ALL_RED;

			itime_to_use = &itime_traffic_yellow;

			break;

		default:

			itime_to_use = &itime_traffic_none;

			break;
	}

	currentState = nextState;
	timer_settime(timer_id, 0, itime_to_use, NULL);	// and start the timer!
}

void changeTrafficState_offPeak() {

	switch(currentState) {

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

void initiliseTimer() {

	chid = ChannelCreate(0); // Create a communications channel

	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);

	if(event.sigev_coid != -1) {

		event.sigev_notify = SIGEV_PULSE;

		struct sched_param th_param;
		pthread_getschedparam(pthread_self(), NULL, &th_param);	// Get my own thread parameters
		event.sigev_priority = th_param.sched_curpriority;		// Set sig with same priority as myself

		event.sigev_code = MY_PULSE_CODE;


		if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1) {	// create the timer, binding it to the event
			printf( (char *) stderr, "%s:  couldn't create a timer, errno %d\n", gettid(), errno);
			perror(NULL);
			exit(EXIT_FAILURE);
		}

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime_traffic_straight.it_value.tv_sec = TIME_TRAFFIC_STRAIGHT;	// 1 second
		itime_traffic_straight.it_value.tv_nsec = 0;					// 500 million nsecs = .5 secs
		itime_traffic_straight.it_interval.tv_sec = 0;					// 1 second
		itime_traffic_straight.it_interval.tv_nsec = 0;					// 500 million nsecs = .5 secs

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime_traffic_turn.it_value.tv_sec = TIME_TRAFFIC_TURN;				// 1 second
		itime_traffic_turn.it_value.tv_nsec = 0;			// 500 million nsecs = .5 secs
		itime_traffic_turn.it_interval.tv_sec = 0;			// 1 second
		itime_traffic_turn.it_interval.tv_nsec = 0;			// 500 million nsecs = .5 secs

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime_traffic_yellow.it_value.tv_sec = TIME_TRAFFIC_YELLOW;				// 1 second
		itime_traffic_yellow.it_value.tv_nsec = 0;	// 500 million nsecs = .5 secs
		itime_traffic_yellow.it_interval.tv_sec = 0;			// 1 second
		itime_traffic_yellow.it_interval.tv_nsec = 0;			// 500 million nsecs = .5 secs

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime_traffic_all_red.it_value.tv_sec = TIME_TRAFFIC_ALL_RED;				// 1 second
		itime_traffic_all_red.it_value.tv_nsec = 0;	// 500 million nsecs = .5 secs
		itime_traffic_all_red.it_interval.tv_sec = 0;			// 1 second
		itime_traffic_all_red.it_interval.tv_nsec = 0;			// 500 million nsecs = .5 secs

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime_traffic_none.it_value.tv_sec = TIME_TRAFFIC_NONE;				// 1 second
		itime_traffic_none.it_value.tv_nsec = 0;	// 500 million nsecs = .5 secs
		itime_traffic_none.it_interval.tv_sec = 0;			// 1 second
		itime_traffic_none.it_interval.tv_nsec = 0;			// 500 million nsecs = .5 secs

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime_ped_green.it_value.tv_sec = TIME_PED_GREEN;				// 1 second
		itime_ped_green.it_value.tv_nsec = 0;	// 500 million nsecs = .5 secs
		itime_ped_green.it_interval.tv_sec = 0;			// 1 second
		itime_ped_green.it_interval.tv_nsec = 0;			// 500 million nsecs = .5 secs

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime_ped_yellow.it_value.tv_sec = TIME_PED_YELLOW;				// 1 second
		itime_ped_yellow.it_value.tv_nsec = 0;	// 500 million nsecs = .5 secs
		itime_ped_yellow.it_interval.tv_sec = 0;			// 1 second
		itime_ped_yellow.it_interval.tv_nsec = 0;			// 500 million nsecs = .5 secs

	}
	else {

		printf( (char *) stderr, "%c:  couldn't ConnectAttach to self!\n", gettid());
		perror(NULL);
		exit(EXIT_FAILURE);

	}
}

void *trafficStateMachine(void * data) {

	while(1) {

		if (mode == PEAK) {
			changeTrafficState_peak();
		}
		else if (mode == OFF_PEAK) {
			changeTrafficState_peak();
		}
		else {
			printf("Encountered an error\n");
		}

		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);	// Wait for timer signal
	}

	return EXIT_SUCCESS;
}
