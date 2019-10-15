#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "stateMachine.h"
#include "Input.h"
#include "Output.h"
#include "IC_MessagePassing.h"
#include "IntersectionController.h"

int main(void) {

	FILE * fp;
	if((fp = openTrafficLogFile(LOG_FILE_NAME)) == NULL){
		printf("Failed to open log file: %s\n", LOG_FILE_NAME);
	}
	else {
		printf("Successful open log file: %s\n", LOG_FILE_NAME);
		fclose(fp);
	}

	int change_state_channel_id = ChannelCreate(0);

	traffic_state_data_t traffic_state_data = {

		INITIAL, //change to INITIAL
		false,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};

	rail_state_data_t rail_state_data = {	// TODO: to update this data type
		NO_TRAIN,
		false,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};

	traffic_mode_data_t traffic_mode_data = {
		PEAK,
		PTHREAD_MUTEX_INITIALIZER
	};

	fault_data_t fault_data = {
		false,
		PTHREAD_MUTEX_INITIALIZER
	};

	reset_data_t reset_data = {
		false,
		PTHREAD_MUTEX_INITIALIZER
	};



	traffic_input_queue_t input_queue = {
			0,
			{0}
	};

	traffic_input_data_t input_data = {
			&input_queue,
			PTHREAD_RWLOCK_INITIALIZER
	};

	state_machine_thread_data_t state_machine_thread_data = {
			&input_data,
			&traffic_state_data,
			&rail_state_data,
			&change_state_channel_id,
			&traffic_mode_data,
			&fault_data,
			&reset_data
	};

	input_thread_data_t input_thread_data = {
			&input_data,
			&change_state_channel_id,
			&traffic_mode_data
	};

	output_thread_data_t output_thread_data = {
			&traffic_state_data,
			&rail_state_data,
	};


	critical_server_data_t critical_server_data = {
			&traffic_state_data,
			&rail_state_data,
			&traffic_mode_data,
			&fault_data,
			&reset_data
	};


	// messaging threads
	pthread_t t_critical_server_channel;

	// create critical channel to listen for commands from tcc
	pthread_create(&t_critical_server_channel, NULL, &critical_server_channel_thread, &critical_server_data);


	pthread_t t_input;
	pthread_t t_traffic_state_machine;
	pthread_t t_output;

	//pthread_create(&t_input, NULL, &inputThread, &input_thread_data);
	pthread_create(&t_traffic_state_machine, NULL, &trafficStateMachine, &state_machine_thread_data);
	//pthread_create(&t_output, NULL, LCD_Control, &output_thread_data);

	//pthread_join(t_input, NULL);
	pthread_join(t_traffic_state_machine, NULL);
//	pthread_join(t_output, NULL);
	pthread_join(t_critical_server_channel, NULL);



	printf("Main terminating");
	return EXIT_SUCCESS;
}
