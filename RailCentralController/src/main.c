#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "RCC_MessagePassing.h"
#include <traffic_types.h>


int main(void) {
	rail_state_rcc_data_t rcc_state_data = {
		NO_TRAIN,
		true,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};

	pthread_t t_critical_client_channel;
	pthread_t t_critical_server_channel;

	pthread_create(&t_critical_client_channel, NULL, &critical_client_channel_thread, &rcc_state_data);
	pthread_create(&t_critical_server_channel, NULL, &critical_server_channel_thread, &rcc_state_data);

	pthread_join(t_critical_client_channel, NULL);
	pthread_join(t_critical_server_channel, NULL);

	return EXIT_SUCCESS;
}
