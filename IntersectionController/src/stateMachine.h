#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/netmgr.h>
#include <sys/neutrino.h>

#include <traffic_types.h>

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL

typedef union {
	struct _pulse pulse;
} my_message_t;

struct sigevent event;
struct itimerspec itime_traffic_straight;
struct itimerspec itime_traffic_yellow;
struct itimerspec itime_traffic_red;

timer_t timer_id;

int chid;
int rcvid;
int val;

my_message_t msg;

State currentState;
State previousState;
State nextState;

Mode mode;

void changeTrafficState_peak();
void changeTrafficState_offPeak();
void *trafficStateMachine(void * data);

void initiliseTimer();
