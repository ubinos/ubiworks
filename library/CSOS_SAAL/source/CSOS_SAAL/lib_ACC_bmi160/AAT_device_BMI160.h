/*
 * AAT_device_BMA250.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: Kwon
 */

#ifndef APPLICATION_PAARTAG_EXE_PAARTAG_SRC_AAT_DEVICE_BMI160_H_
#define APPLICATION_PAARTAG_EXE_PAARTAG_SRC_AAT_DEVICE_BMI160_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#if (CSOS_SAAL__USE_LIB_ACC_bmi160 == 1)

/************************************************/
/**\name	ADDRESS DEFINITION       */
/***********************************************/
/** 1 Pin - (SDO - Address select in I2C mode)
#define BMI160_I2C_ADDR1                    (0x68)
/**< I2C Address needs to be changed */
//#define BMI160_I2C_ADDR2                    (0x69)
/**< I2C Address needs to be changed */

/************************************************/
/**\name	REGISTER ADDRESS DEFINITION       */
/***********************************************/
/**\name CHIP ID */
/*******************/
#define BMI160_CHIP_ID_ADDR		           		(0x00)
/*******************/
/**\name ERROR STATUS */
/*******************/
#define BMI160_ERROR_ADDR	      						(0X02)
/*******************/
/**\name POWER MODE STATUS */
/*******************/
#define BMI160_PMU_STAT_ADDR       					(0X03)
/*******************/
/**\name GYRO DATA REGISTERS */
/*******************/
#define BMI160_GYRO_X_LSB_ADDR 				     	(0X0C)
#define BMI160_GYRO_X_MSB_ADDR 				     	(0X0D)
#define BMI160_GYRO_Y_LSB_ADDR 				     	(0X0E)
#define BMI160_GYRO_Y_MSB_ADDR			        (0X0F)
#define BMI160_GYRO_Z_LSB_ADDR 			        (0X10)
#define BMI160_GYRO_Z_MSB_ADDR 			        (0X11)
/*******************/
/**\name ACCEL DATA REGISTERS */
/*******************/
#define BMI160_ACCEL_X_LSB_ADDR    		      (0X12)
#define BMI160_ACCEL_X_MSB_ADDR        			(0X13)
#define BMI160_ACCEL_Y_LSB_ADDR            	(0X14)
#define BMI160_ACCEL_Y_MSB_ADDR  						(0X15)
#define BMI160_ACCEL_Z_LSB_ADDR         		(0X16)
#define BMI160_ACCEL_Z_MSB_ADDR   					(0X17)
/*******************/
/**\name SENSOR TIME REGISTERS */
/*******************/
#define BMI160_SENSORTIME_0_ADDR    				(0X18)
#define BMI160_SENSORTIME_1_ADDR    				(0X19)
#define BMI160_SENSORTIME_2_ADDR    				(0X1A)
/*******************/
/**\name STATUS REGISTER FOR SENSOR STATUS FLAG */
/*******************/
#define BMI160_STATUS_ADDR					    		(0X1B)
/*******************/
/**\name INTERRUPY STATUS REGISTERS */
/*******************/
#define BMI160_INTR_STAT_0_ADDR			        (0X1C)
#define BMI160_INTR_STAT_1_ADDR			        (0X1D)
#define BMI160_INTR_STAT_2_ADDR	        		(0X1E)
#define BMI160_INTR_STAT_3_ADDR     				(0X1F)
/*******************/
/**\name TEMPERATURE REGISTERS */
/*******************/
#define BMI160_TEMPERATURE_LSB_ADDR    			(0X20)
#define BMI160_TEMPERATURE_MSB_ADDR         (0X21)
/*******************/
/**\name FIFO REGISTERS */
/*******************/
#define BMI160_FIFO_LENGTH_LSB_ADDR    			(0X22)
#define BMI160_FIFO_LENGTH_MSB_ADDR  				(0X23)
#define BMI160_FIFO_DATA_ADDR   						(0X24)
/***************************************************/
/**\name ACCEL CONFIG REGISTERS  FOR ODR, BANDWIDTH AND UNDERSAMPLING*/
/******************************************************/
#define BMI160_ACCEL_CONFIG_ADDR       			(0X40)
/*******************/
/**\name ACCEL RANGE */
/*******************/
#define BMI160_ACCEL_RANGE_ADDR             (0X41)
/***************************************************/
/**\name GYRO CONFIG REGISTERS  FOR ODR AND BANDWIDTH */
/******************************************************/
#define BMI160_GYRO_CONFIG_ADDR             (0X42)
/*******************/
/**\name GYRO RANGE */
/*******************/
#define BMI160_GYRO_RANGE_ADDR              (0X43)
/***************************************************/
/**\name REGISTER FOR GYRO AND ACCEL DOWNSAMPLING RATES FOR FIFO*/
/******************************************************/
#define BMI160_FIFO_DOWN_ADDR               (0X45)
/***************************************************/
/**\name FIFO CONFIG REGISTERS*/
/******************************************************/
#define BMI160_FIFO_CONFIG_0_ADDR           (0X46)
#define BMI160_FIFO_CONFIG_1_ADDR           (0X47)
/***************************************************/
/**\name INTERRUPT ENABLE REGISTERS*/
/******************************************************/
#define BMI160_INTR_ENABLE_0_ADDR    				(0X50)
#define BMI160_INTR_ENABLE_1_ADDR   				(0X51)
#define BMI160_INTR_ENABLE_2_ADDR   				(0X52)
#define BMI160_INTR_OUT_CTRL_ADDR   				(0X53)
/***************************************************/
/**\name LATCH DURATION REGISTERS*/
/******************************************************/
#define BMI160_INTR_LATCH_ADDR     					(0X54)
/***************************************************/
/**\name MAP INTERRUPT 1 and 2 REGISTERS*/
/******************************************************/
#define BMI160_INTR_MAP_0_ADDR  						(0X55)
#define BMI160_INTR_MAP_1_ADDR  						(0X56)
#define BMI160_INTR_MAP_2_ADDR  						(0X57)
/***************************************************/
/**\name DATA SOURCE REGISTERS*/
/******************************************************/
#define BMI160_INTR_DATA_0_ADDR     				(0X58)
#define BMI160_INTR_DATA_1_ADDR     				(0X59)
/***************************************************/
/**\name
 INTERRUPT THRESHOLD, HYSTERESIS, DURATION, MODE CONFIGURATION REGISTERS*/
