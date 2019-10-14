#include "IC_MessagePassing.h"

void * critical_client_send_thread(void * data){

	critical_client_send_data_t * thread_data = (critical_client_send_data_t *) data;

	int coid;
	traffic_state_t current_state;

	pthread_mutex_lock(&thread_data->mutex);
		coid = thread_data->coid;
		current_state = thread_data->current_state;
	pthread_mutex_unlock(&thread_data->mutex);

    msg_data_t msg;
    msg_reply_t reply;

    msg.client_id = I1_CLIENT_ID; // unique number for this client (optional)

	int server_coid = -1;


    // We would have pre-defined data to stuff here
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x00;


	msg.state.traffic_state = current_state;

	if(LOG) {
		sprintf(log_message,"Client (ID:%d), sending data packet with the state value: %d \n", msg.client_id, msg.state.traffic_state);
		writeTrafficLogFile( LOG_FILE_NAME, log_message);
	}
	fflush(stdout);

	if (MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {

		if(LOG) {
			sprintf(log_message," Error data '%d' NOT sent to server\n", msg.state.traffic_state);
			writeTrafficLogFile( LOG_FILE_NAME, log_message);
		}
			// maybe we did not get a reply from the server
	}
	else { // now process the reply
		if(LOG) {
			sprintf(log_message,"   -->Reply is: '%s'\n", reply.buf);
			writeTrafficLogFile( LOG_FILE_NAME, log_message);
		}
	}



	return EXIT_SUCCESS;
}

void * critical_server_channel_thread(void * data) {

	critical_server_data_t * thread_data = (critical_server_data_t *) data;
	rail_state_data_t * state_data = thread_data->rail_state_data;

	name_attach_t * attach;

	// Create a local name (/dev/name/...)
	if ((attach = createChannel(I1_CRITICAL_CHANNEL)) == NULL) {
		pthread_exit(NULL);
	}


	msg_data_t msg;
	int rcvid=0;
	int Stay_alive=1, living=0;	// server stays running (ignores _PULSE_CODE_DISCONNECT request)

	msg_reply_t replymsg; // replymsg structure for sending back to client
	replymsg.hdr.type = 0x01;
	replymsg.hdr.subtype = 0x00;

	living =1;
	while (living) {
	// Do your MsgReceive's here now with the chid
	  rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

	  if (rcvid == -1)  // Error condition, exit
	  {
		  if(LOG){
			  writeTrafficLogFile(LOG_FILE_NAME,"\nFailed to MsgReceive\n");
		  }
		  break;
	  }

	  // did we receive a Pulse or message?
	  // for Pulses:
	  if (rcvid == 0)  //  Pulse received, work out what type
	  {
		  handlePulse(&msg);
		  continue;// go back to top of while loop
	  }

	  // for messages:
	  if(rcvid > 0) // if true then A message was received
	  {

		  handleIOConnect(&msg, rcvid);

		   // A message (presumably ours) received

		   // put your message handling code here and assemble a reply message

		  if(LOG){
			  sprintf(log_message, "Server received data packet with value of '%d' from client (ID:%d), ", msg.state.rail_state, msg.client_id);
			  writeTrafficLogFile(LOG_FILE_NAME,log_message);
		  }
		   fflush(stdout);
			   //sleep(1); // Delay the reply by a second (just for demonstration purposes)

			pthread_mutex_lock(&state_data->mutex_state);
				state_data->current_state = msg.state.rail_state;
			pthread_mutex_unlock(&state_data->mutex_state);


		   sprintf(replymsg.buf,"OK");

		   if(LOG){
			   sprintf(log_message, "\n    -----> replying with: '%s'\n",replymsg.buf);
			   writeTrafficLogFile(LOG_FILE_NAME,log_message);
		   }
		   MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));


	  }
	  else
	  {
		  writeTrafficLogFile(LOG_FILE_NAME, "\nERROR: Server received something, but could not handle it correctly\n");
	  }

	}

	// Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
	ChannelDestroy(attach->chid);
}
void * noncritical_server_channel_thread(void * data){

}

