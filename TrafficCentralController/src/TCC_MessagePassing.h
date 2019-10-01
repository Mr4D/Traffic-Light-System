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

#include <traffic_types.h>


#define INFO_DIR "/tmp/info/"
#define TCC_CRITICAL_SERVER_INFO "/tmp/info/criticalserver.info"
#define TCC_NON_CRITICAL_SERVER_INFO ""/tmp/info/"noncriticalserver.info"

void * critical_client_channel_thread(void * data);
void * noncritical_client_channel_thread(void * data);
void * critical_server_channel_thread(void * data);

void writeServerInfo(const char * filename, pid_t pid, int chid);
void readServerInfo(const char * filename, pid_t * pid, int * chid);