/******************************************************/
#define BMI160_INTR_LOW_DUR_ADDR       			(0X5A)
#define BMI160_INTR_LOW_TH_ADDR     				(0X5B)
#define BMI160_INTR_LOWHIGH_COFIG_ADDR			(0X5C)
#define BMI160_INTR_HIGH_DUR_ADDR   				(0X5D)
#define BMI160_INTR_HIGH_TH_ADDR    				(0X5E)
#define BMI160_INTR_MOTIONS_DUR_ADDR   			(0X5F)
#define BMI160_INTR_ANY_MOTION_TH_ADDR			(0X60)
#define BMI160_INTR_SLO_NO_MOTION_TH_ADDR		(0X61)
#define BMI160_INTR_SIG_CONFIG_ADDR 				(0X62)
#define BMI160_INTR_TAP_DUR_ADDR						(0X63)
#define BMI160_INTR_TAP_TH_ADDR 						(0X64)
#define BMI160_INTR_ORIENT_CONFIG_ADDR			(0X65)
#define BMI160_INTR_ORIENT_THETA_ADDR				(0X66)
#define BMI160_INTR_FLAT_THETA_ADDR 				(0X67)
#define BMI160_INTR_FLAT_CONFIG_ADDR				(0X68)
/***************************************************/
/**\name FAST OFFSET CONFIGURATION REGISTER*/
/******************************************************/
#define BMI160_FOC_CONFIG_ADDR  				(0X69)
/***************************************************/
/**\name MISCELLANEOUS CONFIGURATION REGISTER*/
/******************************************************/
#define BMI160_CONFIG_ADDR  					(0X6A)
/***************************************************/
/**\name SERIAL INTERFACE SETTINGS REGISTER*/
/******************************************************/
#define BMI160_IF_CONFIG_ADDR		     		(0X6B)
/***************************************************/
/**\name GYRO POWER MODE TRIGGER REGISTER */
/******************************************************/
#define BMI160_PMU_TRIGGER_ADDR     			(0X6C)
/***************************************************/
/**\name SELF_TEST REGISTER*/
/******************************************************/
#define BMI160_SELF_TEST_ADDR   				(0X6D)
/***************************************************/
/**\name SPI,I2C SELECTION REGISTER*/
/******************************************************/
#define BMI160_NV_CONFIG_ADDR   				(0x70)
/***************************************************/
/**\name ACCEL AND GYRO OFFSET REGISTERS*/
/******************************************************/
#define BMI160_OFFSET_ACC_X_ADDR				(0X71)
#define BMI160_OFFSET_ACC_Y_ADDR				(0X72)
#define BMI160_OFFSET_ACC_Z_ADDR				(0X73)
#define BMI160_OFFSET_GYR_X_LSB_ADDR			(0X74)
#define BMI160_OFFSET_GYR_Y_LSB_ADDR			(0X75)
#define BMI160_OFFSET_GYR_Z_LSB_ADDR			(0X76)
#define BMI160_OFFSET_GYRS_MSB_ADDR				(0X77)
/***************************************************/
/**\name STEP COUNTER INTERRUPT REGISTERS*/
/******************************************************/
#define BMI160_STEP_COUNT_LSB_ADDR			(0X78)
#define BMI160_STEP_COUNT_MSB_ADDR			(0X79)
/***************************************************/
/**\name STEP COUNTER CONFIGURATION REGISTERS*/
/******************************************************/
#define BMI160_STEP_CONFIG_0_ADDR			(0X7A)
#define BMI160_STEP_CONFIG_1_ADDR			(0X7B)
/***************************************************/
/**\name COMMAND REGISTER*/
/******************************************************/
#define BMI160_CMD_COMMANDS_ADDR				(0X7E)

