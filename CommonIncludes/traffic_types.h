#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>

#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>

#define BUF_SIZE 100

#define I1_HOSTNAME "RMIT_BBB_v5_02"
#define TCC_HOSTNAME "RMIT_BBB_v5_03" //"RMIT_BBB_v5_03" "VM_x86_Target01"
#define RCC_HOSTNAME "RMIT_BBB_v5_06"
#define RAIL_HOSTNAME "RMIT_BBB_v5_07"


#define TCC_CRITICAL_CHANNEL  "s3378373_state_recieve"
#define I1_CRITICAL_CHANNEL  "s3378373_rail_state_recieve"
#define RCC_CRITICAL_CHANNEL  "s3378373_rail_state_recieve"

/* QNET Channels */
#define TCC_CRITICAL_CHANNEL_ATTACH_POINT  "/net/" TCC_HOSTNAME "/dev/name/local/" TCC_CRITICAL_CHANNEL  // hostname using full path, change myname to the name used for server
#define I1_CRITICAL_CHANNEL_ATTACH_POINT  "/net/" I1_HOSTNAME "/dev/name/local/" I1_CRITICAL_CHANNEL  // hostname using full path, change myname to the name used for server
#define RCC_CRITICAL_CHANNEL_ATTACH_POINT  "/net/" RCC_HOSTNAME "/dev/name/local/" RCC_CRITICAL_CHANNEL

/* Client IDs */
#define I1_CLIENT_ID 1
#define I2_CLIENT_ID 2
#define TCC_CLIENT_ID 3
#define X1_CLIENT_ID 4
#define RCC_CLIENT_ID 5

#define NULL_PULSE 0
#define STATE_TIMER_EXPIRE_PULSE 1
#define PED_TIMER_EXPIRE_PULSE 2
#define NS_STR_INPUT_PULSE 3
#define EW_STR_INPUT_PULSE 4
#define NS_R_INPUT_PULSE 5
#define EW_R_INPUT_PULSE 6
#define NS_P_INPUT_PULSE 7
#define EW_P_INPUT_PULSE 8

#define TRAIN_INPUT_PULSE 9
#define NO_TRAIN_INPUT_PULSE 10

#define PULSE_PRIORITY 10

// KEYS
#define	NS_STR 1
#define	NS_R   2
#define	NS_P   3
#define	EW_STR 5
#define	EW_R   6
#define	EW_P   7


/* Boolean typedef */
typedef enum {
    false,
    true
} bool;


/* Traffic light states */
typedef enum {

    INITIAL,
    ALL_RED,
    FAULT,

	NS_S_G_SEN,
	NS_S_G_P_X,
	NS_S_G_P_F,
    NS_S_G,
    NS_S_Y,

	EW_S_G_SEN,
	EW_S_G_P_X,
	EW_S_G_P_F,
	EW_S_G,
    EW_S_Y,

	EW_R_G,
    EW_R_Y,

    NS_R_G,
    NS_R_Y

} traffic_state_t;

/* Train states*/
typedef enum {
	TRAIN,
	NO_TRAIN
} rail_state_t;


typedef enum {
    PEAK,
    OFF_PEAK
} traffic_mode_t;

/* Struct for r/w locking of traffic states */
typedef struct {
	traffic_state_t current_state;

	bool data_ready_output;
	bool data_ready_message;

	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

} traffic_state_data_t;


/* Struct for r/w locking of rail states*/
typedef struct {

	rail_state_t current_state;	

	bool data_ready_msg;
	bool data_ready_output;
	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

} rail_crossing_state_data_t;

/* Struct for r/w locking of rail states*/
typedef struct {

	rail_state_t current_state;	

	bool data_ready;
	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;

} rail_state_data_t;

typedef struct {

	rail_state_t current_state;	

	bool data_ready;
	pthread_mutex_t mutex_state;		// needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed;	// needs to be set to PTHREAD_COND_INITIALIZER;

} rail_state_rcc_data_t;

/* Struct for r/w locking of ped states*/
// typedef struct {

// 	ped_state_t current_state;
// 	bool produce_data;
// 	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
// 	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

// }	ped_state_data_t;

/* tcc state requirements*/
typedef struct {

	traffic_state_t i1_state;	
	traffic_state_t i2_state;
	rail_state_t x1_state;
	
	bool produce_data;	
	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

} tcc_state_data_t;

typedef union {
	traffic_state_t traffic_state;
	rail_state_t rail_state;
} state_t;

typedef struct{
	traffic_state_data_t * traffic_state_data;
	rail_state_data_t * rail_state_data;
} ic_state_data_t;

typedef enum {ON, OFF} InputSignal;

#define INPUT_COUNT 6


#define TRAFFIC_INPUT_COUNT 6

// Input Signal Buffer
typedef struct {
	int end_index;

	int input_buffer[TRAFFIC_INPUT_COUNT];
}traffic_input_buffer_t;

typedef struct {
	traffic_input_buffer_t * input_queue;
	pthread_rwlock_t rwlock_input;
} traffic_input_data_t;

typedef struct {
	traffic_input_data_t * input_data;
	int change_state_channel_id;
	traffic_mode_t * mode;
} traffic_input_thread_data_t;


typedef struct {
	InputSignal train;
	pthread_rwlock_t rwlock_input;
} rail_input_data_t;

typedef struct {
	rail_input_data_t * input_data;
	int change_state_channel_id;
} rail_input_thread_data_t;

typedef struct {
	traffic_input_data_t * input_data;
	ic_state_data_t * ic_state_data;
	int change_state_channel_id;
	traffic_mode_t * mode;
} traffic_state_machine_thread_data_t;

typedef struct {
	rail_input_data_t * input_data;
	rail_crossing_state_data_t * rail_state_data;
	int change_state_channel_id;
} rail_state_machine_thread_data_t;

typedef struct {
	struct _pulse hdr; // Our real data comes after this header
	int client_id;
	state_t state;
} msg_data_t;

typedef struct {
	struct _pulse hdr;  // Our real data comes after this header
    char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
} msg_reply_t;



