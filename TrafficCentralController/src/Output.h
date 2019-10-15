#pragma once

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <share.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/dispatch.h>

#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

#include "TrafficCentralController.h"
#include <traffic_types.h>

void *t_output(void *data);
