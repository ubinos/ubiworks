/*
 * WT_device_OPT3001.c
 *
 *  Created on: 2019. 5. 7.
 *      Author: seo
 */
#include "ubinos.h"

#if (CSOS_SAAL__USE_LIB_ALS_opt3001 == 1)

#include "hw_config.h"

#include "AAT_driver_i2c.h"
#include "AAT_device_OPT3001.h"

int opt3001_read_lux()
{
	int ret;
	int data;

	uint8_t read_data;
	uint8_t tx_data;

	//read sec
	//tx_data = RTC_Seconds_ADDRESS;
	AAT_i2c_read(OPT3001_I2C_ADDR1, &tx_data, 1, &read_data, 1);

	return 0;
}

#endif /*(CSOS_SAAL__USE_LIB_ALS_opt3001 == 1)*/