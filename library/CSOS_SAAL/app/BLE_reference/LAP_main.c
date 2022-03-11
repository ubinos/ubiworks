/*
 * LAP_main.c
 *
 *  Created on: 2020. 06. 11.
 *      Author: YJPark
 */


#include "ble_cell_management.h"
#include "ble_process.h"
#include "ble_profile.h"
#include "ble_stack.h"
#include "LAP_api.h"
#include "LAP_main.h"
#include <ubinos.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <malloc.h>

#include "ble_gap.h"
#include "sw_config.h"
#include "hw_config.h"

#include "../lib_wifi_wizfi360/wiz360_uart_wifi_module.h"

uint16_t cccd_target_connhandle = BLE_CONN_HANDLE_INVALID;

static msgq_pt LAP_msgq;

static bool is_ble_peripheral_connected = false;

void set_ble_peripheral_connected_flag(bool val)
{
	is_ble_peripheral_connected = val;

	#if(BLE_DEBUGGING_LED_ENABLE == 1)
		if(val == true)
		{
			AAT_LED_ON(BLUE);
		}
		else
		{
			AAT_LED_OFF(WHITE);
		}
	#endif

}

bool get_ble_peripheral_connected_flag()
{
	return is_ble_peripheral_connected;
}

//APP_TIMER_DEF(scan_fail_timeout_timer);

static scan_target_paar_id_st ble_test_target_paar_id[TEST_MAX_CONNECTION_DEVICE];

uint32_t target_paarid[TEST_MAX_CONNECTION_DEVICE] =
{
		0x01FFFFE0,
};

void set_scan_target_paar_id_test()
{
	ble_test_target_paar_id[0].scan_target_paar_id_enable = true;

	uint8_t i;
	for(i = 0; i<TEST_MAX_CONNECTION_DEVICE; i++)
	{
		ble_test_target_paar_id[i].scan_target_paar_id[0] = target_paarid[i]>>24;
		ble_test_target_paar_id[i].scan_target_paar_id[1] = target_paarid[i]>>16;
		ble_test_target_paar_id[i].scan_target_paar_id[2] = target_paarid[i]>>8;
		ble_test_target_paar_id[i].scan_target_paar_id[3] = target_paarid[i];
	}
}

bool is_test_target_device(LAP_ble_adv_report* pPkt)
{
	if(pPkt->data_len < LAP_ADV_DATA_LEN)
		return false;

	uint8_t i;
	for(i = 0; i<TEST_MAX_CONNECTION_DEVICE; i++)
	{
		if( memcmp(&ble_test_target_paar_id[i].scan_target_paar_id[0],&pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID0], 4) == 0)
		{
			return true;
		}
	}

	return false;
}

void LAP_process_ble_adv_report(LAP_ble_adv_report* pPkt)
{
	//Do nothing...
}

static void process_LAP_scan_timeout()
{
	//Do nothing...
}

static void processing_LAP_Central_Conn_timeout(LAPEvt_msgt LAP_evt_msg)
{
	//Do nothing...
}

static void processing_LAP_Central_Scan_timeout(LAPEvt_msgt LAP_evt_msg)
{
	//Do nothing...
}

static void processing_LAP_Central_Scan_result(LAPEvt_msgt LAP_evt_msg)
{
	//Do nothing...
}

uint8_t test_send_count = 0;

#define TEST_MSG_SIZE_MIN	19
#define TEST_MSG_SIZE_MAX	240

uint8_t test_msg_size = TEST_MSG_SIZE_MIN;

static void send_test_msg_central(uint16_t conn_handle, uint16_t handle)
{
	uint8_t temp_packet[PAAR_MAXIMUM_PACKET_SIZE] = {0, };

	memset(temp_packet, 0, PAAR_MAXIMUM_PACKET_SIZE);

	temp_packet[0] = test_send_count++;
	if(test_send_count >= TEST_MSG_SIZE_MAX)
		test_send_count = 1;

	uint8_t i;
	for(i=1; i<test_msg_size; i++)
	{
		temp_packet[i] = i;
	}

	printf("BLE send msg : test_msg %d\r\n", temp_packet[0]);

	LAP_send_ble_msg_central(conn_handle, handle, temp_packet, test_msg_size);

	test_msg_size++;
	if(test_msg_size >= TEST_MSG_SIZE_MAX)
		test_msg_size = TEST_MSG_SIZE_MIN;
}

