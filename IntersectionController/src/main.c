#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "IC_MessagePassing.h"
#include "Input.h"
#include "IntersectionController.h"
#include "Output.h"
#include "stateMachine.h"

void initTrafficModeData(traffic_mode_data_t *traffic_mode_data) {
	traffic_mode_data->mode = PEAK;
	pthread_rwlock_init(&traffic_mode_data->rwlock, NULL);
}

void initFaultData(fault_data_t *fault_data) {
	fault_data->fault = false;
	pthread_mutex_init(&fault_data->mutex, NULL);
}

void initResetData(reset_data_t *reset_data) {
	reset_data->reset = false;
	pthread_mutex_init(&reset_data->mutex, NULL);
}

void initInputQueue(traffic_input_queue_t *input_queue) {
	input_queue->length = 0;
	for (int i = 0; i < TRAFFIC_INPUT_COUNT; i++) {
		input_queue->input_buffer[i] = 0;
	}
}

void initInputData(traffic_input_data_t *input_data, traffic_input_queue_t *input_queue) {
	input_data->input_queue = input_queue;
	pthread_rwlock_init(&input_data->rwlock_input, NULL);
}

void initTrafficStateData(traffic_state_data_t *traffic_state_data) {
	traffic_state_data->current_state = INITIAL;
	traffic_state_data->data_ready = false;
	pthread_mutex_init(&traffic_state_data->mutex_state, NULL);
	pthread_cond_init(&traffic_state_data->cond_state_changed, NULL);
}

void initRailStateData(rail_state_data_t *rail_state_data) {
	rail_state_data->current_state = NO_TRAIN;
	rail_state_data->data_ready = false;
	pthread_mutex_init(&rail_state_data->mutex_state, NULL);
	pthread_cond_init(&rail_state_data->cond_state_changed, NULL);
}

void initData(traffic_mode_data_t *traffic_mode_data,
			  traffic_input_queue_t *input_queue,
			  traffic_input_data_t *input_data,
			  traffic_state_data_t *traffic_state_data,
			  rail_state_data_t *rail_state_data,
			  state_machine_thread_data_t *state_machine_thread_data,
			  input_thread_data_t *input_thread_data,
			  output_thread_data_t *output_thread_data,
			  critical_server_data_t *critical_server_data,
			  int *change_state_channel_id,
			  fault_data_t *fault_data,
			  reset_data_t *reset_data) {

	*change_state_channel_id = ChannelCreate(0);

	initTrafficModeData(traffic_mode_data);
	initFaultData(fault_data);
	initResetData(reset_data);
	initInputQueue(input_queue);
	initInputData(input_data, input_queue);
	initTrafficStateData(traffic_state_data);
	initRailStateData(rail_state_data);

	state_machine_thread_data->input_data = input_data;
	state_machine_thread_data->traffic_state_data = traffic_state_data;
	state_machine_thread_data->rail_state_data = rail_state_data;
	state_machine_thread_data->change_state_channel_id = change_state_channel_id;
	state_machine_thread_data->traffic_mode_data = traffic_mode_data;
	state_machine_thread_data->fault_data = fault_data;
	state_machine_thread_data->reset_data = reset_data;

	input_thread_data->input_data = input_data;
	input_thread_data->change_state_channel_id = change_state_channel_id;
	input_thread_data->traffic_mode_data = traffic_mode_data;

	output_thread_data->traffic_state_data = traffic_state_data;
	output_thread_data->rail_state_data = rail_state_data;

	critical_server_data->rail_state_data = traffic_state_data;
	critical_server_data->rail_state_data = rail_state_data;
	critical_server_data->traffic_mode_data = traffic_mode_data;
	critical_server_data->fault_data = fault_data;
	critical_server_data->reset_data = reset_data;
	critical_server_data->change_state_channel_id = change_state_channel_id;
}

int main(void) {

	FILE *fp;
	if ((fp = openTrafficLogFile(LOG_FILE_NAME)) == NULL) {
		printf("Failed to open log file: %s\n", LOG_FILE_NAME);
	}
	else {
		printf("Successful open log file: %s\n", LOG_FILE_NAME);
		fclose(fp);
	}

	while (1) {
		// Declare memory for all data
		int change_state_channel_id;
		traffic_mode_data_t traffic_mode_data;
		traffic_input_queue_t input_queue;
		traffic_input_data_t input_data;
		traffic_state_data_t traffic_state_data;
		rail_state_data_t rail_state_data; // TODO: to update this data type
		state_machine_thread_data_t state_machine_thread_data;
		input_thread_data_t input_thread_data;
		output_thread_data_t output_thread_data;
		critical_server_data_t critical_server_data;
		fault_data_t fault_data;
		reset_data_t reset_data;

		printf("init()\n");

		initData(&traffic_mode_data,
				 &input_queue,
				 &input_data,
				 &traffic_state_data,
				 &rail_state_data,
				 &state_machine_thread_data,
				 &input_thread_data,
				 &output_thread_data,
				 &critical_server_data,
				 &change_state_channel_id,
				 &fault_data,
				 &reset_data);

		// messaging threads
		pthread_t t_critical_server_channel;
		printf("create server()\n");
		// create critical channel to listen for commands from tcc
		pthread_create(&t_critical_server_channel, NULL, &critical_server_channel_thread, &critical_server_data);

		pthread_t t_input;
		pthread_t t_traffic_state_machine;
		pthread_t t_output;

		printf("create state machine()\n");
		//pthread_create(&t_input, NULL, &inputThread, &input_thread_data);
		pthread_create(&t_traffic_state_machine, NULL, &trafficStateMachine, &state_machine_thread_data);
		//pthread_create(&t_output, NULL, LCD_Control, &output_thread_data);

		bool reset = false;
		while (!reset) {
			pthread_mutex_lock(&reset_data.mutex);
			while (!reset_data.data_ready) {
				printf("waiting()\n");
				pthread_cond_wait(&reset_data.cond, &reset_data.mutex);
			}

			reset = reset_data.reset;
			reset_data.data_ready = false;

			pthread_mutex_unlock(&reset_data.mutex);
		}

		printf("reseting\n");

		//pthread_abort(t_input);
		pthread_cancel(t_traffic_state_machine);
		//pthread_abort(t_output);
		pthread_cancel(t_critical_server_channel);
		name_detach(critical_server_data.attach, NULL);
	}
	printf("Main terminating");
	return EXIT_SUCCESS;
}
