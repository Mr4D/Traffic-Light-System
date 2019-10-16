#include <stdio.h>
#include <stdlib.h>

#include "Output.h"
#include "TCC_MessagePassing.h"
#include "TrafficCentralController.h"

void initData(traffic_state_data_t *i1_state_data,
			  traffic_state_data_t *i2_state_data,
			  rail_state_data_t *x1_state_data,
			  tcc_state_data_t *tcc_state_data,
			  output_thread_data_t *output_thread_data,
			  critical_server_thread_data_t *critical_server_thread_data,
			  msg_data_t *msg_send,
			  send_message_thread_data_t *i1_send) {

	initTrafficStateData(i1_state_data);
	initTrafficStateData(i1_state_data);
	initRailStateData(x1_state_data);

	tcc_state_data->i1_state_data = i1_state_data;
	tcc_state_data->i2_state_data = i2_state_data;
	tcc_state_data->x1_state_data = x1_state_data;

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
	output_thread_data_t output_thread_data;
	critical_server_thread_data_t critical_server_thread_data;

	msg_data_t msg_send;
	send_message_thread_data_t i1_send;

	initData(&i1_state_data,
			 &i2_state_data,
			 &x1_state_data,
			 &tcc_state_data,
			 &output_thread_data,
			 &critical_server_thread_data,
			 &msg_send,
			 &i1_send);
	
	// Thread definitions
	pthread_t th_output;
	pthread_t th_critical_server_channel;
	pthread_t th_send;

	// Thread attributes
	pthread_attr_t th_output_attr;
	pthread_attr_t th_critical_server_channel_attr;
	pthread_attr_t th_send_attr;

	// Thread schedule parameters
	struct sched_param th_output_param;
	struct sched_param th_critical_server_channel_param;
	struct sched_param th_send_param;

	// Set thread attributes
	setThreadSettings(&th_output_attr, &th_output_param, TCC_OUTPUT_THREAD_PRIORITY, false);
	setThreadSettings(&th_critical_server_channel_attr, &th_critical_server_channel_param, TCC_CRIT_SERVER_CHANNEL_THREAD_PRIORITY, false);
	setThreadSettings(&th_send_attr, &th_send_param, TCC_SEND_THREAD_PRIORITY, false);

	// Spawn threads
	pthread_create(&th_send, &th_send_attr, &sendMessage, &i1_send);
	pthread_create(&th_critical_server_channel, &th_critical_server_channel_attr, &critical_server_channel_thread, &critical_server_thread_data);
	pthread_create(&th_output, &th_output_attr, &t_output, &output_thread_data);

	pthread_join(send, NULL);
	pthread_join(critical_server_channel, NULL);
	pthread_join(output, NULL);
	
	printf("Main terminating\n");
	return EXIT_SUCCESS;
}
