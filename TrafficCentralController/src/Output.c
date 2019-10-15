#include "Output.h"

void *t_output(void *data) {

	output_thread_data_t *thread_data = (output_thread_data_t *)data;

	traffic_state_t i1_state;
	traffic_state_t i2_state;
	rail_state_t x1_state;

	while (1) {
		// pthread_mutex_lock(&tcc_state_data->mutex_state);

		// while (tcc_state_data->produce_data) {
		// 	pthread_cond_wait(&tcc_state_data->cond_state_changed, &tcc_state_data->mutex_state);
		// }

		// i1_state = tcc_state_data->i1_state;
		// i2_state = tcc_state_data->i2_state;
		// x1_state = tcc_state_data->x1_state;

		// tcc_state_data->produce_data = true;
		// pthread_cond_signal(&tcc_state_data->cond_state_changed);
		// pthread_mutex_unlock(&tcc_state_data->mutex_state);

		printf("State\n\tI1: %d\n\tI2: %d\n\tX1: %d\n\n", i1_state, i2_state, x1_state);
	}
}
