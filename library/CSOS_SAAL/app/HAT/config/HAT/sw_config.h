/*
 * sw_config.h
 *
 *  Created on: 2020. 06. 11.
 *      Author: YJPark
 */

#ifndef APP_CONFIG_SW_CONFIG_H_
#define APP_CONFIG_SW_CONFIG_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define TEST_MAX_CONNECTION_DEVICE		3

#define HAT_SW_MODE_TEST_CENTRAL		0
#define HAT_SW_MODE_TEST_PERIPHERAL		1

#define HAT_SW_MODE_SETUP				HAT_SW_MODE_TEST_PERIPHERAL


#define ADL_DEVICE_TYPE_WINDOW				0x01
#define ADL_DEVICE_TYPE_DRAWER				0x02
#define ADL_DEVICE_TYPE_DOOR				0x03
#define ADL_DEVICE_TYPE_COOKING_UTENSIL		0x04
#define ADL_DEVICE_TYPE_REMOTE_CONTROL		0x05
#define ADL_DEVICE_TYPE_CLEANING_SUPPLY		0x06
#define ADL_DEVICE_TYPE_REFRIGERATOR		0x07
#define ADL_DEVICE_TYPE_TV					0x08
#define ADL_DEVICE_TYPE_COFFEE_PORT			0x09
#define ADL_DEVICE_TYPE_ELECTRIC_FAN		0x0A
#define ADL_DEVICE_TYPE_HAIR_DRYER			0x0B
#define ADL_DEVICE_TYPE_VACCUM_CLEANER		0x0C
#define ADL_DEVICE_TYPE_TASMOTOR			0x0D
#define ADL_DEVICE_TYPE_MICROWAVE			0x0E
#define ADL_DEVICE_TYPE_PC					0x0F
#define ADL_DEVICE_TYPE_TOASTER				0x10
#define ADL_DEVICE_TYPE_ELECTRIC_HEATER		0x11
#define ADL_DEVICE_TYPE_FAUCET				0x12

#define TEST_DEVICE_TYPE_ELECTRIC_FAN		1
#define TEST_DEVICE_TYPE_HAIR_DRYER			2
#define TEST_DEVICE_TYPE_VACUUM_CLEAR		3

#define TEST_DEVICE_TYPE				TEST_DEVICE_TYPE_HAIR_DRYER

#if(TEST_DEVICE_TYPE == TEST_DEVICE_TYPE_ELECTRIC_FAN)
// Definition : PAAR ID(4 Byte)
#define PAAR_ID_0					(0x02)
#define PAAR_ID_1					(0x00)
#define PAAR_ID_2					(0x01)
#define PAAR_ID_3					(0xD8)

#define TEST_HOME_APPLIANCE_ID0				ADL_DEVICE_TYPE_ELECTRIC_FAN
#define TEST_HOME_APPLIANCE_ID1				0x01

#elif(TEST_DEVICE_TYPE == TEST_DEVICE_TYPE_HAIR_DRYER)
// Definition : PAAR ID(4 Byte)
#define PAAR_ID_0					(0x02)
#define PAAR_ID_1					(0x00)
#define PAAR_ID_2					(0x01)
#define PAAR_ID_3					(0xD8)

#define TEST_HOME_APPLIANCE_ID0				ADL_DEVICE_TYPE_MICROWAVE
#define TEST_HOME_APPLIANCE_ID1				0x01

#elif(TEST_DEVICE_TYPE == TEST_DEVICE_TYPE_VACUUM_CLEAR)
// Definition : PAAR ID(4 Byte)
#define PAAR_ID_0					(0x01)
#define PAAR_ID_1					(0x00)
#define PAAR_ID_2					(0x00)
#define PAAR_ID_3					(0xD8)

#define TEST_HOME_APPLIANCE_ID0				ADL_DEVICE_TYPE_VACCUM_CLEANER
#define TEST_HOME_APPLIANCE_ID1				0x01

#endif

#define PAAR_ID_0_FOR_APP			0x88

#define TEST_TARTGET_PAAR_ID_SIZE	4

#define TEST_TARTGET_PAAR_ID0	0x11
#define TEST_TARTGET_PAAR_ID1	0x17
#define TEST_TARTGET_PAAR_ID2	0x35
#define TEST_TARTGET_PAAR_ID3	0xD4

#define TEST_SEND_MSG_DELAY			500
#define TEST_SCAN_START_DELAY		500
#define TEST_ADV_START_DELAY		500
#define TEST_CONNECTION_DELAY		100
//////////////////////////////////////////////////////////
#define APP_TIMER_PRESCALER         0               /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE			2               /**< Size of timer operation queues. */
//////////////////////////////////////////////////////////
#define UNDEFINED										0XFF

// Nordic BLE stack communication enable
#define NRF_NOTIFICATION_ENABLE				0x01
#define NRF_INDICATION_ENABLE				0x02
#define NRF_NOTI_INDI_ENABLE				0x03

