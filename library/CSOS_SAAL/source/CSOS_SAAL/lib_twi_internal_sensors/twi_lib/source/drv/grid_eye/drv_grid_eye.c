/*
 * drv_grid_eye.c
 *
 *  Created on: 2021. 6. 8.
 *      Author: sayyo
 */

#include "grid_eye/drv_grid_eye.h"
#include "ubinos.h"
#include "string.h"

#include "grid_eye/drv_grid_eye.h"
#include "nrf_drv_gpiote.h"
#include "app_scheduler.h"
#include "nrf_log.h"
#include "nrf_delay.h"
#include "macros_common.h"


//extern mutex_pt _g_mutex_peripheral;


typedef struct {
	uint8_t 				twi_addr;
	uint32_t				pint_int;
	nrf_drv_twi_t			const * p_twi_instance;
	nrf_drv_twi_config_t 	const * p_twi_config;
	//drv_grid_eye_handler_t	evt_handler
	bool 					enabled;

} drv_grid_eye_cfg_t;
static drv_grid_eye_cfg_t m_drv_grid_eye;

static uint32_t reg_read(uint8_t reg_addr, uint8_t * p_reg_val) {
	ret_code_t err;

	err = nrf_drv_twi_tx( m_drv_grid_eye.p_twi_instance,
							m_drv_grid_eye.twi_addr,
							&reg_addr,
							1,
							true );
	RETURN_IF_ERROR(err);

	err = nrf_drv_twi_rx( m_drv_grid_eye.p_twi_instance,
							m_drv_grid_eye.twi_addr,
							p_reg_val,
							1 );
	RETURN_IF_ERROR(err);

	return NRF_SUCCESS;
}
static uint32_t reg_read_ext(uint8_t reg_addr, uint8_t * p_reg_val, uint8_t p_reg_size) {
	ret_code_t err;

	err = nrf_drv_twi_tx( m_drv_grid_eye.p_twi_instance,
							m_drv_grid_eye.twi_addr,
							&reg_addr,
							1,
							true );
	RETURN_IF_ERROR(err);

	err = nrf_drv_twi_rx( m_drv_grid_eye.p_twi_instance,
							m_drv_grid_eye.twi_addr,
							p_reg_val,
							p_reg_size );
	RETURN_IF_ERROR(err);

	return NRF_SUCCESS;
}

