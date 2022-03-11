/*
 * PT_i2c_ext_rtc_spo2.h
 *
 *  Created on: 2016. 5. 26.
 *      Author: Administrator
 */

#ifndef APPLICATION_PAARBAND_EXE_PAARBAND_SRC_PT_I2C_H_
#define APPLICATION_PAARBAND_EXE_PAARBAND_SRC_PT_I2C_H_

#include "ubinos.h"

#if (CSOS_SAAL__USE_LIB_AAT_HW_other == 1)

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/************************************************/
/**\name	 FUNCTION DEFINITION       */
/***********************************************/
/**
 * @fn      void AAT_i2c_init(void)
 *
 * @brief		None.
 *
 */
void AAT_i2c_init(void);
/**
 * @fn      void AAT_i2c_uninit(void)
 *
 * @brief		None.
 *
 */
void AAT_i2c_uninit(void);
/**
 * @fn      unsigned int AAT_i2c_read(uint8_t pSlaveAddress, uint8_t *pWriteBuffer, uint32_t pWDataLength, uint8_t *pReadBuffer, uint32_t pRDataLength)
 *
 * @brief		None.
 *
 * @param   pSlaveAddress
 *
 * @param		pWriteBuffer
 *
 * @param 	pWDataLength
 *
 * @param		pReadBuffer
 *
 * @param		pRDataLength
 *
 * @return  unsigned int
 */
unsigned int AAT_i2c_read(uint8_t pSlaveAddress, uint8_t *pWriteBuffer, uint32_t pWDataLength, uint8_t *pReadBuffer, uint32_t pRDataLength);
/**
 * @fn      unsigned int AAT_i2c_write(uint8_t pSlaveAddress, uint8_t *pWriteBuffer, uint32_t pDataLength);
 *
 * @brief		None.
 *
 * @param   pSlaveAddress
 *
 * @param		pWriteBuffer
 *
 * @param		pDataLength
 *
 * @return  unsigned int
 *
 */
unsigned int AAT_i2c_write(uint8_t pSlaveAddress, uint8_t *pWriteBuffer, uint32_t pDataLength);


void OPT3001_init();
bool OPT3001_read_DeviceID();
int OPT3001_lux_read();

#endif /*(CSOS_SAAL__USE_LIB_AAT_HW_other == 1)*/

#endif /* APPLICATION_PAARBAND_EXE_PAARBAND_SRC_PT_I2C_H_ */