/* status byte in Advertising packet payloads */
#define LIDx_STATUS									0x00
#define PNIP_STATUS									0x01
#define REQ_CONN_SOSP_STATUS						0x02
#define REQ_CONN_SMARTDEVICE_STATUS					0x04
#define REQ_LIDx_STATUS								0x08

/**< Name of device. Will be included in the advertising data. */
#define PAAR_DEVICE_NAME                	"HAT_TAG"

#define PAAR_DEVICE_ID_0 				0xBB
#define PAAR_DEVICE_ID_1 				0x00
#define PAAR_DEVICE_ID_2				0x7A
#define PAAR_DEVICE_ID_3 				(PAAR_TAG_DEVICE_TYPE <<2)


//software mode = Smart Plug Hub
#define SP_SW_MODE_SPH						0

#define SP_SW_MODE_SETUP							SP_SW_MODE_SPH


//Test code : Demo
#define SP_TEST_LOCATION_BEDROOM		1
#define SP_TEST_LOCATION_LIVINGROOM		2

#define SP_TEST_LOCATION				SP_TEST_LOCATION_LIVINGROOM

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
#define HAT_SW_MODE_TEST_CENTRAL		0
#define HAT_SW_MODE_TEST_PERIPHERAL		1

#define HAT_SW_MODE_SETUP				HAT_SW_MODE_TEST_PERIPHERAL

#define ADL_DEVICE_TYPE_WINDOW				0x01
#define ADL_DEVICE_TYPE_DRAWER				0x02
#define ADL_DEVICE_TYPE_DOOR				0x03
#define ADL_DEVICE_TYPE_COOKING_UTENSIL		0x04
#define ADL_DEVICE_TYPE_REMOTE_CONTROL		0x05
#define ADL_DEVICE_TYPE_CLEANING_SUPPLY		0x06
#define ADL_DEVICE_TYPE_REFRIGERATOR		0x07
#define ADL_DEVICE_TYPE_TV					0x08
#define ADL_DEVICE_TYPE_COFFEE_PORT			0x09
#define ADL_DEVICE_TYPE_ELECTRIC_FAN		0x0A
#define ADL_DEVICE_TYPE_HAIR_DRYER			0x0B
#define ADL_DEVICE_TYPE_VACCUM_CLEANER		0x0C
#define ADL_DEVICE_TYPE_TASMOTOR			0x0D
#define ADL_DEVICE_TYPE_MICROWAVE			0x0E
#define ADL_DEVICE_TYPE_PC					0x0F
#define ADL_DEVICE_TYPE_TOASTER				0x10
#define ADL_DEVICE_TYPE_ELECTRIC_HEATER		0x11
#define ADL_DEVICE_TYPE_FAUCET				0x12

#define TEST_DEVICE_TYPE_ELECTRIC_FAN		1
#define TEST_DEVICE_TYPE_HAIR_DRYER			2
#define TEST_DEVICE_TYPE_VACUUM_CLEAR		3

#define PAAR_ID_0_FOR_APP			0x88


#define TEST_TARTGET_PAAR_ID_SIZE	4

#define TEST_TARTGET_PAAR_ID0	0x11
#define TEST_TARTGET_PAAR_ID1	0x17
#define TEST_TARTGET_PAAR_ID2	0x35
#define TEST_TARTGET_PAAR_ID3	0xD4

#define TEST_SEND_MSG_DELAY			500
#define TEST_SCAN_START_DELAY		500
#define TEST_ADV_START_DELAY		500
#define TEST_CONNECTION_DELAY		100
//////////////////////////////////////////////////////////
#define APP_TIMER_PRESCALER         0               /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE			2               /**< Size of timer operation queues. */
//////////////////////////////////////////////////////////
#define UNDEFINED										0XFF

// Nordic BLE stack communication enable
#define NRF_NOTIFICATION_ENABLE				0x01
#define NRF_INDICATION_ENABLE				0x02
#define NRF_NOTI_INDI_ENABLE				0x03

/* status byte in Advertising packet payloads */
#define LIDx_STATUS									0x00
#define PNIP_STATUS									0x01
#define REQ_CONN_SOSP_STATUS						0x02
#define REQ_CONN_SMARTDEVICE_STATUS					0x04
#define REQ_LIDx_STATUS								0x08

/**< Name of device. Will be included in the advertising data. */
#define PAAR_DEVICE_NAME                	"HAT_TAG"

#define PAAR_DEVICE_ID_0 				0xBB
#define PAAR_DEVICE_ID_1 				0x00
#define PAAR_DEVICE_ID_2				0x7A
#define PAAR_DEVICE_ID_3 				(PAAR_TAG_DEVICE_TYPE <<2)

///////////////////////////////////////////////////////////////////////////
#endif /* APPLICATION_GASTAG_EXE_GASTAG_ITF_SW_CONFIG_H_ */
