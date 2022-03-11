/*
 * TMR_PPI_SAADC.h
 *
 *  Created on: 2020. 5. 27.
 *      Author: khs
 */


/* # where can i find parameters? #
 *
 * SAADC
 * SAADC OVERSAMPLE 				- .h
 * Number of SAADC buffer in use 	- .h
 * Which buffer is in use			- .h
 * Which channel is in use			- .h
 * Unit config except OVERSAMPLE	- .c
 * Channel config(gain, ref, ...)	- .c
 * Callback handler					- .c
 * Limit for handler limit event	- .c
 * etc callback handler details		- .c
 *
 * TMR
 * TMR number in use				- .h
 * TMR compare event interval		- .h
 * TMR freq							- .c
 * TMR buffer bit width				- .c
 *
 * PPI settings						- .c
 *
 * Sensor related					- .h
 * */


#ifndef APP_TMR_PPI_SAADC_H_
#define APP_TMR_PPI_SAADC_H_

#include <stdio.h>

#include "nrf_drv_timer.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_gpiote.h"


typedef struct _saadc_packet{												// packet type for external path
	uint8_t CRNT_OVR;
	uint8_t CRNT_LVL;
}SAADC_PACKET;


#define SAADC_OVERSAMPLE NRF_SAADC_OVERSAMPLE_DISABLED						// SAADC OVERSAMPLE mode
#define SAMPLES_IN_BUFFER 		1											// Collectable SAMPLE count of each buffer - if you want more samples in buffer, you should modify src file.
#define BUFFER_CNT_IN_USE		2											// Buffer count in use
#define SAADC_BUFFER_A			0											// Buffer 1
#define SAADC_BUFFER_B			1											// Buffer 2
#define SAADC_CHANNEL_A			1											// Channel number in use

#define TIMER_NUMBER			1											// TMR number in use (ex. 2 : TMR2)
#define TIMER_COMPARE_MS		1000										// Compare value for TMR COMPARE EVENT (msec)

// If output of sensor is in shape of ripple, only use local maximum.	-	if not, SENSOR_RIPPLE_CYCLE should be 0, SAMPLING_TIMES 1
#define SENSOR_RIPPLE_CYCLE		20											// Sensor data cycle in msec
#define SAMPLING_TIMES			30											// In one data cycle, n times sample
																			// SENSOR_RIPPLE_CYCLE / SAMPLING_TIMES should not be less than saadc channel's acquisition time.
#define CALIB_CYCLE				3											// The value for how many cycles to be used for the calibration value.
																			// The average of the values in each cycle is the final calibration value.

#define CHECK_WAIT_CYCLE		5											// A variable that determines how many cycles
																			// to wait for power to be turned on after a calibration.
#define CHECK_SEQ_CYCLE			30											// Number of cycles consumed in the check sequence.
#define CHECK_SEQ_TIME_RATIO	10											// Ratio of timer period to check sequence cycle:
																			// Check sequence cycle = counter cycle / CHECK_SEQ_TIME_RATIO
																			// should be less than 50

// Sensor Output Related
#define MAX_CRNT				15											// Maximum current level value for packet
#define RESOLUTION				16384										// ADC Resolution maximum value
#define VREF					825											// Ref mV
#define SENSITIVITY				60											// Sensor mV/A
#define REVERSE_GAIN			3											// 1/ADC_Gain


// If the boundary(LIMIT_BOUND) is crossed LIMIT_BOUND_COUNT times, a limit event occurs.
#if(TEST_DEVICE_TYPE == TEST_DEVICE_TYPE_ELECTRIC_FAN)

#define LIMIT_BOUND				50//120											// Limit event boundary [bit]
#define LIMIT_BOUND_COUNT		5//10											// Limit event times allowed


#else
#define LIMIT_BOUND				100//120											// Limit event boundary [bit]
#define LIMIT_BOUND_COUNT		10//10											// Limit event times allowed
#endif

void timer_handler(nrf_timer_event_t event_type, void* p_context);		// Empty func for reference of TMR Initializing
void saadc_sampling_event_enable(void);									// PPI enable
void saadc_sampling_event_disable(void);								// PPI disable
void saadc_sampling_event_init(void);									// TMR & PPI initialize
void saadc_callback(nrf_drv_saadc_evt_t const * p_event);				// Event handler for SAADC
void saadc_init(void);													// SAADC initialize
void saadc_off_calib(void);												// declare saadc output of initial state.
void saadc_get_packet(SAADC_PACKET* node, uint8_t crnt_max);			// get info using SAADC_PACKET node.

#endif /* APP_TMR_PPI_SAADC_H_ */
