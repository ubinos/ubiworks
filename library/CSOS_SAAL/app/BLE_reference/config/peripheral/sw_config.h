/*
 * sw_config.h
 *
 *  Created on: 2021. 12. 16.
 *      Author: YuJin Park
 *
 */

#ifndef APPLICATION_CONFIG_SW_CONFIG_H_
#define APPLICATION_CONFIG_SW_CONFIG_H_

#define PAAR_DEVICE_NAME                		"BLE_ref" /**< Name of device. Will be included in the advertising data. */

#define BLE_REF_CENTRAL				0
#define BLE_REF_PERIPHRAL			1

#define	SW_MODE_DEFAULT			BLE_REF_PERIPHRAL

#define PAAR_ID_0					(0x01)
#define PAAR_ID_1					(0xFF)
#define PAAR_ID_2					(0xFF)
#define PAAR_ID_3					(0xE0)

#define TEST_MAX_CONNECTION_DEVICE		1

#define TEST_SEND_MSG_DELAY			500
#define TEST_SCAN_START_DELAY		500
#define TEST_ADV_START_DELAY		2000
#define TEST_CONNECTION_DELAY		100

#endif /* APPLICATION_CONFIG_SW_CONFIG_H_ */
