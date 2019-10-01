#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/netmgr.h>
#include <sys/neutrino.h>

#include <traffic_types.h>

#define TIME_TRAFFIC_INITIAL 1
#define TIME_TRAFFIC_STRAIGHT 5
#define TIME_TRAFFIC_TURN 3
#define TIME_TRAFFIC_YELLOW 1
#define TIME_TRAFFIC_ALL_RED 1
#define TIME_TRAFFIC_NONE 0

#define TIME_PED_GREEN 30
#define TIME_PED_YELLOW 30

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL

typedef union {
	struct _pulse pulse;
} my_message_t;

struct sigevent event;

timer_t state_timer_id;
timer_t ped_timer_id;

struct itimerspec itime_state_timer;
struct itimerspec itime_ped_timer;

// timer_gettime() use this to check the value of the timer for peds
int chid;
int rcvid;
int val;

my_message_t msg;

traffic_mode_t mode;

void initiliseTimer();
void changeTrafficState_peak(traffic_state_data_t * statedata);
void changeTrafficState_offPeak(traffic_state_data_t * statedata);
void updateStateTimer(traffic_state_t state, struct itimerspec * statetimer);
void * trafficStateMachine(void * data);


void printState(traffic_state_t state);

