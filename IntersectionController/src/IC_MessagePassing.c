#include "IC_MessagePassing.h"

void * critical_client_channel_thread(void * data){
    my_data msg;
    my_reply reply;

    msg.ClientID = 700; // unique number for this client (optional)

    int nd = 0;
    pid_t pid = 0;
    int chid = 0;
    int server_coid;
    int index = 0;

    if((nd = netmgr_strtond(TCC_SERVER,NULL)) == -1){
    	printf("netmgr_strtond() error\n");
    	pthread_exit(0);
    }
    readServerInfo(TCC_CRITICAL_SERVER_INFO, &pid, &chid);

    printf("  ---> Trying to connect to server ProcessID: %d, Channel ID: %d\n", pid, chid);
    if ((server_coid = ConnectAttach(nd, pid, chid, 0, 0)) == -1)
    {
        printf("\n    ERROR, could not connect to server!\n\n");
        return EXIT_FAILURE;
    }
    printf("Connection established:\n");
    printf("\tNode: %d\n", nd);
    printf("\tpid : %d\n", pid);
    printf("\tchid: %d\n\n", chid);

    // We would have pre-defined data to stuff here
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x00;

	// Do whatever work you wanted with server connection
	for (index=0; index < 5; index++) // send data packets
	{
		// set up data packet
		msg.data=10+index;

		// the data we are sending is in msg.data
		printf("Client (ID:%d), sending data packet with the integer value: %d \n", msg.ClientID, msg.data);
		fflush(stdout);

		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
		{
			printf(" Error data '%d' NOT sent to server\n", msg.data);
				// maybe we did not get a reply from the server
			break;
		}
		else
		{ // now process the reply
			printf("   -->Reply is: '%s'\n", reply.buf);
		}

		sleep(5);	// wait a few seconds before sending the next data packet
	}

	// Close the connection
	printf("\n Sending message to server to tell it to close the connection\n");
	ConnectDetach(server_coid);
}
void * critical_server_channel_thread(void * data){

}
void * noncritical_server_channel_thread(void * data){

}

void writeServerInfo(const char * filename, pid_t pid, int chid){

	FILE * serverInfoFile;
	serverInfoFile = fopen( filename, "w+");

	char s_pid[20]; // 10 digits plus newline and null = 12 digits, choose 20 to be safe;
	char s_chid[20]; // 10 digits plus newline and null = 12 digits, choose 20 to be safe;

	sprintf(s_pid, "%d\n", pid);
	sprintf(s_chid, "%d\n", chid);

	fputs( s_pid, serverInfoFile );
	fputs( s_chid, serverInfoFile );

	fclose(serverInfoFile);
}

void readServerInfo(const char * filename, pid_t * pid, int * chid){
	FILE * serverInfoFile;
	serverInfoFile = fopen( filename, "r");

	char s_pid[20]; // 10 digits plus newline and null = 12 digits, choose 20 to be safe;
	char s_chid[20]; // 10 digits plus newline and null = 12 digits, choose 20 to be safe;

	fgets( s_pid, 20, serverInfoFile );
	fgets( s_chid, 20, serverInfoFile );

	*pid = atoi(s_pid);
	*chid = atoi(s_chid);

	fclose(serverInfoFile);
}
