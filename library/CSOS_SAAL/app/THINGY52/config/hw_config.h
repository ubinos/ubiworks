/*
 * hw_config.h
 *
 *  Created on: 2021. 9. 15.
 *      Author: YuJin Park
 *
 */

#ifndef APPLICATION_CONFIG_HW_CONFIG_H_
#define APPLICATION_CONFIG_HW_CONFIG_H_

//#include "SAAL_hw_config.h"

#define SAAL_HW_DEVICE_SPH           0
#define SAAL_HW_DEVICE_AAT           1
#define SAAL_HW_DEVICE_HAT           2
#define SAAL_HW_DEVICE_THINGY52      3

#define SAAL_HW_DEVICE_TYPE      SAAL_HW_DEVICE_THINGY52  

#define SPH_TWI_SCL			7
#define SPH_TWI_SDA			8

#define SPH_LPS_INT			23
#define SPH_HTS_INT			24
#define SPH_LIS_INT			12

#define GRID_EYE_INT    1

#define CCS_PWR_CTRL	10
#define CCS_RESET		11
#define CCS_WAKE_UP 	12

#define THING_VDD_PWR_CTRL             30
#endif /* APPLICATION_CONFIG_HW_CONFIG_H_ */
