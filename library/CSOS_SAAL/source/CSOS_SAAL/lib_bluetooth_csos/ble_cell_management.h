/*
 * ble_cell_management.h
 *
 *  Created on: 2019. 10. 12.
 *      Author: YuJin Park
 */

#ifndef APP_BLE_CELL_MANAGEMENT_H_
#define APP_BLE_CELL_MANAGEMENT_H_

#include <LAP_api.h>
#include <stdbool.h>
#include "ble_gap.h"

//Cell management System
typedef struct {
	bool			is_empty;
	uint8_t 		connecting_state;
	bool			is_authourized;
	int8_t 			life_time;
	uint8_t 		connection_retry_cnt;
	uint8_t 		paarID[PAAR_ID_SIZE];
	ble_gap_addr_t 	gap_addr;
	uint16_t 		conn_handle;
	uuidhandle 		profile_data;
	int8_t			conn_delay;
	int8_t			heart_beat_cnt;
} CellManagement_data_t;

enum{
	BLE_CONNECTING_STATE_DISCONNECTED,
	BLE_CONNECTING_STATE_CONNECTING,
	BLE_CONNECTING_STATE_CONNECTED,
};

uint8_t BLE_cell_management_check_connection(uint16_t conn_handle);

int BLE_cell_management_data_add(LAP_ble_adv_report* pPkt);

int BLE_cell_management_data_delete(uint8_t index);

void BLE_cell_management_data_init();

uint8_t processing_BLE_cell_managemnet();

void BLE_cell_management_set_data_authourize(uint8_t index);

uint8_t BLE_cell_management_search_data_index(uint8_t* PAAR_id);

uint8_t BLE_cell_management_search_data_index_by_connhandle(uint16_t connhandle);

uint8_t* BLE_cell_management_search_paar_id_by_connhandle(uint16_t connhandle);

int BLE_cell_management_current_cccd_handle(uuidhandle* handle);

int get_BLE_cell_management_profile_data_by_index(uint8_t index, uuidhandle* handle);

int get_BLE_cell_management_profile_data_by_connhandle(uint16_t connhandle, uuidhandle* handle);

uint8_t get_BLE_cell_management_data_count();

void increase_conn_count();

void decrease_conn_count();

void BLE_cell_management_connect_timeout();

#endif /* APP_BLE_CELL_MANAGEMENT_H_ */
