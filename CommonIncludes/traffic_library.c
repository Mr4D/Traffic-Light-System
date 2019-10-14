#include "traffic_library.h"

name_attach_t *createChannel(const char *attach_point) {

	name_attach_t *attach = NULL;

	// Create a local name (/dev/name/...)
	if ((attach = name_attach(NULL, attach_point, 0)) == NULL) {
		printf("\nFailed to name_attach on ATTACH_POINT: %s \n", attach_point);
		printf("\n Possibly another server with the same name is already running !\n");
	}

	printf("Server Listening for Clients on ATTACH_POINT: %s \n", attach_point);

	return attach;
}

int connectChannel(const char *attach_point) {

	int server_coid = -1;

	printf("  ---> Trying to connect to server named: %s\n", attach_point);

	if ((server_coid = name_open(attach_point, 0)) == -1) {
		printf("\n    ERROR, could not connect to server!\n\n");
	}

	return server_coid;
}

void handlePulse(msg_data_t *msg) {
	
	switch (msg->hdr.code) {

		case _PULSE_CODE_DISCONNECT:
			printf("\nServer received Detach pulse from ClientID:%d but rejected it ...\n", msg->client_id);
			break;

		case _PULSE_CODE_UNBLOCK:
			// REPLY blocked client wants to unblock (was hit by a signal
			// or timed out).  It's up to you if you reply now or later.
			printf("\nServer got _PULSE_CODE_UNBLOCK\n");
			break;

		case _PULSE_CODE_COIDDEATH: // from the kernel
			printf("\nServer got _PULSE_CODE_COIDDEATH\n");
			break;

		case _PULSE_CODE_THREADDEATH: // from the kernel
			printf("\nServer got _PULSE_CODE_THREADDEATH\n");
			break;

		default:
			// Some other pulse sent by one of your processes or the kernel
			printf("\nServer got some other pulse\n");
			break;
	}
}

void handleIOConnect(msg_data_t *msg, int rcvid) {

	// If the Global Name Service (gns) is running, name_open() sends a connect message. The server must EOK it.
	if (msg->hdr.type == _IO_CONNECT) {
		MsgReply(rcvid, EOK, NULL, 0);
		printf("\n gns service is running....");
	}

	// Some other I/O message was received; reject it
	if (msg->hdr.type > _IO_BASE && msg->hdr.type <= _IO_MAX) {
		MsgError(rcvid, ENOSYS);
		printf("\n Server received and IO message and rejected it....");
	}
}