/****************************************************************************
 *											BMI160 Register Setting Map												  *
 ****************************************************************************/

/*BMI160_CHIP_ID_ADDR (0x00) */
#define BMI160_CHIP_ID                      (0xD1)

/*BMI160_ERROR_ADDR (0x02) */
#define BMI160_FATAL_ERR                    (0x01)
#define BMI160_DROP_CMD_ERR                 (0x40)

/*BMI160_PMU_STAT_ADDR (0x03) */
#define BMI160_PMU_ACCEL_GYRO_SUSPEND_MODE      (0x00)
#define BMI160_PMU_GYRO_NORMAL_MODE             (0x04)
#define BMI160_PMU_GYRO_FAST_START_UP_MODE      (0x0C)
#define BMI160_PMU_ACCEL_NORMAL_MODE            (0x10)
#define BMI160_PMU_ACCEL_LOW_POWER_MODE         (0x20)

/*BMI160_STATUS_ADDR (0x1B) */
#define BMI160_DRDY_ACC_STATUS              (0x80)
#define BMI160_DRDY_GYRO_STATUS             (0x40)
#define BMI160_RDY_NVM_STATUS               (0x10)
#define BMI160_RDY_FOC_STATUS               (0x08)
#define BMI160_GYR_SLEF_TEST_STATUS         (0x02)

/*BMI160_INTR_STAT_0_ADDR (0x1C) */
#define BMI160_FLAT_INT_STATUS              (0x80)
#define BMI160_ORIENT_INT_STATUS            (0x40)
#define BMI160_S_TAP_INT_STATUS             (0x20)
#define BMI160_D_TAP_INT_STATUS             (0x10)
#define BMI160_PMU_TRIGGER_INT_STATUS       (0x08)
#define BMI160_ANYM_INT_STATUS              (0x04)
#define BMI160_SIGMOT_INT_STATUS            (0x02)
#define BMI160_STEP_INT_STATUS              (0x01)

/*BMI160_INTR_STAT_0_ADDR (0x1D) */
#define BMI160_nomo_INT_STATUS              (0x80)
#define BMI160_FWM_INT_STATUS               (0x40)
#define BMI160_FFULL_INT_STATUS             (0x20)
#define BMI160_DRDY_INT_STATUS              (0x10)
#define BMI160_LOW_G_INT_STATUS             (0x08)
#define BMI160_HIGH_G_INT_STAUTS            (0x04)

/*BMI160_INTR_STAT_0_ADDR (0x1E) */
#define BMI160_TAP_SIGN_INT_STATUS          (0x80)
#define BMI160_TAP_FIRST_Z_INT_STATUS       (0x40)
#define BMI160_TAP_FIRST_Y_INT_STATUS       (0x20)
#define BMI160_TAP_FIRST_X_INT_STATUS       (0x10)
#define BMI160_ANYM_SIGN_INT_STATUS         (0x08)
#define BMI160_ANYM_FIRTS_Z_INT_STATUS      (0x04)
#define BMI160_ANYM_FIRTS_Y_INT_STATUS      (0x02)
#define BMI160_ANYM_FIRTS_X_INT_STATUS      (0x01)

