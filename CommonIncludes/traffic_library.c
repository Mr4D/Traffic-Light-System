#include "traffic_library.h"

void * sendMessage(void * data){

	send_message_thread_data_t * thread_data = (send_message_thread_data_t *) data;


	msg_data_t msg;
	pthread_rwlock_rdlock(&thread_data->rwlock);
		memcpy(&msg, thread_data->msg, sizeof(msg));
	pthread_rwlock_unlock(&thread_data->rwlock);

	printf("msg.type: %d\n", msg.hdr.type);

	msg_reply_t reply;

	int server_coid = -1;


	printf("  ---> Trying to connect to server named: %s\n", thread_data->attach_point);
	if ((server_coid = name_open(thread_data->attach_point, 0)) == -1) {
		printf("\n    ERROR, could not connect to server!\n\n");
	}



	printf("Connection established to: %s\n", thread_data->attach_point);

	fflush(stdout);


	if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {
		printf(" Error data '%d' NOT sent to server\n", msg.data.traffic_state);
			// maybe we did not get a reply from the server
	}
	else { // now process the reply
		printf("   -->Reply is: '%d'\n", reply.data.traffic_state);
	}


	ConnectDetach(server_coid);
}

name_attach_t * createChannel(const char *attach_point) {

	name_attach_t * attach = NULL;

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
