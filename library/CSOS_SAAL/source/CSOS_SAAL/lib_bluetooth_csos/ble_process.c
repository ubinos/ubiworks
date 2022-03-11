/*
 * ble_process.c
 *
 *  Created on: 2020. 06. 11.
 *      Author: YuJin Park
 */

#ifndef APPLICATION_PAAR_FRONTEND_EXE_PAAR_FRONTEND_SRC_BLE_PROCESS_C_
#define APPLICATION_PAAR_FRONTEND_EXE_PAAR_FRONTEND_SRC_BLE_PROCESS_C_


#include <ubinos.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <malloc.h>

// nrf board
//#include "_bsp_board.h"

#include "ble_gap.h"
#include "ble_process.h"
#include "ble_stack.h"
#include "LAP_api.h"
#include "LAP_main.h"

#include <hw_config.h>
#include <sw_config.h>

static msgq_pt BLE_process_msgq;

BLE_Ctr_Connection_info Central_conn_info[MAXIMUM_NUM_CENTRAL_CONNECTION];

uint16_t current_conn_handle;

uint8_t temp_connectioned = 0;

static scan_target_paar_id_st ble_target_paar_id;

LAP_ble_adv_report scan_target_result;

void BLE_process_event_send(uint8_t event, uint8_t status, uint16_t conn_handle,
									uint16_t handle, uint32_t msg_len, uint8_t* msg)
{
	BLEPEvt_msgt ble_evt_msg;

	ble_evt_msg.event = event;
	ble_evt_msg.status = status;
	ble_evt_msg.conn_handle = conn_handle;
	ble_evt_msg.handle = handle;
	ble_evt_msg.msg_len = msg_len;
	ble_evt_msg.msg = msg;

	int r;
	r = msgq_send(BLE_process_msgq, (unsigned char*) &ble_evt_msg);
	if(r != 0)
	{
		printf("BLE event send error : %d\r\n", r);
	}

}

void clear_scan_target_paar_id()
{
	ble_target_paar_id.scan_target_paar_id_enable = false;
	memset(&ble_target_paar_id.scan_target_paar_id, 0, 4);
}

void set_scan_target_paar_id(uint8_t* target_paar_id)
{
	ble_target_paar_id.scan_target_paar_id_enable = true;
	memcpy(&ble_target_paar_id.scan_target_paar_id, target_paar_id, 4);
}

//======Process Function of BLE Central Event & Command===============
static void processing_BLE_Central_Scan_Timeout(BLEPEvt_msgt BLE_evt_msg)
{
	LAP_event_send(LAP_CENTRAL_EVT, LAP_CENTRAL_ST_SCAN_TIMEOUT, BLE_evt_msg.conn_handle, BLE_evt_msg.handle, BLE_evt_msg.msg_len, NULL);
}

static void processing_BLE_Central_Conn_Timeout(BLEPEvt_msgt BLE_evt_msg)
{
	LAP_event_send(LAP_CENTRAL_EVT, LAP_CENTRAL_ST_CONN_TIMEOUT, BLE_evt_msg.conn_handle, BLE_evt_msg.handle, BLE_evt_msg.msg_len, NULL);
}

static void processing_BLE_Central_Connected(BLEPEvt_msgt BLE_evt_msg)
{
	current_conn_handle = BLE_evt_msg.conn_handle;

	//send LAP_event : BLE Central Connected
	LAP_event_send(LAP_CENTRAL_EVT, LAP_CENTRAL_ST_CONNECTED, BLE_evt_msg.conn_handle, BLE_evt_msg.handle, BLE_evt_msg.msg_len, NULL);

}

static void processing_BLE_Central_Disconnected(BLEPEvt_msgt BLE_evt_msg)
{
	LAP_event_send(LAP_CENTRAL_EVT, LAP_CENTRAL_ST_DISCONNECTED, BLE_evt_msg.conn_handle, BLE_evt_msg.handle, BLE_evt_msg.msg_len, NULL);
}

static void processing_BLE_Central_Data_Received(BLEPEvt_msgt BLE_evt_msg)
{
	uint8_t* temp_msg = NULL;
	if(BLE_evt_msg.msg != NULL)
	{
		temp_msg = malloc(BLE_evt_msg.msg_len);
		if(temp_msg == NULL)
		{
			//printf("malloc error : BLE_Central_Data_Received\r\n");
			return;
		}
		else
		{
			//printf("malloc processing_BLE_Central_Data_Received\r\n");
		}

		memcpy(temp_msg, BLE_evt_msg.msg, BLE_evt_msg.msg_len);
	}
	LAP_event_send(LAP_CENTRAL_EVT, LAP_CENTRAL_ST_DATA_RECEIVED, BLE_evt_msg.conn_handle, BLE_evt_msg.handle, BLE_evt_msg.msg_len, temp_msg);
}

