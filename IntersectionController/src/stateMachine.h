#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <traffic_types.h>

State currentState;
State previousState;
State nextState;

Mode mode;

void changeTrafficState_peak();
void changeTrafficState_offPeak();
void *trafficStateMachine(void * data);
