#include <stdio.h>
#include <stdlib.h>

#include "TCC_MessagePassing.h"

int main(void) {

	pthread_t critical_server_channel;

	pthread_create(&critical_server_channel, NULL, &critical_server_channel_thread, NULL);

	pthread_join(critical_server_channel, NULL);

	printf("Main terminating");
	return EXIT_SUCCESS;
}
