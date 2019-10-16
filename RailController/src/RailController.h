/*
 * RailController.h
 *
 *  Created on: 14Oct.,2019
 *      Author: NickM_e75acww
 */

#ifndef RAILCONTROLLER_H_
#define RAILCONTROLLER_H_

#include <traffic_file_lib.h>
#include <traffic_library.h>
#include <traffic_types.h>

#define NULL_PULSE 0
#define TRAIN_INPUT_PULSE 1
#define NO_TRAIN_INPUT_PULSE 2

typedef enum {
	ON,
	OFF
} InputSignal;

typedef struct {
	InputSignal train;
	pthread_rwlock_t rwlock_input;
} rail_input_data_t;

// thread data

typedef struct {
	bool input;
	bool fault;
	bool reset;
}change_state_flags_t;

// state machine thread
typedef struct {
	rail_input_data_t *input_data;
	rail_state_data_t *rail_state_data;
	change_state_flags_t *change_state_flags;
} state_machine_thread_data_t;

// input thread
typedef struct {
	rail_input_data_t *input_data;
	change_state_flags_t *change_state_flags;
} input_thread_data_t;

// output thread
typedef struct {
	rail_state_data_t *rail_state_data;
} ouput_thread_data_t;

// critical client thread
typedef struct {
	rail_state_data_t *rail_state_data;
} critial_client_thread_data_t;

// critical server thread
typedef struct {
	fault_data_t *fault_data;
	reset_data_t *reset_data;
	change_state_flags_t *change_state_flags;
	name_attach_t *attach;
} critial_server_thread_data_t;



#endif /* RAILCONTROLLER_H_ */
