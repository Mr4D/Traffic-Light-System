/*
 * stateMachine.c
 *
 *  Created on: 24Sep.,2019
 *      Author: aprab
 */

#include "stateMachine.h"

void initiliseTimer(int chid) {


	state_timer_event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);

	if(state_timer_event.sigev_coid != -1) {

		state_timer_event.sigev_notify = SIGEV_PULSE;

		struct sched_param th_param;
		pthread_getschedparam(pthread_self(), NULL, &th_param);	// Get my own thread parameters
		state_timer_event.sigev_priority = th_param.sched_curpriority;		// Set sig with same priority as myself

		state_timer_event.sigev_code = STATE_TIMER_EXPIRE_PULSE;


		// Create State Timer
		if (timer_create(CLOCK_REALTIME, &state_timer_event, &state_timer_id) == -1) {	// create the timer, binding it to the event
			if(LOG){
				sprintf( log_message, "Couldn't create State Timer, errno: %d\n", errno);
				writeTrafficLogFile(LOG_FILE_NAME, log_message);
			}
			exit(EXIT_FAILURE);
		}


		// Timer for the state transition of the traffic intersection
		// one-shot timer (default = 0s)
		itime_state_timer.it_value.tv_sec = 0;		// 0 second
		itime_state_timer.it_value.tv_nsec = 0;		// 0 nano seconds
		itime_state_timer.it_interval.tv_sec = 0;	// 0 second 		(reload)
		itime_state_timer.it_interval.tv_nsec = 0;	// 0 nano seconds 	(reload)




	}
	else {
		if(LOG){
			sprintf( log_message, "Couldn't ConnectAttach to, chid: %d\n", chid);
			writeTrafficLogFile(LOG_FILE_NAME, log_message);
		}

		exit(EXIT_FAILURE);
	}



	ped_timer_event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);

	if(ped_timer_event.sigev_coid != -1){

		ped_timer_event.sigev_notify = SIGEV_PULSE;

		struct sched_param th_param;
		pthread_getschedparam(pthread_self(), NULL, &th_param);	// Get my own thread parameters
		ped_timer_event.sigev_priority = th_param.sched_curpriority;		// Set sig with same priority as myself

		ped_timer_event.sigev_code = PED_TIMER_EXPIRE_PULSE;

		// Create Pedestrian Timer
		if (timer_create(CLOCK_REALTIME, &ped_timer_event, &ped_timer_id) == -1) {	// create the timer, binding it to the event
			if(LOG){
				sprintf( log_message, "Couldn't create Pedestrian Timer, errno: %d\n", errno);
				writeTrafficLogFile(LOG_FILE_NAME, log_message);
			}
			exit(EXIT_FAILURE);
		}


		// Timer for the pedestrian crossing at the traffic intersection
		// one-shot timer (default = 0s)
		itime_ped_timer.it_value.tv_sec = 0;		// 0 second
		itime_ped_timer.it_value.tv_nsec = 0;		// 0 nano seconds
		itime_ped_timer.it_interval.tv_sec = 0;		// 0 second 		(reload)
		itime_ped_timer.it_interval.tv_nsec = 0;	// 0 nano seconds 	(reload)
	}
	else {
		if(LOG){
			sprintf( log_message, "Couldn't ConnectAttach to, chid: %d\n", chid);
			writeTrafficLogFile(LOG_FILE_NAME, log_message);
		}

		exit(EXIT_FAILURE);
	}
}