/*BMI160_INTR_STAT_0_ADDR (0x1F) */
#define BMI160_FLAT_INT_STATUS              (0x80)
#define BMI160_ORIENT_2_INT_STATUS          (0x40)
#define BMI160_ORIENT_0_1_INT_STATUS        (0x30)
#define BMI160_HIGH_SIGN_INT_STATUS         (0x08)
#define BMI160_HIGH_FIRTS_Z_INT_STATUS      (0x04)
#define BMI160_HIGH_FIRTS_Y_INT_STATUS      (0x02)
#define BMI160_HIGH_FIRTS_X_INT_STATUS      (0x01)

/*BMI160_ACCEL_CONFIG_ADDR (0x40) */
#define BMI160_ACCEL_UNDERSAMPLING_MODE     (0x80)
//BMI160_ACCEL_BANDWIDTH_PARAMETER (AVG - number of averaging cycles)
#define BMI160_ACCEL_BANDWIDTH_AVG_1        (0x00)
#define BMI160_ACCEL_BANDWIDTH_AVG_2        (0x10)
#define BMI160_ACCEL_BANDWIDTH_AVG_4        (0x20)
#define BMI160_ACCEL_BANDWIDTH_AVG_8        (0x30)
#define BMI160_ACCEL_BANDWIDTH_AVG_16       (0x40)
#define BMI160_ACCEL_BANDWIDTH_AVG_21       (0x50)
#define BMI160_ACCEL_BANDWIDTH_AVG_64       (0x60)
#define BMI160_ACCEL_BANDWIDTH_AVG_128      (0x70)
//BMI160_ACCEL_OUTPUT_DATA_RATE (HZ)
#define BMI160_ACCEL_ODR_0_78125            (0x01)  // 25/32 = 0.78125HZ = 1.25ms
#define BMI160_ACCEL_ODR_1_5625             (0x02)  // 25/16 = 1.5625HZ = 0.64ms
#define BMI160_ACCEL_ODR_3_125              (0x03)  // 25/8  = 3.125HZ = 0.32ms
#define BMI160_ACCEL_ODR_6_25               (0x04)  // 25/4  = 6.25HZ = 0.16ms
#define BMI160_ACCEL_ODR_12_5               (0x05)  // 25/2  = 12.5HZ = 0.08ms
#define BMI160_ACCEL_ODR_25                 (0x06)  // 25/1  = 25HZ = 0.04ms
#define BMI160_ACCEL_ODR_50                 (0x07)  // 25*2  = 50HZ = 0.02ms
#define BMI160_ACCEL_ODR_100                (0x08)  // 25*4  = 100HZ = 0.01ms
#define BMI160_ACCEL_ODR_200                (0x09)  // 25*8  = 200HZ = 0.005ms
#define BMI160_ACCEL_ODR_400                (0x0A)  // 25*16 = 400HZ = 0.0025ms
#define BMI160_ACCEL_ODR_800                (0x0B)  // 25*32 = 800HZ = 0.00125ms
#define BMI160_ACCEL_ODR_1600               (0x0C)  // 25*64 = 1600HZ = 0.000625ms

/*BMI160_ACCEL_RANGE_ADDR (0x41) */
#define BMI160_ACCEL_RANGE_2G               (0x03)
#define BMI160_ACCEL_RANGE_4G               (0x05)
#define BMI160_ACCEL_RANGE_8G               (0x08)
#define BMI160_ACCEL_RANGE_16G              (0x0C)

/*BMI160_GYRO_CONFIG_ADDR (0x42) */
//BMI160_GYRO_BANDWIDTH (Filter mode set)
//GyroScope bandwidth coefficient defines the 3 dB cutoff frequency
//of the low pass filter for the sensor data
#define BMI160_GYRO_NORMAL					(0x20)	// ODR=50HZ -> 20.8HZ
#define BMI160_GYRO_OSR2					(0x10)	// ODR=50HZ -> 10.12HZ
#define BMI160_GYRO_OSR4					(0x00)	// ODR=50HZ -> 5.06HZ

