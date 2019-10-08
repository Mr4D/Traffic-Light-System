#include "stateMachine.h"

void * railStateMachine(void * data) {

	printf("RailStateMachine()\n");

	struct _pulse change_state_msg;
	int change_state_channel_id;
	int rcvid;


	rail_state_t current_state = NO_TRAIN;
	InputSignal rail_input;

	rail_state_machine_thread_data_t * state_machine_thread_data = (rail_state_machine_thread_data_t*) data;

	rail_input_data_t * input_data = state_machine_thread_data->input_data;
	rail_state_data_t * state_data = state_machine_thread_data->rail_state_data;


	change_state_channel_id = state_machine_thread_data->change_state_channel_id; // Create a communications channel


	while(1) {


		// get input flags
		pthread_rwlock_rdlock( &input_data->rwlock_input );
			rail_input = input_data->train;
		pthread_rwlock_unlock( &input_data->rwlock_input );


		// get current state
		if(rail_input == ON){
			current_state = TRAIN;
		}
		else if(rail_input == OFF){
			current_state = NO_TRAIN;
		}
		else{
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




		// wait for timer
		bool change_state = false;
		while(!change_state){
			rcvid = MsgReceivePulse(change_state_channel_id, &change_state_msg, sizeof(change_state_msg), NULL);	// Wait for timer signal

			if(change_state_msg.code == TRAIN_INPUT_PULSE){
				printf("code: %d\n",change_state_msg.code);

				if(current_state == NO_TRAIN){
					change_state = true;
				}
				printf("INPUT_PULSE\n");
			}
			else if(change_state_msg.code == NO_TRAIN_INPUT_PULSE){
				printf("code: %d\n",change_state_msg.code);

				if(current_state == TRAIN){
					change_state = true;
				}
				printf("INPUT_PULSE\n");
			}
		}

		printf("\t\tCHANGE STATE\n");

	}

	return EXIT_SUCCESS;
}

void printState(traffic_state_t state){

	switch(state) {

		case TRAIN:
			printf("\tState: %d [TRAIN]\n", state);
			break;

		case NO_TRAIN:
			printf("\tState: %d [NO_TRAIN]\n", state);
			break;

	}
}
