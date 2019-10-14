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
#include "RailController.h"

void * railStateMachine(void * data);
void printState(rail_state_t state);


