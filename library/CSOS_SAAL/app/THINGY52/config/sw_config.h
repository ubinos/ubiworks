/*
 * sw_config.h
 *
 *  Created on: 2021. 9. 15.
 *      Author: YuJin Park
 *
 */

#ifndef APPLICATION_CONFIG_SW_CONFIG_H_
#define APPLICATION_CONFIG_SW_CONFIG_H_

#define PAAR_DEVICE_NAME                		"AA_TAG" /**< Name of device. Will be included in the advertising data. */

/*
enum{
	TEST_DEVICE_TYPE_FAUSET = 0,
	TEST_DEVICE_TYPE_DRAWER,
	TEST_DEVICE_TYPE_REFRIGATOR,
};

enum{
	AAT_SW_MODE_ACC,
	AAT_SW_MODE_GYRO,
};

#define TEST_DEVICE_TYPE				AAT_SW_MODE_ACC

#if(TEST_DEVICE_TYPE == TEST_DEVICE_TYPE_FAUSET)
#define AAT_SW_MODE				AAT_SW_MODE_ACC
#elif(TEST_DEVICE_TYPE == TEST_DEVICE_TYPE_DRAWER || TEST_DEVICE_TYPE == TEST_DEVICE_TYPE_REFRIGATOR)
#define AAT_SW_MODE				AAT_SW_MODE_GYRO
#endif
*/

#define SAAL_DEFAULT_LOCATION_INFO			"테스트룸A_거실_부엌"
//#define SAAL_DEFAULT_LOCATION_INFO			"테스트룸A_화장실"
//#define SAAL_DEFAULT_LOCATION_INFO			"테스트룸A_방1"

#define PAAR_ID_0					(0x01)
#define PAAR_ID_1					(0x09)
#define PAAR_ID_2					(0x09)
#define PAAR_ID_3					(0xE0)

/*
#define ADL_DEVICE_TYPE0	0x04
#define ADL_DEVICE_TYPE1	0x01

#define AAT_SERVICE_ID					0x12

//software mode = AAT_SW_MODE_NORMAL
#define AAT_SW_MODE_NORMAL						0

#define SP_SW_MODE_SETUP							AAT_SW_MODE_NORMAL
*/

#define TEST_MAX_CONNECTION_DEVICE		3

#define TEST_SEND_MSG_DELAY			500
#define TEST_SCAN_START_DELAY		500
#define TEST_ADV_START_DELAY		2000
#define TEST_CONNECTION_DELAY		100

#endif /* APPLICATION_CONFIG_SW_CONFIG_H_ */
