/*
 * AAT_RTC.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: Kwon
 */

#include <stdint.h>

#include "AAT_device_RTC.h"

#if (CSOS_SAAL__USE_LIB_AAT_HW_other == 1)

#include "AAT_driver_i2c.h"

void AAT_RTC_device_init(void){
	AAT_RTC_Write_control(0x00);
	AAT_RTC_Write_time_interrurt(0x06);
}
void AAT_RTC_interrupt_init(uint8_t time){

	AAT_RTC_Write_control(0x04);
	AAT_RTC_Write_time_interrupt(time | 0x06);
}
////////////////////////////////////////////////////////////////////
void AAT_RTC_SoftReset(void){
	uint8_t cmd_buffer[2];
	cmd_buffer[0] = 0x00;
	cmd_buffer[1] = 0x58;
	AAT_i2c_write(RTC_ADDRESS, cmd_buffer, 2);
}

void AAT_RTC_Write_control(uint8_t cmd) {
	uint8_t cmd_buffer[2];
	cmd_buffer[0] = 0x00;
	cmd_buffer[1] = cmd;

	AAT_i2c_write(RTC_ADDRESS, cmd_buffer, 2);
}

void AAT_RTC_Write_time_interrupt(uint8_t cmd) {
	uint8_t cmd_buffer[2];
	cmd_buffer[0] = 0x01;
	cmd_buffer[1] = cmd;

	AAT_i2c_write(RTC_ADDRESS, cmd_buffer, 2);

//	cmd_buffer[0] = 0x02;
//	cmd_buffer[1] = 0x80;
//
//	AAT_i2c_write(RTC_ADDRESS, cmd_buffer, 2);
}

uint8_t AAT_RTC_Read_control(void) {
	uint8_t read_data;
	uint8_t tx_data = 0x00;

	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);

	return read_data;
}
////////////////////////////////////////////////////////////////////////
void AAT_RTC_Write_time(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
	uint8_t cmd_buffer[2];

	//set year
	cmd_buffer[0] = RTC_Years_ADDRESS;

	if (year < 2000 || year > 2100) {
		year = 2000;
	}

	year = year - 2000;

	cmd_buffer[1] = ((year / 10) << 4) + (year % 10);
	AAT_i2c_write(RTC_ADDRESS, cmd_buffer, 2);

	//set mon
	cmd_buffer[0] = RTC_Months_ADDRESS;

	if (mon > 12) {
		mon = 1;
	}

	cmd_buffer[1] = ((mon / 10) << 4) + (mon % 10);
	AAT_i2c_write(RTC_ADDRESS, cmd_buffer, 2);

	//set days
	cmd_buffer[0] = RTC_Days_ADDRESS;

	if (day < 1 || day > 31) {
		day = 1;
	}

	cmd_buffer[1] = ((day / 10) << 4) + (day % 10);
	AAT_i2c_write(RTC_ADDRESS, cmd_buffer, 2);

	//set hour
	cmd_buffer[0] = RTC_Hours_ADDRESS;
	if (hour > 24) {
		hour = 1;
	}

	cmd_buffer[1] = ((hour / 10) << 4) + (hour % 10);
	AAT_i2c_write(RTC_ADDRESS, cmd_buffer, 2);

	//set min
	cmd_buffer[0] = RTC_Minutes_ADDRESS;
	if (min > 60) {
		min = 1;
	}

	cmd_buffer[1] = ((min / 10) << 4) + (min % 10);
	AAT_i2c_write(RTC_ADDRESS, cmd_buffer, 2);

	//set  sec
	cmd_buffer[0] = RTC_Seconds_ADDRESS;
	if (sec > 60) {
		sec = 1;
	}

	cmd_buffer[1] = ((sec / 10) << 4) + (sec % 10);
	AAT_i2c_write(RTC_ADDRESS, cmd_buffer, 2);
}

void AAT_RTC_Read_time(uint8_t* year, uint8_t* mon, uint8_t* days, uint8_t* hours, uint8_t* min, uint8_t* sec) {
	uint8_t read_data;
	uint8_t tx_data;

	//read sec
	tx_data = RTC_Seconds_ADDRESS;
	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);
	*sec = (read_data & 0b00001111) + ((read_data & 0b01110000) >> 4) * 10;

	//read min
	tx_data = RTC_Minutes_ADDRESS;
	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);
	*min = (read_data & 0b00001111) + ((read_data & 0b01110000) >> 4) * 10;

	//read hours
	tx_data = RTC_Hours_ADDRESS;
	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);
	*hours = (read_data & 0b00001111) + ((read_data & 0b00110000) >> 4) * 10;

	//read days
	tx_data = RTC_Days_ADDRESS;
	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);
	*days = (read_data & 0b00001111) + ((read_data & 0b00110000) >> 4) * 10;

	//read mon
	tx_data = RTC_Months_ADDRESS;
	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);
	*mon = (read_data & 0b00001111) + ((read_data & 0b00010000) >> 4) * 10;

	//read year
	tx_data = RTC_Years_ADDRESS;
	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);
	*year = (read_data & 0b00001111) + ((read_data & 0b11110000) >> 4) * 10;
}

void AAT_RTC_Read_time_test(uint8_t * mon, uint8_t * days, uint8_t* hours, uint8_t* min) {
	uint8_t read_data;
	uint8_t tx_data;

	//read min
	tx_data = RTC_Minutes_ADDRESS;
	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);
	*min = (read_data & 0b00001111) + ((read_data & 0b01110000) >> 4) * 10;

	//read hours
	tx_data = RTC_Hours_ADDRESS;
	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);
	*hours = (read_data & 0b00001111) + ((read_data & 0b00110000) >> 4) * 10;

	//read days
	tx_data = RTC_Days_ADDRESS;
	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);
	*days = (read_data & 0b00001111) + ((read_data & 0b00110000) >> 4) * 10;

	//read mon
	tx_data = RTC_Months_ADDRESS;
	AAT_i2c_read(RTC_ADDRESS, &tx_data, 1, &read_data, 1);
	*mon = (read_data & 0b00001111) + ((read_data & 0b00010000) >> 4) * 10;
}

#endif /*(CSOS_SAAL__USE_LIB_AAT_HW_other == 1)*/