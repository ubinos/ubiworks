/*
 * WT_device_OPT3001.h
 *
 *  Created on: 2019. 5. 7.
 *      Author: seo
 */

#ifndef LIB_TAP_WATER_TAG_ITF_WT_DEVICE_OPT3001_H_
#define LIB_TAP_WATER_TAG_ITF_WT_DEVICE_OPT3001_H_

#include "ubinos.h"

#if (CSOS_SAAL__USE_LIB_ALS_opt3001 == 1)

#define OPT3001_I2C_ADDR1		0x44	//(0x44 << 1)
#define OPT3001_I2C_ADDR2		0x45	//(0x45 << 1)
#define OPT3001_I2C_ADDR3		0x46	//(0x46 << 1)
#define OPT3001_I2C_ADDR4		0x47	//(0x47 << 1)

#define OPT3001_REG_RESULT		0x00
#define OPT3001_REG_CONFIGURE		0x01
#define OPT3001_RANGE_OFFSET			12
#define OPT3001_RANGE_MASK			0x0fff
#define OPT3001_MODE_OFFSET			9
#define OPT3001_MODE_MASK			0xf9ff
enum opt3001_mode {
	OPT3001_MODE_SUSPEND,
	OPT3001_MODE_FORCED,
	OPT3001_MODE_CONTINUOUS,
};

#define OPT3001_REG_INT_LIMIT_LSB	0x02
#define OPT3001_REG_INT_LIMIT_MSB	0x03
#define OPT3001_REG_MAN_ID		0x7e
#define OPT3001_REG_DEV_ID		0x7f

/* OPT3001 register values */
#define OPT3001_MANUFACTURER_ID		0x5449
#define OPT3001_DEVICE_ID		0x3001

/* Min and Max sampling frequency in mHz */
#define OPT3001_LIGHT_MIN_FREQ          1250
#define OPT3001_LIGHT_MAX_FREQ          10000

#endif /*(CSOS_SAAL__USE_LIB_ALS_opt3001 == 1)*/

#endif /* LIB_TAP_WATER_TAG_ITF_WT_DEVICE_OPT3001_H_ */