//BMI160_GYRO_OUTPUT_DATA_RATE (HZ)
#define BMI160_GYRO_ODR_25                  (0x06)  // 25/1 = 25HZ = 0.04ms
#define BMI160_GYRO_ODR_50                  (0x07)  // 25*2 = 50HZ = 0.02ms
#define BMI160_GYRO_ODR_100                 (0x08)  // 25*4 = 100HZ = 0.01ms
#define BMI160_GYRO_ODR_200                 (0x09)  // 25*8 = 200HZ = 0.005ms
#define BMI160_GYRO_ODR_400                 (0x0A)  // 25*16 = 400HZ = 0.0025ms
#define BMI160_GYRO_ODR_800                 (0x0B)  // 25*32 = 800HZ = 0.00125ms
#define BMI160_GYRO_ODR_1600                (0x0C)  // 25*64 = 1600HZ = 0.000625ms
#define BMI160_GYRO_ODR_3200                (0x0D)  // 25*128 = 32200HZ = 0.0003125ms

/*BMI160_GYRO_RANGE_ADDR (0x43) */
#define BMI160_GYRO_RANGE_2000              (0x00)  //
#define BMI160_GYRO_RANGE_1000              (0x01)  //
#define BMI160_GYRO_RANGE_500               (0x02)  //
#define BMI160_GYRI_RANGE_250               (0x03)  //
#define BMI160_GYRO_RANGE_125               (0x04)  //

/*BMI160_INTR_ENABLE_0_ADDR (0x50) */
#define BMI160_INT_FLAT_ENABLE              (0x80)
#define BMI160_INT_ORIENT_ENABLE            (0x40)
#define BMI160_INT_SINGLE_TAP_ENABLE        (0x20)
#define BMI160_INT_DOUBLE_TAP_ENABLE        (0x10)
#define BMI160_INT_ANYMO_Z_ENABLE           (0x04)
#define BMI160_INT_ANYMO_Y_ENABLE           (0x02)
#define BMI160_INT_ANYMO_X_ENABLE           (0x01)

/*BMI160_INTR_ENABLE_1_ADDR (0x51) */
#define BMI160_INT_FIFO_WATERMARK_ENABLE    (0x40)
#define BMI160_INT_FIFO_FULL_ENABLE         (0x20)
#define BMI160_INT_DATA_READY_ENABLE        (0x10)
#define BMI160_INT_LOW_G_ENABLE             (0x08)
#define BMI160_INT_HIGH_G_Z_ENABLE          (0x04)
#define BMI160_INT_HIGH_G_Y_ENABLE          (0x02)
#define BMI160_INT_HIGH_G_X_ENABLE          (0x01)

/*BMI160_INTR_ENABLE_2_ADDR (0x52) */
#define BMI160_INT_STEP_DET_ENABLE          (0x08)
#define BMI160_INT_NOMO_Z_ENABLE            (0x04)
#define BMI160_INT_NOMO_Y_ENABLE            (0x02)
#define BMI160_INT_NOMO_X_ENABLE            (0x01)

/*BMI160_INTR_OUT_CTRL_ADDR (0x53) */
#define BMI160_INT_2_OUTPUT_ENABLE          (0x80)
#define BMI160_INT_2_OD                     (0x40)
#define BMI160_INT_2_LVL                    (0x20)
#define BMI160_INT_2_EDGE_CTRL              (0x10)
#define BMI160_INT_1_OUTOUT_ENABLE          (0x08)
#define BMI160_INT_1_OD                     (0x04)
#define BMI160_INT_1_LVL                    (0x02)
#define BMI160_INT_1_EDGE_CTRL              (0x01)

/*BMI160_INTR_LATCH_ADDR (0x54) */
#define BMI160_INT_2_INPUT_ENABLE           (0x20)
#define BMI160_INT_1_INPUT_ENABLE           (0x10)
//interrupt mode
/*The times for the temporary modes have been selected to be 50%
 of the pre-filtered data rate and then power of two increments*/
#define BMI160_INT_NON_LATCHED              (0x00)
#define BMI160_INT_TMP_312_5us              (0x01)
#define BMI160_INT_TMP_625us                (0x02)
#define BMI160_INT_TMP_1_25ms               (0x03)
#define BMI160_INT_TMP_2_5ms                (0x04)
#define BMI160_INT_TMP_5ms                  (0x05)
#define BMI160_INT_TMP_10ms                 (0x06)
#define BMI160_INT_TMP_20ms                 (0x07)
#define BMI160_INT_TMP_40ms                 (0x08)
#define BMI160_INT_TMP_80ms                 (0x09)
#define BMI160_INT_TMP_160ms                (0x0A)
#define BMI160_INT_TMP_320ms                (0x0B)
#define BMI160_INT_TMP_640ms                (0x0C)
#define BMI160_INT_TMP_1_28S                (0x0D)
#define BMI160_INT_TMP_2_56S                (0x0E)
#define BMI160_INT_LATCHED                  (0x0F)

