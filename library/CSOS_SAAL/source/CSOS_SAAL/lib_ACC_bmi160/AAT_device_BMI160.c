/*
 * AAT_device_BMA250.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: Kwon
 */
#include "ubinos.h"

#if (CSOS_SAAL__USE_LIB_ACC_bmi160 == 1)

#include "hw_config.h"
#include "sw_config.h"
#include "nrf_drv_gpiote.h"

#include "AAT_driver_spi.h"
#include "AAT_device_BMI160.h"

#define DEFAULT_ACCEL_RANGE BMI160_ACCEL_RANGE_4G
#define DEFAULT_GYRO_RANGE	BMI160_GYRO_RANGE_2000

static bool is_AAT_wake_up = false;

bool get_AAT_sleep_state()
{
	return is_AAT_wake_up;
}

void AAT_wake_up(void) {

	if(is_AAT_wake_up == true)
		return;

	nrf_drv_gpiote_in_event_disable(AAT_BOARD_PIN_ACEEL_INT_1);

	BMI160_INT_Disable();
	//BMI160_Acc_Config(BMI160_CMD_ACCEL_LOWPOWER, DEFAULT_ACCEL_RANGE);

	Bmi160_Gyro_Config(BMI160_CMD_GYRO_NORMAL, DEFAULT_GYRO_RANGE);

	is_AAT_wake_up = true;

}

void AAT_enter_sleep(void) {

	//BMI160_Acc_Config(BMI160_CMD_ACCEL_LOWPOWER, DEFAULT_ACCEL_RANGE);

	Bmi160_Gyro_Config(BMI160_CMD_GYRO_SUSPEND, DEFAULT_GYRO_RANGE);

	nrf_drv_gpiote_in_event_enable(AAT_BOARD_PIN_ACEEL_INT_1, true);

	BMI160_INT_Enable();

	is_AAT_wake_up = false;
}

void AAT_Bmi160_init(void) {

	bool check;

	BMI160_Soft_Reset();

	check = BMI160_Chip_ID_READ();
	while (check != true) {
		task_sleep(100);
		check = BMI160_Chip_ID_READ();
	}

#if(TEST_DEVICE_TYPE == TEST_DEVICE_TYPE_FAUSET)
	BMI160_Acc_Config(BMI160_CMD_ACCEL_LOWPOWER, DEFAULT_ACCEL_RANGE);

	Bmi160_Gyro_Config(BMI160_CMD_GYRO_SUSPEND, DEFAULT_GYRO_RANGE);
#elif(TEST_DEVICE_TYPE == TEST_DEVICE_TYPE_DRAWER || TEST_DEVICE_TYPE == TEST_DEVICE_TYPE_REFRIGATOR)
	BMI160_Acc_Config(BMI160_CMD_ACCEL_LOWPOWER, DEFAULT_ACCEL_RANGE);

	Bmi160_Gyro_Config(BMI160_CMD_GYRO_SUSPEND, DEFAULT_GYRO_RANGE);
#endif

	BMI160_INT_OUTPUT_SET();

	BMI160_INT_Latch(BMI160_INT_TMP_640ms);

	BMI160_INT_Enable();  //Any-motion, Sleep-motion
}

void BMI160_Soft_Reset(void) {
	int i = 0;

	AAT_Bmi160_writeReg(BMI160_CMD_COMMANDS_ADDR, BMI160_CMD_SOFTRESET);
	//Delay_set
	for (i = 0; i < 64000; i++)
		__asm(" NOP");
}

bool BMI160_Chip_ID_READ(void) {
	unsigned char temp;
	//Read_register
	AAT_Bmi160_readReg(BMI160_CHIP_ID_ADDR, &temp);

	return (temp == BMI160_CHIP_ID) ? true : false;
}

void Bmi160_Gyro_Config(uint8_t mode, uint8_t Range_reg) {
	int i;
	uint8_t Config_reg = 0x00;
	//uint8_t Range_reg = 0x00;
	uint8_t Mode_reg = 0x00;

	Mode_reg = mode;//BMI160_CMD_GYRO_SUSPEND;
	Config_reg |= BMI160_GYRO_NORMAL;
	Config_reg |= BMI160_GYRO_ODR_100;
	//Range_reg = BMI160_GYRO_RANGE_2000;

	//Acc power mode set
	AAT_Bmi160_writeReg(BMI160_CMD_COMMANDS_ADDR, Mode_reg);
	//Delay
	for (i = 0; i < 32000; i++)
		__asm(" NOP");
	//Gyro Config Set
	AAT_Bmi160_writeReg(BMI160_GYRO_CONFIG_ADDR, Config_reg);
	//Gyro Range Set
	AAT_Bmi160_writeReg(BMI160_GYRO_RANGE_ADDR, Range_reg);
}

