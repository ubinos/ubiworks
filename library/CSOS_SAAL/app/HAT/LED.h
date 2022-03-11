/*
 * LED.h
 *
 *  Created on: 2020. 6. 15.
 *      Author: khs
 */

#ifndef APP_LED_H_
#define APP_LED_H_

#include "hw_config.h"
#include "nrf_gpio.h"


#define LED_init()									\
		do{											\
			nrf_gpio_cfg_output(PIN_LED1);			\
			nrf_gpio_cfg_output(PIN_LED2);			\
			nrf_gpio_cfg_output(PIN_LED3);			\
			nrf_gpio_cfg_output(PIN_LED4);			\
													\
			nrf_gpio_pin_set(PIN_LED1);				\
			nrf_gpio_pin_set(PIN_LED2);				\
			nrf_gpio_pin_set(PIN_LED3);				\
			nrf_gpio_pin_set(PIN_LED4);				\
		}while(0)

#define LED_on(pin)			nrf_gpio_pin_clear(pin)

#define LED_off(pin)		nrf_gpio_pin_set(pin)

#define LED_toggle(pin)		nrf_gpio_pin_toggle(pin)

#define LED_all_on()								\
		do{											\
			nrf_gpio_pin_clear(PIN_LED1);			\
			nrf_gpio_pin_clear(PIN_LED2);			\
			nrf_gpio_pin_clear(PIN_LED3);			\
			nrf_gpio_pin_clear(PIN_LED4);			\
		}while(0)

#define LED_all_off()								\
		do{											\
			nrf_gpio_pin_set(PIN_LED1);				\
			nrf_gpio_pin_set(PIN_LED2);				\
			nrf_gpio_pin_set(PIN_LED3);				\
			nrf_gpio_pin_set(PIN_LED4);				\
		}while(0)

#endif /* APP_LED_H_ */
