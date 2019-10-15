#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "RCC_MessagePassing.h"
#include <traffic_types.h>
#include "RailCentralController.h"

int main(void) {

	rail_state_data_t rail_state_data = {
		NO_TRAIN,
		true,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};

	critical_client_thread_data_t critical_client_channel_thread_data = {
		&rail_state_data
	};

	critical_server_thread_data_t critical_server_channel_thread_data = {
		&rail_state_data
	};
	pthread_t t_critical_client_channel;
	pthread_t t_critical_server_channel;

	pthread_create(&t_critical_client_channel, NULL, &critical_client_channel_thread, &critical_client_channel_thread_data);
	pthread_create(&t_critical_server_channel, NULL, &critical_server_channel_thread, &critical_server_channel_thread_data);

	pthread_join(t_critical_client_channel, NULL);
	pthread_join(t_critical_server_channel, NULL);

	return EXIT_SUCCESS;
}