/*BMI160_INTR_MAP_0_ADDR (0x55) */
#define BMI160_INT_1_MAP_FLAT               (0x80)
#define BMI160_INT_1_MAP_ORIENTATION        (0x40)
#define BMI160_INT_1_MAP_SINGLE_TAP         (0x20)
#define BMI160_INT_1_MAP_DOUBLE_TAP         (0x10)
#define BMI160_INT_1_MAP_NO_MOTION          (0x08)
#define BMI160_INT_1_MAP_ANY_MOTION         (0x04)  // Anymotion / significant motion
#define BMI160_INT_1_MAP_HIGH_G             (0x02)
#define BMI160_INT_1_MAP_LoW_G              (0x01)  // Low-g / Step detection

/*BMI160_INTR_MAP_1_ADDR (0x56) */
#define BMI160_INT_1_MAP_DATA_READY         (0x80)
#define BMI160_INT_1_MAP_FIFO_WATERMARK     (0x40)
#define BMI160_INT_1_MAP_FIFO_FULL          (0x20)
#define BMI160_INT_1_PMU_TRIGGER            (0x10)
#define BMI160_INT_2_MAP_DATA_READY         (0x08)
#define BMI160_INT_2_MAP_FIFO_WATERMARK     (0x04)
#define BMI160_INT_2_MAP_FIFO_FULL          (0x02)
#define BMI160_INT_2_PMU_TRIGGER            (0x01)

/*BMI160_INTR_MAP_2_ADDR (0x57) */
#define BMI160_INT_2_MAP_FLAT               (0x80)
#define BMI160_INT_2_MAP_ORIENTATION        (0x40)
#define BMI160_INT_2_MAP_SINGLE_TAP         (0x20)
#define BMI160_INT_2_MAP_DOUBLE_TAP         (0x10)
#define BMI160_INT_2_MAP_NO_MOTION          (0x08)
#define BMI160_INT_2_MAP_ANY_MOTION         (0x04)  // Anymotion / significant motion
#define BMI160_INT_2_MAP_HIGH_G             (0x02)
#define BMI160_INT_2_MAP_LoW_G              (0x01)  // Low-g / Step detection

/*BMI160_INTR_DATA_0_ADDR (0x58) */
#define BMI160_INT_LOW_HIGH_SRC             (0x80)
#define BMI160_INT_INT_TAP_SRC              (0x08)

/*BMI160_INTR_DATA_1_ADDR (0x59) */
#define BMI160_INT_MOTION_SRC               (0x80)

/*BMI160_INTR_LOWHIGH_COFIG_ADDR (0x5C) */
#define BMI160_INT_LOW_MODE                 (0x04)

/*BMI160_INTR_TAP_DUR_ADDR (0x63) */
#define BMI160_INT_TAP_QUIET                (0x80) //selects a tap quiet duration of '0'->30ms, '1'->20ms
#define BMI160_INT_TAP_SHOCK                (0x40) //selects a tap shock duration of '0'->50ms, '1'->75ms
//BMI160_INT_TAP_DURATION
#define BMI160_INT_TAP_DUR_50ms             (0x00)
#define BMI160_INT_TAP_DUR_100ms            (0x01)
#define BMI160_INT_TAP_DUR_150ms            (0x02)
#define BMI160_INT_TAP_DUR_200ms            (0x03)
#define BMI160_INT_TAP_DUR_250ms            (0x04)
#define BMI160_INT_TAP_DUR_375ms            (0x05)
#define BMI160_INT_TAP_DUR_500ms            (0x06)
#define BMI160_INT_TAP_DUR_700ms            (0x07)

