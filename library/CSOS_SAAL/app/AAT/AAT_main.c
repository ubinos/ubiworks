/*
  * AAT_main.c
 *
 *  Created on: 2017. 8. 7.
 *      Author: Kwon
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "hw_config.h"
#include "sw_config.h"

//nrf_Driver inclde
#include "nrf_drv_gpiote.h"
#include "nrf_drv_uart.h"
#include "nrf_drv_timer.h"

//nrf_library include
#include "app_uart.h"
#include "app_timer.h"

//user library include
#include "AAT_main.h"

#include "AAT_device_EXT_FLASH.h"
#include "AAT_driver_i2c.h"
#include "AAT_driver_spi.h"
#include "AAT_device_LED.h"

#include "AAT_device_BMI160.h"
#include "AAT_device_OPT3001.h"

#include "LAP_api.h"
#include "ble_stack.h"

// packet index
#define	HEADER_INDEX						0
#define	SERVICE_ID_INDEX					1
#define	SEQ_NUM_INDEX						2
#define	LEN_INDEX							3
#define	CMD_INDEX							4
#define	DEVICETYPE_INDEX					5

#define	M2M_HEADER							0x88
#define WATER_SERVICE_SEQ_NUM				0x11

#define BLE_MAX_PACKET_LEN					19

#define ATT_ACC_ON_CHECK_MIN	3000
#define ATT_ACC_ON_CHECK_MAX	10000

#define ATT_ACC_OFF_CHECK_MIN	-3000
#define ATT_ACC_OFF_CHECK_MAX	1500

#define ATT_GYRO_ON_CHECK_MIN	100
#define ATT_GYRO_ON_CHECK_MAX	10000

#define ATT_GYRO_OFF_CHECK_MIN	-10000
#define ATT_GYRO_OFF_CHECK_MAX	-50

static msgq_pt Main_Msgq;

uint8_t sleep_count = 0;
#define update_val_sleep_count 100

uint8_t ATT_sw_mode = AAT_DEFUALT_SW_MODE;

void update_sleep_count()
{
	sleep_count = update_val_sleep_count;
}

//APP_TIMER_DEF(ACC_check_timer_id);
bool ConnectionFlag = false;

void AAT_send_packet_config(uint8_t water, uint8_t lux, uint8_t *send_packet);

void AAT_Pin_cfg_init(uint32_t pin_number) {
	nrf_gpio_cfg(pin_number, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
	//						 NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_D0S1, NRF_GPIO_PIN_NOSENSE);
							 NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
	nrf_gpio_pin_clear(pin_number);
}

void ACC_init(nrf_drv_gpiote_evt_handler_t gpiote_evt_handler) {
	//Interrupt Mode Check
	nrf_drv_gpiote_in_config_t pin_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
	pin_config.pull = NRF_GPIO_PIN_PULLUP;
	nrf_drv_gpiote_in_init(AAT_BOARD_PIN_ACEEL_INT_1, &pin_config, gpiote_evt_handler);
	nrf_drv_gpiote_in_event_disable(AAT_BOARD_PIN_ACEEL_INT_1);

	AAT_Bmi160_init();

	task_sleep(300);

	nrf_drv_gpiote_in_event_enable(AAT_BOARD_PIN_ACEEL_INT_1, true);
}

void ALS_init(nrf_drv_gpiote_evt_handler_t gpiote_evt_handler) {
	//Interrupt Mode Check
	nrf_drv_gpiote_in_config_t pin_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
	pin_config.pull = NRF_GPIO_PIN_PULLUP;
	nrf_drv_gpiote_in_init(AAT_BOARD_PIN_ACEEL_INT_1, &pin_config, gpiote_evt_handler);
	nrf_drv_gpiote_in_event_enable(AAT_BOARD_PIN_ACEEL_INT_1, false);
}

void RTC_init(nrf_drv_gpiote_evt_handler_t gpiote_evt_handler) {
	//nrf_drv_gpiote_in_config_t pin_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
	nrf_drv_gpiote_in_config_t pin_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

	nrf_drv_gpiote_in_init(AAT_BOARD_ALS_INT, &pin_config, gpiote_evt_handler);
	nrf_drv_gpiote_in_event_enable(AAT_BOARD_ALS_INT, true);

	AAT_RTC_device_init();
}

bool FLASH_init(void) {
	return AAT_falsh_data_all_erase();
}

////////////////
void ACC_check_handler()
{
	AAT_Main_event_send(AAT_ACC_EVT, 0, NULL);
}

void ACC_PIN_Interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {

	//Task check INT Check
	nrf_drv_gpiote_in_event_disable(AAT_BOARD_PIN_ACEEL_INT_1);

	//uint16_t temp_sen_data;
	//temp_sen_data = BMI160_ACC_DATA_READ_Y_AXIS();

	//AAT_Main_event_send(AAT_ACC_INT_EVT, 0, NULL);

	//nrf_drv_gpiote_in_event_enable(AAT_BOARD_PIN_ACEEL_INT_1, false);

	update_sleep_count();
}

void ALS_PIN_Interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
	nrf_drv_gpiote_in_event_disable(AAT_BOARD_ALS_INT);

	AAT_Main_event_send(AAT_ALS_EVT, 0x00, NULL);

	nrf_drv_gpiote_in_event_enable(AAT_BOARD_ALS_INT, true);
}

///////////////////////////////////////////////////////////////////////////////////
void RTC_Interrupt_handler(nrf_drv_gpiote_pin_t pin,
		nrf_gpiote_polarity_t action) {
	nrf_drv_gpiote_in_event_disable(AAT_BOARD_RTC_INT);

	nrf_drv_gpiote_in_event_enable(AAT_BOARD_RTC_INT, true);
}

void send_ATT_result_msg(uint8_t state_val)
{
	uint8_t light = 0;
	uint8_t tx_temp_packet[BLE_MAX_PACKET_LEN] = {0, };

	if(state_val == ANALOG_ADL_ON)
	{
			light = OPT3001_lux_read();

			if(light <= 10)
				light = LIGHT_OFF;
			else
				light = LIGHT_ON;

			memset(tx_temp_packet, 0, BLE_MAX_PACKET_LEN);

/*
			if(tx_temp_packet == NULL)
			{
				AAT_LED_OFF(WHITE);
				AAT_LED_ON(BLUE);
				task_sleep(500);
				AAT_LED_OFF(WHITE);
				task_sleep(500);
				AAT_LED_ON(BLUE);
				task_sleep(500);
				AAT_LED_OFF(WHITE);
			}
*/

			AAT_send_packet_config(ANALOG_ADL_ON, light, tx_temp_packet);	// txpacket malloc 시키기

			LAP_send_ble_msg_peripheral(tx_temp_packet, BLE_MAX_PACKET_LEN);
			//AAT_BLE_Protocol_event_send(AAT_PERIPHERAL_EVT, AAT_BLE_PERIPHERAL_DATA_SEND_STATE, 0, 0, (uint8_t*)tx_temp_packet);

			AAT_LED_OFF(WHITE);
			AAT_LED_ON(RED);
			task_sleep(300);
			AAT_LED_OFF(WHITE);

			update_sleep_count();

			#if(BLE_DEBUGGING_LED_ENABLE == 1)
			if(get_ble_peripheral_connected_flag() == true)
			{
				AAT_LED_ON(BLUE);
			}
			#endif

			task_sleep(100);
	}
	else if(state_val == ANALOG_ADL_OFF)
	{
			light = OPT3001_lux_read();

			if(light <= 10)
				light = LIGHT_OFF;
			else
				light = LIGHT_ON;

			memset(tx_temp_packet, 0, BLE_MAX_PACKET_LEN);

/*
			if(tx_temp_packet == NULL)
			{
				AAT_LED_OFF(WHITE);
				AAT_LED_ON(BLUE);
				task_sleep(500);
				AAT_LED_OFF(WHITE);
				task_sleep(500);
				AAT_LED_ON(BLUE);
				task_sleep(500);
				AAT_LED_OFF(WHITE);
			}
*/
			update_sleep_count();

			AAT_send_packet_config(ANALOG_ADL_OFF, light, tx_temp_packet);

			LAP_send_ble_msg_peripheral(tx_temp_packet, BLE_MAX_PACKET_LEN);
			//AAT_BLE_Protocol_event_send(AAT_PERIPHERAL_EVT, AAT_BLE_PERIPHERAL_DATA_SEND_STATE, 0, 0, (uint8_t*)tx_temp_packet);

			AAT_LED_OFF(WHITE);
			AAT_LED_ON(RED);
			task_sleep(300);
			AAT_LED_OFF(WHITE);

			#if(BLE_DEBUGGING_LED_ENABLE == 1)
			if(get_ble_peripheral_connected_flag() == true)
			{
				AAT_LED_ON(BLUE);
			}
			#endif

			task_sleep(100);
	}
}