//int test_ct = 0;

static void processing_BLE_Central_Scan_Start(BLEPEvt_msgt BLE_evt_msg)
{
	if(BLE_evt_msg.msg != NULL)
	{
		set_scan_target_paar_id(BLE_evt_msg.msg);
		memset(&scan_target_result, 0, sizeof(LAP_ble_adv_report));
	}
	else
	{
		clear_scan_target_paar_id();
	}
	//printf("SCAN_test_cound : %d \r\n", test_ct++);
	PAAR_scan_start();
}

static void processing_BLE_Central_Connect_Target(BLEPEvt_msgt BLE_evt_msg)
{
	ble_gap_addr_t target_address;

	memcpy(&target_address, (ble_gap_addr_t*)BLE_evt_msg.msg, sizeof(ble_gap_addr_t));
	PAAR_ble_gap_connect(&target_address);
}

static void processing_BLE_Central_Disconnect_Target(BLEPEvt_msgt BLE_evt_msg)
{
	PAAR_ble_gap_disconnect(BLE_evt_msg.conn_handle);
}

static int processing_BLE_Central_Data_Send(BLEPEvt_msgt BLE_evt_msg)
{
	return PAAR_send_ble_msg_central(BLE_evt_msg.conn_handle, BLE_evt_msg.handle, BLE_evt_msg.msg, BLE_evt_msg.msg_len);
}

//======Process Function of BLE Peripheral Event & Command===============
static int processing_BLE_Peripheral_Connected(BLEPEvt_msgt BLE_evt_msg)
{
	return LAP_event_send(LAP_PERIPHERAL_EVT, LAP_PERIPHERAL_ST_CONNECTED, BLE_evt_msg.conn_handle,
															BLE_evt_msg.handle, BLE_evt_msg.msg_len, NULL);
}

static int processing_BLE_Peripheral_Disconnected(BLEPEvt_msgt BLE_evt_msg)
{
	return LAP_event_send(LAP_PERIPHERAL_EVT, LAP_PERIPHERAL_ST_DISCONNECTED, BLE_evt_msg.conn_handle,
															BLE_evt_msg.handle, BLE_evt_msg.msg_len, NULL);
}

static int processing_BLE_Peripheral_Data_Received(BLEPEvt_msgt BLE_evt_msg)
{

	uint8_t* temp_msg = NULL;
	if(BLE_evt_msg.msg != NULL)
	{
		temp_msg = malloc(BLE_evt_msg.msg_len);
		if(temp_msg == NULL)
		{
			//printf("malloc error : BLE Peripheral Data Received \r\n");
			return -1;
		}
		else
		{
			//printf("malloc processing_BLE_Peripheral_Data_Received\r\n");
		}

		memcpy(temp_msg, BLE_evt_msg.msg, BLE_evt_msg.msg_len);
	}

	return LAP_event_send(LAP_PERIPHERAL_EVT, LAP_PERIPHERAL_ST_DATA_RECEIVED, BLE_evt_msg.conn_handle,
													BLE_evt_msg.handle, BLE_evt_msg.msg_len, temp_msg);
}

static void processing_BLE_Peripheral_CCCD_Enabled(BLEPEvt_msgt BLE_evt_msg)
{
	LAP_event_send(LAP_PERIPHERAL_EVT, LAP_PERIPHERAL_ST_CCCD_ENABLED, BLE_evt_msg.conn_handle,
														BLE_evt_msg.handle, BLE_evt_msg.msg_len, NULL);
}

static void processing_BLE_Peripheral_ADV_Start(BLEPEvt_msgt BLE_evt_msg)
{
	PAAR_adv_start();
}

static void processing_BLE_Peripheral_ADV_Stop(BLEPEvt_msgt BLE_evt_msg)
{
	PAAR_adv_stop();
}

static void processing_BLE_Peripheral_Data_Send(BLEPEvt_msgt BLE_evt_msg)
{
	PAAR_send_ble_msg_peripheral(BLE_evt_msg.msg, BLE_evt_msg.msg_len);
}

bool is_Target_adv_packet(LAP_ble_adv_report* pPkt)
{

	if(pPkt->data_len < LAP_ADV_DATA_LEN)
		return false;

	if( memcmp(&ble_target_paar_id.scan_target_paar_id[0],&pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID0], 4) == 0)
	{
		return true;
	}else{
		return false;
	}

	return false;
}

