#pragma once

#define BUF_SIZE 100

#define I1_HOSTNAME "RMIT_BBB_v5_02"
#define TCC_HOSTNAME "RMIT_BBB_v5_03" //"RMIT_BBB_v5_03" "VM_x86_Target01"
#define RCC_HOSTNAME "RMIT_BBB_v5_06"


#define TCC_CRITICAL_CHANNEL  "s3378373_state_recieve"
#define I1_CRITICAL_CHANNEL  "s3378373_rail_state_recieve"

/* QNET Channels */
#define TCC_CRITICAL_CHANNEL_ATTACH_POINT  "/net/" TCC_HOSTNAME "/dev/name/local/" TCC_CRITICAL_CHANNEL  // hostname using full path, change myname to the name used for server
#define I1_CRITICAL_CHANNEL_ATTACH_POINT  "/net/" I1_HOSTNAME "/dev/name/local/" I1_CRITICAL_CHANNEL  // hostname using full path, change myname to the name used for server


/* Client IDs */
#define I1_CLIENT_ID 1
#define I2_CLIENT_ID 2
#define TCC_CLIENT_ID 3
#define X1_CLIENT_ID 4
#define RCC_CLIENT_ID 5

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
    NS_S_G,
    NS_S_Y,
    EW_R_G,
    EW_R_Y,
    EW_S_G,
    EW_S_Y,
    NS_R_G,
    NS_R_Y
} traffic_state_t;

/* Train states*/
typedef enum {
	TRAIN,
	NO_TRAIN
} rail_state_t;

/* Peak/offpeak superstate for traffic */
typedef enum {
	ALL_RED_P,
	NS_P_G,
	NS_P_Y,
	EW_P_G,
	EW_P_Y,
} ped_state_t;

typedef enum {
    PEAK,
    OFF_PEAK
} traffic_mode_t;

/* Struct for r/w locking of traffic states */
typedef struct {
	traffic_state_t current_state;
	bool produce_data;	
	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

} traffic_state_data_t;


/* Struct for r/w locking of rail states*/
typedef struct {

	rail_state_t current_state;	
	bool produce_data;	
	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

} rail_state_data_t;

/* Struct for r/w locking of ped states*/
typedef struct {

	ped_state_t current_state;
	bool produce_data;
	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

}	ped_state_data_t;

/* tcc state requirements*/
typedef struct {

	traffic_state_t i1_state;	
	traffic_state_t i2_state;
	rail_state_t x1_state;
	
	bool produce_data;	
	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

} tcc_state_data_t;

/* Rail doesn't consider traffic situation, hence rcc only uses rail state data.*/
typedef rail_state_data_t rcc_state_data_t;



typedef union {
	
	traffic_state_t traffic_state;
	rail_state_t rail_state;
	
} state_t;

typedef struct{
	traffic_state_t traffic_state;
	rail_state_t	rail_state;
	ped_state_t		ped_state;

	bool produce_data;
	pthread_mutex_t mutex_state;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond_state_changed; // needs to be set to PTHREAD_COND_INITIALIZER;

}ic_state_data_t;

typedef struct {
	struct _pulse hdr; // Our real data comes after this header
	int client_id;
	state_t state;
} msg_data_t;

typedef struct {
	struct _pulse hdr;  // Our real data comes after this header
    char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
} msg_reply_t;
