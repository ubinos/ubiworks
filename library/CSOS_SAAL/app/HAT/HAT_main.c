/*
 * HAT_main.c
 *
 *  Created on: 2020. 06. 11.
 *      Author: YJPark
 */

#include <ubinos.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "HAT_main.h"
#include "ble_stack.h"
#include "Button.h"
#include "LED.h"
#include "TMR_PPI_SAADC.h"
#include "LAP_main.h"

#include "sw_config.h"

#define MSG_CALIB_NOT_ACCESSIBLE	0x20
#define MSG_CALIB_ACCEPTED			0x21
#define MSG_CALIB_DONE				0x22
#define MSG_CHECK_SEQ_FAILED		0x30
#define MSG_CHECK_SEQ_WAIT_SUCCESS	0x31
#define MSG_CHECK_SEQ_SUCCESS		0x32

#define DEFAULT_APP_ID				0xFD		// FF FE FD 사용 중
#define MAX_REPLY_CNT				5

#define PAC_TYPE_INDEX			0
#define PAC_SERVICE_INDEX		1
#define PAC_SEQ_INDEX			2
#define PAC_LEN_INDEX			3
#define PAC_CMD_INDEX			4

#define PAC_APP_ID0_INDEX		5
#define PAC_APP_ID1_INDEX		6
#define PAC_APP_MAX_CRNT_INDEX	6
#define PAC_ACT_PWR_INDEX		7
#define PAC_ACT_LVL_INDEX		7


#define PAC_TYPE 				0x88
#define SERVICE_ID				0x17
#define SEQ_NUM					0x11

#define LEN_OPT					0x02
#define LEN_CAL					0x01
#define LEN_ACK					0x02
#define LEN_RES					0x04

#define CMD_OPT					0x01
#define CMD_CAL					0x02
#define CMD_RES					0x03
#define CMD_REQ					0x04
#define CMD_TST					0x09


#define CMD_ACK_OPT					0x01		// ACK for CMD
#define CMD_ACK_CAL					0x02
#define CMD_ACK_RES					0x03

// ACK for CALIB SEQ
#define MSG_CALIB_NOT_ACCESSIBLE	0x20
#define MSG_CALIB_ACCEPTED			0x21
#define MSG_CALIB_DONE				0x22
#define MSG_CHECK_SEQ_FAILED		0x30
#define MSG_CHECK_SEQ_WAIT_SUCCESS	0x31
#define MSG_CHECK_SEQ_SUCCESS		0x32

#define MAX_REPLY_COUNT				5

#define APP_TICK_EVENT_INTERVAL     APP_TIMER_TICKS(2000)
APP_TIMER_DEF(m_tick_timer);


static msgq_pt HAT_msgq;
uint8_t ovr, lvl;
uint8_t App_max_crnt = 15;
uint8_t res_reply_flag = MAX_REPLY_CNT;
ret_code_t err_code;

// extern!!
uint8_t option_flag = 0;		// 가전 ID 등의 옵션이 적용되었는가



uint8_t timer_cnt = 0;
uint8_t tmp_crnt = 0;

void app_tick_handler(void* p_context){
//______________________________________________________________
/*
	timer_cnt++;

	if(timer_cnt == 8){
		timer_cnt = 0;
		if(ble_connect_flag_for_main){
		    LED_toggle(PIN_LED2);
		    HAT_event_send(HAT_REPLY_TMR_EVT, 0, NULL);
			if(tmp_crnt == 0) 	tmp_crnt = 2;
			else 				tmp_crnt = 0;
		}
	}
*/
//______________________________________________________________


	if(res_reply_flag > 0){
		LED_toggle(PIN_LED4);

		HAT_event_send(HAT_REPLY_TMR_EVT, 0, NULL);
		res_reply_flag--;
	}

	else{
		err_code = app_timer_stop(m_tick_timer);
		APP_ERROR_CHECK(err_code);

		res_reply_flag = MAX_REPLY_CNT;
	}

}

void BLE_send_result(uint8_t App_ID, uint8_t Act_ON, uint8_t Act_LVL)
{
	uint8_t temp_packet[PAAR_MAXIMUM_PACKET_SIZE] = {0, };

	memset(temp_packet, 0, PAAR_MAXIMUM_PACKET_SIZE);
	temp_packet[PAC_TYPE_INDEX] 	= PAC_TYPE;
	temp_packet[PAC_SERVICE_INDEX] 	= SERVICE_ID;
	temp_packet[PAC_SEQ_INDEX] 		= SEQ_NUM;
	temp_packet[PAC_LEN_INDEX] 		= LEN_RES;

	temp_packet[PAC_CMD_INDEX] 		= CMD_RES;
	temp_packet[PAC_APP_ID0_INDEX] 	= TEST_HOME_APPLIANCE_ID0;
	temp_packet[PAC_APP_ID1_INDEX] 	= TEST_HOME_APPLIANCE_ID1;
	temp_packet[PAC_ACT_PWR_INDEX] 	= Act_ON;

	task_sleep(TEST_SEND_MSG_DELAY);

	printf("RES msg : PWR %d, LVL %d\r\n", temp_packet[PAC_ACT_PWR_INDEX], temp_packet[PAC_ACT_LVL_INDEX]);

	LAP_send_ble_msg_peripheral(temp_packet, PAAR_MAXIMUM_PACKET_SIZE);
}

