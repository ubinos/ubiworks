/*
 * AAT_RTC.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: Kwon
 */

#ifndef APPLICATION_PAARTAG_EXE_PAARTAG_SRC_PT_DEVICE_RTC_H_
#define APPLICATION_PAARTAG_EXE_PAARTAG_SRC_PT_DEVICE_RTC_H_
#include "ubinos.h"

#if (CSOS_SAAL__USE_LIB_AAT_HW_other == 1)

#include <stdint.h>

/************************************************/
/**\name	ADDRESS DEFINITION       */
/***********************************************/
#define RTC_ADDRESS		0x51

/************************************************/
/**\name	REGISTER ADDRESS DEFINITION       */
/***********************************************/
#define RTC_Control_1_ADDRESS			0x00
#define RTC_Control_2_ADDRESS			0x01
#define RTC_Offset_ADDRESS				0x02
#define RTC_RAM_byte_ADDRESS			0x03
#define RTC_Seconds_ADDRESS				0x04
#define RTC_Minutes_ADDRESS				0x05
#define RTC_Hours_ADDRESS					0x06
#define RTC_Days_ADDRESS					0x07
#define RTC_Weekdays_ADDRESS			0x08
#define RTC_Months_ADDRESS				0x09
#define RTC_Years_ADDRESS					0x0A

/************************************************/
/**\name	INTERRUPT DEFINITION       */
/***********************************************/
#define RTC_INTERRUPT_NO					0x00
#define RTC_INTERRUPT_MIN					0x20
#define RTC_INTERRUPT_HALF_MIN		0x10
#define RTC_INTERRUPT_HALF_SEC		0x30

/////////////////////////////////////////////////////////////////////////////////////
/**
 * @fn      void AAT_RTC_device_init(void)
 *
 * @brief		None.
 *
 */
void AAT_RTC_device_init(void);
/**
 * @fn      void AAT_RTC_interrupt_init(uint8_t time)
 *
 * @brief		None.
 *
 * @param   time
 *
 */
void AAT_RTC_interrupt_init(uint8_t time);
/**
 * @fn      void AAT_RTC_SoftReset(void)
 *
 * @brief		None.
 *
 */
void AAT_RTC_SoftReset(void);
/**
 * @fn      void AAT_RTC_Write_control(uint8_t cmd)
 *
 * @brief		None.
 *
 * @return  uint8_t cmd
 */
void AAT_RTC_Write_control(uint8_t cmd);
/**
 * @fn      void AAT_RTC_Write_time_interrupt(uint8_t cmd)
 *
 * @brief		None.
 *
 * @param   cmd
 *
 */
void AAT_RTC_Write_time_interrupt(uint8_t cmd);
/**
 * @fn      uint8_t AAT_RTC_Read_control(void)
 *
 * @brief		None.
 *
 * @return  uint8_t
 */
uint8_t AAT_RTC_Read_control(void);
/**
 * @fn      void AAT_RTC_Write_time(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
 *
 * @brief		None.
 *
 * @param   year
 *
 * @param		mon
 *
 * @param		day
 *
 * @param 	hour
 *
 * @param		min
 *
 * @param 	sec
 *
 */
void AAT_RTC_Write_time(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
/**
 * @fn     void AAT_RTC_Read_time(uint8_t* year, uint8_t* mon, uint8_t* days, uint8_t* hours, uint8_t* min, uint8_t* sec)
 *
 * @brief		None.
 *
 * @param   year
 *
 * @param		mon
 *
 * @param		days
 *
 * @param 	hours
 *
 * @param		min
 *
 * @param 	sec
 *
 */
void AAT_RTC_Read_time(uint8_t* year, uint8_t* mon, uint8_t* days, uint8_t* hours, uint8_t* min, uint8_t* sec);

void AAT_RTC_Read_time_test(uint8_t * mon, uint8_t * days, uint8_t* hours, uint8_t* min);

#endif /*(CSOS_SAAL__USE_LIB_AAT_HW_other == 1)*/

#endif /* APPLICATION_PAARTAG_EXE_PAARTAG_SRC_AAT_DEVICE_RTC_H_ */