// F3 01 22 58
void AAT_main_task(void * arg) {
	int r;

	AATMainEvt_t Main_msgRxBuffer;
	int16_t ACC_Y;
	int16_t DefaultCalibOn = -1000;
	int16_t DefaultCalibOff = -1200;
	bool WaterOnflag = false;
	int16_t GYRO_test[3];

	int testALS = 0;

	ConnectionFlag = false;
	CalibMod = true;

	nrf_drv_gpiote_init();

	nrf_gpio_cfg_output(AAT_BOARD_ACCEL_PWR_EN);
	nrf_gpio_pin_set(AAT_BOARD_ACCEL_PWR_EN);

	nrf_gpio_cfg_output(AAT_BOARD_ALS_PWR_EN);
	nrf_gpio_pin_set(AAT_BOARD_ALS_PWR_EN);

	AAT_spi_init();
	AAT_i2c_init();

	OPT3001_init();

	//nrf_gpio_pin_clear(AAT_BOARD_ALS_PWR_EN);

	ACC_init(ACC_PIN_Interrupt_handler);

	task_sleep(1000);

	AAT_LED_port_init();
	AAT_LED_ON(RED);
	task_sleep(500);
	AAT_LED_OFF(RED);
	task_sleep(500);
	AAT_LED_ON(RED);
	task_sleep(500);
	AAT_LED_OFF(RED);
	task_sleep(500);
	AAT_LED_ON(RED);
	task_sleep(500);
	AAT_LED_OFF(RED);
	task_sleep(500);


	uint8_t data_send_state = ANALOG_ADL_OFF;

	int16_t temp_sensor_val;

	int16_t temp_acc_y;

	temp_acc_y = 0;
	temp_sensor_val = 0;

	task_sleep(3000);

	bool is_AAT_ON = false;

	task_sleep(1000);

	uint8_t light = LIGHT_OFF;

	while(1)
	{
		while(sleep_count == 0)
		{
			task_sleep(30);
		}

		AAT_wake_up();
		
		//task_sleep(100);

		switch(ATT_sw_mode)
		{
			case AAT_SW_MODE_ACC :
			{
				temp_sensor_val = BMI160_ACC_DATA_READ_Y_AXIS();
				task_sleep(100);
				if(temp_sensor_val > ATT_ACC_ON_CHECK_MIN && temp_sensor_val < ATT_ACC_ON_CHECK_MAX && is_AAT_ON == false)
				{
					send_ATT_result_msg(ANALOG_ADL_ON);
					is_AAT_ON = true;
				}
				else if(temp_sensor_val <= ATT_ACC_OFF_CHECK_MAX && temp_sensor_val >= ATT_ACC_OFF_CHECK_MIN && is_AAT_ON == true)
				{
					send_ATT_result_msg(ANALOG_ADL_OFF);
					is_AAT_ON = false;
				}
			}
			break;
			case AAT_SW_MODE_GYRO :
			{
				temp_sensor_val = BMI160_GYRO_DATA_READ_Z_AXIS();
				if(temp_sensor_val > ATT_GYRO_ON_CHECK_MIN && temp_sensor_val < ATT_GYRO_ON_CHECK_MAX && is_AAT_ON == false)
				{
					send_ATT_result_msg(ANALOG_ADL_ON);
					is_AAT_ON = true;
					task_sleep(100);
				}
				else if(temp_sensor_val <= ATT_GYRO_OFF_CHECK_MAX && temp_sensor_val >= ATT_GYRO_OFF_CHECK_MIN && is_AAT_ON == true)
				{
					send_ATT_result_msg(ANALOG_ADL_OFF);
					is_AAT_ON = false;
					task_sleep(100);
				}
			}
			break;
		}

		sleep_count--;
		if(sleep_count <= 0)
		{
			AAT_LED_OFF(WHITE);
			AAT_LED_ON(BLUE);
			task_sleep(100);
			AAT_LED_OFF(WHITE);
			sleep_count = 0;
			nrf_drv_gpiote_in_event_disable(AAT_BOARD_PIN_ACEEL_INT_1);
			task_sleep(300);
			//nrf_drv_gpiote_in_event_enable(AAT_BOARD_PIN_ACEEL_INT_1, false);
			AAT_enter_sleep();
			//nrf_drv_gpiote_in_event_enable(AAT_BOARD_PIN_ACEEL_INT_1, true);
			//nrf_drv_gpiote_in_event_enable(AAT_BOARD_PIN_ACEEL_INT_1, true);
		}
		else if(sleep_count > 200)
		{
			AAT_LED_OFF(WHITE);
			AAT_LED_ON(GREEN);
			task_sleep(100);
			AAT_LED_OFF(WHITE);
			AAT_LED_OFF(WHITE);
			AAT_LED_ON(GREEN);
			task_sleep(100);
			AAT_LED_OFF(WHITE);
			AAT_LED_OFF(WHITE);
			AAT_LED_ON(GREEN);
			task_sleep(100);
			AAT_LED_OFF(WHITE);

			sleep_count = 0;
			nrf_drv_gpiote_in_event_disable(AAT_BOARD_PIN_ACEEL_INT_1);
			task_sleep(300);
			AAT_enter_sleep();
			
			//nrf_drv_gpiote_in_event_enable(AAT_BOARD_PIN_ACEEL_INT_1, false);
		}
	}
}

