#include <stdio.h>
#include <stdlib.h>

#include "Input.h"
#include "Output.h"
#include "TCC_MessagePassing.h"
#include "TrafficCentralController.h"

void initData(traffic_state_data_t *i1_state_data,
			  traffic_state_data_t *i2_state_data,
			  rail_state_data_t *x1_state_data,
			  tcc_state_data_t *tcc_state_data,
			  input_thread_data_t * input_thread_data,
			  output_thread_data_t *output_thread_data,
			  critical_server_thread_data_t *critical_server_thread_data,
			  traffic_mode_data_t * traffic_mode_data,
			  msg_data_t *msg_send,
			  send_message_thread_data_t *i1_send) {

	initTrafficStateData(i1_state_data);
	initTrafficStateData(i1_state_data);
	initRailStateData(x1_state_data);
	initModeData(traffic_mode_data);

	tcc_state_data->i1_state_data = i1_state_data;
	tcc_state_data->i2_state_data = i2_state_data;
	tcc_state_data->x1_state_data = x1_state_data;
	tcc_state_data->traffic_mode_data = traffic_mode_data;

	input_thread_data->tcc_state_data = tcc_state_data;

	output_thread_data->tcc_state_data = tcc_state_data;

	critical_server_thread_data->x1_state_data = x1_state_data;

	msg_send->client_id = TCC_CLIENT_ID;
	msg_send->hdr.type = RESET_MSG;
	msg_send->data.reset = true;

	i1_send->attach_point = I1_CRITICAL_CHANNEL_ATTACH_POINT;
	i1_send->msg = msg_send;
}

int main(void) {

	traffic_state_data_t i1_state_data;
	traffic_state_data_t i2_state_data;
	rail_state_data_t x1_state_data;
	tcc_state_data_t tcc_state_data;
	input_thread_data_t input_thread_data;
	output_thread_data_t output_thread_data;
	critical_server_thread_data_t critical_server_thread_data;
	traffic_mode_data_t traffic_mode_data;

	msg_data_t msg_send;
	send_message_thread_data_t i1_send;

	initData(&i1_state_data,
			 &i2_state_data,
			 &x1_state_data,
			 &tcc_state_data,
			 &input_thread_data,
			 &output_thread_data,
			 &critical_server_thread_data,
			 &traffic_mode_data,
			 &msg_send,
			 &i1_send);

	pthread_t input;
	pthread_t output;
	pthread_t critical_server_channel;

	pthread_t send;
//	pthread_create(&send, NULL, &sendMessage, &i1_send);
//	pthread_join(send, NULL);

	pthread_create(&critical_server_channel, NULL, &critical_server_channel_thread, &critical_server_thread_data);
	pthread_create(&input, NULL, &inputThread, &input_thread_data);
	//pthread_create(&output, NULL, &t_output, &output_thread_data);

	pthread_join(critical_server_channel, NULL);
	pthread_join(input, NULL);
	//pthread_join(output, NULL);

	printf("Main terminating\n");
	return EXIT_SUCCESS;
}
