/*
 * sw_config.h
 *
 *  Created on: 2020. 06. 11.
 *      Author: YJPark
 */

#ifndef APPLICATION_GASTAG_EXE_GASTAG_ITF_SW_CONFIG_H_
#define APPLICATION_GASTAG_EXE_GASTAG_ITF_SW_CONFIG_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "SEGGER_RTT.h"

//software mode = Smart Plug Hub
#define SP_SW_MODE_SPH						0
#define SP_SW_MODE_TEST_CENTRAL				1
#define SP_SW_MODE_TEST_PERIPHERAL			2

#define SP_SW_MODE_SETUP					SP_SW_MODE_SPH


//Test code : Demo
#define SP_TEST_LOCATION_ROOM		    1
#define SP_TEST_LOCATION_LIVINGROOM		2
#define SP_TEST_LOCATION_TOILET		    3

#define SP_TEST_LOCATION				SP_TEST_LOCATION_LIVINGROOM

//Test Information
#if(SP_TEST_LOCATION == SP_TEST_LOCATION_LIVINGROOM)
#define SH_ID "AB001309"
#define SP_ID "020001D4"
#define SP_LOCATION	"Professor_ROOMA"

#define TEST_MQTT_CLIENT_ID		"test_ADL_2"

#define TEST_MQTT_USER_NAME		"Test_User2"
#define TEST_MQTT_USER_PASSWORD	"testtest"

#define TEST_MQTT_TOPIC_PUBLISH			"CSOS/AB001309/020001D4/ADL_DATA"
#define TEST_MQTT_TOPIC_SUBSCRIBE		"sensorID"


#elif(SP_TEST_LOCATION == SP_TEST_LOCATION_TOILET)
#define SH_ID "AB001309"
#define SP_ID "020000D4"
#define SP_LOCATION	"livingroom"
#endif

#if(SP_SW_MODE_SETUP == SP_SW_MODE_SPH)
#define TEST_MAX_CONNECTION_DEVICE		2
#elif(SP_SW_MODE_SETUP == SP_SW_MODE_TEST_CENTRAL || SP_SW_MODE_SETUP == SP_SW_MODE_TEST_PERIPHERAL)
#define TEST_MAX_CONNECTION_DEVICE		1
#endif

#if(SP_SW_MODE_SETUP == SP_SW_MODE_TEST_CENTRAL || SP_SW_MODE_SETUP == SP_SW_MODE_TEST_PERIPHERAL)
// Definition : PAAR ID(4 Byte)
#define PAAR_ID_0					(0x01)
#define PAAR_ID_1					(0x00)
#define PAAR_ID_2					(0x00)
#define PAAR_ID_3					(0xE4) //Device ID : Smart Plug Hub
#elif(SP_SW_MODE_SETUP == SP_SW_MODE_SPH)
#define PAAR_ID_0					0x04
#define PAAR_ID_1					0x00
#define PAAR_ID_2					0x01
#define PAAR_ID_3				    0xD4 //Device ID : Smart Plug Hub

#define PAAR_ID_STR					"020001D4"

#endif

#define TEST_TARGET_PAAR_ID_SIZE	3

#define TEST_SEND_MSG_DELAY			500
#define TEST_SCAN_START_DELAY		500
#define TEST_ADV_START_DELAY		500
#define TEST_CONNECTION_DELAY		100

/**< Name of device. Will be included in the advertising data. */
#define PAAR_DEVICE_NAME                	"SP_Hub"

#endif /* APPLICATION_SW_CONFIG_H_ */