/*BMI160_PMU_TRIGGER_ADDR (0x6C) */
#define BMI160_PMU_WAKEUP_INT               (0x40)
#define BMI160_PMU_GYRO_SLEEP_STATE         (0x20) // 1 == SUSPEND , 0 == fast start-up
//GYRO_WAKEUP_TRIGGER (0x01 | 0x80)
#define BMI160_PMU_GYRO_WAKEUP_TRIGGER_0    (0x00) // anymotion (no), INT1 PIN (no)
#define BMI160_PMU_GYRO_WAKEUP_TRIGGER_1    (0x08) // anymotion (no), INT1 PIN (Yes)
#define BMI160_PMU_GYRO_WAKEUP_TRIGGER_2    (0x10) // anymotion (yes), INT1 PIN (NO)
#define BMI160_PMU_GYRO_WAKEUP_TRIGGER_3    (0x18) // anymotion (yes), INT1 PINT(YES)
//GYRO_SLEEP_TRIGGER (0x04 | 0x02 | 0x01)
#define BMI160_PMU_GYRO_SLEEP_TRIGGER_0     (0x00) // nomotion (no) , NOT INT1 PIN(no), INT2 PIN (no)
#define BMI160_PMU_GYRO_SLEEP_TRIGGER_1     (0x01) // nomotion (no) , NOT INT1 PIN(no), INT2 PIN (yes)
#define BMI160_PMU_GYRO_SLEEP_TRIGGER_2     (0x02) // nomotion (no) , NOT INT1 PIN(yes), INT2 PIN (no)
#define BMI160_PMU_GYRO_SLEEP_TRIGGER_3     (0x03) // nomotion (no) , NOT INT1 PIN(yes), INT2 PIN (yes)
#define BMI160_PMU_GYRO_SLEEP_TRIGGER_4     (0x04) // nomotion (yes) , NOT INT1 PIN(no), INT2 PIN (no)
#define BMI160_PMU_GYRO_SLEEP_TRIGGER_5     (0x05) // nomotion (yes) , NOT INT1 PIN(no), INT2 PIN (yes)
#define BMI160_PMU_GYRO_SLEEP_TRIGGER_6     (0x06) // nomotion (yes) , NOT INT1 PIN(yes), INT2 PIN (no)
#define BMI160_PMU_GYRO_SLEEP_TRIGGER_7     (0x07) // nomotion (yes) , NOT INT1 PIN(yes), INT2 PIN (yes)

/*BMI160_STEP_CONFIG_0_ADDR (0x7A) */
//Step MIN_threshold (0x10 | 0x80)
#define BMI160_STEP_MIN_THRES_SENSIT        (0x08)  // sensitived
#define BMI160_STEP_MIN_THRES_NORMAL        (0x10)  // Normal
#define BMI160_STEP_MIN_THRES_ROBUST        (0x18)  // Robust
//Step Steptime_min (0x04 | 0x02 | 0x01) offset 1
#define BMI160_STEP_MIN_TIME_1              (0x01)
#define BMI160_STEP_MIN_TIME_2              (0x02)
#define BMI160_STEP_MIN_TIME_3              (0x03)
#define BMI160_STEP_MIN_TIME_4              (0x04)
#define BMI160_STEP_MIN_TIME_5              (0x05)  //Average
#define BMI160_STEP_MIN_TIME_6              (0x06)
#define BMI160_STEP_MIN_TIME_7              (0x07)

/*BMI160_STEP_CONFIG_1_ADDR (0x7B) */
#define BMI160_STEP_CNT_ENABLE              (0x08)
//Step Min Step Buf (0x04 | 0x02 | 0x01)
#define BMI160_STEP_MIN_BUF_0               (0x00)  //Sensitive Mode
#define BMI160_STEP_MIN_BUF_1               (0x01)
#define BMI160_STEP_MIN_BUF_2               (0x02)
#define BMI160_STEP_MIN_BUF_3               (0x03)  //Normal Mode
#define BMI160_STEP_MIN_BUF_4               (0x04)
#define BMI160_STEP_MIN_BUF_5               (0x05)
#define BMI160_STEP_MIN_BUF_6               (0x06)
#define BMI160_STEP_MIN_BUF_7               (0x07)  //Robust Mode

/*BMI160_CMD_COMMANDS_ADDR (0x7E) */
#define BMI160_CMD_START_FOC                (0x03)

//#define BMI160_CMD_ACCEL_NORMAL             (0x10)
//#define BMI160_CMD_ACCEL_LOWPOWER           (0x11)
//#define BMI160_CMD_ACCEL_SUSPEND            (0x12)

#define BMI160_CMD_ACCEL_NORMAL			(0x11)
#define	BMI160_CMD_ACCEL_LOWPOWER		(0X12)
#define	BMI160_CMD_ACCEL_SUSPEND		(0X10)

#define BMI160_CMD_GYRO_SUSPEND             (0x14)
#define BMI160_CMD_GYRO_NORMAL              (0x15)
#define BMI160_CMD_GYRO_FAST_START          (0x17)

#define BMI160_CMD_PROG_NVM                 (0xA0)
#define BMI160_CMD_FIFO_FLUSH               (0xB0)
#define BMI160_CMD_INT_RESET                (0xB1)