traffic_state_t changeTrafficState_peak(traffic_state_t current_state, traffic_state_t previous_state, traffic_input_buffer_t * input_queue) {

	printf("changeTrafficState_peak\n");
	traffic_state_t next_state = INITIAL;

	switch(current_state) {

		case INITIAL:
			next_state = ALL_RED;
			break;

		case ALL_RED:
			if ((previous_state == INITIAL) || (previous_state == NS_R_Y)) {
				next_state = NS_S_G_SEN;
			}
			else if (previous_state == NS_S_Y) {
				next_state = EW_R_G;
			}
			else if (previous_state == EW_R_Y) {
				next_state = EW_S_G_SEN;
			}
			else if (previous_state == EW_S_Y) {
				next_state = NS_R_G;
			}
			break;

		// North south straight transitions
		case NS_S_G_SEN:
			if(containsTrafficInputBuf(input_queue->input_buffer, input_queue->end_index, NS_P)){
				next_state = NS_S_G_P_X;
			}
			else{
				next_state = NS_S_G;
			}
			break;

		case NS_S_G_P_X:
			next_state = NS_S_G_P_F;
			break;

		case NS_S_G_P_F:
			next_state = NS_S_G;
			break;

		case NS_S_G:
			next_state = NS_S_Y;
			break;

		case NS_S_Y:
			next_state = ALL_RED;
			break;

		// East west straight transitions
		case EW_S_G_SEN:
			if(containsTrafficInputBuf(input_queue->input_buffer, input_queue->end_index, EW_P)){
				next_state = EW_S_G_P_X;
			}
			else {
				next_state = EW_S_G;
			}
			break;

		case EW_S_G_P_X:
			next_state = EW_S_G_P_F;
			break;
		
		case EW_S_G_P_F:
			next_state = EW_S_G;
			break;

		case EW_S_G:
			next_state = EW_S_Y;
			break;
		
		case EW_S_Y:
			next_state = ALL_RED;
			break;
		
		case NS_R_G:
			next_state = NS_R_Y;
			break;

		case NS_R_Y:
			next_state = ALL_RED;
			break;

		// East west right transitions
		case EW_R_G:
			next_state = EW_R_Y;
			break;

		case EW_R_Y:
			next_state = ALL_RED;
			break;
		case FAULT:

			break;
		default:
			next_state = FAULT;
			break;
	}

	return next_state;
}

traffic_state_t changeTrafficState_offPeak(traffic_state_t current_state, traffic_state_t previous_state, traffic_input_buffer_t * input_queue) {


	traffic_state_t next_state = INITIAL;
	int input = readTrafficInputBuf(input_queue); // 0 == next input

	switch(current_state) {

		case INITIAL:
			next_state = ALL_RED;
			break;

		case ALL_RED:

			if (input == NS_STR || input == NS_P) {
				next_state = NS_S_G_P_X;
			}
			else if (input == NS_R) {
				next_state = NS_R_G;
			} 
			else if (input == EW_R) {
				next_state = EW_R_G;
			}
			else if (input == EW_STR || input == EW_P) {
				next_state = EW_S_G_SEN;
			}
			else {
				next_state = EW_S_G_SEN;
			}
			break;

		// NS Straight state transitions
		case NS_S_G_P_X:
			next_state = NS_S_G_P_F;
			break;

		case NS_S_G_P_F:
			next_state = NS_S_Y;
			break;

		case NS_S_Y:
			next_state = ALL_RED;
			break;

		// EW Right turn state transitions
		case EW_R_G:
			next_state = EW_R_Y;
			break;

		case EW_R_Y:
			next_state = ALL_RED;
			break;

		// NS Right turn transitions
		case NS_R_G:
			next_state = NS_R_Y;
			break;

		case NS_R_Y:
			next_state = ALL_RED;
			break;

		// EW Straight state transitions
		case EW_S_G_SEN:

			if (input == EW_P) {
				next_state = EW_S_G_P_X;
			} 
			else if ( input == NS_STR || input == NS_R || input == NS_P || input == EW_R ) {
				next_state = EW_S_Y;
			}
			else {
				next_state = EW_S_G_SEN;	// Go back when no sensors and timer done
			}

			break;

		case EW_S_G_P_X:
			next_state = EW_S_G_P_F;
			break;
		
		case EW_S_G_P_F:
			next_state = EW_S_G_SEN;
			break;
		
		case EW_S_Y:
			next_state = ALL_RED;
			break;
		
		case FAULT:

			break;

		// Get away from unused states in off-peak
		case NS_S_G_SEN:
				next_state = NS_S_Y;
			break;
		case NS_S_G:
				next_state = NS_S_Y;
			break;
		case EW_S_G:
				next_state = EW_S_Y;
			break;

		// Default state
		default:
			next_state = FAULT;
			break;
	}

	return next_state;
}


