/*
 * AAT_device_LED.c
 *
 *  Created on: 2019. 4. 25.
 *      Author: seo
 */


// header include
#include "AAT_device_LED.h"

#if (CSOS_SAAL__USE_LIB_AAT_HW_other == 1)

// hw_config include
#include "hw_config.h"


// nrf driver include
#include "nrf_gpio.h"

void AAT_LED_port_init(void) {
	AAT_Pin_cfg_init(LED_RED);
	AAT_Pin_cfg_init(LED_GREEN);
	AAT_Pin_cfg_init(LED_BLUE);
	//PT_Pin_cfg_init(AAT_BOARD_PIN_SUB_PW_EN);
}

void AAT_LED_ON(AAT_LED_Color_t color) {

	switch(color)
	{
	case RED:
		nrf_gpio_pin_set(LED_RED);
		nrf_gpio_pin_clear(LED_GREEN);
		nrf_gpio_pin_clear(LED_BLUE);
		break;
	case GREEN:
		nrf_gpio_pin_set(LED_GREEN);
		nrf_gpio_pin_clear(LED_RED);
		nrf_gpio_pin_clear(LED_BLUE);
		break;
	case BLUE:
		nrf_gpio_pin_set(LED_BLUE);
		nrf_gpio_pin_clear(LED_GREEN);
		nrf_gpio_pin_clear(LED_RED);
		break;
	case WHITE:
		nrf_gpio_pin_set(LED_GREEN);
		nrf_gpio_pin_set(LED_RED);
		nrf_gpio_pin_set(LED_BLUE);
		break;
	}
}

void AAT_LED_OFF(AAT_LED_Color_t color) {

	switch(color)
	{
	case RED:
		nrf_gpio_pin_clear(LED_RED);
		break;
	case GREEN:
		nrf_gpio_pin_clear(LED_BLUE);
		break;
	case BLUE:
		nrf_gpio_pin_clear(LED_BLUE);
		break;
	case WHITE:
		nrf_gpio_pin_clear(LED_RED);
		nrf_gpio_pin_clear(LED_GREEN);
		nrf_gpio_pin_clear(LED_BLUE);
		break;
	}
}

#endif /*(CSOS_SAAL__USE_LIB_AAT_HW_other == 1)*/