/*
 * LAP_api.h
 *
 *  Created on: 2020. 06. 11.
 *      Author: YJPark
 */

#ifndef APPLICATION_SPB_EXE_SPB_SRC_LAP_API_H_
#define APPLICATION_SPB_EXE_SPB_SRC_LAP_API_H_

#include <lib_bluetooth_csos/ble_process.h>
#include <lib_bluetooth_csos/ble_stack.h>
#include <stdint.h>
#include <stdlib.h>

#include "ble_gap.h"
#include "ble.h"
#include "sw_config.h"

#define PAAR_ID_SIZE	4
#define PAAR_SERVICE_ID_SIZE	1
#define PAAR_PACKET_HEADER_LEN						4

#define PAAR_PACKET_INDEX_PACKET_TYPE				0
#define PAAR_PACKET_INDEX_SERVICE_ID				1
#define PAAR_PACKET_INDEX_SEQUENCE_NUM				2
#define PAAR_PACKET_INDEX_DATA_LEN					3
#define PAAR_PACKET_INDEX_BODY_DATA_CMD				4
#define PAAR_PACKET_INDEX_BODY_DATA_BODY			5

#define PAAR_PACKET_ACK_LEN							5
#define PAAR_PACKET_ACK_DATA_LEN					1

/* status byte in Advertising packet payloads */
#define LIDx_STATUS								0x00
#define PINP_STATUS								0x01
#define REQ_CONN_SOSP_STATUS					0x02
#define REQ_CONN_SMARTDEVICE_STATUS				0x04
#define REQ_LOCAION_ADV							0x08
#define SLIMHUB_INFO_ADV						0x10
#define REQ_CONN_SOSP_WO_MAC					0x40

/**
 * see @ref LAP_ADV_IDX_STATUS
 */
#define LAP_ADV_STATUS_BYTE_LIDX				0x00
#define LAP_ADV_STATUS_BYTE_PNIP				0x01
#define LAP_ADV_STATUS_BYTE_AMD					0x02

#define LAP_ADV_STATUS_BYTE_EC					0x03
#define LAP_ADV_STATUS_BYTE_INOUT				0x04

#define LAP_ADV_STATUS_BYTE_LOCATION_REQ		0x08
#define LAP_ADV_STATUS_BYTE_SLIMHUB_INFO		0x10
#define LAP_ADV_STATUS_BYTE_CONN_SOSP_WO_MAC	0x40

#define DEFAULT_LAP_ADV_LF_RSSI_VALUE			0x00

/**
 * LAP Advertising Packet Index N
 */
#define LAP_ADV_IDX_EAH0_LENGTH				0
#define LAP_ADV_IDX_EAH1_LENGTH				3
#define LAP_ADV_IDX_STATUS					7
#define LAP_ADV_IDX_PAAR_DEVICE_ID0			8
#define LAP_ADV_IDX_PAAR_DEVICE_ID1			9
#define LAP_ADV_IDX_PAAR_DEVICE_ID2			10
#define LAP_ADV_IDX_PAAR_DEVICE_ID3			11
#define LAP_ADV_IDX_LF_RSSI					12

#if 0
#define LAP_ADV_IDX_ANCHOR_NODE_MAC0		13
#define LAP_ADV_IDX_ANCHOR_NODE_MAC1		14
#define LAP_ADV_IDX_ANCHOR_NODE_MAC2		15
#define LAP_ADV_IDX_ANCHOR_NODE_MAC3		16
#define LAP_ADV_IDX_ANCHOR_NODE_MAC4		17
#define LAP_ADV_IDX_ANCHOR_NODE_MAC5		18
#else
#define LAP_ADV_IDX_LF_SERVICE_ID			13
#define LAP_ADV_IDX_LF_SERVICE_DATA			14
#endif

#define LAP_ADV_IDX_NOTIFICATION_HANDLE		19
#define LAP_ADV_IDX_WRITE_REQUEST_HANDLE	21
#define LAP_ADV_IDX_NOTI_EN_HANDLE			23

#define LAP_DEVICE_TYPE_PAAR_BAND			0x58
/**
 * LAP Message Packet Index N
 */
#define MSG_IDX_PACKET_TYPE			0
#define MSG_IDX_SERVICE_ID			1
#define MSG_IDX_SEQ_NUM				2
#define MSG_IDX_DATA_LEN			3
#define MSG_IDX_CMD					4
#define MSG_IDX_DATA				5

#define DEFAULT_MAX_SCAN_RES				10

#define LAP_ADV_DATA_LEN	25
#define LAP_SCAN_RSP_DATA_LEN	18

typedef struct {
	uint16_t tx_handle;
	uint16_t rx_handle;
	uint16_t cccd_handle;
} uuidhandle;

typedef struct {

	ble_gap_addr_t 	gap_addr;
	uint32_t 		paarID;		// peer device 's device id (4 byte, copyright CSOS)

	uint8_t 		status_byte;
	uint8_t 		LF_dist;
	uint8_t 		RF_rssi;

	uuidhandle 		profile_data;
	bool 			scan_check;
} scanDevRev_t;

typedef struct {
	uint16_t 		connHandle;
	ble_gap_addr_t 	peer_addr;

	uuidhandle 		peer_profile;
	bool			is_connected;

	uint32_t 		peer_paarID;
	uint8_t			device_type;
	uint8_t 		service_type;
} conn_info_t;


uint8_t get_scanDevCnt(void);
void set_scanDevCnt(uint8_t val);

scanDevRev_t* get_scanDevList(void) ;
void clear_scanDevList(void);

conn_info_t * get_connInfo(void);
void clear_connInfo(void);

void add_List_LAP_advdata(ble_gap_evt_adv_report_t* pkt);

conn_info_t* get_LAP_connInfoList(void);

//Start BLE advertising : LIDx
void LAP_start_ble_adv_LIDx();

//Stop BLE advertising : LIDx
void LAP_stop_ble_adv_LIDx();

//Start BLE Scan
void LAP_start_ble_scan(uint8_t* target_paar_id);
//Start BLE connection(Central Role Mode)
void LAP_start_ble_connect(ble_gap_evt_adv_report_t* adv_data);
//Start BLE disconnection(Cenral Role Mode)
void LAP_start_ble_disconnect(uint16_t conn_handle);
//Save UUID handle from ble advertising data(PAAR)
void LAP_save_uuid_handle(ble_gap_evt_adv_report_t* pPkt, paar_uuidhandle* uuid_handle);
//Send ble msg : Central Connection
int LAP_send_ble_msg_central(uint16_t conn_handle, uint16_t handle, uint8_t* msg, uint8_t msg_len);

//Checking LAP ADV Packet
bool is_LAP_adv_packet(LAP_ble_adv_report* pPkt);

//Checking LAP ADV Packet : Location Request
int process_LAP_location_request_packet(LAP_ble_adv_report* pPkt);

void decrease_temp_adv_buffer_count();

//send ble peripheral message : PAAR profile
int LAP_send_ble_msg_peripheral(uint8_t* msg, uint8_t msg_len);
#endif /* APPLICATION_SPB_EXE_SPB_SRC_LAP_API_H_ */
