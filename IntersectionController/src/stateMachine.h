#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/netmgr.h>
#include <sys/neutrino.h>

#include <traffic_types.h>

#define TIME_TRAFFIC_INITIAL 1
#define TIME_TRAFFIC_STRAIGHT 10 //NEVER LESS THAN 10
#define TIME_TRAFFIC_TURN 3
#define TIME_TRAFFIC_YELLOW 1
#define TIME_TRAFFIC_ALL_RED 1
#define TIME_TRAFFIC_NONE 0

#define TIME_PED_CROSS 3
#define TIME_PED_FLASHING 2
#define TIME_PED_TIME_OUT 4

#define TIME_PED_GREEN 30
#define TIME_PED_YELLOW 30



struct sigevent state_timer_event;
struct sigevent ped_timer_event;

timer_t state_timer_id;
timer_t ped_timer_id;

struct itimerspec itime_state_timer;
struct itimerspec itime_ped_timer; // used for ped/ped sense



void initiliseTimer();
traffic_state_t changeTrafficState_peak(traffic_state_t current_state, traffic_state_t previous_state, traffic_input_buffer_t * input_data);
traffic_state_t changeTrafficState_offPeak(traffic_state_t current_state, traffic_state_t previous_state, traffic_input_buffer_t * input_data);
void updateTimers(traffic_state_t current_state);

bool decideChangeStatePeak(struct _pulse * change_state_msg, traffic_state_t current_state);
bool decideChangeStateOffPeak(struct _pulse * change_state_msg, traffic_state_t current_state, traffic_input_buffer_t * input_queue);
void clearInputPeak(traffic_input_buffer_t * input_queue, traffic_state_t state);
void clearInputOffPeak(traffic_input_buffer_t * input_queue, traffic_state_t state);

void * trafficStateMachine(void * data);


void printState(traffic_state_t state);

