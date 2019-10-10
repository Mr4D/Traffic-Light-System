#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "input.h"
#include "stateMachine.h"
#include "RC_MessagePassing.h"
#include "output.h"

int main(void) {

	int change_state_channel_id = ChannelCreate(0);

		rail_state_data_t rail_state_data = {
			NO_TRAIN,
			false,
			PTHREAD_MUTEX_INITIALIZER,
			PTHREAD_COND_INITIALIZER
		};

		rail_input_data_t input_data = {
				{OFF},
				PTHREAD_RWLOCK_INITIALIZER
		};

		rail_input_thread_data_t input_thread_data = {
				&input_data,
				change_state_channel_id
		};

		rail_state_machine_thread_data_t state_machine_thread_data = {
				&input_data,
				&rail_state_data,
				change_state_channel_id
		};



		pthread_t t_input;
		pthread_t t_rail_state_machine;
		pthread_t t_output;

		// messaging threads
		pthread_t t_critical_client_channel;
		pthread_t t_critical_server_channel;

		pthread_create(&t_input, NULL, &inputThread, &input_thread_data);
		pthread_create(&t_rail_state_machine, NULL, &railStateMachine, &state_machine_thread_data);
		pthread_create(&t_output, NULL, LCD_Control, &rail_state_data);

		pthread_create(&t_critical_client_channel, NULL, &critical_client_channel_thread, &rail_state_data);
		//pthread_create(&t_critical_server_channel, NULL, &critical_server_channel_thread, &rail_state_data);

		pthread_join(t_input, NULL);
		pthread_join(t_rail_state_machine, NULL);
		pthread_join(t_output, NULL);
		pthread_join(t_critical_client_channel, NULL);
		//pthread_join(t_critical_server_channel, NULL);

	return EXIT_SUCCESS;
}
