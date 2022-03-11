/*
 * BLE_cell_management.c
 *
 *  Created on: 2019. 10. 24.
 *      Author: YuJin Park
 */

#include "ble_cell_management.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "ble_gap.h"
#include "ble_stack.h"

#include "LAP_api.h"
#include "LAP_main.h"

#include <sw_config.h>


#define BLE_CELL_MANAGEMENT_MAX_CONNECTION						6
#define BLE_CELL_MANAGEMENT_DATA_MAX_COUNT						6
#define BLE_CELL_MANAGEMENT_CONN_DELAY_TIME						5
#define BLE_CELL_MANAGEMENT_DEFAULT_CONNECTION_RETRY_COUNT		3
#define BLE_CELL_MANAGEMENT_LIFE_TIME							10
#define BLE_CELL_MANAGEMENT_LIFE_TIME_CONNECTING				20
#define BLE_CELL_MANAGEMENT_HB_COUNT							5
#define BLE_CELL_MANAGEMENT_HB_PACKET_SIZE						19

// BLE Management
static uint8_t BLE_cell_management_data_count = 0;
static uint8_t BLE_cell_management_connection_count = 0;
static CellManagement_data_t BLE_cell_management_data[BLE_CELL_MANAGEMENT_DATA_MAX_COUNT];
static uint8_t current_connecting_index = 0xFF;

mutex_pt BLE_cell_mutex;

#define ENABLE_BLE_CELL_MUTEX		0

uint8_t get_BLE_cell_management_data_count()
{
	return BLE_cell_management_data_count;
}

void increase_conn_count()
{
	BLE_cell_management_connection_count++;
}

void decrease_conn_count()
{
	BLE_cell_management_connection_count--;
}

void BLE_cell_management_data_init()
{
	BLE_cell_management_data_count = 0;
	BLE_cell_management_connection_count = 0;
	memset(BLE_cell_management_data, 0, sizeof(CellManagement_data_t)*BLE_CELL_MANAGEMENT_DATA_MAX_COUNT);

	uint8_t i;
	for(i=0; i<BLE_CELL_MANAGEMENT_DATA_MAX_COUNT; i++)
	{
		BLE_cell_management_data[i].is_empty = true;
	}

	int r;
	r = mutex_create(&BLE_cell_mutex);
	if(r != 0)
	{
		printf("fail at mutex_create : BLE_cell_mutex");
	}
}

uint8_t BLE_cell_management_search_data_index(uint8_t* PAAR_id)
{
	uint8_t i;
	for(i=0; i<BLE_CELL_MANAGEMENT_DATA_MAX_COUNT; i++)
	{
		if(BLE_cell_management_data[i].is_empty == true)
			continue;

		if(memcmp(&BLE_cell_management_data[i].paarID[0], PAAR_id, PAAR_ID_SIZE) == 0)
			return i;
	}
	return 0xFF;
}

uint8_t BLE_cell_management_search_data_index_by_connhandle(uint16_t connhandle)
{
	uint8_t i;
	for(i=0; i<BLE_CELL_MANAGEMENT_DATA_MAX_COUNT; i++)
	{
		if(BLE_cell_management_data[i].conn_handle == connhandle)
			return i;
	}
	return 0xFF;
}

uint8_t* BLE_cell_management_search_paar_id_by_connhandle(uint16_t connhandle)
{
	uint8_t i;
	for(i=0; i<BLE_CELL_MANAGEMENT_DATA_MAX_COUNT; i++)
	{
		if(BLE_cell_management_data[i].conn_handle == connhandle)
		{
			if(BLE_cell_management_data[i].is_empty == false)
			{
				return (uint8_t*)&(BLE_cell_management_data[i].paarID);
			}
		}
	}

	return NULL;

}

uint8_t* BLE_cell_management_get_PAAR_ID_by_index(uint8_t index)
{
	if(BLE_cell_management_data[index].is_empty == false)
	{
		return (uint8_t*)&(BLE_cell_management_data[index].paarID);
	}
	return NULL;
}

static uint8_t BLE_cell_management_search_empty_index()
{
	uint8_t i;
//	uint8_t check_paar_id[PAAR_ID_SIZE] = {0, };
	for(i=0; i<BLE_CELL_MANAGEMENT_DATA_MAX_COUNT; i++)
	{
		if(BLE_cell_management_data[i].is_empty == true)
			return i;
	}
	return 0xFF;
}

