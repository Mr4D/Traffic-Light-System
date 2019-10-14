#include <stdio.h>
#include <stdlib.h>

#include "TCC_MessagePassing.h"
#include "Output.h"
#include "TrafficCentralController.h"

int main(void) {


	traffic_state_data_t i1_state_data = {
		INITIAL, //change to INITIAL
		false,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};

	traffic_state_data_t i2_state_data = {
		INITIAL, //change to INITIAL
		false,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};

	rail_state_data_t x1_state_data = {
		NO_TRAIN,
		false,
		PTHREAD_MUTEX_INITIALIZER
	};

	tcc_state_data_t tcc_state_data = {
		&i1_state_data,
		&i2_state_data,
		&x1_state_data
	};

	output_thread_data_t output_thread_data = {
			&tcc_state_data
	};

	critical_client_channel_thread_data_t critical_client_channel_thread_data = {

	};

	pthread_t output;
	pthread_t critical_server_channel;
	pthread_t critical_client_channel;

	pthread_create(&critical_server_channel, NULL, &critical_server_channel_thread, &tcc_state_data);
	pthread_create(&critical_client_channel, NULL, &critical_client_channel_thread, &tcc_state_data);

	pthread_create(&output, NULL, &t_output, &tcc_state_data);


	pthread_join(critical_server_channel, NULL);
	pthread_join(critical_client_channel, NULL);

	pthread_join(output, NULL);

	printf("Main terminating");
	return EXIT_SUCCESS;
}