void updateTimersPeak(traffic_state_t current_state){

	switch(current_state) {

			case INITIAL:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_INITIAL;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case ALL_RED:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_ALL_RED;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case NS_S_G_SEN:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_STRAIGHT;
				itime_ped_timer.it_value.tv_sec = TIME_PED_TIME_OUT;

				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case NS_S_G_P_X:
				itime_ped_timer.it_value.tv_sec = TIME_PED_CROSS;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case NS_S_G_P_F:
				itime_ped_timer.it_value.tv_sec = TIME_PED_FLASHING;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case NS_S_G:
				// DO NOT SET TIMER
				break;

			case NS_S_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case EW_S_G_SEN:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_STRAIGHT;
				itime_ped_timer.it_value.tv_sec = TIME_PED_TIME_OUT;

				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case EW_S_G_P_X:
				itime_ped_timer.it_value.tv_sec = TIME_PED_CROSS;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case EW_S_G_P_F:
				itime_ped_timer.it_value.tv_sec = TIME_PED_FLASHING;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case EW_S_G:
				// DO NOT SET TIMER
				break;

			case EW_S_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case NS_R_G:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_TURN;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case NS_R_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case EW_R_G:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_TURN;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case EW_R_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;
			case FAULT:
				// DO NOT SET TIMER
				break;
			default:
				// DO NOT SET TIMER
				break;
		}

}

void updateTimersOffPeak(traffic_state_t current_state){

	switch(current_state) {

			case INITIAL:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_INITIAL;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case ALL_RED:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_ALL_RED;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case NS_S_G_SEN:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_STRAIGHT;

				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case NS_S_G_P_X:
				itime_ped_timer.it_value.tv_sec = TIME_PED_CROSS;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case NS_S_G_P_F:
				itime_ped_timer.it_value.tv_sec = TIME_PED_FLASHING;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case NS_S_G:
				// DO NOT SET TIMER
				break;

			case NS_S_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case EW_S_G_SEN:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_STRAIGHT;

				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case EW_S_G_P_X:
				itime_ped_timer.it_value.tv_sec = TIME_PED_CROSS;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case EW_S_G_P_F:
				itime_ped_timer.it_value.tv_sec = TIME_PED_FLASHING;
				timer_settime(ped_timer_id, 0, &itime_ped_timer, NULL);	// and start the timer!
				break;

			case EW_S_G:
				// DO NOT SET TIMER
				break;

			case EW_S_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case NS_R_G:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_TURN;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case NS_R_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case EW_R_G:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_TURN;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;

			case EW_R_Y:
				itime_state_timer.it_value.tv_sec = TIME_TRAFFIC_YELLOW;
				timer_settime(state_timer_id, 0, &itime_state_timer, NULL);	// and start the timer!
				break;
			case FAULT:
				// DO NOT SET TIMER
				break;
			default:
				// DO NOT SET TIMER
				break;
		}

}

bool decideChangeStatePeak(struct _pulse * change_state_msg, traffic_state_t current_state){

	bool change_state = false;

	if(change_state_msg->code == STATE_TIMER_EXPIRE_PULSE){
		writeTrafficLogFile(LOG_FILE_NAME, "STATE_TIMER_EXPIRE_PULSE\n");

		change_state = true;
	}
	else if(change_state_msg->code == PED_TIMER_EXPIRE_PULSE){
		writeTrafficLogFile(LOG_FILE_NAME, "PED_TIMER_EXPIRE_PULSE\n");

		change_state = true;
	}
	else if(change_state_msg->code == NS_P_INPUT_PULSE){
		writeTrafficLogFile(LOG_FILE_NAME, "INPUT_PULSE\n");

		struct itimerspec ped_itimerspec;
		timer_gettime( ped_timer_id, &ped_itimerspec);

		if(current_state == NS_S_G_SEN && ped_itimerspec.it_value.tv_sec > 1){
			change_state = true;
		}
	}
	else if(change_state_msg->code == EW_P_INPUT_PULSE){
		writeTrafficLogFile(LOG_FILE_NAME, "INPUT_PULSE\n");

		struct itimerspec ped_itimerspec;
		timer_gettime( ped_timer_id, &ped_itimerspec);

		if(current_state == EW_S_G_SEN && ped_itimerspec.it_value.tv_sec > 1){
			change_state = true;
		}
	}

	return change_state;
}

