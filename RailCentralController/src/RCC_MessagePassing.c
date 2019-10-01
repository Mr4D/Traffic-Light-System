#include "RCC_MessagePassing.h"

void * critical_client_channel_thread(void * data){

	rcc_state_data_t * state_data = (rcc_state_data_t *) data;

    msg_data_t msg;
    msg_reply_t reply;

    msg.client_id = RCC_CLIENT_ID; // unique number for this client (optional)

	int server_coid;
	int index = 0;

	printf("  ---> Trying to connect to server named: %s\n", TCC_CRITICAL_CHANNEL_ATTACH_POINT);
	if ((server_coid = name_open(TCC_CRITICAL_CHANNEL_ATTACH_POINT, 0)) == -1)
	{
		printf("\n    ERROR, could not connect to server!\n\n");
		return EXIT_FAILURE;
	}

	printf("Connection established to: %s\n", TCC_CRITICAL_CHANNEL_ATTACH_POINT);

    // We would have pre-defined data to stuff here
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x00;

	while(1){
		if(state_data->current_state == TRAIN){
			state_data->current_state = NO_TRAIN;
		}
		else{
			state_data->current_state = TRAIN;
		}
//		pthread_mutex_lock(&state_data->mutex_state);
//
//		while (state_data->produce_data){
//			pthread_cond_wait(&state_data->cond_state_changed, &state_data->mutex_state);
//		}

		msg.state.rail_state = state_data->current_state;

//		state_data->produce_data = true;
//		pthread_cond_signal(&state_data->cond_state_changed);
//		pthread_mutex_unlock(&state_data->mutex_state);

		printf("data consumed\n");

		printf("Client (ID:%d), sending data packet with the state value: %d \n", msg.client_id, msg.state.rail_state);
		fflush(stdout);
		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {
			printf(" Error data '%d' NOT sent to server\n", msg.state.rail_state);
				// maybe we did not get a reply from the server
			break;
		}
		else { // now process the reply
			printf("   -->Reply is: '%s'\n", reply.buf);
		}

		sleep(5);

	}

	ConnectDetach(server_coid);
}

