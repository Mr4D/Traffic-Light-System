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

timer_t timer_id;

struct itimerspec itime_traffic_straight;
struct itimerspec itime_traffic_turn;
struct itimerspec itime_traffic_yellow;
struct itimerspec itime_traffic_all_red;
struct itimerspec itime_traffic_none;
struct itimerspec itime_ped_green;
struct itimerspec itime_ped_yellow;

// timer_gettime() use this to check the value of the timer for peds
int chid;
int rcvid;
int val;

my_message_t msg;

State currentState;
State previousState;
State nextState;

Mode mode;

void changeTrafficState_peak(state_data * data);
void changeTrafficState_offPeak(state_data * data);
void *trafficStateMachine(void * data);
void initiliseTimer();

