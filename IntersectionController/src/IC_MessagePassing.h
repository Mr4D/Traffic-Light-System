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

#define BUF_SIZE 100

#define TCC_SERVER "/net/VM_x86_Target01"
#define INFO_DIR "/tmp/info/"
#define TCC_CRITICAL_SERVER_INFO "/net/VM_x86_Target01/tmp/info/criticalserver.info"
#define TCC_NON_CRITICAL_SERVER_INFO TCC_SERVER + INFO_DIR + "noncriticalserver.info"

void * critical_client_channel_thread(void * data);
void * critical_server_channel_thread(void * data);
void * noncritical_server_channel_thread(void * data);

void writeServerInfo(const char * filename, pid_t pid, int chid);
void readServerInfo(const char * filename, pid_t * pid, int * chid);

typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
	int ClientID; // our data (unique id from client)
    int data;     // our data
} my_data;

typedef struct
{
	struct _pulse hdr;  // Our real data comes after this header
    char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
} my_reply;