void HAT_main_task(void* arg){
	int r;
	HATEvt_msgt HAT_evt_msg;

	nrf_drv_gpiote_init();
	ble_stack_init_wait();
	LED_init();

	Appliance_ID = DEFAULT_APP_ID;
	ble_connect_flag_for_main = 0;

	// saadc ------------------------------------------------------------------------------------------------------------------------------------------------------------
	nrf_drv_saadc_uninit();
	NRF_SAADC->INTENCLR = (SAADC_INTENCLR_END_Clear << SAADC_INTENCLR_END_Pos);
	NVIC_ClearPendingIRQ(SAADC_IRQn);
	// Clear all previous unit settings and interrupt settings.


	saadc_sampling_event_init();		// Initializing TMR2 & PPI
	saadc_init();						// Initializing SAADC

	SAADC_PACKET saadc_packet;
	saadc_packet.CRNT_LVL = 0;
	saadc_packet.CRNT_OVR = 0;

    saadc_off_calib();
	// saadc ------------------------------------------------------------------------------------------------------------------------------------------------------------

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_tick_timer, APP_TIMER_MODE_REPEATED, app_tick_handler);
    APP_ERROR_CHECK(err_code);

    LED_toggle(PIN_LED1);
    LED_toggle(PIN_LED2);

//_____________________________________________________________________________

//    while(ble_connect_flag_for_main == 0){
//    	task_sleep(100);
//    }
//    err_code = app_timer_start(m_tick_timer, APP_TICK_EVENT_INTERVAL, NULL);

    task_sleep(5000);
    err_code = app_timer_start(m_tick_timer, APP_TICK_EVENT_INTERVAL, NULL);
//_____________________________________________________________________________



    uint8_t current_state = 0;

	for (;;) {
		r = msgq_receive(HAT_msgq, (unsigned char*) &HAT_evt_msg);
		if (0 != r) {
			logme("fail at msgq_receive\r\n");
		}
		else {
			switch( HAT_evt_msg.event ){

			case HAT_APPLIANCE_ID_SET:
				Appliance_ID = HAT_evt_msg.msg[0];
				App_max_crnt = HAT_evt_msg.msg[1];
				// default ID - 0xFF, default max_crnt - 15
				break;

			case HAT_LVL_ACK:
				err_code = app_timer_stop(m_tick_timer);
				APP_ERROR_CHECK(err_code);

				res_reply_flag = MAX_REPLY_CNT;
				break;

			/* Three Events in Power Measurements.
			 * 1. Power on 	- Events occur in callback functions in SAADC.c.
			 * 2. Power off - Same as 1.
			 * 1, 2 events are both treated as PT_SAADC_LIMIT_BOUND.
			 * 3. Request from communication module - Event occurs in on_ble_evt in PT_ble_main.
			 * The way events occur is all different, but the tasks that must be done are all the same, so they are treated the same.*/
			//-------------------------------------------------------------------------
			case HAT_SAADC_LIMIT_BOUND:
//			case HAT_SAADC_REQUEST:
//			case HAT_CRNT_LVL_CHANGE:
					saadc_get_packet(&saadc_packet, App_max_crnt);					// Get the current lvl.

					//ovr = saadc_packet.CRNT_OVR;
					ovr = HAT_evt_msg.status;
					lvl = saadc_packet.CRNT_LVL;

					err_code = app_timer_start(m_tick_timer, APP_TICK_EVENT_INTERVAL, NULL);
				    APP_ERROR_CHECK(err_code);
			case HAT_REPLY_TMR_EVT:

					if(current_state != ovr)
					{
						/* App ID + ON/OFF + LVL */
						BLE_send_result(Appliance_ID, ovr, lvl);
						LED_toggle(PIN_LED2);
					}
					current_state = ovr;
				break;
			//-------------------------------------------------------------------------



/*
			case HAT_REPLY_TMR_EVT:
				//_____________________________________________________________________________
				BLE_send_res(Appliance_ID, 0, tmp_crnt);
				//_____________________________________________________________________________
				break;
*/

			default :
				break;

			}



			if( HAT_evt_msg.msg != NULL ){
				free(HAT_evt_msg.msg);
			}

		}
	}
}


void HAT_main_task_init(void){
	int r;

	r = msgq_create(&HAT_msgq, sizeof(HATEvt_msgt), 20);
	if (0 != r) {
		printf("fail at msgq create\r\n");
	}

	r = task_create(NULL, HAT_main_task, NULL, task_gethighestpriority()-2, 512, NULL);
	if (r != 0) {
		printf("== HAT_main_task failed \n\r");
	} else {
		printf("== HAT_main_task created \n\r");
	}
}

int HAT_event_send(uint8_t evt, uint8_t state, uint8_t* msg)
{
	HATEvt_msgt hat_msg;

	hat_msg.event = evt;
	hat_msg.status = state;
	hat_msg.msg = msg;

	return msgq_send(HAT_msgq, (unsigned char*) &hat_msg);
}
