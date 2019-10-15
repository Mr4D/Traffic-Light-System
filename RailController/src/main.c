#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "RC_MessagePassing.h"
#include "RailController.h"
#include "input.h"
#include "output.h"
#include "stateMachine.h"
#include <traffic_library.h>

void initRailInputData(rail_input_data_t *input_data) {
	input_data->train = OFF;
	pthread_rwlock_init(&input_data->rwlock_input, NULL);
}

void initData(rail_state_data_t *rail_state_data,
			  rail_input_data_t *input_data,
			  state_machine_thread_data_t *state_machine_thread_data,
			  input_thread_data_t *input_thread_data,
			  ouput_thread_data_t *ouput_thread_data,
			  critial_client_thread_data_t *critial_client_thread_data,
			  int *change_state_channel_id) {

	*change_state_channel_id = ChannelCreate(0);

	initRailStateData(rail_state_data);
	initRailInputData(input_data);

	state_machine_thread_data->input_data = input_data;
	state_machine_thread_data->rail_state_data = rail_state_data;
	state_machine_thread_data->change_state_channel_id = change_state_channel_id;

	input_thread_data->input_data = input_data;
	input_thread_data->change_state_channel_id = change_state_channel_id;

	ouput_thread_data->rail_state_data = rail_state_data;

	critial_client_thread_data->rail_state_data = rail_state_data;
}

int main(void) {

	rail_state_data_t rail_state_data;
	rail_input_data_t input_data;
	state_machine_thread_data_t state_machine_thread_data;
	input_thread_data_t input_thread_data;
	ouput_thread_data_t ouput_thread_data;
	critial_client_thread_data_t critial_client_thread_data;
	int change_state_channel_id;

	initData(
		&rail_state_data,
		&input_data,
		&state_machine_thread_data,
		&input_thread_data,
		&ouput_thread_data,
		&critial_client_thread_data,
		&change_state_channel_id);

	pthread_t t_input;
	pthread_t t_rail_state_machine;
	pthread_t t_output;

	pthread_t t_critical_client_channel; // messaging threads
	pthread_t t_critical_server_channel;

	pthread_create(&t_input, NULL, &inputThread, &input_thread_data);
	pthread_create(&t_rail_state_machine, NULL, &railStateMachine, &state_machine_thread_data);
	//pthread_create(&t_output, NULL, LCD_Control, &ouput_thread_data);

	pthread_create(&t_critical_client_channel, NULL, &critical_client_channel_thread, &critial_client_thread_data);
	//pthread_create(&t_critical_server_channel, NULL, &critical_server_channel_thread, &rail_state_data);

	pthread_join(t_input, NULL);
	pthread_join(t_rail_state_machine, NULL);
	//pthread_join(t_output, NULL);
	pthread_join(t_critical_client_channel, NULL);
	//pthread_join(t_critical_server_channel, NULL);

	return EXIT_SUCCESS;
}
