/*
 * hw_config.h
 *
 *  Created on: 2021. 9. 15.
 *      Author: YuJin Park
 *
 */

#ifndef APPLICATION_CONFIG_HW_CONFIG_H_
#define APPLICATION_CONFIG_HW_CONFIG_H_


//AAA HW Pin configuration

#define AAT_BOARD_RTC_INT 			4
#define AAT_BOARD_MAIN_PW_EN		5
#define AAT_BOARD_PW_SW				6

//ALS,RTC I2C
#define AAT_BOARD_I2C_SCL			7
#define AAT_BOARD_I2C_SDA			8

//Anbient Light Sensor
#define AAT_BOARD_ALS_PWR_EN		9

// Water Tag LED
#define LED_RED			16
#define LED_GREEN		17
#define LED_BLUE		18

#define AAT_BOARD_ALS_INT		19

#define AAT_BOARD_ACCEL_PWR_EN	23

// External Flash

#define AAT_BOARD_PIN_FLASH_ACCEL_SPI_MISO		27	//25
#define AAT_BOARD_PIN_FLASH_ACCEL_SPI_MOSI		26	//26
#define AAT_BOARD_PIN_FLASH_ACCEL_SPI_SCK		25	//27

#define AAT_BOARD_PIN_FLASH_SPI_CS				28
#define AAT_BOARD_PIN_ACCEL_SPI_CS				29

#define AAT_BOARD_PIN_ACEEL_INT_1				12
#define AAT_BOARD_PIN_ACEEL_INT_2				13

#endif /* APPLICATION_CONFIG_HW_CONFIG_H_ */
