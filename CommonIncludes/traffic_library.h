#pragma once

#include "traffic_types.h"

name_attach_t *createChannel(const char *attach_point);
int connectChannel(const char * attach_point);
void handlePulse(msg_data_t *msg);
void handleIOConnect(msg_data_t *msg, int rcvid);
