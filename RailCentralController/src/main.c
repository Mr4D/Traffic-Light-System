#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "RCC_MessagePassing.h"
#include <traffic_types.h>


int main(void) {
	rcc_state_data_t rcc_state_data = {
		NO_TRAIN,
		true,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};

	pthread_t critical_client_channel;

	pthread_create(&critical_client_channel, NULL, &critical_client_channel_thread, &rcc_state_data);

	pthread_join(critical_client_channel, NULL);

	return EXIT_SUCCESS;
}