int BLE_cell_management_data_add(LAP_ble_adv_report* pPkt)
{
#if(ENABLE_BLE_CELL_MUTEX == 1)
	mutex_lock(BLE_cell_mutex);
#endif

	if(BLE_cell_management_data_count > BLE_CELL_MANAGEMENT_DATA_MAX_COUNT)
	{
#if(ENABLE_BLE_CELL_MUTEX == 1)
		mutex_unlock(BLE_cell_mutex);
#endif
		return -1;
	}


	uint8_t temp_paar_id[4];

	memcpy(temp_paar_id, &(pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID0]), 4);

#if(BLE_SW_MODE_SETUP == BLE_SW_MODE_ADL_NEW_DEVICE_CENTRAL)
//	if(pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID3] != TEST_TARTGET_DEVICE_ID)
//		return -1;
#endif

	if(BLE_cell_management_search_data_index(&(pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID0])) == 0xFF)
	{
		uint8_t index = BLE_cell_management_search_empty_index();

		if(index == 0xFF)
		{
#if(ENABLE_BLE_CELL_MUTEX == 1)
			mutex_unlock(BLE_cell_mutex);
#endif
			return -2;
		}

		//save BLE cell Management data
		BLE_cell_management_data[index].is_empty = false;
		BLE_cell_management_data[index].connecting_state = BLE_CONNECTING_STATE_DISCONNECTED;
		BLE_cell_management_data[index].is_authourized = false;
		BLE_cell_management_data[index].life_time = BLE_CELL_MANAGEMENT_LIFE_TIME;

		/*
#if(BLE_SW_MODE_SETUP == BLE_SW_MODE_ADL_NEW_DEVICE_CENTRAL)
		BLE_cell_management_data[index].is_authourized = true;
#endif
*/
		BLE_cell_management_data[index].is_authourized = true;

		BLE_cell_management_data[index].conn_handle = PAAR_BLE_CONN_HANDLE_INVALID;
		BLE_cell_management_data[index].connection_retry_cnt = BLE_CELL_MANAGEMENT_DEFAULT_CONNECTION_RETRY_COUNT;

		memcpy(&BLE_cell_management_data[index].paarID[0], &(pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID0]), PAAR_ID_SIZE);
		memcpy(&BLE_cell_management_data[index].gap_addr, &pPkt->peer_address, sizeof(ble_gap_addr_t));

		//save attribute handle value
		BLE_cell_management_data[index].profile_data.tx_handle = (pPkt->data[LAP_ADV_IDX_NOTIFICATION_HANDLE + 1] << 8) |
											(pPkt->data[LAP_ADV_IDX_NOTIFICATION_HANDLE]);

		BLE_cell_management_data[index].profile_data.rx_handle = (pPkt->data[LAP_ADV_IDX_WRITE_REQUEST_HANDLE + 1] << 8) |
											(pPkt->data[LAP_ADV_IDX_WRITE_REQUEST_HANDLE]);

		BLE_cell_management_data[index].profile_data.cccd_handle = (pPkt->data[LAP_ADV_IDX_NOTI_EN_HANDLE + 1] << 8) |
											(pPkt->data[LAP_ADV_IDX_NOTI_EN_HANDLE]);

		BLE_cell_management_data[index].conn_delay = BLE_CELL_MANAGEMENT_CONN_DELAY_TIME;
		BLE_cell_management_data_count++;

		printf("BLE Cell managemnet add : %02X %02X %02X %02X\r\n",
				pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID0], pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID1],
				pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID2], pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID3]);
#if(ENABLE_BLE_CELL_MUTEX == 1)
		mutex_unlock(BLE_cell_mutex);
#endif
		return 0;
	}
	else
	{
		uint8_t index = BLE_cell_management_search_data_index(&(pPkt->data[LAP_ADV_IDX_PAAR_DEVICE_ID0]));
		if(BLE_cell_management_data[index].connecting_state != BLE_CONNECTING_STATE_DISCONNECTED)
			return -1;

		BLE_cell_management_data[index].life_time = BLE_CELL_MANAGEMENT_LIFE_TIME;
	}

#if(ENABLE_BLE_CELL_MUTEX == 1)
	mutex_unlock(BLE_cell_mutex);
#endif
	return -1;
}

