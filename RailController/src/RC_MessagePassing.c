#include "RC_MessagePassing.h"

void *critical_client_channel_thread(void *data) {

	critial_client_thread_data_t *thread_data = (critial_client_thread_data_t *)data;
	rail_state_data_t *state_data = thread_data->rail_state_data;

	msg_data_t msg;
	msg_reply_t reply;

	int server_coid = -1;

	while (server_coid == -1) {
		printf("  ---> Trying to connect to server named: %s\n", RCC_CRITICAL_CHANNEL_ATTACH_POINT);
		if ((server_coid = name_open(RCC_CRITICAL_CHANNEL_ATTACH_POINT, 0)) == -1) {
			printf("\n    ERROR, could not connect to server!\n\n");
			// return EXIT_FAILURE;
		}
		sleep(3);
	}

	if (LOG) {
		sprintf(log_message, "Connection established to: %s\n", RCC_CRITICAL_CHANNEL_ATTACH_POINT);
		writeTrafficLogFile(LOG_FILE_NAME, log_message);
	}

	// We would have pre-defined data to stuff here
	msg.hdr.type = RAIL_STATE_MSG;
	msg.hdr.subtype = 0x00;
	msg.client_id = X1_CLIENT_ID;

	while (1) {
		pthread_mutex_lock(&state_data->mutex_state);

		while (!state_data->data_ready) {
			pthread_cond_wait(&state_data->cond_state_changed, &state_data->mutex_state);
		}

		msg.data.rail_state = state_data->current_state;
		state_data->data_ready = false;

		pthread_cond_signal(&state_data->cond_state_changed);
		pthread_mutex_unlock(&state_data->mutex_state);

		if (LOG) {
			sprintf(log_message, "Client (ID:%d), sending data packet with the state value: %d \n", msg.client_id, msg.data.rail_state);
			writeTrafficLogFile(LOG_FILE_NAME, log_message);
		}

		printf("Client (ID:%d), sending data packet with the state value: %d \n", msg.client_id, msg.data.rail_state);
		fflush(stdout);
		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {
			printf(" Error data '%d' NOT sent to server\n", msg.data.rail_state);
			// maybe we did not get a reply from the server
			break;
		} else { // now process the reply
			printf("   -->Reply is: '%s'\n", reply.data.buf);
		}
	}

	ConnectDetach(server_coid);
}

void *critical_server_channel_thread(void *data) {

	critial_server_thread_data_t *thread_data = (critial_server_thread_data_t*) data;

	fault_data_t *fault_data = thread_data->fault_data;
	reset_data_t *reset_data = thread_data->reset_data;
	change_state_flags_t * change_state_flags = thread_data->change_state_flags;
	name_attach_t *attach;

	// Create a local name (/dev/name/...)
	if ((attach = createChannel(RC_CRITICAL_CHANNEL)) == NULL) {
		printf("couldn't create channel\n");
		pthread_exit(NULL);
	}
	else {
		thread_data->attach = attach;
		printf("channel: %s\n", RC_CRITICAL_CHANNEL);
	}

	msg_data_t msg;
	int rcvid = 0;
	int Stay_alive = 1, living = 0; // server stays running (ignores _PULSE_CODE_DISCONNECT request)

	msg_reply_t replymsg; // replymsg structure for sending back to client
	replymsg.hdr.type = REPLY_MSG;
	replymsg.hdr.subtype = 0x00;

	living = 1;

	bool reset = false;
	while (!reset) {
		// Do your MsgReceive's here now with the chid
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		printf("message received\n");

		if (rcvid == -1) { // Error condition, exit
			if(LOG){
				writeTrafficLogFile(LOG_FILE_NAME,"\nFailed to MsgReceive\n");
			}
			break;
		}

		// did we receive a Pulse or message?
		// for Pulses:
		if (rcvid == 0) { //  Pulse received, work out what type
			handlePulse(&msg);
			continue;// go back to top of while loop
		}

		// for messages:
		if(rcvid > 0) { // if true then A message was received


			if(msg.hdr.type >= _IO_BASE && msg.hdr.type <= _IO_MAX ){
				handleIOConnect(&msg, rcvid);
			}
			else {
				// A message (presumably ours) received

				// put your message handling code here and assemble a reply message

				if(LOG){
					sprintf(log_message, "Server received data packet with value of '%d' from client (ID:%d), ", msg.data.rail_state, msg.client_id);
					writeTrafficLogFile(LOG_FILE_NAME,log_message);
				}
				fflush(stdout);
				//sleep(1); // Delay the reply by a second (just for demonstration purposes)

				if(msg.hdr.type == FAULT_MSG){
					
					printf("FAULT_MSG\n");

					pthread_sleepon_lock();
						change_state_flags->fault = true;
						pthread_sleepon_signal(change_state_flags);
					pthread_sleepon_unlock();

					sprintf(replymsg.data.buf,"OK");
				}
				else if(msg.hdr.type == RESET_MSG){
					
					printf("RESET_MSG\n");

					if(msg.data.reset){

						pthread_sleepon_lock();
							change_state_flags->reset = true;
							pthread_sleepon_signal(change_state_flags);
						pthread_sleepon_unlock();
					}
					sprintf(replymsg.data.buf,"OK");
				}
				else {
					printf("msg.type: %d\n", msg.hdr.type);
					printf("message type not defined\n");
				}

				if(LOG){
					sprintf(log_message, "\n    -----> replying with: '%d'\n",replymsg.data.traffic_state);
					writeTrafficLogFile(LOG_FILE_NAME,log_message);
				}

				MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));
			}

		}
		else {
			writeTrafficLogFile(LOG_FILE_NAME, "\nERROR: Server received something, but could not handle it correctly\n");
		}
	}
}
