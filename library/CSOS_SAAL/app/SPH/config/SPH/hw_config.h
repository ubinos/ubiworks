/*
 * hw_config.h
 *
 *  Created on: 2020. 9. 2.
 *      Author: HMKANG
 */

#ifndef APP_HW_CONFIG_H_
#define APP_HW_CONFIG_H_

#include "nrf_gpio.h"

#define SAAL_HW_DEVICE_SPH           0
#define SAAL_HW_DEVICE_AAT           1
#define SAAL_HW_DEVICE_HAT           2
#define SAAL_HW_DEVICE_THINGY52      3

#define SAAL_HW_DEVICE_TYPE      SAAL_HW_DEVICE_SPH

#define SPI_SS_PIN		29			//P0.29		nSEL
#define SPI_MISO_PIN	3			//P0.03		SDO
#define SPI_MOSI_PIN	4			//P0.04		SDI
#define SPI_SCK_PIN		28			//P0.28		SCLK
#define SPI_SDN_PIN		10			//P0.10		SDN	(to reset si4463)
#define SPI_NIRQ_PIN	9			//P0.09		NIRQ (to wake up si4463 by intterupt)


#define PIN_BUTTON1		11
#define PIN_BUTTON2 	12
#define PIN_BUTTON3		24
#define PIN_BUTTON4		25

#define PIN_LED1		13
#define PIN_LED2		14
#define PIN_LED3		15
#define PIN_LED4		16

#define UART_DATA_SEND_ENABLE_PIN		NRF_GPIO_PIN_MAP(0,11)
#define UART_433_RF_POWER_CTRL			NRF_GPIO_PIN_MAP(0,13)

#define SPH_TWI_SCL			NRF_GPIO_PIN_MAP(0,7)
#define SPH_TWI_SDA			NRF_GPIO_PIN_MAP(0,8)

#define SPH_LPS_INT			NRF_GPIO_PIN_MAP(0,23)
#define SPH_HTS_INT			NRF_GPIO_PIN_MAP(0,24)
#define SPH_LIS_INT			NRF_GPIO_PIN_MAP(0,9)

#define CCS_PWR_CTRL	NRF_GPIO_PIN_MAP(1,10)
#define CCS_RESET		NRF_GPIO_PIN_MAP(1,11)
#define CCS_WAKE_UP 	NRF_GPIO_PIN_MAP(1,12)
#define CCS_INT1		NRF_GPIO_PIN_MAP(0,22)

#define WIZFI_RST		NRF_GPIO_PIN_MAP(1,0)		//ACTIVE LOW
#define WIZFI_WAKE_UP	NRF_GPIO_PIN_MAP(1,1)		//ACTIVE hIGH
#define WIZFI_CTS		NRF_GPIO_PIN_MAP(0,19)
#define WIZFI_RTS		NRF_GPIO_PIN_MAP(0,20)
#define WIZFI_TX		NRF_GPIO_PIN_MAP(0,17)
#define WIZFI_RX		NRF_GPIO_PIN_MAP(0,16)

#define GRID_EYE_INT	NRF_GPIO_PIN_MAP(0,14)
//nrf_drv_gpiote_pin_t UART433_DATA_RECEIVE_ISR_PIN = 11;

enum {
	BUTTON_EVT_1 = 1,
	BUTTON_EVT_2,
	BUTTON_EVT_3,
	BUTTON_EVT_4
};




#endif /* APP_HW_CONFIG_H_ */
