/*
 * SH_main.h
 *
 *  Created on: 2020. 06. 11.
 *      Author: YJPark
 */

#ifndef APP_SH_MAIN_H_
#define APP_SH_MAIN_H_

#include <ubinos.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>


enum{
	SH_DEFAULT_EVT,
	SH_BLE_SENSOR_PAIRING_EVT,
	SH_TWI_SENSOR_DATA_READ,
};

enum{
	SPH_DEFAULT_ST
};

//commands to control smart hub in central mode
//Send msg bia bluetooth packet
#define SH_CMD_DEFAULT			0x00
#define SH_CMD_PAIRING			0xFF
#define SH_CMD_SEND_433_SIGNAL	0XA0
#define SH_CMD_TWI_SENSOR_READ  0xA1

typedef struct
{
	uint8_t event;
	uint8_t status;
	uint8_t* msg;
} SPHEvt_msgt;

void SPH_main_task_init(void);
int SPH_event_send(uint8_t evt, uint8_t state, uint8_t* msg);

#endif /* APP_SH_MAIN_H_ */
