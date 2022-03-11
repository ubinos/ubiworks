/*
 * hw_config.h
 *
 *  Created on: 2020. 06. 11.
 *      Author: YJPark
 */

#ifndef APPLICATION_PAAR_FRONTEND_EXE_PAAR_FRONTEND_ITF_PIN_CONFIG_H_
#define APPLICATION_PAAR_FRONTEND_EXE_PAAR_FRONTEND_ITF_PIN_CONFIG_H_


// ESTK

#define PIN_BUTTON1			11
#define PIN_BUTTON2			12
#define PIN_BUTTON3			24
#define PIN_BUTTON4			25

#define PIN_LED1			(((1) << 5) | ((11) & 0x1F))
#define PIN_LED2			(((1) << 5) | ((13) & 0x1F))
#define PIN_LED3			(((1) << 5) | ((15) & 0x1F))
#define PIN_LED4			(((1) << 5) | ((14) & 0x1F)) // NC



// 40 DK
/*
#define PIN_BUTTON1			11
#define PIN_BUTTON2			12
#define PIN_BUTTON3			24
#define PIN_BUTTON4			25

#define PIN_LED1			13
#define PIN_LED2			14
#define PIN_LED3			15
#define PIN_LED4			16
*/


#endif /* APPLICATION_PAAR_FRONTEND_EXE_PAAR_FRONTEND_ITF_PIN_CONFIG_H_ */
