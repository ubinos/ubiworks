/*
 * SH_main.c
 *
 *  Created on: 2020. 06. 11.
 *      Author: YJPark
 */

#include "../lib_bluetooth_csos/ble_stack.h"
#include "SPH_main.h"
#include <ubinos.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static msgq_pt SPH_msgq;

void SPH_main_task(void* arg){
	int r;
	SPHEvt_msgt SPH_evt_msg;

	ble_stack_init_wait();

	printf("test");

	for (;;) {
		r = msgq_receive(SPH_msgq, (unsigned char*) &SPH_evt_msg);
		if (0 != r) {
			logme("fail at msgq_receive\r\n");
		} else {
			switch( SPH_evt_msg.event ) {
			case SH_DEFAULT_EVT :

				break;

			case SH_BLE_SENSOR_PAIRING_EVT:

				break;

			case SH_TWI_SENSOR_DATA_READ:


				break;

			default :
				break;
			}

			if( SPH_evt_msg.msg != NULL ) {
				free(SPH_evt_msg.msg);
			}
		}
	}
}

void SPH_main_task_init(void){
	int r;

	r = msgq_create(&SPH_msgq, sizeof(SPHEvt_msgt), 20);
	if (0 != r) {
		printf("fail at msgq create\r\n");
	}

	r = task_create(NULL, SPH_main_task, NULL, task_gethighestpriority()-2, 512, NULL);
	if (r != 0) {
		printf("== SPH_main_task failed \n\r");
	} else {
		printf("== SPH_main_task created \n\r");
	}
}

int SPH_event_send(uint8_t evt, uint8_t state, uint8_t* msg)
{
	SPHEvt_msgt SPH_msg;

	SPH_msg.event = evt;
	SPH_msg.status = state;
	SPH_msg.msg = msg;

	return msgq_send(SPH_msgq, (unsigned char*) &SPH_msg);
}

//////EOF///////
