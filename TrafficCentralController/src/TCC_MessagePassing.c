#include "TCC_MessagePassing.h"

void * critical_client_channel_thread(void * data){

}
void * noncritical_client_channel_thread(void * data){

}
void * critical_server_channel_thread(void * data){

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
	int Stay_alive=0, living=0;	// server stays running (ignores _PULSE_CODE_DISCONNECT request)

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
	   sprintf(replymsg.buf,"OK");
	   printf("Server received data packet with value of '%d' from client (ID:%d), ", msg.state.traffic_state, msg.client_id);
		   fflush(stdout);
		   sleep(1); // Delay the reply by a second (just for demonstration purposes)

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

