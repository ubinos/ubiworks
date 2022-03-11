/*
 * AAT_driver_spi.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: Kwon
 */

#ifndef APPLICATION_PAARTAG_EXE_PAARTAG_SRC_AAT_DRIVER_SPI_H_
#define APPLICATION_PAARTAG_EXE_PAARTAG_SRC_AAT_DRIVER_SPI_H_

#include "ubinos.h"

#if (CSOS_SAAL__USE_LIB_AAT_HW_other == 1)

#include <stdint.h>

/************************************************/
/**\name	 FUNCTION DEFINITION       */
/***********************************************/
/**
 * @fn      void AAT_spi_init(void)
 *
 * @brief		None.
 *
 */
void AAT_spi_init(void);
/**
 * @fn      void AAT_spi_uninit(void)
 *
 * @brief		None.
 *
 */
void AAT_spi_uninit(void);
/**
 * @fn      void AAT_spi_write_ACC(uint8_t* buf, uint8_t length)
 *
 * @brief		None.
 *
 * @param		buf
 *
 * @param		length
 *
 */
uint32_t AAT_spi_write_ACC(uint8_t* buf, uint8_t length);
/**
 * @fn      void AAT_spi_read_ACC(uint8_t* buf, uint8_t length)
 *
 * @brief		None.
 *
 * @param		buf
 *
 * @param		length
 *
 */
void AAT_spi_read_ACC(uint8_t* buf, uint8_t length);
/**
 * @fn      void AAT_spi_write_read_ACC(uint8_t* w_buf, uint8_t w_len, uint8_t* r_buf, uint8_t r_len)
 *
 * @brief		None.
 *
 * @param		w_buf
 *
 * @param		w_len
 *
 * @param		r_buf
 *
 * @param		r_len
 *
 */
void AAT_spi_write_read_ACC(uint8_t* w_buf, uint8_t w_len, uint8_t* r_buf, uint8_t r_len);
/**
 * @fn      void AAT_spi_write_Flash(uint8_t* buf, uint8_t length)
 *
 * @brief		None.
 *
 * @param		buf
 *
 * @param		length
 *
 */
void AAT_spi_write_Flash(uint8_t* buf, uint8_t length);
/**
 * @fn      void AAT_spi_read_Flash(uint8_t* buf, uint8_t length)
 *
 * @brief		None.
 *
 * @param		buf
 *
 * @param		length
 *
 */
void AAT_spi_read_Flash(uint8_t* buf, uint8_t length);
/**
 * @fn      void AAT_spi_write_read_Flash(uint8_t* w_buf, uint8_t w_len, uint8_t* r_buf, uint8_t r_len);
 *
 * @brief		None.
 *
 * @param		w_buf
 *
 * @param		w_len
 *
 * @param		r_buf
 *
 * @param		r_len
 *
 */
void AAT_spi_write_read_Flash(uint8_t* w_buf, uint8_t w_len, uint8_t* r_buf, uint8_t r_len);
/**
 * @fn      void AAT_spi_write_write_Flash(uint8_t* w_buf, uint8_t w_len, uint8_t* w_next_buf, uint8_t r_next_len);
 *
 * @brief		None.
 *
 * @param		w_buf
 *
 * @param		w_len
 *
 * @param		w_next_buf
 *
 * @param		r_next_len
 *
 */
void AAT_spi_write_write_Flash(uint8_t* w_buf, uint8_t w_len, uint8_t* w_next_buf, uint8_t r_next_len);

#endif /*(CSOS_SAAL__USE_LIB_AAT_HW_other == 1)*/

#endif /* APPLICATION_PAARTAG_EXE_PAARTAG_SRC_AAT_DRIVER_SPI_H_ */
