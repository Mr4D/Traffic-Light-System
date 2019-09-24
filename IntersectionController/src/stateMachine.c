/*
 * stateMachine.c
 *
 *  Created on: 24Sep.,2019
 *      Author: aprab
 */

#include "stateMachine.h"

void changeTrafficState_peak() {

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
	return EXIT_SUCCESS;
}





