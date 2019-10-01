#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "stateMachine.h"
#include "Input.h"
#include "Output.h"
#include "IC_MessagePassing.h"

int main(void) {

	traffic_state_data_t state_data = {
		INITIAL,
		INITIAL,
		INITIAL,
		true,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};


	mode = PEAK;

	InputFlags flags = {OFF,OFF,OFF,OFF,OFF,OFF};

	initiliseTimer();

	pthread_t traffic_state_machine;
	//pthread_t t_input;
	pthread_t t_output;

	// messaging threads
	pthread_t critical_client_channel;

	//pthread_create(&t_input, NULL, &inputThread, &flags);
	pthread_create(&traffic_state_machine, NULL, &trafficStateMachine, &state_data);
	//pthread_create(&t_output,NULL,LCD_Control,&sd);
	pthread_create(&critical_client_channel, NULL, &critical_client_channel_thread, &state_data);

	//pthread_join(t_input, NULL);
	pthread_join(traffic_state_machine, NULL);
	//pthread_join(t_output, NULL);
	pthread_join(critical_client_channel, NULL);



	printf("Main terminating");
	return EXIT_SUCCESS;
}
