#include <pthread.h>
#include "stateMachine.h"

int main(void) {

	currentState = INITIAL;
	previousState = INITIAL;
	nextState = INITIAL;

	mode = PEAK;

	pthread_t traffic_state_machine;

	pthread_create(&traffic_state_machine, NULL, trafficStateMachine, NULL);

	return EXIT_SUCCESS;
}