bool decideChangeStateOffPeak(struct _pulse * change_state_msg, traffic_state_t current_state, traffic_input_buffer_t * input_queue) {
	bool change_state = false;

	if ( change_state_msg->code == STATE_TIMER_EXPIRE_PULSE){
		writeTrafficLogFile(LOG_FILE_NAME, "STATE_TIMER_EXPIRE_PULSE\n");

		if(!(current_state == EW_S_G_SEN && input_queue->end_index == 0)){
			change_state = true;
		}
	}
	else if ( change_state_msg->code == PED_TIMER_EXPIRE_PULSE) {
		writeTrafficLogFile(LOG_FILE_NAME, "PED_TIMER_EXPIRE_PULSE\n");

		change_state = true;
	}
	else if( change_state_msg->code == NS_STR_INPUT_PULSE ||
			change_state_msg->code == NS_R_INPUT_PULSE ||
			change_state_msg->code == EW_R_INPUT_PULSE ||
			change_state_msg->code == NS_P_INPUT_PULSE ||
			change_state_msg->code == EW_P_INPUT_PULSE )	{

		writeTrafficLogFile(LOG_FILE_NAME, "INPUT_PULSE\n");

		struct itimerspec state_itimerspec;
		timer_gettime( state_timer_id, &state_itimerspec);

		if(current_state == EW_S_G_SEN && state_itimerspec.it_value.tv_sec == 0){
			change_state = true;
		}
	}

	return change_state;
}

void clearInputPeak(traffic_input_data_t * input_data, traffic_state_t state) {
	// clear input signal
	if(state == NS_S_G_P_X){
		deleteTrafficInputBufCell(input_data, NS_P);
	}
	else if (state == EW_S_G_P_X) {
		deleteTrafficInputBufCell(input_data, EW_P);
	}
}
void clearInputOffPeak(traffic_input_data_t * input_data, traffic_state_t state){
	if(state == NS_S_G_P_X){
		int cell_count = 2;
		int deletCells[] = {NS_STR,NS_P};

		deleteTrafficInputBufCells(input_data, deletCells, cell_count);
	}
	else if (state == EW_S_G_P_X) {
		int cell_count = 2;
		int deletCells[] = {EW_STR,EW_P};

		deleteTrafficInputBufCells(input_data, deletCells, cell_count);
	}
	if(state == NS_S_G_SEN || state == NS_S_G){
		deleteTrafficInputBufCell(input_data, NS_STR);
	}
	else if(state == EW_S_G_SEN || state == EW_S_G){
		deleteTrafficInputBufCell(input_data, EW_STR);
	}
	else if (state == NS_R_G) {
		deleteTrafficInputBufCell(input_data, NS_R);
	}
	else if (state == EW_R_G) {
		deleteTrafficInputBufCell(input_data, EW_R);
	}
}

