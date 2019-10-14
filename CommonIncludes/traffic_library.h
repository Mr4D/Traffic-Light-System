#pragma once

#include "traffic_types.h"
#include "traffic_file_lib.h"

typedef struct {
	connection_id_data_t * coid_data;
	const char * attach_point;
	int max_attempts;
	int wait_time;
} connect_channel_thread_data_t;

void * connectChannelThread(void * data);


name_attach_t *createChannel(const char *attach_point);

int connectChannel(const char * attach_point);

void handlePulse(msg_data_t *msg);

void handleIOConnect(msg_data_t *msg, int rcvid);
