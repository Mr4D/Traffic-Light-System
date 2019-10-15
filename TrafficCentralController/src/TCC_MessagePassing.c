#include "TCC_MessagePassing.h"



void * critical_server_channel_thread(void * data){

	critical_server_thread_data_t * thread_data = (critical_server_thread_data_t*) data;
	rail_state_data_t * x1_data_state = thread_data->x1_state_data;

	name_attach_t * attach = createChannel(TCC_CRITICAL_CHANNEL);

	msg_data_t msg;
	int rcvid=0;
	int Stay_alive=1, living=0;	// server stays running (ignores _PULSE_CODE_DISCONNECT request)

	msg_reply_t replymsg; // replymsg structure for sending back to client
	replymsg.hdr.type = REPLY_MSG;
	replymsg.hdr.subtype = 0x00;

	msg_data_t msg_send;
	msg_send.client_id = TCC_CLIENT_ID;

	living =1;
	while (living)
	{
	// Do your MsgReceive's here now with the chid
	  rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

	  if (rcvid == -1)  // Error condition, exit
	  {
		  printf("\nFailed to MsgReceive\n");
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

				   if(msg.client_id == RCC_CLIENT_ID){
					   if(msg.hdr.type == RAIL_STATE_MSG){
						   printf("RAIL_STATE_MSG\n");
						   pthread_mutex_lock(&x1_data_state->mutex_state);
							   x1_data_state->current_state = msg.data.rail_state;
						   pthread_mutex_unlock(&x1_data_state->mutex_state);

						   msg_send.hdr.type = RAIL_STATE_MSG;
						   msg_send.data.rail_state = msg.data.rail_state;

						   send_message_thread_data_t i1_send = {
								   I1_CRITICAL_CHANNEL,
								   &msg_send
						   };
						   pthread_create(NULL,NULL,&sendMessage, &i1_send);

//						   send_message_thread_data_t i2_send = {
//								   I2_CRITICAL_CHANNEL,
//								   &msg_send
//						   };
//						   pthread_create(NULL,NULL,&sendMessage, &i2_send);

						   sprintf(replymsg.data.buf,"OK");
					   }
				   }


			   if(LOG){
				   sprintf(log_message, "\n    -----> replying with: '%s'\n",replymsg.data.buf);
				   writeTrafficLogFile(LOG_FILE_NAME,log_message);
			   }
			   MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));
		  	  }
		  }
		  else  {
		   printf("\nERROR: Server received something, but could not handle it correctly\n");
		  }


	}

	// Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
	ChannelDestroy(attach->chid);

}

