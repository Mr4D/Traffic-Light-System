/*
 * TrafficCentralController.h
 *
 *  Created on: 14Oct.,2019
 *      Author: NickM_e75acww
 */

#ifndef TRAFFICCENTRALCONTROLLER_H_
#define TRAFFICCENTRALCONTROLLER_H_

#include <traffic_file_lib.h>

// TCC
/* tcc state requirements*/
typedef struct {

	traffic_state_data_t * i1_state;
	traffic_state_data_t * i2_state;
	rail_state_data_t * x1_state;

} tcc_state_data_t;

typedef struct {
	tcc_state_data_t * tcc_state_data;
} output_thread_data_t;

typedef struct {

} critical_server_channel_thread_data_t;

typedef struct {

} critical_client_channel_thread_data_t;

typedef struct {

} non_critical_client_channel_thread_data_t;

#endif /* TRAFFICCENTRALCONTROLLER_H_ */