void * trafficStateMachine(void * data) {

	printf("TrafficStateMachine()\n");

	struct _pulse change_state_msg;
	int change_state_channel_id;
	int rcvid;


	traffic_state_t next_state = INITIAL;
	traffic_state_t current_state = INITIAL;
	traffic_state_t previous_state = INITIAL;
	traffic_input_buffer_t input_queue;

	traffic_state_machine_thread_data_t * state_machine_thread_data = (traffic_state_machine_thread_data_t*) data;

	traffic_input_data_t * input_data = state_machine_thread_data->input_data;
	ic_state_data_t * ic_state_data = state_machine_thread_data->ic_state_data;
	traffic_mode_t mode = *state_machine_thread_data->mode;


	traffic_state_data_t * state_data = ic_state_data->traffic_state_data;
	//rail_state_data_t * rail_state_data = ic_state_data->rail_state_data;


	change_state_channel_id = state_machine_thread_data->change_state_channel_id; // Create a communications channel
	initiliseTimer(change_state_channel_id);



	while(1) {

		// get input flags
		pthread_rwlock_rdlock( &input_data->rwlock_input );
			memcpy(&input_queue, input_data->input_queue, sizeof(input_queue));
		pthread_rwlock_unlock( &input_data->rwlock_input );

		printInputBuf(input_queue.input_buffer, input_queue.end_index);
		// get next state
		if (mode == PEAK) {
			next_state = changeTrafficState_peak(current_state, previous_state, &input_queue);
		}
		else if (mode == OFF_PEAK) {
			next_state = changeTrafficState_offPeak(current_state, previous_state, &input_queue);
		}
		else {
			if(LOG){
				writeTrafficLogFile("Incorrect Mode, mode: %d\n", mode);
			}
		}



		// update previous state
		previous_state = current_state;
		current_state = next_state;

		// update current state
		pthread_mutex_lock(&state_data->mutex_state);
			state_data->current_state = current_state;

			state_data->data_ready_message = true;
			state_data->data_ready_output = true;
			pthread_cond_broadcast(&state_data->cond_state_changed);
		pthread_mutex_unlock(&state_data->mutex_state);


		if(mode == PEAK){
			updateTimersPeak(current_state);
		}
		else if(mode == OFF_PEAK){
			updateTimersOffPeak(current_state);
		}
		else {
			printf("Mode Error\n");
		}



		// Print state info
		stateToString(state_data->current_state, log_message);
		strcat(log_message, "\n");
		writeTrafficLogFile(LOG_FILE_NAME, log_message);
		sprintf(log_message, "Timer:  %ld\n", itime_state_timer.it_value.tv_sec);
		writeTrafficLogFile(LOG_FILE_NAME, log_message);



		// wait for timer
		bool change_state = false;
		while(!change_state){
			rcvid = MsgReceivePulse(change_state_channel_id, &change_state_msg, sizeof(change_state_msg), NULL);	// Wait for timer signal
			if(mode == PEAK){
				change_state = decideChangeStatePeak(&change_state_msg, current_state);
			}
			else if(mode == OFF_PEAK){
				pthread_rwlock_rdlock( &input_data->rwlock_input );
					memcpy(&input_queue, input_data->input_queue, sizeof(input_queue));
				pthread_rwlock_unlock( &input_data->rwlock_input );
				change_state = decideChangeStateOffPeak(&change_state_msg, current_state, &input_queue);
			}
		}


		if(mode == PEAK){
			clearInputPeak(input_data, current_state);
		}
		else if(mode == OFF_PEAK){
			clearInputOffPeak(input_data, current_state);
		}


		printf("\t\tCHANGE STATE\n");

	}

	return EXIT_SUCCESS;
}

void stateToString(traffic_state_t state, char * state_str) {

	switch(state) {
			case INITIAL:
				sprintf(state_str,"INITIAL");
				break;

			case ALL_RED:
				sprintf(state_str,"ALL_RED");
				break;

			case FAULT:
				sprintf(state_str,"FAULT");
				break;

			case NS_S_G_SEN:
				sprintf(state_str,"NS_S_G_SEN");
				break;

			case NS_S_G_P_X:
				sprintf(state_str,"NS_S_G_P_X");
				break;

			case NS_S_G_P_F:
				sprintf(state_str,"NS_S_G_P_F");
				break;

			case NS_S_G:
				sprintf(state_str,"NS_S_G");
				break;

			case NS_S_Y:
				sprintf(state_str,"NS_S_Y");
				break;

			case EW_R_G:
				sprintf(state_str,"EW_R_G");
				break;

			case EW_R_Y:
				sprintf(state_str,"EW_R_Y");
				break;

			case EW_S_G_SEN:
				sprintf(state_str,"EW_S_G_SEN");
				break;

			case EW_S_G_P_X:
				sprintf(state_str,"EW_S_G_P_X");
				break;

			case EW_S_G_P_F:
				sprintf(state_str,"EW_S_G_P_F");
				break;

			case EW_S_G:
				sprintf(state_str,"EW_S_G");
				break;

			case EW_S_Y:
				sprintf(state_str,"EW_S_Y");
				break;

			case NS_R_G:
				sprintf(state_str,"NS_R_G");
				break;

			case NS_R_Y:
				sprintf(state_str,"NS_R_Y");
				break;

			default:

				break;
		}
}
