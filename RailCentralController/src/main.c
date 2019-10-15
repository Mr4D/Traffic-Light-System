#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "RCC_MessagePassing.h"
#include "RailCentralController.h"
#include <traffic_library.h>
#include <traffic_types.h>

void initData(rail_state_data_t *rail_state_data,
			  critical_client_thread_data_t *critical_client_channel_thread_data,
			  critical_server_thread_data_t *critical_server_channel_thread_data) {

	initRailStateData(rail_state_data);

	critical_client_channel_thread_data->rail_state_data = rail_state_data;
	critical_server_channel_thread_data->rail_state_data = rail_state_data;
}

int main(void) {

	rail_state_data_t rail_state_data;
	critical_client_thread_data_t critical_client_channel_thread_data;
	critical_server_thread_data_t critical_server_channel_thread_data;

	initData(&rail_state_data,
			 &critical_client_channel_thread_data,
			 &critical_server_channel_thread_data);

	pthread_t t_critical_client_channel;
	pthread_t t_critical_server_channel;

	pthread_create(&t_critical_client_channel, NULL, &critical_client_channel_thread, &critical_client_channel_thread_data);
	pthread_create(&t_critical_server_channel, NULL, &critical_server_channel_thread, &critical_server_channel_thread_data);

	pthread_join(t_critical_client_channel, NULL);
	pthread_join(t_critical_server_channel, NULL);

	return EXIT_SUCCESS;
}