static void send_test_msg_peripheral()
{
	uint8_t temp_packet[PAAR_MAXIMUM_PACKET_SIZE] = {0, };

	memset(temp_packet, 0, PAAR_MAXIMUM_PACKET_SIZE);
	temp_packet[0] = test_send_count++;
	if(test_send_count >= TEST_MSG_SIZE_MAX)
		test_send_count = 1;

	uint8_t i;
	for(i=1; i<test_msg_size; i++)
	{
		temp_packet[i] = i;
	}

	printf("BLE send msg : test_msg %d\r\n",temp_packet[0]);

	LAP_send_ble_msg_peripheral(temp_packet, test_msg_size);

	test_msg_size++;
	if(test_msg_size >= TEST_MSG_SIZE_MAX )
		test_msg_size = TEST_MSG_SIZE_MIN;

}

static void send_cccd_handle_enable(uint16_t conn_handle, uint16_t cccd_handle)
{
	uint8_t temp_packet[2];

	temp_packet[0] = (uint8_t)NRF_NOTI_INDI_ENABLE;		// ble notification msg 데이터
	temp_packet[1] = 0x00;

	printf("BLE send msg : CCCD enable\r\n");

	LAP_send_ble_msg_central(conn_handle, cccd_handle, (uint8_t*)temp_packet, 2);

}

static void processing_LAP_Central_Connected(LAPEvt_msgt LAP_evt_msg)
{
	//Do nothing...
}

static void processing_LAP_Central_Disconnected(LAPEvt_msgt LAP_evt_msg)
{
	//Do nothing...
}

static void processing_LAP_Central_Data_Received(LAPEvt_msgt LAP_evt_msg)
{
	//Do nothing...
}

static void processing_LAP_Peripheral_Connected(LAPEvt_msgt LAP_evt_msg)
{
	
	printf("BLE Peripheral connect\r\n");
	set_ble_peripheral_connected_flag(true);
}

static void processing_LAP_Peripheral_Disconnected(LAPEvt_msgt LAP_evt_msg)
{
	task_sleep(TEST_ADV_START_DELAY);
	printf("BLE Peripheral disconnect\r\n");

	set_ble_peripheral_connected_flag(false);

	task_sleep(1000);

	printf("BLE ADV start\r\n");
	LAP_start_ble_adv_LIDx();
}

static void processing_LAP_Peripheral_Data_Received(LAPEvt_msgt LAP_evt_msg)
{
	printf("BLE receive msg : ");
	uint8_t i;

	task_sleep(100);

	for(i=0; i<LAP_evt_msg.msg_len; i++)
	{
		printf("%02X ", LAP_evt_msg.msg[i]);
		task_sleep(10);
	}
	printf("\r\n");

//	send_test_msg_peripheral();
}

static void processing_LAP_Peripheral_CCCD_Enabled(LAPEvt_msgt LAP_evt_msg)
{
	printf("BLE CCCD is enabled. \r\n");

	task_sleep(100);
}

static void processing_LAP_Central_event(LAPEvt_msgt LAP_evt_msg)
{
	switch(LAP_evt_msg.status)
	{
	case LAP_CENTRAL_ST_SCAN_TIMEOUT :
		processing_LAP_Central_Scan_timeout(LAP_evt_msg);
		break;
	case LAP_CENTRAL_ST_CONN_TIMEOUT :
		processing_LAP_Central_Conn_timeout(LAP_evt_msg);
		break;
	case LAP_CENTRAL_ST_SCAN_RESULT :
		processing_LAP_Central_Scan_result(LAP_evt_msg);
		break;
	case LAP_CENTRAL_ST_CONNECTED :
		processing_LAP_Central_Connected(LAP_evt_msg);
		break;
	case LAP_CENTRAL_ST_DISCONNECTED :
		processing_LAP_Central_Disconnected(LAP_evt_msg);
		break;
	case LAP_CENTRAL_ST_DATA_RECEIVED :
		processing_LAP_Central_Data_Received(LAP_evt_msg);
		break;
	}
}

static void processing_LAP_Peripheral_event(LAPEvt_msgt LAP_evt_msg)
{
	switch(LAP_evt_msg.status)
	{
	case LAP_PERIPHERAL_ST_CONNECTED :
		processing_LAP_Peripheral_Connected(LAP_evt_msg);
		break;
	case LAP_PERIPHERAL_ST_DISCONNECTED :
		processing_LAP_Peripheral_Disconnected(LAP_evt_msg);
		break;
	case LAP_PERIPHERAL_ST_DATA_RECEIVED :
		processing_LAP_Peripheral_Data_Received(LAP_evt_msg);
		break;
	case LAP_PERIPHERAL_ST_CCCD_ENABLED :
		processing_LAP_Peripheral_CCCD_Enabled(LAP_evt_msg);
		break;
	}
}

