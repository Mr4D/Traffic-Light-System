#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "stateMachine.h"
#include "Input.h"
#include "Output.h"
#include "IC_MessagePassing.h"
#include <traffic_file_lib.h>

int main(void) {

	openTrafficLogFile("name.log");

	int change_state_channel_id = ChannelCreate(0);
	traffic_mode_t mode = OFF_PEAK;

	traffic_input_buffer_t input_queue = {
			0,
			{0}
	};


	traffic_state_data_t traffic_state_data = {

		INITIAL, //change to INITIAL
		false,
		false,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};

	rail_state_data_t rail_state_data = {	// TODO: to update this data type
		NO_TRAIN,
		false,
		PTHREAD_MUTEX_INITIALIZER
	};

	traffic_input_data_t input_data = {
			&input_queue,
			PTHREAD_RWLOCK_INITIALIZER
	};

	traffic_input_thread_data_t input_thread_data = {
			&input_data,
			change_state_channel_id,
			&mode
	};

	ic_state_data_t ic_state_data = {
			&traffic_state_data,
			&rail_state_data,
	};

	traffic_state_machine_thread_data_t state_machine_thread_data = {
			&input_data,
			&ic_state_data,
			change_state_channel_id,
			&mode
	};


	pthread_t t_input;
	pthread_t t_traffic_state_machine;
	pthread_t t_output;

	// messaging threads
	pthread_t t_critical_client_channel;
	pthread_t t_critical_server_channel;

	pthread_create(&t_input, NULL, &inputThread, &input_thread_data);
	pthread_create(&t_traffic_state_machine, NULL, &trafficStateMachine, &state_machine_thread_data);
	pthread_create(&t_output, NULL, LCD_Control, &ic_state_data);

	//pthread_create(&t_critical_client_channel, NULL, &critical_client_channel_thread, &traffic_state_data);
	//pthread_create(&t_critical_server_channel, NULL, &critical_server_channel_thread, &rail_state_data);

	pthread_join(t_input, NULL);
	pthread_join(t_traffic_state_machine, NULL);
	pthread_join(t_output, NULL);
	pthread_join(t_critical_client_channel, NULL);
	pthread_join(t_critical_server_channel, NULL);



	printf("Main terminating");
	return EXIT_SUCCESS;
}
