/*
 * AAT_device_LED.h
 *
 *  Created on: 2019. 4. 25.
 *      Author: seo
 */

#ifndef LIB_TAP_WATER_TAG_ITF_WT_DEVICE_LED_H_
#define LIB_TAP_WATER_TAG_ITF_WT_DEVICE_LED_H_

#include "ubinos.h"

#if (CSOS_SAAL__USE_LIB_AAT_HW_other == 1)

typedef enum{
	RED = 0,
	GREEN,
	BLUE,
	WHITE,
}AAT_LED_Color_t;

void AAT_LED_port_init(void);
void AAT_LED_ON(AAT_LED_Color_t color);
void AAT_LED_OFF(AAT_LED_Color_t color);

#endif /*(CSOS_SAAL__USE_LIB_AAT_HW_other == 1)*/

#endif /* LIB_TAP_WATER_TAG_ITF_AAT_DEVICE_LED_H_ */