void AAT_send_packet_config(uint8_t water, uint8_t lux, uint8_t *send_packet)
{
	uint8_t buf[BLE_MAX_PACKET_LEN] = {0,};

	buf[HEADER_INDEX] = M2M_HEADER;
	buf[SEQ_NUM_INDEX] = WATER_SERVICE_SEQ_NUM;
	buf[SERVICE_ID_INDEX] = AAT_SERVICE_ID;
	buf[LEN_INDEX] = 0x04;

	buf[CMD_INDEX] = 0x03;

	buf[CMD_INDEX+1] = ADL_DEVICE_TYPE0;
	buf[CMD_INDEX+2] = ADL_DEVICE_TYPE1;

	if(water == ANALOG_ADL_ON)
		buf[CMD_INDEX+3] = ANALOG_ADL_ON;
	else
		buf[CMD_INDEX+3] = ANALOG_ADL_OFF;

	memcpy(send_packet, buf, BLE_MAX_PACKET_LEN);
}

void AAT_main_task_init(void) {
	int r;

	r = msgq_create(&Main_Msgq, sizeof(AATMainEvt_t), MAIN_MAX_MSGQ_COUNT);
	if (0 != r) {
		logme("fail at msgq_create\r\n");
	}

// Task create
	r = task_create(NULL, AAT_main_task, NULL, task_gethighestpriority() - 2, 512, "Main");
	if (r != 0) {
		printf("task_create(AAT_main_task) failed\n\r");
	} else {
		printf("task_create(AAT_main_task) created\n\r");
	}

/*
	r = task_create(NULL, ACC_check_task, NULL, task_gethighestpriority() - 3, 512, "ACC_check");
	if (r != 0) {
		printf("task_create(AAT_main_task) failed\n\r");
	} else {
		printf("task_create(AAT_main_task) created\n\r");
	}
	*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t AAT_Main_event_send(uint8_t Main_evt, uint16_t Main_evt_state,
		uint8_t* msg) {
	AATMainEvt_t temp_msg;

	temp_msg.event = Main_evt;
	temp_msg.status = Main_evt_state;
	temp_msg.msg = msg;

	msgq_send(Main_Msgq, (unsigned char*) &temp_msg);

	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void Work_Log_test_code(void) {
	uint8_t write_buffer_test[16] = { 0, };

	write_buffer_test[0] = 0;
	write_buffer_test[1] = AAT_Log_Go_into;
	write_buffer_test[2] = 11;
	write_buffer_test[3] = 8;
	write_buffer_test[4] = 11;
	write_buffer_test[5] = 11;
	write_buffer_test[6] = 0xB2;
	write_buffer_test[7] = 0x00;
	write_buffer_test[8] = 0x7A;
	write_buffer_test[9] = 0x5C;
	write_buffer_test[10] = 0x5D;
	write_buffer_test[11] = 5;
	write_buffer_test[12] = 8;
	write_buffer_test[13] = 7;
	write_buffer_test[14] = 1;
	write_buffer_test[15] = 2;

	AAT_flash_LOG_data_write(write_buffer_test, 10, 0);
}