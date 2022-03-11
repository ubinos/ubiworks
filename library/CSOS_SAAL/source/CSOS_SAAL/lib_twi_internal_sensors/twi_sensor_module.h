/*
 * twi_sensor_module.h
 *
 *  Created on: 2021. 2. 8.
 *      Author: HMKANG
 */

#ifndef APP_TWI_SENSOR_READ_TWI_SENSOR_MODULE_H_
#define APP_TWI_SENSOR_READ_TWI_SENSOR_MODULE_H_

#include <ubinos.h>

#include "nrf_drv_twi.h"
#include "twi_include.h"

#include "sw_config.h"

#define LPS22HB_ADDR	0x5c		//Low Power Accelerometer 	0b 1011100
#define LIS2DH12_ADDR	0x19		//Pressure Sensor			0b 0011000
#define HTS221_ADDR		0x5f		//Humidity Sensor
#define BH1745_ADDR		0x38		//Color Sensor
#define CCS811_ADDR		0x5a		//Gas Sensor
#define GRID_EYE_ADDR	0x68		//GRID EYE Infrared Array Sensor

#define PRESSURE_SENSOR_INIT_WAIT_DELAY 			300
#define TEMPERATURE_HUMIDITY_SENSOR_INIT_WAIT_DELAY	300
#define GAS_SENSOR_INIT_WAIT_DELAY 					300
#define COLOR_SENSOR_INIT_WAIT_DELAY 				300

#define NO_INTERRUPT_PIN	0xffffffff


typedef struct _twi433_msgq_event_t {
	uint8_t event;
	uint8_t status;
	uint8_t * msg;
} twi433_msgq_event_t;

typedef struct _twi433_color_data_t {
	uint16_t red;
	uint16_t green;
	uint16_t blue;
	uint16_t clear;
} color_data_t;

typedef struct _twi433_sensor_data_t {
	float pressure;
	float _pressure_temp;
	int16_t humidity;
	float temperature;
	uint16_t gas_adc_raw;
	drv_ccs811_alg_result_t gas_alg_data;
	drv_bh1745_data_t color;
} twi433_sensor_data_t;

/*
typedef struct _sensor_log_t {
	twi433_sensor_data_t env_data;
	sh_time_t sh_sensor_log_time;
} sensor_log_t;
*/

#define GRID_EYE_ROW	0
#define GRID_EYE_COL	1

#define GRID_EYE_FRAME_SIZE		10
#define GRID_EYE_PIXEL_DIFF_COUNT_THRESHOLD		5
#define GRID_EYE_TEMP_DIFF_THRESHOLD		125
#define GRID_EYE_PIXEL_ABSOL_COUNT_THRESHOLD	20
#define GRID_EYE_STDEV_THRESHOLD		0.625

#define GRID_EYE_STATE_LEFT		10
#define GRID_EYE_STATE_RIGHT	20
#define GRID_EYE_STATE_UP		30
#define GRID_EYE_STATE_DOWN		40

#define GRID_EYE_LEFT_TO_RIGHT	1
#define GRID_EYE_RIGHT_TO_LEFT	2
#define GRID_EYE_UP_TO_DOWN		3
#define GRID_EYE_DOWN_TO_UP		4

#define GRID_EYE_NOTHING		-2
#define ENV_DATA_PERIODIC		0xFE
#define ENV_DATA_EVENT			0xFF

#define GRID_EYE_HEAT_SENSED	1
#define GRID_EYE_NOTHING_HEAT	0

#define GRID_EYE_LEFT_4			7
#define GRID_EYE_LEFT_3			6
#define GRID_EYE_LEFT_2			5
#define GRID_EYE_LEFT_1			4
#define GRID_EYE_RIGHT_1		3
#define GRID_EYE_RIGHT_2		2
#define GRID_EYE_RIGHT_3		1
#define GRID_EYE_RIGHT_4		0

#define GRID_EYE_UP_4			7
#define GRID_EYE_UP_3			6
#define GRID_EYE_UP_2			5
#define GRID_EYE_UP_1			4
#define GRID_EYE_DOWN_1			3
#define GRID_EYE_DOWN_2			2
#define GRID_EYE_DOWN_3			1
#define GRID_EYE_DOWN_4			0



typedef struct _grid_eye_queue {
	int front, rear;
} grid_eye_queue;

typedef struct _grid_eye_list_node {
	int data;
	struct _grid_eye_list_node * next;
} grid_eye_list_node;

typedef struct _grid_eye_list_queue {
	grid_eye_list_node * front;
	grid_eye_list_node * rear;
	int count;
} grid_eye_list_queue;





typedef enum{
	TWI433_DEFAULT_EVT = 0,
	TWI_SENSOR_START,
	TWI_BLE_REQUEST,
	TWI_SEND_DATA_BY_MQTT_433,
	TWI_MQTT_LOG_REQUSET
} _twi433_sensor_evt ;

typedef enum {
	TWI433_BLE_DEFAULT  = 0,
	TWI433_BLE_REQUEST_ALL,
	TWI433_BLE_REQUEST_TEMPERATURE,
	TWI433_BLE_REQUEST_HUMIDITY,
	TWI433_BLE_REQUEST_PRESSURE,
	TWI433_BLE_REQUEST_GAS,
	TWI433_BLE_REQUEST_COLOR,
	TWI433_BLE_REQUEST_GRIDEYE,
	TWI433_BLE_REQUEST_ACC,

	TWI433_BLE_ENV_RESPONSE,
} twi433_ble_event;

enum {
	TWI_433_REQUEST_DEFAULT = 0,
	TWI_433_REQUEST_ALL,
};

enum {
	TWI_MQTT_LOG_REQUSET_DEFAULT = 0,
	TWI_MQTT_LOG_REQUEST_ALL,
};

void twi433_module_task_init(void);
int twi433_event_send(uint8_t evt, uint8_t state, uint8_t * msg);
int twi433_sensor_processing_event_send(uint8_t evt, uint8_t state, uint8_t * msg);

#endif /* APP_TWI_SENSOR_READ_TWI_SENSOR_MODULE_H_ */