static uint32_t reg_write(uint8_t reg_addr, uint8_t reg_val) {
	ret_code_t err;
	uint8_t buffer[2] = {reg_addr, reg_val};

	err = nrf_drv_twi_tx( m_drv_grid_eye.p_twi_instance,
							m_drv_grid_eye.twi_addr,
							buffer,
							2,
							false );
	RETURN_IF_ERROR(err);

	return NRF_SUCCESS;
}
static uint32_t drv_low_grid_eye_enable(void) {
	ret_code_t err = NRF_SUCCESS;

	nrf_drv_twi_enable(m_drv_grid_eye.p_twi_instance);

	return err;
}
static uint32_t drv_low_grid_eye_disable(void) {
	ret_code_t err = NRF_SUCCESS;

	nrf_drv_twi_disable(m_drv_grid_eye.p_twi_instance);

	return err;
}
static uint32_t drv_low_set_power_mode(power_modes mode) {
	ret_code_t err = NRF_SUCCESS;

	err = drv_low_grid_eye_enable();
	APP_ERROR_CHECK(err);

	err = reg_write(GRID_EYE_PCTL, mode);
	RETURN_IF_ERROR(err);

	err = drv_low_grid_eye_disable();
	APP_ERROR_CHECK(err);

	return err;
}
static uint32_t drv_low_sw_reset(void) {
	ret_code_t err = NRF_SUCCESS;

	err = drv_low_grid_eye_enable();
	APP_ERROR_CHECK(err);

	err = reg_write(GRID_EYE_RST, GRID_EYE_INITIAL_RESET);
	RETURN_IF_ERROR(err);

	nrf_delay_ms(2);

	err = reg_write(GRID_EYE_RST, GRID_EYE_FLAG_RESET);
	RETURN_IF_ERROR(err);

	err = drv_low_grid_eye_disable();
	APP_ERROR_CHECK(err);

	return err;
}
static uint32_t drv_low_disable_interrupt(void) {
	ret_code_t err = NRF_SUCCESS;
	uint8_t is_enable 	= GRID_EYE_INT_DISABLED;
	uint8_t int_mode 	= GRID_EYE_ABSOLUTE_VALUE;

	uint8_t cmd = ((is_enable << 1 | int_mode) & 0x03);

	err = drv_low_grid_eye_enable();
	APP_ERROR_CHECK(err);

	err = reg_write(GRID_EYE_INTC, cmd);
	RETURN_IF_ERROR(err);

	err = drv_low_grid_eye_disable();
	APP_ERROR_CHECK(err);

	return err;
}
static uint32_t drv_low_enable_interrupt(void) {
	ret_code_t err = NRF_SUCCESS;
	uint8_t is_enable	= GRID_EYE_INT_ENABLED;
	uint8_t int_mode	= GRID_EYE_ABSOLUTE_VALUE;

	uint8_t cmd = ((is_enable << 1 | int_mode) & 0x03);

	err = drv_low_grid_eye_enable();
	APP_ERROR_CHECK(err);

	err = reg_write(GRID_EYE_INTC, cmd);
	RETURN_IF_ERROR(err);

	err = drv_low_grid_eye_disable();
	APP_ERROR_CHECK(err);

	return err;
}
static uint32_t drv_low_fps_set(uint8_t mode) {
	ret_code_t err = NRF_SUCCESS;

	err = drv_low_grid_eye_enable();
	APP_ERROR_CHECK(err);

	err = reg_write(GRID_EYE_FPSC, mode);
	RETURN_IF_ERROR(err);

	err = drv_low_grid_eye_disable();
	APP_ERROR_CHECK(err);

	return err;
}
static int custom_uint12_to_int12(uint16_t data) {
	int16_t _data = (data << 4);
	_data = _data >> 4;
	_data = _data * 25;
	return _data;
}
static float custom_int_12_to_float(uint16_t data) {
	int16_t _data = (data << 4);

	return _data >> 4;
	////changed at 07.19 - float연산 속도가 너무 길어서 변경
	////				 - 최종 conversion 연산에서 *0.25를 해주던걸 여기서 >>2로 변경
//	int16_t _data = (data << 4);
//
//	return data >> 2;
}
static float custom_signed_12_to_float(uint16_t val) {
	uint16_t absolute_val = (val & 0x7ff);

	return (val & 0x800) ? 0 - (float)absolute_val : (float)absolute_val;
}


uint32_t drv_grid_eye_init(drv_grid_eye_init_t * p_params) {
	ret_code_t err;

    NULL_PARAM_CHECK(p_params);
    NULL_PARAM_CHECK(p_params->p_twi_instance);
    NULL_PARAM_CHECK(p_params->p_twi_cfg);
//    NULL_PARAM_CHECK(p_params->evt_handler);

    m_drv_grid_eye.twi_addr 		= p_params->twi_addr;
    m_drv_grid_eye.pint_int 		= p_params->pin_int;
    m_drv_grid_eye.p_twi_instance 	= p_params->p_twi_instance;
    m_drv_grid_eye.p_twi_config		= p_params->p_twi_cfg;

    m_drv_grid_eye.enabled			= true;

//    if(nrf_drv_twi_is_enabled)
    nrf_drv_twi_enable(m_drv_grid_eye.p_twi_instance);
	
    err = drv_low_grid_eye_enable();
    APP_ERROR_CHECK(err);

    err = drv_low_set_power_mode(GRID_EYE_NORMAL_MODE);
    APP_ERROR_CHECK(err);
    nrf_delay_ms(50);

    err = drv_low_sw_reset();
    APP_ERROR_CHECK(err);

    err = drv_low_disable_interrupt();
    APP_ERROR_CHECK(err);

    err = drv_low_fps_set(GRID_EYE_FPS_10);
    APP_ERROR_CHECK(err);
    nrf_delay_ms(100);

    err = drv_low_grid_eye_disable();
    APP_ERROR_CHECK(err);

    return err;
}