void BLE_cell_management_set_data_authourize(uint8_t index)
{
#if(ENABLE_BLE_CELL_MUTEX == 1)
	mutex_lock(BLE_cell_mutex);
#endif
	BLE_cell_management_data[index].is_authourized = true;

	printf("BLE cell authourize : %02X %02X %02X %02X\r\n",
			BLE_cell_management_data[index].paarID[0], BLE_cell_management_data[index].paarID[1],
			BLE_cell_management_data[index].paarID[2], BLE_cell_management_data[index].paarID[3]);

#if(ENABLE_BLE_CELL_MUTEX == 1)
	mutex_unlock(BLE_cell_mutex);
#endif
}

int BLE_cell_management_data_delete(uint8_t index)
{
#if(ENABLE_BLE_CELL_MUTEX == 1)
	mutex_lock(BLE_cell_mutex);
#endif
	if(BLE_cell_management_data_count <= 0)
		return -1;

	printf("BLE cell delete : %02X %02X %02X %02X\r\n",
			BLE_cell_management_data[index].paarID[0], BLE_cell_management_data[index].paarID[1],
			BLE_cell_management_data[index].paarID[2], BLE_cell_management_data[index].paarID[3]);

	memset(&BLE_cell_management_data[index], 0, sizeof(CellManagement_data_t));

	BLE_cell_management_data[index].is_empty = true;
	BLE_cell_management_data[index].is_authourized = false;
	BLE_cell_management_data[index].connection_retry_cnt = 3;
	BLE_cell_management_data[index].life_time = 0;
	BLE_cell_management_data[index].conn_handle = PAAR_BLE_CONN_HANDLE_INVALID;
	BLE_cell_management_data[index].connecting_state = BLE_CONNECTING_STATE_DISCONNECTED;

	BLE_cell_management_data_count--;
#if(ENABLE_BLE_CELL_MUTEX == 1)
	mutex_unlock(BLE_cell_mutex);
#endif
	return 0;
}

uint8_t BLE_cell_management_check_connection(uint16_t conn_handle)
{
#if(ENABLE_BLE_CELL_MUTEX == 1)
	mutex_lock(BLE_cell_mutex);
#endif
	if(current_connecting_index != 0xFF)
	{
		BLE_cell_management_data[current_connecting_index].connecting_state = BLE_CONNECTING_STATE_CONNECTED;
		BLE_cell_management_data[current_connecting_index].conn_handle = conn_handle;
		BLE_cell_management_data[current_connecting_index].heart_beat_cnt = BLE_CELL_MANAGEMENT_HB_COUNT;

		current_connecting_index = 0xFF;
#if(ENABLE_BLE_CELL_MUTEX == 1)
		mutex_unlock(BLE_cell_mutex);
#endif
		return 0;
	}
	else
	{
		task_sleep(500);

		PAAR_ble_gap_disconnect(conn_handle);
	}
#if(ENABLE_BLE_CELL_MUTEX == 1)
	mutex_unlock(BLE_cell_mutex);
#endif
	return 0xFF;
}

int get_BLE_cell_management_profile_data_by_index(uint8_t index, uuidhandle* handle)
{

	if(index >= BLE_CELL_MANAGEMENT_DATA_MAX_COUNT)
		return -1;

	if(BLE_cell_management_data[index].is_empty == false)
	{
		memcpy(handle, &(BLE_cell_management_data[index].profile_data), sizeof(uuidhandle));
		return 0;
	}
	else
	{
		return -1;
	}

}

int get_BLE_cell_management_profile_data_by_connhandle(uint16_t connhandle, uuidhandle* handle)
{
	uint8_t target_index;

	target_index = BLE_cell_management_search_data_index_by_connhandle(connhandle);
	if(target_index == 0xFF)
		return -1;

	memcpy(handle, &(BLE_cell_management_data[target_index].profile_data), sizeof(uuidhandle));

	return 0;
}

int BLE_cell_management_current_cccd_handle(uuidhandle* handle)
{
	if(current_connecting_index != 0xFF)
	{
		memcpy(handle, &(BLE_cell_management_data[current_connecting_index].profile_data), sizeof(uuidhandle));
		return 0;
	}

	return -1;
}

uint8_t test_connecting_index = 0;

