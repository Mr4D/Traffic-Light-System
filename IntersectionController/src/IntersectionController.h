#pragma once

#include <traffic_types.h>
#include <traffic_file_lib.h>
#include <traffic_library.h>

#define NULL_PULSE 0
#define STATE_TIMER_EXPIRE_PULSE 1
#define PED_TIMER_EXPIRE_PULSE 2
#define NS_STR_INPUT_PULSE 3
#define EW_STR_INPUT_PULSE 4
#define NS_R_INPUT_PULSE 5
#define EW_R_INPUT_PULSE 6
#define NS_P_INPUT_PULSE 7
#define EW_P_INPUT_PULSE 8

#define TRAFFIC_INPUT_COUNT 6

// KEYS
#define	NS_STR 1
#define	NS_R   2
#define	NS_P   3
#define	EW_STR 5
#define	EW_R   6
#define	EW_P   7

typedef enum {
    PEAK,
    OFF_PEAK
} traffic_mode_t;

typedef struct {
	traffic_mode_t mode;
	pthread_rwlock_t rwlock_input;
} traffic_mode_data_t;

// Input Signal Buffer
typedef struct {
	int length;

	int input_buffer[TRAFFIC_INPUT_COUNT];
}traffic_input_queue_t;

typedef struct {
	traffic_input_queue_t * input_queue;
	pthread_rwlock_t rwlock_input;
} traffic_input_data_t;


// thread data

// state machine thread
typedef struct {
	traffic_input_data_t * input_data;
	traffic_state_data_t * traffic_state_data;
	rail_state_data_t * rail_state_data;
	int * change_state_channel_id;
	connection_id_data_t * tcc_critical_server_coid_data;
	traffic_mode_data_t * traffic_mode_data;
} state_machine_thread_data_t;

// input thread
typedef struct {
	traffic_input_data_t * input_data;
	int * change_state_channel_id;
	traffic_mode_data_t * traffic_mode_data;
} input_thread_data_t;

// output thread
typedef struct {
	traffic_state_data_t * traffic_state_data;
	rail_state_data_t * rail_state_data;
} output_thread_data_t;

// critical client thread
typedef struct {
	int coid;
	traffic_state_t current_state;

	pthread_mutex_t mutex;
} critical_client_send_data_t;

// critical server thread
typedef struct {
	rail_state_data_t * rail_state_data;
} critical_server_data_t;

// non-critical server thread
typedef struct {
	traffic_mode_data_t * traffic_mode_data;
} non_critical_server_data_t;

