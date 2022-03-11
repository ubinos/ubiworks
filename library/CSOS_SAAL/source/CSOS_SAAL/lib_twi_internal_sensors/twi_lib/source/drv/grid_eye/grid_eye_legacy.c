///*
//* grid_eye_legacy.c
//*
//*  Created on: 2021. 7. 23.
//*      Author: sayyo
//*/
//
//#include "twi_sensor_module.h"
//
//#include <nrf_drv_twi.h>
//#include "sh_bluetooth/ble_stack.h"
//#include "sh_bluetooth/LAP_main.h"
//#include "sh_bluetooth/LAP_api.h"
//
//#include "twi_include.h"
//
//#include "../hw_config.h"
//#include "../sw_config.h"
//
//#include "dk_io/button.h"
//#include "dk_io/led.h"
//
//#include "stdlib.h"
//#include "sh_uart_433_module/sh_uart_433_module.h"
//#include "sh_uart_wifi_module/sh_uart_wifi_task.h"
//#include "assert.h"
//
//#include "nordic_common.h"
//
//
//
////////////////////////////////////////////////
///////////////					 /////////////
///////////////    GRID_EYE_API    /////////////
///////////////                    /////////////
////////////////////////////////////////////////
//
//static uint32_t sh_grid_eye_store_data(uint8_t frame, int data_arr[GRID_EYE_PIXEL_ARRAY_SIZE], int store_arr[GRID_EYE_FRAME_SIZE][2][8]) {
//	ret_code_t err = NRF_SUCCESS;
//	float tmp_row_data;
//	float tmp_col_data[8] = {0.0, };
//	int index = 0;
//	/*
//	 * get Sum of 8 x 8 array matrix's data line by line
//	 *
//	 * 		@	@	@	@	@	@	@	@	--> [r1]
//	 *
//	 * 		@	@	@	@	@	@	@	@	--> [r2]
//	 *
//	 * 		@	@	@	@	@	@	@	@	--> [r3]
//	 *
//	 * 		@	@	@	@	@	@	@	@	--> [r4]
//	 *
//	 * 		@	@	@	@	@	@	@	@	--> [r5]
//	 *
//	 * 		@	@	@	@	@	@	@	@	--> [r6]
//	 *
//	 * 		@	@	@	@	@	@	@	@	--> [r7]
//	 *
//	 * 		@	@	@	@	@	@	@	@	--> [r8]
//	 *
//	 * 		||	||	||	||	||	||	||	||
//	 *
//	 * 		c0	c1	c2	c3	c4	c5	c6	c7
//	 *
//	 * 		This is data of one frame. Queue takes 10 frames sorted by time.
//	 * 		Each time when do "enqueue", queue->rear act as "frame",
//	 * 		and data stored on "store_arr" (actually, grid_eye_data_buf_raw and grid_eye_data_buf_diff)
//	 */
//
//	for(int i=1; i<=GRID_EYE_PIXEL_ARRAY_SIZE; i++) {
//		tmp_row_data += data_arr[i-1];
//
//		switch(i%8) {
//		case 0:
//			store_arr[frame][GRID_EYE_ROW][index] = tmp_row_data;
//			tmp_row_data = 0;
//			index++;
//			tmp_col_data[7] += data_arr[i-1];
//			break;
//		case 1:
//			tmp_col_data[0] += data_arr[i-1];
//			break;
//		case 2:
//			tmp_col_data[1] += data_arr[i-1];
//			break;
//		case 3:
//			tmp_col_data[2] += data_arr[i-1];
//			break;
//		case 4:
//			tmp_col_data[3] += data_arr[i-1];
//			break;
//		case 5:
//			tmp_col_data[4] += data_arr[i-1];
//			break;
//		case 6:
//			tmp_col_data[5] += data_arr[i-1];
//			break;
//		case 7:
//			tmp_col_data[6] += data_arr[i-1];
//			break;
//		}
//	}
//	for(int i=0; i<8; i++) {
//		store_arr[frame][GRID_EYE_COL][i] = tmp_col_data[i];
//	}
//	return err;
//}
//
///////////////////////////////////
//////////// QUEUE CALCS //////////
///////////////////////////////////
//
//static void grid_eye_queue_init(grid_eye_queue * queue) {
//	queue->front = 0;
//	queue->rear = 0;
//}
//static bool is_grid_eye_queue_empty(grid_eye_queue * queue) {
//	if(queue->front == queue->rear)
//		return true;
//	else
//		return false;
//}
//static bool is_grid_eye_queue_full(grid_eye_queue * queue) {
//	if( ( (queue->rear+1) % GRID_EYE_FRAME_SIZE) == queue->front) {
//		return true;
//	} else
//		return false;
//}
//static int grid_eye_queue_enqueue(grid_eye_queue * queue, int * buf, int store_arr[GRID_EYE_FRAME_SIZE][2][8]) {
//	if(is_grid_eye_queue_full(queue)) {
//		return -1;
//	} else {
//		queue->rear = (queue->rear+1) % GRID_EYE_FRAME_SIZE;
////		sh_grid_eye_store_data(queue->rear, grid_eye_pixel_buf_first, grid_eye_data_raw);
//		sh_grid_eye_store_data(queue->rear, buf, store_arr);
//		return 0;
//	}
//}
//static int grid_eye_queue_dequeue(grid_eye_queue * queue) {
//	if(is_grid_eye_queue_empty(queue)) {
//		return -1;
//	} else {
//		queue->front = (queue->front+1) % GRID_EYE_FRAME_SIZE;
//		return 0;
//	}
//}
//static int grid_eye_queue_update(grid_eye_queue * queue, int * buf, int store_arr[GRID_EYE_FRAME_SIZE][2][8]) {
//	ret_code_t err = NRF_SUCCESS;
//	if( is_grid_eye_queue_full(queue) ) {
//		err = grid_eye_queue_dequeue(queue);
//		err = grid_eye_queue_enqueue(queue, buf, store_arr);
//		return 0;
//	} else if ( !is_grid_eye_queue_full(queue) ){
//		printf("grid eye loading...\r\n");
//		err = grid_eye_queue_enqueue(queue, buf, store_arr);
//		return 0;
//	}
//
//	return err;
//}
//static void grid_eye_queue_print(grid_eye_queue * queue) {
//	int r_i = queue->front;
//	int c_i = queue->front;
//	int frame_cnt = 0;
//	if( is_grid_eye_queue_empty(queue) ) {
//		printf("Data initializing...\r\n");
//		return;
//	}
//
//	//print ROW
//	printf("ROW\r\n");
//	do {
//		r_i = (r_i+1) % GRID_EYE_FRAME_SIZE;
//		printf("%d\t", frame_cnt);
//		for(int i=0; i<8; i++) {
//			printf("%d\t", grid_eye_data_raw[r_i][GRID_EYE_ROW][i]);
//		}
//		printf("\r\n");
//		frame_cnt++;
//	} while(r_i != queue->front);
//	//print COL
//	frame_cnt = 0;
//	printf("COL\r\n");
//	do {
//		c_i = (c_i+1) % GRID_EYE_FRAME_SIZE;
//		printf("%d\t", frame_cnt);
//		for(int i=0; i<8; i++) {
//			printf("%d\t", grid_eye_data_raw[c_i][GRID_EYE_COL][i]);
//		}
//		printf("\r\n");
//		frame_cnt++;
//	} while(c_i != queue->front);
//	printf("\r\n\r\n");
//}
//
//
//static float get_float_abs(float num) {
//	if(num >= 0) {
//		return num;
//	} else {
//		return -num;
//	}
//}
//
//static void grid_eye_queue_print_with_mean_value(grid_eye_queue * queue_raw, grid_eye_queue * queue_diff) {
//	int r_i = queue_raw->front;
//	int c_i = queue_raw->front;
//	int frame_cnt = 0;
//	if( is_grid_eye_queue_empty(queue_raw) ) {
//		printf("Data initializing...\r\n");
//		return;
//	}
//
//	// get mean value of ROW
//	int mean_row_raw = 0;
//	do {
//		r_i = (r_i+1) % GRID_EYE_FRAME_SIZE;
//		for (int i = 0; i < 8; i++) {
//			mean_row_raw += grid_eye_data_raw[r_i][GRID_EYE_ROW][i];
//		}
//	} while(r_i != queue_raw->front);
//	mean_row_raw /= 8 * GRID_EYE_FRAME_SIZE;
//	r_i = queue_raw->front;
//
//	int mean_row_diff = 0;
//	do {
//		r_i = (r_i + 1) % GRID_EYE_FRAME_SIZE;
//		for (int i = 0; i < 8; i++) {
//			mean_row_diff += grid_eye_data_diff[r_i][GRID_EYE_ROW][i];
//		}
//	} while (r_i != queue_raw->front);
//	mean_row_diff /= 8 * GRID_EYE_FRAME_SIZE;
//	r_i = queue_raw->front;
//
//
//	//print ROW bigger than mean value
//	printf("ROW\r\n");
//	do {
//		r_i = (r_i+1) % GRID_EYE_FRAME_SIZE;
//		printf("%d\t", frame_cnt);
//		for(int i=7; i>0; i--) {
////			mutex_lock(_g_mutex_grid_print);
//			if(grid_eye_data_raw[r_i][GRID_EYE_ROW][i] > mean_row_raw) {
////				printf("%d\t", grid_eye_data_raw[r_i][GRID_EYE_ROW][i]);
//				printf("0 ");
//			} else {
//				printf("  ");
//			}
////			mutex_unlock(_g_mutex_grid_print);
//		}
//		printf(" || ");
//		for(int i=0; i<8; i++) {
////			mutex_lock(_g_mutex_grid_print);
//			if(grid_eye_data_diff[r_i][GRID_EYE_ROW][i] > mean_row_diff) {
//				printf("@ ");
//			} else {
//				printf("  ");
//			}
////			mutex_unlock(_g_mutex_grid_print);
//		}
//		printf("\r\n");
//		frame_cnt++;
//	} while(r_i != queue_raw->front);
//	printf("mean row : %d\r\n", mean_row_raw);
//
//	// get mean value of COL
//	int mean_col_raw = 0;
//	do {
//		c_i = (c_i+1) % GRID_EYE_FRAME_SIZE;
//		for(int i = 0; i< 8; i++) {
//			mean_col_raw += grid_eye_data_raw[c_i][GRID_EYE_COL][i];
//		}
//	} while(c_i != queue_raw->front);
//	mean_col_raw /= 8 * GRID_EYE_FRAME_SIZE;
//	c_i = queue_raw->front;
//
//	int mean_col_diff = 0;
//	do {
//		c_i = (c_i+1) % GRID_EYE_FRAME_SIZE;
//		for(int i = 0; i< 8; i++) {
//			mean_col_diff += grid_eye_data_diff[c_i][GRID_EYE_COL][i];
//		}
//	} while(c_i != queue_raw->front);
//	mean_col_diff /= 8 * GRID_EYE_FRAME_SIZE;
//	c_i = queue_raw->front;
//
//
//	//print COL bigger than mean value
//	frame_cnt = 0;
//	printf("COL\r\n");
//	do {
//		c_i = (c_i+1) % GRID_EYE_FRAME_SIZE;
//		printf("%d\t", frame_cnt);
//		for(int i=0; i<8; i++) {
////			mutex_lock(_g_mutex_grid_print);
//			if(grid_eye_data_raw[c_i][GRID_EYE_COL][i] > mean_col_raw) {
////				printf("%d\t", grid_eye_data_raw[c_i][GRID_EYE_COL][i]);
//				printf("0 ");
//			} else {
//				printf("  ");
//			}
////			mutex_unlock(_g_mutex_grid_print);
//		}
//		printf(" || ");
//		for(int i=0; i<8; i++) {
////			mutex_lock(_g_mutex_grid_print);
//			if(grid_eye_data_diff[c_i][GRID_EYE_COL][i] > mean_col_diff) {
//				printf("@ ");
//			} else {
//				printf("  ");
//			}
////			mutex_unlock(_g_mutex_grid_print);
//		}
//		printf("\r\n");
//		frame_cnt++;
//	} while(c_i != queue_raw->front);
//	printf("mean col : %d\r\n", mean_col_raw);
//	printf("\r\n\r\n");
//}
///////////////////////////////////
///////////////////////////////////
//
//
//
//static void sh_grid_eye_monitor_task(void * arg) {
//	ret_code_t err;
//
//	float grid_eye_thermistor;
//	int grid_eye_task_active_count = 0;
//
//	memset(grid_eye_data_raw, 0, sizeof(grid_eye_data_raw));
//
//	ble_stack_init_wait();
//	uart_wifi_init_wait();
//
//	grid_eye_queue_init(&grid_eye_data_queue_raw);
//	grid_eye_queue_init(&grid_eye_data_queue_diff);
//
//	//Sense heat source
//	for(;;) {
//
//		grid_eye_thermistor = drv_grid_eye_read_thermistor();
////		printf("GRID EYE Thermistor		: [%.2f]\r\n", grid_eye_thermistor);
//
//		if((grid_eye_task_active_count % 2) == 0) {
//			err = drv_grid_eye_get_pixels(grid_eye_pixel_buf_first, GRID_EYE_PIXEL_ARRAY_SIZE);
//			APP_ERROR_CHECK(err);
//			for (int i = 1; i <= GRID_EYE_PIXEL_ARRAY_SIZE; i++) {
//				grid_eye_pixel_buf_diff[i-1] = get_float_abs(grid_eye_pixel_buf_second[i-1] - grid_eye_pixel_buf_first[i-1]);
//			}
////			err = grid_eye_queue_update(&grid_eye_data_queue_diff, grid_eye_pixel_buf_diff, grid_eye_data_diff);
////			err = grid_eye_queue_update(&grid_eye_data_queue_raw, grid_eye_pixel_buf_second, grid_eye_data_raw);
//		}
//		else if((grid_eye_task_active_count % 2) == 1) {
//			err = drv_grid_eye_get_pixels(grid_eye_pixel_buf_second, GRID_EYE_PIXEL_ARRAY_SIZE);
//			APP_ERROR_CHECK(err);
//			for(int i=1; i<=GRID_EYE_PIXEL_ARRAY_SIZE; i++) {
//				grid_eye_pixel_buf_diff[i-1] = get_float_abs(grid_eye_pixel_buf_first[i-1] - grid_eye_pixel_buf_second[i-1]);
//			}
////			err = grid_eye_queue_update(&grid_eye_data_queue_diff, grid_eye_pixel_buf_diff, grid_eye_data_diff);
////			err = grid_eye_queue_update(&grid_eye_data_queue_raw, grid_eye_pixel_buf_first, grid_eye_data_raw);
//		}
//
////		if(grid_eye_task_active_count % GRID_EYE_FRAME_SIZE == 0) {
////			printf("%dsec\r\n", (grid_eye_task_active_count - (grid_eye_task_active_count&10))/10);
//////			grid_eye_queue_print(&grid_eye_data_queue_raw_raw);
////			grid_eye_queue_print_with_mean_value(&grid_eye_data_queue_raw, &grid_eye_data_queue_diff);
////			printf("\r\n");
////		}
//		grid_eye_is_movement_sensed(&grid_eye_data_queue_raw, &grid_eye_data_queue_diff);
//		grid_eye_task_active_count++;
//
//		task_sleepms(100);
//	}
//
//
//}
