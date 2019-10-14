#include "traffic_library.h"

void * connectChannelThread(void * data){
	connect_channel_thread_data_t * thread_data = (connect_channel_thread_data_t*)data;

	int retval = EXIT_FAILURE;

	int coid;
	connection_id_data_t * coid_data = thread_data->coid_data;
	char * attach_point = thread_data->attach_point;
	int max_attempts = thread_data->max_attempts;
	int wait_time = thread_data->wait_time;

	int attempts = 0;
	while(attempts < max_attempts){

		if((coid = connectChannel(attach_point)) == -1){
			sleep(wait_time);
		}
		else {
			retval = EXIT_SUCCESS;
			break;
		}

		attempts++;
	}

	pthread_rwlock_wrlock( &coid_data->rwlock_input );
		coid_data->coid = coid;
	pthread_rwlock_unlock( &coid_data->rwlock_input );

	return retval;
}


name_attach_t * createChannel(const char *attach_point) {

	name_attach_t *attach = NULL;

	// Create a local name (/dev/name/...)
	if ((attach = name_attach(NULL, attach_point, 0)) == NULL) {
		sprintf(log_message,"Failed to name_attach on ATTACH_POINT: %s \n", attach_point);
		writeTrafficLogFile(LOG_FILE_NAME,log_message);
	}
	else {
		sprintf(log_message,"Server Listening for Clients on ATTACH_POINT: %s \n", attach_point);
		writeTrafficLogFile(LOG_FILE_NAME,log_message);
	}
	return attach;
}

int connectChannel(const char * attach_point) {

	int server_coid = -1;

	sprintf(log_message,"  ---> Trying to connect to server: %s\n", attach_point);
	writeTrafficLogFile(LOG_FILE_NAME,log_message);

	if ((server_coid = name_open(attach_point, 0)) == -1) {
		writeTrafficLogFile(LOG_FILE_NAME,"\n    ERROR, could not connect to server!\n\n");
	}

	return server_coid;
}

void handlePulse(msg_data_t *msg) {
	switch (msg->hdr.code) {

		case _PULSE_CODE_DISCONNECT:
			if(LOG){
				sprintf(log_message,"\nServer received Detach pulse from ClientID:%d but rejected it ...\n",  msg->client_id);
				writeTrafficLogFile(LOG_FILE_NAME,log_message);
			}
			break;

		case _PULSE_CODE_UNBLOCK:
			// REPLY blocked client wants to unblock (was hit by a signal
			// or timed out).  It's up to you if you reply now or later.
			if(LOG){
				writeTrafficLogFile(LOG_FILE_NAME,"\nServer got _PULSE_CODE_UNBLOCK\n");
			}
			break;

		case _PULSE_CODE_COIDDEATH: // from the kernel
			if(LOG){
				writeTrafficLogFile(LOG_FILE_NAME,"\nServer got _PULSE_CODE_COIDDEATH\n");
			}
			break;

		case _PULSE_CODE_THREADDEATH: // from the kernel
			if(LOG){
				writeTrafficLogFile(LOG_FILE_NAME,"\nServer got _PULSE_CODE_THREADDEATH\n");
			}
			break;

		default:
			// Some other pulse sent by one of your processes or the kernel
			if(LOG){
				sprintf(log_message,"\nServer got some other pulse: %d\n",  msg->hdr.code);
				writeTrafficLogFile(LOG_FILE_NAME, log_message);
			}
			break;
	}

}


void handleIOConnect(msg_data_t *msg, int rcvid ) {

	// If the Global Name Service (gns) is running, name_open() sends a connect message. The server must EOK it.
	if (msg->hdr.type == _IO_CONNECT ) {
		MsgReply( rcvid, EOK, NULL, 0 );
		if(LOG){
			writeTrafficLogFile(LOG_FILE_NAME,"\n gns service is running....");
		}
	}

	// Some other I/O message was received; reject it
	if (msg->hdr.type > _IO_BASE && msg->hdr.type <= _IO_MAX ) {
		MsgError( rcvid, ENOSYS );

		if(LOG){
			writeTrafficLogFile(LOG_FILE_NAME,"\n Server received and IO message and rejected it....");
		}
	}

}
