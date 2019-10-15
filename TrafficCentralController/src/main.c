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
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};

	tcc_state_data_t tcc_state_data = {
		&i1_state_data,
		&i2_state_data,
		&x1_state_data
	};

	output_thread_data_t output_thread_data = {
			&tcc_state_data
	};

	critical_server_thread_data_t critical_server_thread_data = {
			&x1_state_data
	};

	pthread_t output;
	pthread_t critical_server_channel;

	msg_data_t msg_send;
	msg_send.client_id = TCC_CLIENT_ID;
	msg_send.hdr.type = FAULT_MSG;
	msg_send.data.fault = true;

	send_message_thread_data_t i1_send = {
			I1_CRITICAL_CHANNEL_ATTACH_POINT,
		   &msg_send
	};

	pthread_t send;
	pthread_create(&send, NULL, &sendMessage, &i1_send);
	pthread_join(send, NULL);


	//pthread_create(&critical_server_channel, NULL, &critical_server_channel_thread, &critical_server_thread_data);

	//pthread_create(&output, NULL, &t_output, &output_thread_data);


	//pthread_join(critical_server_channel, NULL);
	//pthread_join(output, NULL);

	printf("Main terminating\n");
	return EXIT_SUCCESS;
}
