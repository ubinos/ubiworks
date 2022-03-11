/*
 * AAT_driver_spi.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: Kwon
 */

//#include "nrf_gpio.h"
//#include "app_util_platform.h"

#include "AAT_driver_spi.h"

#if (CSOS_SAAL__USE_LIB_AAT_HW_other == 1)

#include "hw_config.h"

#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "app_util_platform.h"

static const nrf_drv_spi_t AAT_spi = NRF_DRV_SPI_INSTANCE(1);  /**< SPI instance. */

static mutex_pt AAT_spi_mutex;

void AAT_spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
	/*
    spi_xfer_done = true;
    NRF_LOG_INFO("Transfer completed.");
    if (m_rx_buf[0] != 0)
    {
        NRF_LOG_INFO(" Received:");
        NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    }
    */
}

void AAT_spi_init(void)
{
	int r;

	r = mutex_create(&AAT_spi_mutex);
	if(0 != r){
		logme("fail at mutex_create");
	}

	nrf_drv_spi_config_t AAT_spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;

	AAT_spi_config.miso_pin	= AAT_BOARD_PIN_FLASH_ACCEL_SPI_MISO;
	AAT_spi_config.mosi_pin	= AAT_BOARD_PIN_FLASH_ACCEL_SPI_MOSI;
	AAT_spi_config.sck_pin 	= AAT_BOARD_PIN_FLASH_ACCEL_SPI_SCK;
	AAT_spi_config.ss_pin		= NRF_DRV_SPI_PIN_NOT_USED;
	AAT_spi_config.frequency = NRF_DRV_SPI_FREQ_4M;

	nrf_gpio_cfg_output(AAT_BOARD_PIN_FLASH_SPI_CS);
	nrf_gpio_cfg_output(AAT_BOARD_PIN_ACCEL_SPI_CS);

	nrf_gpio_pin_set(AAT_BOARD_PIN_FLASH_SPI_CS);
	nrf_gpio_pin_set(AAT_BOARD_PIN_ACCEL_SPI_CS);

	nrf_drv_spi_init(&AAT_spi, &AAT_spi_config, NULL, NULL);

}

void AAT_spi_uninit(void){
	nrf_drv_spi_uninit(&AAT_spi);
}

uint32_t AAT_spi_write_ACC(uint8_t* buf, uint8_t length){
	uint32_t result;
	mutex_lock(AAT_spi_mutex);

	//clear ACC_CS_pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_FLASH_SPI_CS);
	nrf_gpio_pin_clear(AAT_BOARD_PIN_ACCEL_SPI_CS);

	result = nrf_drv_spi_transfer(&AAT_spi, buf, length, NULL, NULL);

	//set ACC_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_ACCEL_SPI_CS);

	mutex_unlock(AAT_spi_mutex);
	
	return result;

}

void AAT_spi_read_ACC(uint8_t* buf, uint8_t length){

	mutex_lock(AAT_spi_mutex);

	//clear ACC_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_FLASH_SPI_CS);
	nrf_gpio_pin_clear(AAT_BOARD_PIN_ACCEL_SPI_CS);

	nrf_drv_spi_transfer(&AAT_spi, NULL, NULL, buf, length);

	//set ACC_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_ACCEL_SPI_CS);

	mutex_unlock(AAT_spi_mutex);
}

void AAT_spi_write_read_ACC(uint8_t* w_buf, uint8_t w_len, uint8_t* r_buf, uint8_t r_len){

	mutex_lock(AAT_spi_mutex);

	//clear ACC_CS_PIN
	nrf_gpio_pin_set(AAT_BOARD_PIN_FLASH_SPI_CS);
	nrf_gpio_pin_clear(AAT_BOARD_PIN_ACCEL_SPI_CS);

	nrf_drv_spi_transfer(&AAT_spi, w_buf, w_len, NULL, NULL);
	nrf_drv_spi_transfer(&AAT_spi, NULL, NULL, r_buf, r_len);

	//set Flash_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_ACCEL_SPI_CS);

	mutex_unlock(AAT_spi_mutex);
}

void AAT_spi_write_Flash(uint8_t* buf, uint8_t length){

	mutex_lock(AAT_spi_mutex);

	//clear Flash_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_ACCEL_SPI_CS);
	nrf_gpio_pin_clear(AAT_BOARD_PIN_FLASH_SPI_CS);

	nrf_drv_spi_transfer(&AAT_spi, buf, length, NULL, 0);

	//set Flash_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_FLASH_SPI_CS);

	mutex_unlock(AAT_spi_mutex);
}

void AAT_spi_read_Flash(uint8_t* buf, uint8_t length){

	mutex_lock(AAT_spi_mutex);

	//clear Flash_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_ACCEL_SPI_CS);
	nrf_gpio_pin_clear(AAT_BOARD_PIN_FLASH_SPI_CS);

	nrf_drv_spi_transfer(&AAT_spi, NULL, 0, buf, length);

	//set Flash_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_FLASH_SPI_CS);

	mutex_unlock(AAT_spi_mutex);
}

void AAT_spi_write_read_Flash(uint8_t* w_buf, uint8_t w_len, uint8_t* r_buf, uint8_t r_len){

	mutex_lock(AAT_spi_mutex);

	//clear Flash_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_ACCEL_SPI_CS);
	nrf_gpio_pin_clear(AAT_BOARD_PIN_FLASH_SPI_CS);

	nrf_drv_spi_transfer(&AAT_spi, w_buf, w_len, NULL, 0);

	nrf_drv_spi_transfer(&AAT_spi, NULL, 0, r_buf, r_len);

	//set Flash_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_FLASH_SPI_CS);

	mutex_unlock(AAT_spi_mutex);
}

void AAT_spi_write_write_Flash(uint8_t* w_buf, uint8_t w_len, uint8_t* w_next_buf, uint8_t r_next_len){

	mutex_lock(AAT_spi_mutex);

	//clear Flash_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_ACCEL_SPI_CS);
	nrf_gpio_pin_clear(AAT_BOARD_PIN_FLASH_SPI_CS);

	nrf_drv_spi_transfer(&AAT_spi, w_buf, w_len, NULL, 0);

	nrf_drv_spi_transfer(&AAT_spi, w_next_buf, r_next_len, NULL, 0);

	//set Flash_CS_Pin
	nrf_gpio_pin_set(AAT_BOARD_PIN_ACCEL_SPI_CS);
	nrf_gpio_pin_set(AAT_BOARD_PIN_FLASH_SPI_CS);

	mutex_unlock(AAT_spi_mutex);
}

#endif /*(CSOS_SAAL__USE_LIB_AAT_HW_other == 1)*/