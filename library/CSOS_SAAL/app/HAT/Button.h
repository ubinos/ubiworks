/*
 * Button.h
 *
 *  Created on: 2020. 6. 15.
 *      Author: khs
 */

#ifndef APP_BUTTON_H_
#define APP_BUTTON_H_


#include "hw_config.h"
#include "nrf_drv_gpiote.h"

// hw_config include

void Button_init(nrf_drv_gpiote_evt_handler_t handler1,
				nrf_drv_gpiote_evt_handler_t handler2,
				nrf_drv_gpiote_evt_handler_t handler3,
				nrf_drv_gpiote_evt_handler_t handler4)
{
	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
	in_config.pull = NRF_GPIO_PIN_PULLUP;

	nrf_drv_gpiote_in_init(PIN_BUTTON1, &in_config,	(nrf_drv_gpiote_evt_handler_t)handler1);
	nrf_drv_gpiote_in_init(PIN_BUTTON2, &in_config,	(nrf_drv_gpiote_evt_handler_t)handler2);
	nrf_drv_gpiote_in_init(PIN_BUTTON3, &in_config,	(nrf_drv_gpiote_evt_handler_t)handler3);
	nrf_drv_gpiote_in_init(PIN_BUTTON4, &in_config,	(nrf_drv_gpiote_evt_handler_t)handler4);

	nrf_drv_gpiote_in_event_enable(PIN_BUTTON1, true);
	nrf_drv_gpiote_in_event_enable(PIN_BUTTON2, true);
	nrf_drv_gpiote_in_event_enable(PIN_BUTTON3, true);
	nrf_drv_gpiote_in_event_enable(PIN_BUTTON4, true);
}


#endif /* APP_BUTTON_H_ */
