#include "stateMachine.h"

void *railStateMachine(void *data) {

	printf("RailStateMachine()\n");

	struct _pulse change_state_msg;
	int rcvid;

	rail_state_t current_state = NO_TRAIN;
	InputSignal rail_input;

	state_machine_thread_data_t *thread_data = (state_machine_thread_data_t *)data;

	rail_input_data_t *input_data = thread_data->input_data;
	rail_state_data_t *state_data = thread_data->rail_state_data;
	
	change_state_flags_t * change_state_flags = thread_data->change_state_flags;
	change_state_flags_t state_flags;

	while (1) {

		// get input flags
		pthread_rwlock_rdlock(&input_data->rwlock_input);
			rail_input = input_data->train;
		pthread_rwlock_unlock(&input_data->rwlock_input);

		// get current state
		if (rail_input == ON) {
			current_state = TRAIN;
		}
		else if (rail_input == OFF) {
			current_state = NO_TRAIN;
		}
		else {
			printf("STATE ERROR\n");
		}

		// update previous state

		// update current state shared data
		pthread_mutex_lock(&state_data->mutex_state);
			state_data->current_state = current_state;
			state_data->data_ready = true;
			pthread_cond_broadcast(&state_data->cond_state_changed);
		pthread_mutex_unlock(&state_data->mutex_state);

		// Print state info
		printState(state_data->current_state);


		printf("Waiting for input\n");


		bool change_state = false;
		while (!change_state) {

			pthread_sleepon_lock();
				pthread_sleepon_wait(change_state_flags);
				memcpy(&state_flags, change_state_flags, sizeof(state_flags));
			pthread_sleepon_unlock();

			if (state_flags.input || state_flags.fault || state_flags.reset) {
				change_state = true;
			}

		}

		// Clear the input flag
		pthread_sleepon_lock();
			state_flags.input = false;
		pthread_sleepon_unlock();			

		printf("\t\tCHANGE STATE\n");
	}
	return EXIT_SUCCESS;
}

void printState(rail_state_t state) {

	switch (state) {

		case TRAIN:
			printf("\tState: %d [TRAIN]\n", state);
			break;

		case NO_TRAIN:
			printf("\tState: %d [NO_TRAIN]\n", state);
			break;
	}
}