#define BMI160_CMD_SOFTRESET                (0xB6)
#define BMI160_CMD_STEP_CNT_CLR             (0xB2)

//////////////////////////////////////////////////////////////////////////////////////////////
/************************************************/
/**\name	 FUNCTION DEFINITION       */
/***********************************************/

/**
 * @fn      BMI160_Soft_Reset
 *
 * @brief		None.
 *
 */
void BMI160_Soft_Reset(void);

/**
 * @fn      BMI160_Chip_ID_READ
 *
 * @brief		None.
 *
 * @return  bool
 */
bool BMI160_Chip_ID_READ(void);

/**
 * @fn      Bmi160_Gyro_Config
 *
 * @brief		None.
 *
 * @param   mode
 *
 * @param   Range_reg
 *
 */
void Bmi160_Gyro_Config(uint8_t mode, uint8_t Range_reg);

/**
 * @fn      BMI160_Acc_Config
 *
 * @brief		None.
 *
 * @param   mode
 *
 * @param   Range_reg
 *
 */
void BMI160_Acc_Config(uint8_t mode, uint8_t Range_reg);

/**
 * @fn      BMI160_INT_Latch
 *
 * @brief		None.
 *
 * @param   Int_Latch
 *
 */
void BMI160_INT_Latch(uint8_t Int_Latch);

/**
 * @fn      BMI160_INT_OUTPUT_SET
 *
 * @brief		None.
 *
 */
void BMI160_INT_OUTPUT_SET();

/**
 * @fn      BMI160_INT_Enable
 *
 * @brief		None.
 *
 */
void BMI160_INT_Enable();

/**
 * @fn      BMI160_ACC_DATA_READ_X_AXIS
 *
 * @brief		None.
 *
 * @return  int16_t
 */
int16_t BMI160_ACC_DATA_READ_X_AXIS(void);
/**
 * @fn      BMI160_ACC_DATA_READ_Y_AXIS
 *
 * @brief		None.
 *
 * @return  int16_t
 */
int16_t BMI160_ACC_DATA_READ_Y_AXIS(void);
/**
 * @fn      BMI160_ACC_DATA_READ_Z_AXIS
 *
 * @brief		None.
 *
 * @return  int16_t
 */
int16_t BMI160_ACC_DATA_READ_Z_AXIS(void);

/**
 * @fn      BMI160_GYRO_DATA_READ_X_AXIS
 *
 * @brief		None.
 *
 * @return  int16_t
 */
int16_t BMI160_GYRO_DATA_READ_X_AXIS(void);
/**
 * @fn      BMI160_ACC_DATA_READ_Y_AXIS
 *
 * @brief		None.
 *
 * @return  int16_t
 */
int16_t BMI160_GYRO_DATA_READ_Y_AXIS(void);
/**
 * @fn      BMI160_GYRO_DATA_READ_Z_AXIS
 *
 * @brief		None.
 *
 * @return  int16_t
 */
int16_t BMI160_GYRO_DATA_READ_Z_AXIS(void);

/**
 * @fn      AAT_Bmi160_writeReg
 *
 * @brief   SPI Data Write
 *
 * @param   reg ��������
 *
 * @param   val ������
 *
 * @return  None.
 */
uint32_t AAT_Bmi160_writeReg(uint8_t reg, uint8_t val);

/**
 * @fn      AAT_Bmi160_readReg
 *
 * @brief		SPI Data Read
 *
 * @param   None.
 *
 * @return  None.
 */
void AAT_Bmi160_readReg(uint8_t reg, uint8_t *pVal);

/*********************************************************************
 * @fn      AAT_Bmi160_init
 *
 * @brief		None.
 *
 */
void AAT_Bmi160_init(void);
/*********************************************************************
 * @fn      BMI160_INT_Check
 *
 * @brief		None.
 *
 * @param   flag
 *
 * @return  uint8_t
 */
uint8_t BMI160_INT_Check(uint8_t flag);
/*********************************************************************
 * @fn      BMI160_INT_Disable
 *
 * @brief		None.
 *
 */
void BMI160_INT_Disable(void);

void AAT_enter_sleep(void);
void AAT_wake_up(void);
bool get_AAT_sleep_state();

#endif /*(CSOS_SAAL__USE_LIB_ACC_bmi160 == 1)*/

#endif /* APPLICATION_PAARTAG_EXE_PAARTAG_SRC_AAT_DEVICE_BMI160_H_ */