static int processing_BLE_cell_index(uint8_t index)
{
	if(BLE_cell_management_data[index].is_empty == true)
		return 0;

	//check Location authorization
	if(BLE_cell_management_data[index].is_authourized == false)
	{
		if(BLE_cell_management_data[index].conn_delay > 0)
		{
			BLE_cell_management_data[index].conn_delay--;
			return 0;
		}

//		BLE_send_msg_location_request((uint8_t*) &(BLE_cell_management_data[index].paarID) );

		BLE_cell_management_data[index].life_time--;

		if(BLE_cell_management_data[index].life_time <= 0)
			BLE_cell_management_data_delete(index);

		return 0;
	}

	if(BLE_cell_management_data[index].connecting_state == BLE_CONNECTING_STATE_CONNECTED)
	{
//		send_BLE_adv_report_dummy((uint8_t*) &(BLE_cell_management_data[index].paarID));



		BLE_cell_management_data[index].heart_beat_cnt--;

		if(BLE_cell_management_data[index].heart_beat_cnt <= 0)
		{
			uint8_t BLE_HB_packet[BLE_CELL_MANAGEMENT_HB_PACKET_SIZE];

			memset(BLE_HB_packet, 0, BLE_CELL_MANAGEMENT_HB_PACKET_SIZE);

#if(BLE_SW_CONFIG_HB_ENABLE == 1)
			uint32_t r;

			BLE_cell_management_data[index].heart_beat_cnt = BLE_CELL_MANAGEMENT_HB_COUNT;
			r = PAAR_send_ble_test_msg_central(BLE_cell_management_data[index].conn_handle,
					BLE_cell_management_data[index].profile_data.rx_handle, BLE_HB_packet, BLE_CELL_MANAGEMENT_HB_PACKET_SIZE);
			task_sleep(50);
			if(r != NRF_SUCCESS && r != NRF_ERROR_BUSY)
			{
				PAAR_ble_gap_disconnect(BLE_cell_management_data[index].conn_handle);
				BLE_cell_management_data_delete(index);
			}
#endif
		}

		return 0;
	}

	if(BLE_cell_management_data[index].connecting_state == BLE_CONNECTING_STATE_DISCONNECTED)
	{
		if(current_connecting_index != 0xFF)
			return 0;

		ble_gap_addr_t* target_address = NULL;

		target_address = (ble_gap_addr_t*) malloc(sizeof(ble_gap_addr_t));

		if(target_address == NULL)
		{
			printf("malloc error : processing_BLE_cell_index \r\n");
			return -1;
		}
		else
		{
			printf("malloc processing_BLE_cell_index\r\n");
		}


		memcpy(target_address, &(BLE_cell_management_data[index].gap_addr), sizeof(ble_gap_addr_t));

		BLE_cell_management_data[index].connecting_state = BLE_CONNECTING_STATE_CONNECTING;

		BLE_cell_management_data[index].life_time = BLE_CELL_MANAGEMENT_LIFE_TIME_CONNECTING;

		current_connecting_index = index;

		BLE_process_event_send(BLE_CENTRAL_EVT, BLE_CENTRAL_CMD_CONNECT,
				PAAR_BLE_CONN_HANDLE_INVALID, 0, sizeof(target_address), (uint8_t*) target_address);

		task_sleep(1500);

		return 0xFF;
	}

	if(BLE_cell_management_data[index].connecting_state == BLE_CONNECTING_STATE_CONNECTING)
	{
		BLE_cell_management_data[index].life_time--;

		if(BLE_cell_management_data[index].life_time <= 0)
		{
			BLE_cell_management_data_delete(index);
			current_connecting_index = 0xFF;
		}

		return 0;
	}

	return 0;

}

uint8_t processing_BLE_cell_managemnet()
{
	if(BLE_cell_management_data_count == 0)
		return 0;

	uint8_t i, result = 0;
	for(i = 0; i < BLE_CELL_MANAGEMENT_DATA_MAX_COUNT; i++)
	{
		result = processing_BLE_cell_index(i);
		if(result == 0xFF)
			return result;
	}
	return result;
}

void BLE_cell_management_connect_timeout()
{
	if(current_connecting_index < BLE_CELL_MANAGEMENT_DATA_MAX_COUNT)
	{
		BLE_cell_management_data_delete(current_connecting_index);
		current_connecting_index = 0xFF;
	}

	//BLE_cell_management_data[current_connecting_index].connecting_state = BLE_CONNECTING_STATE_DISCONNECTED;
}


