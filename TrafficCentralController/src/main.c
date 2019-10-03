#include <stdio.h>
#include <stdlib.h>

#include "TCC_MessagePassing.h"
#include "Output.h"

int main(void) {

	tcc_state_data_t tcc_state_data = {
			INITIAL,
			INITIAL,
			NO_TRAIN,
			true,
			PTHREAD_MUTEX_INITIALIZER,
			PTHREAD_COND_INITIALIZER
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
