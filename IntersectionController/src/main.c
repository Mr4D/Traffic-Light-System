#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "stateMachine.h"
#include "Input.h"

int main(void) {

	currentState = INITIAL;
	previousState = INITIAL;
	nextState = INITIAL;

	mode = PEAK;

	InputFlags flags = {OFF,OFF,OFF,OFF,OFF,OFF};

	initiliseTimer();

	pthread_t traffic_state_machine;
	pthread_t t_input;

	pthread_create(&t_input, NULL, &inputThread, &flags);
	pthread_create(&traffic_state_machine, NULL, trafficStateMachine, NULL);

	pthread_join(t_input, NULL);
	pthread_join(traffic_state_machine, NULL);

	printf("Main terminating");
	return EXIT_SUCCESS;
}
