/*
 * AAT_main.h
 *
 *  Created on: 2021. 9. 15.
 *      Author: YuJin Park
 */

#ifndef APP_AAT_MAIN_H_
#define APP_AAT_MAIN_H_

#define MAIN_MAX_MSGQ_SIZE		10
#define MAIN_MAX_MSGQ_COUNT		10

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	uint8_t event;
	uint8_t status;
	uint8_t* msg;
}AATMainEvt_t;

enum{
	AAT_BATT_EVT = 0,
	AAT_KEY_CHANGE_EVT,
	AAT_ACC_EVT,
	AAT_ACC_INT_EVT,
	AAT_ALS_EVT,
	AAT_SEND_EVT,
	AAT_FLASH_EVT,
	AAT_LED_EVT,
};

enum{
	ANALOG_ADL_ON = 0x01,
	ANALOG_ADL_OFF = 0x02,
};

enum{
	LIGHT_OFF = 0x00,
	LIGHT_ON,
};

//AAT LF Event State
//enum
//{
//	AAT_LF_DATA_EVT_ST = 0,
//	SESSION_TIMEOUT_EVT,
//};

//AAT BUTTON Event State
enum{
	AAT_SHORT_KEY_EVT_ST = 0,
	AAT_LONG_KEY_EVT_ST
};

enum{
	AAT_FLASH_WORK_START = 0,
	AAT_FLASH_WORK_STOP,
	AAT_FLASH_WORK_Length,
	AAT_FLASH_WORK_LOAD,
};

#define AAT_Log_Error			0x00
#define AAT_Log_Go_into			0x01
#define	AAT_Log_Go_out			0x02
#define AAT_Log_Work_Start		0x03
#define AAT_Log_Work_Stop		0x04

enum{
	AAT_LED_CONNECT = 0,
	AAT_LED_DISCONNECT,
};

//typedef struct _date_time_
//{
//	uint32_t	ms;
//	uint8_t 	LF_id;
//	float		rssi;
//} LF_date_time;

uint8_t AAT_Main_event_send(uint8_t Main_evt, uint16_t Main_evt_state, uint8_t* msg);
void AAT_main_task_init(void);
uint8_t get_log_count(void);
void Work_Log_test_code(void);

extern bool ConnectionFlag;
bool CalibMod;
uint8_t Calib_up[3];
uint8_t Calib_Down[3];
uint8_t Calib_Right[3];
uint8_t Calib_Left[3];

#endif /* APP_AAT_MAIN_H_ */