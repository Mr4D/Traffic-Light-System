#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/netmgr.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>

#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>

#include <traffic_types.h>
#include "IntersectionController.h"



void * critical_client_send_thread(void * data);
void * critical_server_channel_thread(void * data);
void * noncritical_server_channel_thread(void * data);


