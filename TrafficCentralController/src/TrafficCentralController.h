/*
 * TrafficCentralController.h
 *
 *  Created on: 14Oct.,2019
 *      Author: NickM_e75acww
 */

#ifndef TRAFFICCENTRALCONTROLLER_H_
#define TRAFFICCENTRALCONTROLLER_H_

#include <traffic_file_lib.h>
#include <traffic_library.h>
#include <traffic_types.h>

// TCC
/* tcc state requirements*/
typedef struct {

	traffic_state_data_t *i1_state_data;
	traffic_state_data_t *i2_state_data;
	rail_state_data_t *x1_state_data;

} tcc_state_data_t;

typedef struct {
	tcc_state_data_t *tcc_state_data;
} output_thread_data_t;

typedef struct {
	rail_state_data_t *x1_state_data;
} critical_server_thread_data_t;

#endif /* TRAFFICCENTRALCONTROLLER_H_ */
