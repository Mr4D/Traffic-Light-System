#include "TCC_MessagePassing.h"

void *critical_client_channel_thread(void *data) {

	tcc_state_data_t *state_data = (tcc_state_data_t *)data;

	msg_data_t msg_rail_state;
	msg_reply_t reply;

	msg_rail_state.client_id = TCC_CLIENT_ID; // unique number for this client (optional)

	int server_coid_i1 = -1;

	while (server_coid_i1 == -1) {
		server_coid_i1 = connectChannel(I1_CRITICAL_CHANNEL_ATTACH_POINT);
	}

	printf("Connection established to: %s\n", I1_CRITICAL_CHANNEL);

	// We would have pre-defined data to stuff here
	msg_rail_state.hdr.type = 0x00;
	msg_rail_state.hdr.subtype = 0x00;

	while (1) {

		pthread_mutex_lock(&state_data->mutex_state);

		while (state_data->produce_data) {
			pthread_cond_wait(&state_data->cond_state_changed, &state_data->mutex_state);
		}

		msg_rail_state.state.traffic_state = state_data->x1_state;
		state_data->produce_data = true;

		pthread_cond_signal(&state_data->cond_state_changed);
		pthread_mutex_unlock(&state_data->mutex_state);

		printf("data consumed\n");
		printf("Client (ID:%d), sending data packet with the state value: %d \n", msg_rail_state.client_id, msg_rail_state.state.traffic_state);
		fflush(stdout);

		if (MsgSend(server_coid_i1, &msg_rail_state, sizeof(msg_rail_state), &reply, sizeof(reply)) == -1) {
			printf(" Error data '%d' NOT sent to server\n", msg_rail_state.state.traffic_state);
			// maybe we did not get a reply from the server
			break;
		} else { // now process the reply
			printf("   -->Reply is: '%s'\n", reply.buf);
		}
	}

	ConnectDetach(server_coid_i1);
}

//void * noncritical_client_channel_thread(void * data){
//
//}

void *critical_server_channel_thread(void *data) {

	tcc_state_data_t *tcc_state_data = (tcc_state_data_t *)data;

	printf("critical_server_channel()\n");
	name_attach_t *attach;

	// Create a local name (/dev/name/...)
	if ((attach = name_attach(NULL, TCC_CRITICAL_CHANNEL, 0)) == NULL) {
		printf("\nFailed to name_attach on ATTACH_POINT: %s \n", TCC_CRITICAL_CHANNEL);
		printf("\n Possibly another server with the same name is already running !\n");
		return EXIT_FAILURE;
	}

	printf("Server Listening for Clients on ATTACH_POINT: %s \n", TCC_CRITICAL_CHANNEL);

	msg_data_t msg;
	int rcvid = 0;
	int Stay_alive = 1, living = 0; // server stays running (ignores _PULSE_CODE_DISCONNECT request)

	msg_reply_t replymsg; // replymsg structure for sending back to client
	replymsg.hdr.type = 0x01;
	replymsg.hdr.subtype = 0x00;

	living = 1;
	while (living) { // Do your MsgReceive's here now with the chid

		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

		if (rcvid == -1) { // Error condition, exit
			printf("\nFailed to MsgReceive\n");
			break;
		}

		if (rcvid == 0) { // Received a pulse, work out what type
			handlePulse(&msg);
		}

		if (rcvid > 0) { // Received a message

			handleIOConnect(&msg, rcvid);

			// A message (presumably ours) received
			printf("Server received data packet with value of '%d' from client (ID:%d), ", msg.state.traffic_state, msg.client_id);
			fflush(stdout);

			// Save the message to memory. Convdar to lock.
			pthread_mutex_lock(&tcc_state_data->mutex_state);
			while (!tcc_state_data->produce_data) {
				pthread_cond_wait(&tcc_state_data->cond_state_changed, &tcc_state_data->mutex_state);
			}

			if (msg.client_id == I1_CLIENT_ID) {
				tcc_state_data->i1_state = msg.state.traffic_state;
			} else if (msg.client_id == I2_CLIENT_ID) {
				tcc_state_data->i2_state = msg.state.traffic_state;
			} else if (msg.client_id == RCC_CLIENT_ID) {
				tcc_state_data->x1_state = msg.state.rail_state;
			} else {
				printf("Message recieved from unknow client ID. Client ID: %d\n", msg.client_id);
			}

			// Condvar: signal and unlock mutex
			tcc_state_data->produce_data = false;
			pthread_cond_signal(&tcc_state_data->cond_state_changed);
			pthread_mutex_unlock(&tcc_state_data->mutex_state);
			printf("data produced\n");

			sprintf(replymsg.buf, "OK");
			printf("\n    -----> replying with: '%s'\n", replymsg.buf);
			MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));
		} else {
			printf("\nERROR: Server received something, but could not handle it correctly\n");
		}
	}

	ChannelDestroy(attach->chid); // Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
}
