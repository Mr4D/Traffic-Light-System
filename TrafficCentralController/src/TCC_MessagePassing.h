#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>

#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>

#include "../../CommonIncludes/traffic_types.h"

#define BUF_SIZE 100


#define INFO_DIR "/tmp/info/"
#define TCC_CRITICAL_SERVER_INFO "/tmp/info/criticalserver.info"
#define TCC_NON_CRITICAL_SERVER_INFO ""/tmp/info/"noncriticalserver.info"

void * critical_client_channel_thread(void * data);
void * noncritical_client_channel_thread(void * data);
void * critical_server_channel_thread(void * data);

void writeServerInfo(const char * filename, pid_t pid, int chid);
void readServerInfo(const char * filename, pid_t * pid, int * chid);

typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
	int clientID;
	State Trafficstate; // our data (unique id from client)
} traffic_state_data;

typedef struct
{
	struct _pulse hdr;  // Our real data comes after this header
    char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
} my_reply;
