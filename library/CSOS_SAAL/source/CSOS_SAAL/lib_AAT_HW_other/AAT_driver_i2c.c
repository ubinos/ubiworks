/*
 * AAT_i2c_ext_rtc_spo2.c
 *
 *  Created on: 2016. 5. 26.
 *      Author: Administrator
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "AAT_driver_i2c.h"

#if (CSOS_SAAL__USE_LIB_AAT_HW_other == 1)

#include "nrf_drv_twi.h"
#include "app_util_platform.h"

#include "hw_config.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define OPT3001_I2C_ADDR1			0x44	//(0x44 << 1)
#define OPT3001_I2C_ADDR2			0x45	//(0x45 << 1)
#define OPT3001_I2C_ADDR3			0x46	//(0x46 << 1)
#define OPT3001_I2C_ADDR4			0x47	//(0x47 << 1)

#define OPT3001_REG_RESULT			0x00
#define OPT3001_REG_CONFIG			0x01

#define OPT3001_REG_LOW_LIMIT		0x02
#define OPT3001_REG_HIGH_LIMIT		0x03

#define OPT3001_REG_MANUFACTUR_ID	0x7E
#define OPT3001_REG_DEVICE_ID		0x7F

#define OPT3001_DEVICE_ID			0x3001

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define OPT3001_CFG_FC_1    0x0000		// Fault Count field
#define OPT3001_CFG_FC_2    0x0100
#define OPT3001_CFG_FC_4    0x0200
#define OPT3001_CFG_FC_8    0x0300

#define OPT3001_CFG_MASK    0x0400		// Mask Exponent field
#define OPT3001_CFG_POLPOS  0x0800		// Polarity field
#define OPT3001_CFG_LATCH   0x1000		// Latch field
#define OPT3001_CFG_FLAGL   0x2000		// Flag low field
#define OPT3001_CFG_FLAGH   0x4000		// Flag high field
#define OPT3001_CFG_CRF     0x8000		// Conversion ready field
#define OPT3001_CFG_OVF     0x0001		// Overflow flag field
#define OPT3001_CFG_SHDN    0x0000
#define OPT3001_CFG_SHOT    0x0002
#define OPT3001_CFG_CONT    0x0004		// Conversion time field
#define OPT3001_CFG_100MS   0x0000
#define OPT3001_CFG_800MS   0x0008
#define OPT3001_CFG_RNAUTO  0x00C0

#define OPT3001_CFG (OPT3001_CFG_FC_1 | OPT3001_CFG_SHOT | OPT3001_CFG_100MS | OPT3001_CFG_RNAUTO )
#define OPT3001_CFG_DEFAULT 0x10C8
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static nrf_drv_twi_t twi_master = NRF_DRV_TWI_INSTANCE(0);

static mutex_pt AAT_i2c_mutex;

void AAT_twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
	/*
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
            {
                data_handler(m_sample);
            }
            m_xfer_done = true;
            break;
        default:
            break;
    }
    */
}

//
void AAT_i2c_init() {
	nrf_drv_twi_config_t twi_master_config = { .scl = AAT_BOARD_I2C_SCL, .sda =
			AAT_BOARD_I2C_SDA, .frequency = TWI_FREQUENCY_FREQUENCY_K100,
	.interrupt_priority = APP_IRQ_PRIORITY_THREAD };

	ret_code_t err_code;

	err_code = nrf_drv_twi_init(&twi_master, &twi_master_config, NULL, NULL);
	APP_ERROR_CHECK(err_code);

	nrf_drv_twi_enable(&twi_master);

	int r;
	r = mutex_create(&AAT_i2c_mutex);
	if (0 != r) {
		logme("fail at mutex_create");
	}

}
void AAT_i2c_uninit(void) {
	nrf_drv_twi_uninit(&twi_master);
}

//void i2cm_uninit(void);
unsigned int AAT_i2c_read(uint8_t pSlaveAddress, uint8_t *pWriteBuffer, uint32_t pWDataLength,
												 uint8_t *pReadBuffer, uint32_t pRDataLength) {
	//uint32_t err_code;

	mutex_lock(AAT_i2c_mutex);

	/* if do not initialize i2c master, initialize i2c master */
	nrf_drv_twi_tx(&twi_master, pSlaveAddress, pWriteBuffer, pWDataLength, true);
	nrf_drv_twi_rx(&twi_master, pSlaveAddress, pReadBuffer, pRDataLength);

	mutex_unlock(AAT_i2c_mutex);
	return 0;
}

unsigned int AAT_i2c_write(uint8_t pSlaveAddress, uint8_t *pWriteBuffer, uint32_t pDataLength) {

	mutex_lock(AAT_i2c_mutex);

	nrf_drv_twi_tx(&twi_master, pSlaveAddress, pWriteBuffer, pDataLength, false);

	mutex_unlock(AAT_i2c_mutex);
	return 0;
}

void OPT3001_init()
{
	bool check;
	uint8_t Write_Byte;
	uint16_t Read_Byte;

	check = OPT3001_read_DeviceID(&Read_Byte);

	while (!check) {
		task_sleep(100);
		check = OPT3001_read_DeviceID(&Read_Byte);
	}

	Write_Byte = OPT3001_REG_CONFIG;
	AAT_i2c_write(OPT3001_I2C_ADDR1, &Write_Byte, 1);
	AAT_i2c_read(OPT3001_I2C_ADDR1, &Write_Byte, 1, (uint8_t*)&Read_Byte, 2);
}

bool OPT3001_read_DeviceID(uint16_t *Read_Byte)
{
	uint16_t Write_Byte = OPT3001_REG_DEVICE_ID;
	uint16_t deviceID = 0;

	AAT_i2c_read(OPT3001_I2C_ADDR1, (uint8_t*)&Write_Byte, 2, (uint8_t*)Read_Byte, 2);

	deviceID = (*Read_Byte << 8) | (*Read_Byte >> 8);

	if(deviceID == OPT3001_DEVICE_ID)
		return true;
	else
		return false;
}

int OPT3001_lux_read()
{
	uint16_t data;
	uint8_t temp[3];
	int i;

	temp[0] = OPT3001_REG_CONFIG;
	temp[1] = OPT3001_CFG & 0xFF;
	temp[2] = OPT3001_CFG >> 8;

	AAT_i2c_write(OPT3001_I2C_ADDR1, temp, 3);

	i = 100;
	do {
		task_sleep(10);

		temp[0] = OPT3001_REG_CONFIG;
		AAT_i2c_write(OPT3001_I2C_ADDR1, temp, 1);
		AAT_i2c_read(OPT3001_I2C_ADDR1, temp, 1, (uint8_t*)&data, 2);

		if(data & OPT3001_CFG_CRF) {
			break;
		}
		i--;
	}while(i);

	if(i == 0) { return -1; }


	temp[0] = OPT3001_REG_RESULT;
	AAT_i2c_write(OPT3001_I2C_ADDR1, temp, 1);
	AAT_i2c_read(OPT3001_I2C_ADDR1, temp, 1, (uint8_t*)&data, 2);

	data = (data << 8) | (data >> 8);

	uint8_t exp = (data >> 12) & 0x0F;
	uint32_t lsb_size_x100 = (1 << exp);

	data &= 0x0FFF;

	return (((int)data * lsb_size_x100) / 100);
}

#endif /*(CSOS_SAAL__USE_LIB_AAT_HW_other == 1)*/