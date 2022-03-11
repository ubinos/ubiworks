/*
 * drv_grid_eye.h
 *
 *  Created on: 2021. 6. 8.
 *      Author: sayyo
 */

#ifndef APP_TWI_SENSOR_READ_TWI_LIB_INCLUDE_DRV_GRID_EYE_DRV_GRID_EYE_H_
#define APP_TWI_SENSOR_READ_TWI_LIB_INCLUDE_DRV_GRID_EYE_DRV_GRID_EYE_H_


#include "nrf_drv_twi.h"
#include "stdint.h"

typedef enum {
	DRV_GRID_EYE_EVT_DATA = 0,
	DRV_GRID_EYE_EVT_ERROR,
} drv_grid_eye_evt_t;

typedef struct {
	uint8_t					twi_addr;
	uint32_t				pin_int;
	nrf_drv_twi_t			const * p_twi_instance;
	nrf_drv_twi_config_t	const * p_twi_cfg;
	//drv_grid_eye_evt_handler_t	evt_handler;
} drv_grid_eye_init_t;

#define GRID_EYE_QUEUE_MAX_SIZE			100
#define GRID_EYE_PIXEL_ARRAY_SIZE		64
#define GRID_EYE_PIXEL_TEMP_CONVERSION	.25
#define GRID_EYE_THERMISTOR_CONVERSION	.0625

enum {
  GRID_EYE_PCTL = 0x00,
  GRID_EYE_RST = 0x01,
  GRID_EYE_FPSC = 0x02,
  GRID_EYE_INTC = 0x03,
  GRID_EYE_STAT = 0x04,
  GRID_EYE_SCLR = 0x05,
  // 0x06 reserved
  GRID_EYE_AVE = 0x07,
  GRID_EYE_INTHL = 0x08,
  GRID_EYE_INTHH = 0x09,
  GRID_EYE_INTLL = 0x0A,
  GRID_EYE_INTLH = 0x0B,
  GRID_EYE_IHYSL = 0x0C,
  GRID_EYE_IHYSH = 0x0D,
  GRID_EYE_TTHL = 0x0E,
  GRID_EYE_TTHH = 0x0F,
  GRID_EYE_INT_OFFSET = 0x010,
  GRID_EYE_PIXEL_OFFSET = 0x80
};

typedef enum _power_modes {
  GRID_EYE_NORMAL_MODE = 0x00,
  GRID_EYE_SLEEP_MODE = 0x01,
  GRID_EYE_STAND_BY_60 = 0x20,
  GRID_EYE_STAND_BY_10 = 0x21
} power_modes;

enum sw_resets {
	GRID_EYE_FLAG_RESET = 0x30,
	GRID_EYE_INITIAL_RESET = 0x3F
};

enum frame_rates {
	GRID_EYE_FPS_10 = 0x00,
	GRID_EYE_FPS_1 = 0x01
};

enum int_enables {
	GRID_EYE_INT_DISABLED = 0x00,
	GRID_EYE_INT_ENABLED = 0x01
};

enum int_modes {
	GRID_EYE_DIFFERENCE = 0x00,
	GRID_EYE_ABSOLUTE_VALUE = 0x01
};
#define GRID_EYE_INT_MODE_DEFAULT 	GRID_EYE_ABSOLUTE_VALUE


uint32_t drv_grid_eye_init(drv_grid_eye_init_t * p_params);

uint32_t drv_grid_eye_enable(void);

uint32_t drv_grid_eye_disable(void);

uint32_t drv_grid_eye_reset(void);

//uint32_t drv_grid_eye_get_pixels(int * buf, uint8_t size);
uint32_t drv_grid_eye_get_pixels(int * buf, uint8_t size);

float drv_grid_eye_read_thermistor(void);




#endif /* APP_TWI_SENSOR_READ_TWI_LIB_INCLUDE_DRV_GRID_EYE_DRV_GRID_EYE_H_ */
