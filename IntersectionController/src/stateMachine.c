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


void *trafficStateMachine(void * data) {
	printf("State machine spawned\n");

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

	}

	return EXIT_SUCCESS;
}