LAP_ble_adv_report* PAAR_get_ble_scan_target_result()
{
	return &scan_target_result;
}

uint32_t BLE_check_disconnect_role()
{
	return 0;
}

//process BLE central event
static void processing_BLE_Central_event(BLEPEvt_msgt BLE_evt_msg)
{
	switch(BLE_evt_msg.status)
	{
	//BLE central event
	case BLE_CENTRAL_ST_SCAN_TIMEOUT :
		processing_BLE_Central_Scan_Timeout(BLE_evt_msg);
		break;
	case BLE_CENTRAL_ST_CONNECTION_TIMEOUT :
		processing_BLE_Central_Conn_Timeout(BLE_evt_msg);
		break;
	case BLE_CENTRAL_ST_CONNECTED :
		processing_BLE_Central_Connected(BLE_evt_msg);
		break;
	case BLE_CENTRAL_ST_DISCONNECTED :
		processing_BLE_Central_Disconnected(BLE_evt_msg);
		break;
	case BLE_CENTRAL_ST_DATA_RECEIVED :
		processing_BLE_Central_Data_Received(BLE_evt_msg);
		break;

	//BLE central command
	case BLE_CENTRAL_CMD_SCAN_START :
		processing_BLE_Central_Scan_Start(BLE_evt_msg);
		break;
	case BLE_CENTRAL_CMD_CONNECT :
		processing_BLE_Central_Connect_Target(BLE_evt_msg);
		break;
	case BLE_CENTRAL_CMD_DISCONNECT :
		processing_BLE_Central_Disconnect_Target(BLE_evt_msg);
		break;
	case BLE_CENTRAL_CMD_SEND_MSG :
		processing_BLE_Central_Data_Send(BLE_evt_msg);
		break;
	}
}

//process BLE peripheral event
static void processing_BLE_Peripheral_event(BLEPEvt_msgt BLE_evt_msg)
{
	switch(BLE_evt_msg.status)
	{
	//BLE Peripheral event
	case BLE_PERIPHERAL_ST_CONNECTED :
		processing_BLE_Peripheral_Connected(BLE_evt_msg);
		break;
	case BLE_PERIPHERAL_ST_DISCONNECTED :
		processing_BLE_Peripheral_Disconnected(BLE_evt_msg);
		break;
	case BLE_PERIPHERAL_ST_DATA_RECEIVED :
		processing_BLE_Peripheral_Data_Received(BLE_evt_msg);
		break;
	case BLE_PERIPHERAL_ST_CCCD_ENABLED :
		processing_BLE_Peripheral_CCCD_Enabled(BLE_evt_msg);
		break;

	//BLE Peripheral command
	case BLE_PERIPHERAL_CMD_ADV_START :
		processing_BLE_Peripheral_ADV_Start(BLE_evt_msg);
		break;
	case BLE_PERIPHERAL_CMD_ADV_STOP :
		processing_BLE_Peripheral_ADV_Stop(BLE_evt_msg);
		break;
	case BLE_PERIPHERAL_CMD_SEND_MSG :
		processing_BLE_Peripheral_Data_Send(BLE_evt_msg);
		break;
	}
}

static void BLE_Process_task(void* arg){
	int r;
	BLEPEvt_msgt BLE_process_evt_msg;

	ble_stack_init_wait();

	for (;;) {
		r = msgq_receive(BLE_process_msgq, (unsigned char*) &BLE_process_evt_msg);
		if (0 != r) {
			logme("fail at msgq_receive\r\n");
		} else {
			switch( BLE_process_evt_msg.event ){
			case BLE_CENTRAL_EVT :
				processing_BLE_Central_event(BLE_process_evt_msg);
				break;
			case BLE_PERIPHERAL_EVT :
				processing_BLE_Peripheral_event(BLE_process_evt_msg);
				break;
			}

			if( BLE_process_evt_msg.msg != NULL ){
				free(BLE_process_evt_msg.msg);
			}
		}
	}
}

void BLE_process_task_init(void){
	int r;

	r = msgq_create(&BLE_process_msgq, sizeof(BLEPEvt_msgt), 20);
	if (0 != r) {
		printf("fail at msgq create\r\n");
	}

	r = task_create(NULL, BLE_Process_task, NULL, task_gethighestpriority()-1, 512, NULL);
	if (r != 0) {
		printf("== LAP_main_task failed \n\r");
	} else {
		printf("== LAP_main_task created \n\r");
	}
}
#endif /* APPLICATION_PAAR_FRONTEND_EXE_PAAR_FRONTEND_SRC_BLE_PROCESS_C_ */