void BMI160_Acc_Config(uint8_t mode, uint8_t Range_reg) {
	int i;
	uint8_t Config_reg = 0x00;
	uint8_t Mode_reg = 0x00;
	unsigned char temp;

	Mode_reg =  mode;//BMI160_CMD_ACCEL_LOWPOWER;//BMI160_CMD_ACCEL_NORMAL;//
	Config_reg |= BMI160_ACCEL_UNDERSAMPLING_MODE;
	Config_reg |= BMI160_ACCEL_BANDWIDTH_AVG_2;
	Config_reg |= BMI160_ACCEL_ODR_25;

	//Acc power mode set
	AAT_Bmi160_writeReg(BMI160_CMD_COMMANDS_ADDR, Mode_reg);

	//Delay
	for (i = 0; i < 32000; i++)
		__asm(" NOP");

	//Accel Config Set
	AAT_Bmi160_writeReg(BMI160_ACCEL_CONFIG_ADDR, Config_reg);
	//Accel Range Set
	AAT_Bmi160_writeReg(BMI160_ACCEL_RANGE_ADDR, Range_reg);
}

void BMI160_INT_OUTPUT_SET() {
	//INT_OUT_CTRL_SET
	uint8_t Int_out_ctrl = 0x00;
	//Int_out_ctrl |= BMI160_INT_2_OUTPUT_ENABLE | BMI160_INT_2_LVL;
	Int_out_ctrl |= BMI160_INT_1_OUTOUT_ENABLE | BMI160_INT_1_LVL;

	AAT_Bmi160_writeReg(BMI160_INTR_OUT_CTRL_ADDR, Int_out_ctrl);
}

void BMI160_INT_Latch(uint8_t Int_Latch) {
	//INT_LATCH_SET
	AAT_Bmi160_writeReg(BMI160_INTR_LATCH_ADDR, Int_Latch);
}

void BMI160_INT_Enable() {

	//INT_ENABLE_SET
	uint8_t Enable_0_reg = 0x00;
	uint8_t Enable_1_reg = 0x00;
	uint8_t Enable_2_reg = 0x00;

	Enable_0_reg = BMI160_INT_ANYMO_Y_ENABLE; // | BMI160_INT_ANYMO_Y_ENABLE | BMI160_INT_ANYMO_X_ENABLE;
	Enable_1_reg = 0x00;
	Enable_2_reg = BMI160_INT_NOMO_Y_ENABLE;  //| BMI160_INT_NOMO_Y_ENABLE | BMI160_INT_NOMO_X_ENABLE;

	AAT_Bmi160_writeReg(BMI160_INTR_ENABLE_0_ADDR, Enable_0_reg);
	AAT_Bmi160_writeReg(BMI160_INTR_ENABLE_1_ADDR, Enable_1_reg);
	AAT_Bmi160_writeReg(BMI160_INTR_ENABLE_2_ADDR, Enable_2_reg);

	AAT_Bmi160_writeReg(BMI160_INTR_MAP_0_ADDR, BMI160_INT_1_MAP_ANY_MOTION);	//BMI160_INT_1_MAP_ANY_MOTION);//
	AAT_Bmi160_writeReg(BMI160_INTR_MAP_1_ADDR, 0x00);
	AAT_Bmi160_writeReg(BMI160_INTR_MAP_2_ADDR, 0x01);

//	AAT_Bmi160_writeReg(BMI160_INTR_MAP_0_ADDR, BMI160_INT_1_MAP_ANY_MOTION | BMI160_INT_1_MAP_NO_MOTION );//
//	AAT_Bmi160_writeReg(BMI160_INTR_MAP_1_ADDR, 0x00);
//	AAT_Bmi160_writeReg(BMI160_INTR_MAP_2_ADDR, 0x00);

	//Threshold Setting Modify
	//AAT_Bmi160_writeReg(BMI160_INTR_SLO_NO_MOTION_TH_ADDR, );
	//AAT_Bmi160_writeReg(BMI160_INTR_SIG_CONFIG_ADDR, 0x01);
	AAT_Bmi160_writeReg(BMI160_INTR_ANY_MOTION_TH_ADDR, 0x04);
}

