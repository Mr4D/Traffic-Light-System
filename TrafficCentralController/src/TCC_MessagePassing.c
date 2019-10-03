#include "TCC_MessagePassing.h"

void * critical_client_channel_thread(void * data){
	traffic_state_data_t * state_data = (traffic_state_data_t *) data;

	msg_data_t msg_rail_state;
	msg_reply_t reply_i1;
	msg_reply_t reply_i2;

	msg_rail_state.client_id = TCC_CLIENT_ID; // unique number for this client (optional)

	int server_coid_i1 = -1;
	int server_coid_i2;

	while (server_coid_i1 == -1) {
		printf("  ---> Trying to connect to server named: %s\n", I1_CRITICAL_CHANNEL_ATTACH_POINT);
		if ((server_coid_i1 = name_open(I1_CRITICAL_CHANNEL_ATTACH_POINT, 0)) == -1) {
			printf("\n    ERROR, could not connect to server!\n\n");
	//		return EXIT_FAILURE;

		}
		sleep(3);
	}




	//	Edit for I2 when node running
//	printf("  ---> Trying to connect to server named: %s\n", TCC_CRITICAL_CHANNEL_ATTACH_POINT);
//	if ((server_coid_i2 = name_open(TCC_CRITICAL_CHANNEL_ATTACH_POINT, 0)) == -1) {
//		printf("\n    ERROR, could not connect to server!\n\n");
//		return EXIT_FAILURE;
//	}

	printf("Connection established to: %s\n", I1_CRITICAL_CHANNEL);

	// We would have pre-defined data to stuff here
	msg_rail_state.hdr.type = 0x00;
	msg_rail_state.hdr.subtype = 0x00;

	while(1){
		pthread_mutex_lock(&state_data->mutex_state);

		while (state_data->produce_data){
			pthread_cond_wait(&state_data->cond_state_changed, &state_data->mutex_state);
		}

		msg_rail_state.state.traffic_state = state_data->current_state;

		state_data->produce_data = true;
		pthread_cond_signal(&state_data->cond_state_changed);
		pthread_mutex_unlock(&state_data->mutex_state);

		printf("data consumed\n");

		printf("Client (ID:%d), sending data packet with the state value: %d \n", msg_rail_state.client_id, msg_rail_state.state.traffic_state);
		fflush(stdout);


		if (MsgSend(server_coid_i1, &msg_rail_state, sizeof(msg_rail_state), &reply_i1, sizeof(reply_i1)) == -1) {
			printf(" Error data '%d' NOT sent to server\n", msg_rail_state.state.traffic_state);
				// maybe we did not get a reply from the server
			break;
		}
		else { // now process the reply
			printf("   -->Reply is: '%s'\n", reply_i1.buf);
		}

		// For I2
//		if (MsgSend(server_coid_i2, &msg_rail_state, sizeof(msg_rail_state), &reply_i2, sizeof(reply_i2)) == -1) {
//			printf(" Error data '%d' NOT sent to server\n", msg_rail_state.state.traffic_state);
//				// maybe we did not get a reply from the server
//			break;
//		}
//		else { // now process the reply
//			printf("   -->Reply is: '%s'\n", reply_i2.buf);
//		}


	}

	ConnectDetach(server_coid_i1);
	ConnectDetach(server_coid_i2);
}





//void * noncritical_client_channel_thread(void * data){
//
//}


void * critical_server_channel_thread(void * data){

	tcc_state_data_t * tcc_state_data = (tcc_state_data_t*) data;

	printf("critical_server_channel()\n");
	name_attach_t * attach;

	// Create a local name (/dev/name/...)
	if ((attach = name_attach(NULL, TCC_CRITICAL_CHANNEL, 0)) == NULL)
	{
	   printf("\nFailed to name_attach on ATTACH_POINT: %s \n", TCC_CRITICAL_CHANNEL);
	   printf("\n Possibly another server with the same name is already running !\n");
	   return EXIT_FAILURE;
	}

	printf("Server Listening for Clients on ATTACH_POINT: %s \n", TCC_CRITICAL_CHANNEL);



	msg_data_t msg;
	int rcvid=0;
	int Stay_alive=1, living=0;	// server stays running (ignores _PULSE_CODE_DISCONNECT request)

	msg_reply_t replymsg; // replymsg structure for sending back to client
	replymsg.hdr.type = 0x01;
	replymsg.hdr.subtype = 0x00;

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
		  switch (msg.hdr.code)
		  {
		   case _PULSE_CODE_DISCONNECT:
				// A client disconnected all its connections by running
				// name_close() for each name_open()  or terminated
			   if( Stay_alive == 0)
			   {
				   ConnectDetach(msg.hdr.scoid);
				   printf("\nServer was told to Detach from ClientID:%d ...\n", msg.client_id);
				   living = 0; // kill while loop
				   continue;
			   }
			   else
			   {
				   printf("\nServer received Detach pulse from ClientID:%d but rejected it ...\n", msg.client_id);
			   }
			   break;

		   case _PULSE_CODE_UNBLOCK:
				// REPLY blocked client wants to unblock (was hit by a signal
				// or timed out).  It's up to you if you reply now or later.
			   printf("\nServer got _PULSE_CODE_UNBLOCK\n");
			   break;

		   case _PULSE_CODE_COIDDEATH:  // from the kernel
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
		  continue;// go back to top of while loop
	  }

	  // for messages:
	  if(rcvid > 0) // if true then A message was received
	  {

	   // If the Global Name Service (gns) is running, name_open() sends a connect message. The server must EOK it.
	   if (msg.hdr.type == _IO_CONNECT )
	   {
		   MsgReply( rcvid, EOK, NULL, 0 );
		   printf("\n gns service is running....");
		   continue;	// go back to top of while loop
	   }

	   // Some other I/O message was received; reject it
	   if (msg.hdr.type > _IO_BASE && msg.hdr.type <= _IO_MAX )
	   {
		   MsgError( rcvid, ENOSYS );
		   printf("\n Server received and IO message and rejected it....");
		   continue;	// go back to top of while loop
	   }

	   // A message (presumably ours) received

	   // put your message handling code here and assemble a reply message

	   printf("Server received data packet with value of '%d' from client (ID:%d), ", msg.state.traffic_state, msg.client_id);
		   fflush(stdout);
		   //sleep(1); // Delay the reply by a second (just for demonstration purposes)

		pthread_mutex_lock(&tcc_state_data->mutex_state);

		while (!tcc_state_data->produce_data){
		pthread_cond_wait(&tcc_state_data->cond_state_changed, &tcc_state_data->mutex_state);
		}

	   if(msg.client_id == I1_CLIENT_ID){

		   tcc_state_data->i1_state = msg.state.traffic_state;
	   }

	   else if(msg.client_id == I2_CLIENT_ID){

		   tcc_state_data->i2_state = msg.state.traffic_state;
	   }

	   else if(msg.client_id == RCC_CLIENT_ID){

		   tcc_state_data->x1_state = msg.state.rail_state;
	   }

	   tcc_state_data->produce_data = false;
	   pthread_cond_signal(&tcc_state_data->cond_state_changed);
	   pthread_mutex_unlock(&tcc_state_data->mutex_state);
	   printf("data produced\n");

	   sprintf(replymsg.buf,"OK");
	   printf("\n    -----> replying with: '%s'\n",replymsg.buf);
	   MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));
	  }
	  else
	  {
	   printf("\nERROR: Server received something, but could not handle it correctly\n");
	  }

	}

	// Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
	ChannelDestroy(attach->chid);

}

