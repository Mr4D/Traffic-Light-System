#include "input.h"


void * client_input_thread(void * data)
{

	tool_data_t * tools = (tool_data_t *) data;
	msg_data_t msg;
	msg_reply_t reply;


	char attach[80] = {0};
	char clientID[80];

	if (tools->aflag == 0)
	{
		sprintf(attach, I1_CRITICAL_CHANNEL_ATTACH_POINT);
		sprintf(clientID, I1_CLIENT_ID);
	}
	else if (tools->aflag == 1)
	{
		sprintf(attach,I2_CRITICAL_CHANNEL_ATTACH_POINT);
		sprintf(clientID, I2_CLIENT_ID);
	}

	msg.client_id = clientID ; // unique number for this client (optional)

	int server_coid = -1;
	while (server_coid == -1)
	{
		printf("  ---> Trying to connect to server named: %s\n", attach);
		if ((server_coid = name_open(attach, 0)) == -1)
		{
			printf("\n    ERROR, could not connect to server!\n\n");
			//return EXIT_FAILURE;
		}
		sleep(3);
	}

	printf("Connection established to: %s\n", attach);

	// We would have pre-defined data to stuff here
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x00;

	while(1)
	{
		//pthread_mutex_lock(&state_data->mutex_state);

		//while (!state_data->data_ready_message){
		//	pthread_cond_wait(&state_data->cond_state_changed, &state_data->mutex_state);
		//}

		//msg.state.traffic_state = state_data->current_state;

		//state_data->data_ready_message = false;
		//pthread_cond_signal(&state_data->cond_state_changed);
		//pthread_mutex_unlock(&state_data->mutex_state);

		printf("data consumed\n");

		//printf("Client (ID:%d), sending data packet with the state value: %d \n", msg.client_id, msg.state.traffic_state);
		fflush(stdout);
		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
		{
		//	printf(" Error data '%d' NOT sent to server\n", msg.state.traffic_state);
				// maybe we did not get a reply from the server
			break;
		}
		else
		{ // now process the reply
		//	printf("   -->Reply is: '%s'\n", reply.buf);
		}
	}


	ConnectDetach(server_coid);
	return NULL;
}