void BMI160_INT_Disable(void){
	uint8_t Disable_0_reg = 0x00;
	uint8_t Disable_1_reg = 0x00;
	uint8_t Disable_2_reg = 0x00;

	Disable_0_reg = 0x00; // | BMI160_INT_ANYMO_Y_ENABLE | BMI160_INT_ANYMO_X_ENABLE;
	Disable_1_reg = 0x00;
	Disable_2_reg = 0x00;  //| BMI160_INT_NOMO_Y_ENABLE | BMI160_INT_NOMO_X_ENABLE;

	AAT_Bmi160_writeReg(BMI160_INTR_ENABLE_0_ADDR, Disable_0_reg);
	AAT_Bmi160_writeReg(BMI160_INTR_ENABLE_1_ADDR, Disable_1_reg);
	AAT_Bmi160_writeReg(BMI160_INTR_ENABLE_2_ADDR, Disable_2_reg);
}

uint8_t BMI160_INT_Check(uint8_t flag){
	uint8_t temp_1 = 0x00;
	uint8_t temp_2 = 0x00;
	AAT_Bmi160_readReg( BMI160_INTR_STAT_0_ADDR,&temp_1);
	AAT_Bmi160_readReg( BMI160_INTR_STAT_1_ADDR,&temp_2);

	return temp_1 | temp_2;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
int16_t BMI160_ACC_DATA_READ_X_AXIS(void) {
	uint8_t temp[2];

	AAT_Bmi160_readReg(BMI160_ACCEL_X_LSB_ADDR, (uint8_t*)&temp[0]);
	AAT_Bmi160_readReg(BMI160_ACCEL_X_MSB_ADDR, (uint8_t*)&temp[1]);
	return (int16_t)((temp[1] << 8) | temp[0]);
}

int16_t BMI160_ACC_DATA_READ_Y_AXIS(void) {
	uint8_t temp[2];

	AAT_Bmi160_readReg(BMI160_ACCEL_Y_LSB_ADDR, (uint8_t*)&temp[0]);
	AAT_Bmi160_readReg(BMI160_ACCEL_Y_MSB_ADDR, (uint8_t*)&temp[1]);
	return (int16_t)((temp[1] << 8) | temp[0]);
}

int16_t BMI160_ACC_DATA_READ_Z_AXIS(void) {
	uint8_t temp[2];

	AAT_Bmi160_readReg(BMI160_ACCEL_Z_LSB_ADDR, (uint8_t*)&temp[0]);
	AAT_Bmi160_readReg(BMI160_ACCEL_Z_MSB_ADDR, (uint8_t*)&temp[1]);
	return (int16_t)((temp[1] << 8) | temp[0]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
int16_t BMI160_GYRO_DATA_READ_X_AXIS(void) {
	uint8_t temp[2];

	AAT_Bmi160_readReg(BMI160_GYRO_X_LSB_ADDR, (uint8_t*)&temp[0]);
	AAT_Bmi160_readReg(BMI160_GYRO_X_MSB_ADDR, (uint8_t*)&temp[1]);
	return (int16_t)((temp[1] << 8) | temp[0]);
}

int16_t BMI160_GYRO_DATA_READ_Y_AXIS(void) {
	uint8_t temp[2];

	AAT_Bmi160_readReg(BMI160_GYRO_Y_LSB_ADDR, (uint8_t*)&temp[0]);
	AAT_Bmi160_readReg(BMI160_GYRO_Y_MSB_ADDR, (uint8_t*)&temp[1]);
	return (int16_t)((temp[1] << 8) | temp[0]);
}

int16_t BMI160_GYRO_DATA_READ_Z_AXIS(void) {
	uint8_t temp[2];

	AAT_Bmi160_readReg(BMI160_GYRO_Z_LSB_ADDR, (uint8_t*)&temp[0]);
	AAT_Bmi160_readReg(BMI160_GYRO_Z_MSB_ADDR, (uint8_t*)&temp[1]);
	return (int16_t)((temp[1] << 8) | temp[0]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t AAT_Bmi160_writeReg(uint8_t reg, uint8_t val) {
	uint8_t write_w[2] = { 0, 0 };
	uint32_t result;

	write_w[0] = reg;
	write_w[1] = val;

	task_sleep(10);
	result = AAT_spi_write_ACC(write_w, 2);

	if(result != NRF_SUCCESS)
		result++;
		
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AAT_Bmi160_readReg(uint8_t reg, uint8_t *pVal) {
	uint8_t read_r[2] = { 0, 0 };
	uint8_t write_w[2] = { 0, 0 };

	write_w[0] = reg | 0x80;
	write_w[1] = 0x00;

	AAT_spi_write_read_ACC(&write_w[0], 1, &read_r[0], 1);

	*pVal = read_r[0];
}

#endif /*(CSOS_SAAL__USE_LIB_ACC_bmi160 == 1)*/