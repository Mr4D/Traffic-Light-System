/*
 * stateMachine.c
 *
 *  Created on: 24Sep.,2019
 *      Author: aprab
 */

#include "stateMachine.h"

void changeTrafficState_peak() {

	printf("State: %d\n", currentState);

	switch(currentState) {

		case INITIAL:

			nextState = ALL_RED;
			previousState = INITIAL;

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

			break;

		case FAULT:

			break;

		case NS_S_G:

			previousState = NS_S_G;
			nextState = NS_S_Y;

			break;

		case NS_S_Y:

			previousState = NS_S_Y;
			nextState = ALL_RED;

			break;

		case EW_R_G:

			previousState = EW_R_G;
			nextState = EW_R_Y;

			break;

		case EW_R_Y:

			previousState = EW_R_Y;
			nextState = ALL_RED;

			break;

		case EW_S_G:

			previousState = EW_S_G;
			nextState = EW_S_Y;

			break;

		case EW_S_Y:

			previousState = EW_S_Y;
			nextState = ALL_RED;

			break;

		case NS_R_G:

			previousState = NS_R_G;
			nextState = NS_R_Y;

			break;

		case NS_R_Y:

			previousState = NS_R_Y;
			nextState = ALL_RED;

			break;

		default:

			break;
	}

	currentState = nextState;
	sleep(2);
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
		pthread_getschedparam(pthread_self(), NULL, &th_param);
		event.sigev_priority = th_param.sched_curpriority; // old QNX660 version getprio(0);

		event.sigev_code = MY_PULSE_CODE;

		// create the timer, binding it to the event
		if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1) {
			printf( (char *) stderr, "%s:  couldn't create a timer, errno %d\n", gettid(), errno);
			perror(NULL);
			exit(EXIT_FAILURE);
		}

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime_traffic_straight.it_value.tv_sec = 1;				// 1 second
		itime_traffic_straight.it_value.tv_nsec = 500000000;	// 500 million nsecs = .5 secs
		itime_traffic_straight.it_interval.tv_sec = 1;			// 1 second
		itime_traffic_straight.it_interval.tv_nsec = 500000000;	// 500 million nsecs = .5 secs

		// and start the timer!
		timer_settime(timer_id, 0, &itime_traffic_straight, NULL);

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
			rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
			changeTrafficState_peak();
		}
		else if (mode == OFF_PEAK) {
			changeTrafficState_peak();
		}
		else {
			printf("Encountered an error\n");
		}
	}

	return EXIT_SUCCESS;
}