uint32_t drv_grid_eye_reset(void) {
	ret_code_t err = NRF_SUCCESS;

	err = reg_write(GRID_EYE_RST, GRID_EYE_FLAG_RESET);
	APP_ERROR_CHECK(err);

	return err;
}

uint32_t drv_grid_eye_enable(void) {
	ret_code_t err = NRF_SUCCESS;

	return err;
}

uint32_t drv_grid_eye_disable(void) {
	ret_code_t err = NRF_SUCCESS;

	return err;
}

uint32_t drv_grid_eye_get_pixels(int * buf, uint8_t size) {
	ret_code_t err = NRF_SUCCESS;
	uint16_t recast;
//	float converted;
	int16_t converted_int;
	uint8_t bytes_to_read = min((uint8_t)(size << 1), (uint8_t)(GRID_EYE_PIXEL_ARRAY_SIZE << 1));
	uint8_t raw_array[bytes_to_read];
	uint8_t cmd;
//	uint8_t temp_pixel;
	err = drv_low_grid_eye_enable();
	APP_ERROR_CHECK(err);

//	cmd = GRID_EYE_PIXEL_OFFSET;
//	for(int i=0; i<size; i++) {
//		cmd += 2;
//		err = nrf_drv_twi_tx( m_drv_grid_eye.p_twi_instance,
//								m_drv_grid_eye.twi_addr,
//								&cmd,
//								1,
//								false );
//		APP_ERROR_CHECK(err);
//
//		uint8_t _byte[2];
//		err = nrf_drv_twi_rx( m_drv_grid_eye.p_twi_instance,
//								m_drv_grid_eye.twi_addr,
//								_byte,
//								2 );
//		APP_ERROR_CHECK(err);
//		raw_array[i*2] 		= _byte[0];
//		raw_array[i*2 +1]	= _byte[1];
//	}


	cmd = GRID_EYE_PIXEL_OFFSET;
	//mutex_lock(_g_mutex_peripheral);
	err = nrf_drv_twi_tx( m_drv_grid_eye.p_twi_instance,
							m_drv_grid_eye.twi_addr,
							&cmd,
							1,
							false );
	APP_ERROR_CHECK(err);

	err = nrf_drv_twi_rx( m_drv_grid_eye.p_twi_instance,
							m_drv_grid_eye.twi_addr,
							raw_array,
							bytes_to_read);
	APP_ERROR_CHECK(err);
	//mutex_unlock(_g_mutex_peripheral);

    err = drv_low_grid_eye_disable();
    APP_ERROR_CHECK(err);

	for(int i=0; i<size; i++) {
		uint8_t pos = i<<1;
		recast = ((uint16_t)raw_array[pos+1]<<8) | ((uint16_t)raw_array[pos]);
//		converted = custom_int_12_to_float(recast) * GRID_EYE_PIXEL_TEMP_CONVERSION;
//		buf[i] = converted;
		converted_int = custom_uint12_to_int12(recast);
		buf[i] = converted_int;
	}

	return err;
}

float drv_grid_eye_read_thermistor(void) {
	ret_code_t err;
	uint8_t raw[2];
	uint16_t recast;
	uint8_t cmd;

	err = drv_low_grid_eye_enable();
	APP_ERROR_CHECK(err);

	cmd = GRID_EYE_TTHL;
	//mutex_lock(_g_mutex_peripheral);
	err = nrf_drv_twi_tx( m_drv_grid_eye.p_twi_instance,
							m_drv_grid_eye.twi_addr,
							&cmd,
							1,
							false);
	APP_ERROR_CHECK(err);

	err = nrf_drv_twi_rx( m_drv_grid_eye.p_twi_instance,
							m_drv_grid_eye.twi_addr,
							raw,
							2);
	APP_ERROR_CHECK(err);
	//mutex_unlock(_g_mutex_peripheral);

	err = drv_low_grid_eye_disable();
	APP_ERROR_CHECK(err);

	recast = ((uint16_t)raw[1] << 8) | ((uint16_t)raw[0]);
	return custom_signed_12_to_float(recast) * (float)GRID_EYE_THERMISTOR_CONVERSION;
}





















//EOF
