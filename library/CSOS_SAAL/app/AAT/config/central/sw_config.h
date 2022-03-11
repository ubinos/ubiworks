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

#define SP_SW_MODE_SETUP					SP_SW_MODE_TEST_CENTRAL


//Test code : Demo
#define SP_TEST_LOCATION_BEDROOM		1
#define SP_TEST_LOCATION_LIVINGROOM		2

#define SP_TEST_LOCATION				SP_TEST_LOCATION_BEDROOM

//Test Information
#if(SP_TEST_LOCATION == SP_TEST_LOCATION_BEDROOM)
#define SH_ID "AB001309"
#define SP_ID "010000D4"
#define SP_LOCATION	"bedroom"
#elif(SP_TEST_LOCATION == SP_TEST_LOCATION_LIVINGROOM)
#define SH_ID "AB001309"
#define SP_ID "020000D4"
#define SP_LOCATION	"livingroom"
#endif

#define TEST_MAX_CONNECTION_DEVICE		1

// Definition : PAAR ID(4 Byte)
#define PAAR_ID_0					(0x01)
#define PAAR_ID_1					(0x00)
#define PAAR_ID_2					(0x00)
#define PAAR_ID_3					(0xE4) //Device ID : Smart Plug Hub

#define TEST_TARGET_PAAR_ID_SIZE	4

#define TEST_SEND_MSG_DELAY			500
#define TEST_SCAN_START_DELAY		500
#define TEST_ADV_START_DELAY		500
#define TEST_CONNECTION_DELAY		100

/**< Name of device. Will be included in the advertising data. */
#define PAAR_DEVICE_NAME                	"SP_Hub"

#endif /* APPLICATION_SW_CONFIG_H_ */
