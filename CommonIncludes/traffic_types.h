#pragma once

#define BUF_SIZE 100


#define TCC_HOSTNAME "RMIT_BBB_v5_03" //"RMIT_BBB_v5_03" "VM_x86_Target01"
#define I1_HOSTNAME "RMIT_BBB_v5_06"

#define TCC_CRITICAL_CHANNEL  "s3378373_state_recieve"

// QNET Channels
#define TCC_CRITICAL_CHANNEL_ATTACH_POINT  "/net/" TCC_HOSTNAME "/dev/name/local/" TCC_CRITICAL_CHANNEL  // hostname using full path, change myname to the name used for server


// Client IDs
#define I1_CLIENT_ID 1
#define I2_CLIENT_ID 2
#define TCC_CLIENT_ID 3
#define X1_CLIENT_ID 4
#define RCC_CLIENT_ID 5

typedef enum {
    false,
    true
} bool;

typedef enum {
    INITIAL,
    ALL_RED,
    FAULT,
    NS_S_G,
    NS_S_Y,
    EW_R_G,
    EW_R_Y,
    EW_S_G,
    EW_S_Y,
    NS_R_G,
    NS_R_Y
} traffic_state_t;

typedef enum {
	TRAIN,
	NO_TRAIN
} rail_state_t;

typedef enum {
    PEAK,
    OFF_PEAK
} traffic_mode_t;

typedef struct {

	traffic_state_t previous_state;
	traffic_state_t next_state;

	traffic_state_t current_state;
	
	bool produce_data;	
	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

} traffic_state_data_t;

typedef struct {

	traffic_state_t currentState;	
	
	bool produce_data;	
	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

} rail_state_data_t;

typedef union {
	
	traffic_state_t traffic_state;
	rail_state_t rail_state;
	
} state_t;


typedef struct {
	struct _pulse hdr; // Our real data comes after this header
	int client_id;
	state_t state;
} msg_data_t;

typedef struct {
	struct _pulse hdr;  // Our real data comes after this header
    char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
} msg_reply_t;