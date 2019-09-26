#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "stateMachine.h"
#include "Input.h"
#include "Output.h"

int main(void) {

	currentState = INITIAL;
	previousState = INITIAL;
	nextState = INITIAL;

	mode = PEAK;

	InputFlags flags = {OFF,OFF,OFF,OFF,OFF,OFF};

	initiliseTimer();

	pthread_t traffic_state_machine;
	pthread_t t_input;
	pthread_t t_output;

	pthread_create(&t_input, NULL, &inputThread, &flags);
	pthread_create(&traffic_state_machine, NULL, trafficStateMachine, NULL);
	pthread_create(&t_output,NULL,LCD_Control,NULL);

	pthread_join(t_input, NULL);
	pthread_join(traffic_state_machine, NULL);
	pthread_join(t_output, NULL);



	printf("Main terminating");
	return EXIT_SUCCESS;
}
