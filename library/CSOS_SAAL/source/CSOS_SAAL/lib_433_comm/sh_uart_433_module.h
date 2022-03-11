/*
 * sh_uart_433_module.h
 *
 *  Created on: 2021. 3. 16.
 *      Author: sayyo
 */

#ifndef APP_SH_UART_433_MODULE_SH_UART_433_MODULE_H_
#define APP_SH_UART_433_MODULE_SH_UART_433_MODULE_H_

#include "ubinos.h"
#include <sw_config.h>

#if (CSOS_SAAL__USE_LIB_433 == 1)

#include <stdint.h>
#include "nrf_gpio.h"

#define UART433_READ_BUF_SIZE		2048
#define UART433_WRITE_BUF_SIZE		2048
#define UART433_CMD_BUF_SIZE		256
#define UART433_RSP_BUF_SIZE		256

#define UART433_READ_DATA_BUF_SIZE	256
#define UART433_SENSOR_NUM_MAX		128

typedef struct _uart433_msgq_event_t {
	uint8_t event;
	uint8_t status;
	uint8_t * msg;
} uart433_msgq_event_t;

#define UART433_SENSOR_PROFILE_DEVICE_ID_DEFAULT 0xFFFFFFFF
typedef struct _sensor_433_profile {
	uint8_t room;
	uint8_t sensor;
	uint8_t sensor_func;
	uint32_t device_ID;
} sensor_433_profile;

typedef enum {
	UART433_ERR_OK = 0,
	UART433_ERR_ERROR,
	UART433_ERR_TIMEOUT,
	UART433_ERR_BUSY
} uart433_err_t;

typedef struct _uart433_t {

	char cmd_buf[UART433_CMD_BUF_SIZE];
	uint8_t rsp_buf[UART433_RSP_BUF_SIZE];
	mutex_pt cmd_mutex;

	mutex_pt io_mutex;
	sem_pt io_read_sem;
	sem_pt io_write_sem;

	cbuf_pt io_read_buf;
	cbuf_pt io_write_buf;

	uint32_t rx_overflow_count;
	uint8_t tx_busy;

} uart433_t;

typedef enum {
	UART433_MODULE_DATA_RECEIVE_INT = 0,
	UART433_MODULE_DATA_RECEIVE_DONE,
	UART433_MODULE_SENSOR_PAIRING_MODE,
	UART433_MODULE_SEND_CMD_TO_OTHER_HUB,
} uart433_event_list_t;

typedef enum {
	UART433_MODULE_DATA_DEFAULT = 0,
	UART433_STATUS_DEVICE_INDEX_HANDLE,
} uart433_status_list_t;

#ifndef SENSOR_433_PAIRING_MODE

#define SWITCH_1_MODE_A
#define SWITCH_1_MODE_B
#define DOOR_SENSOR_1
#define DOOR_SENSOR_2
#define DOOR_SENSOR_3

#define SWITCH_MULTI_MODE_1
#define SWITCH_MULTI_MODE_2
#define SWITCH_MULTI_MODE_3
/*
 * And so on...
 * if it's not "PAIRING MODE", Sensor ID must be defined before using.
 */

#endif


void uart433_receive_flag_init();
void uart433_receive_flag_setting(bool set);
bool is_uart433_receive_flag_set();

void uart433_profile_flag_init();
void uart433_profile_flag_setting(bool set);
bool is_uart433_profile_flag_set();


int uart433_event_send(uint8_t evt, uint8_t state, uint8_t * msg);
void uart433_module_task_init(void);

uint32_t u_atoi(char * arr, uint32_t len);

#endif /*(CSOS_SAAL__USE_LIB_433 == 1)*/

#endif /* APP_SH_UART_433_MODULE_SH_UART_433_MODULE_H_ */