void processing_LAP_LIDx_event(LAPEvt_msgt LAP_evt_msg)
{

}

void processing_LAP_PNIP_event(LAPEvt_msgt LAP_evt_msg)
{

}

void processing_LAP_AMD_event(LAPEvt_msgt LAP_evt_msg)
{

}

//Process Other BLE Events
//**interrupt에 의해 호출되는 handler이므로 malloc을 사용할수 없음
void process_ADV_Report(LAP_ble_adv_report* pPkt)
{
	//Scan 전 연결 대상을 지정했을 경우, 해당 대상(paar id) 확인 시, Scan 종료 후 LAP Task로 정보 전달(LAP_CENTRAL_ST_SCAN_RESULT)

	//test_code : 연결할 Test 타겟 검사
	if(is_test_target_device(pPkt) == false)
		return;

	//checking PAAR ADV data...
	if(is_LAP_adv_packet(pPkt) == true)
	{
		//process LAP status byte
		switch(pPkt->data[LAP_ADV_IDX_STATUS])
		{
		case LAP_ADV_STATUS_BYTE_LIDX :
			break;
		case LAP_ADV_STATUS_BYTE_PNIP :
			break;
		case LAP_ADV_STATUS_BYTE_AMD :
			break;
		case LAP_ADV_STATUS_BYTE_EC :
			break;
		case LAP_ADV_STATUS_BYTE_INOUT :
			break;
		case LAP_ADV_STATUS_BYTE_LOCATION_REQ :
				process_LAP_location_request_packet(pPkt);
			break;
		case LAP_ADV_STATUS_BYTE_SLIMHUB_INFO :
			break;
		case LAP_ADV_STATUS_BYTE_CONN_SOSP_WO_MAC :
			break;
		}
	}
}

void LAP_Protocol_start_operation()
{
	task_sleep(TEST_ADV_START_DELAY);
	LAP_start_ble_adv_LIDx();
}

void scan_fail_timer_handler()
{
	LAP_start_ble_scan(NULL);

//	app_timer_start(scan_fail_timeout_timer, APP_TIMER_TICKS(5000), NULL);
}

void LAP_main_task(void* arg){
	int r;
	LAPEvt_msgt LAP_evt_msg;

	ble_stack_init_wait();

	BLE_cell_management_data_init();

	LAP_Protocol_start_operation();

	for (;;) {
		r = msgq_receive(LAP_msgq, (unsigned char*) &LAP_evt_msg);
		if (0 != r) {
			logme("fail at msgq_receive\r\n");
		} else {
			switch( LAP_evt_msg.event ){
			case LAP_CENTRAL_EVT :
				processing_LAP_Central_event(LAP_evt_msg);
				break;
			case LAP_PERIPHERAL_EVT :
				processing_LAP_Peripheral_event(LAP_evt_msg);
				break;
				
			case LAP_LIDx_EVT :
				processing_LAP_LIDx_event(LAP_evt_msg);
				break;
			case LAP_PNIP_EVT :
				processing_LAP_PNIP_event(LAP_evt_msg);
				break;
			case LAP_AMD_EVT :
				processing_LAP_AMD_event(LAP_evt_msg);
				break;
			}

			if( LAP_evt_msg.msg != NULL ){
				free(LAP_evt_msg.msg);
			}
		}
	}
}

void LAP_main_task_init(void){
	int r;

	r = msgq_create(&LAP_msgq, sizeof(LAPEvt_msgt), 20);
	if (0 != r) {
		printf("fail at msgq create\r\n");
	}

	r = task_create(NULL, LAP_main_task, NULL, task_gethighestpriority()-2, 1024, NULL);
	if (r != 0) {
		printf("== LAP_main_task failed \n\r");
	} else {
		printf("== LAP_main_task created \n\r");
	}
}


int LAP_event_send(uint8_t evt, uint8_t state, uint16_t conn_handle, uint16_t handle,
															uint32_t msg_len, uint8_t* msg)
{
	LAPEvt_msgt lap_msg;

	lap_msg.event = evt;
	lap_msg.status = state;
	lap_msg.handle = handle;
	lap_msg.conn_handle = conn_handle;
	lap_msg.msg_len = msg_len;
	lap_msg.msg = msg;

	int r;
	r = msgq_send(LAP_msgq, (unsigned char*) &lap_msg);
	if(r != 0)
	{
		printf("LAP event send error : %d\r\n", r);
	}

	return r;
}
