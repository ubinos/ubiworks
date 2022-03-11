/*
 * sh_uart_433_module.c
 *
 *  Created on: 2021. 9. 23.
 *      Author: sayyo, YJPark
 */

#include <ubinos/bsp.h>
#include <ubinos/bsp/arch.h>
#include <ubinos/bsp_ubik.h>

#include "ubinos.h"

#if (CSOS_SAAL__USE_LIB_wifi_wizfi360 == 1)

#include <stdint.h>

#include "wiz360_uart_wifi_module.h"
#include "ubinos.h"
#include "assert.h"

#include "nrf_drv_uart.h"
#include "bsp.h"

#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

#include "../lib_bluetooth_csos/ble_stack.h"
#include "../lib_bluetooth_csos/LAP_api.h"
#include "LAP_main.h"

#include <hw_config.h>
#include <sw_config.h>

#include "app_error.h"

static msgq_pt wizfi360_wifi_uart_rx_msgq;
static msgq_pt wifi_processing_event_msgq;
static msgq_pt wizfi360_wifi_response_msgq;
static msgq_pt wizfi360_wifi_mqtt_data_msgq;

static nrf_drv_uart_t wizfi360_uart_instance = NRF_DRV_UART_INSTANCE(0);

#define WIZFI360_UART_READ_UNIT_SIZE		1

static uint8_t temp_wifi_uart_buffer;
static uint8_t wizfi360_uart_rsp_buffer_idx = 0;
static uint8_t wizfi360_uart_rsp_buffer_count = 0;
static char wizfi360_uart_rsp_buffer[UART_WIFI_RSP_BUF_MAX_COUNT][UART_WIFI_RSP_BUF_SIZE];

static bool flag_AP_connected = false;
static bool flag_MQTT_SERVER_connected = false;
static bool flag_wifi_ready = false;

APP_TIMER_DEF(mqtt_hb_timer);

void set_wifi_ready(bool val)
{
	flag_wifi_ready = val;
}

bool get_wifi_ready()
{
	return flag_wifi_ready;
}

bool get_flag_AP_connected()
{
	return flag_AP_connected;
}

void set_flag_AP_connected(bool enable)
{
	flag_AP_connected = enable;
}

bool get_flag_MQTT_SERVER_connected()
{
	return flag_MQTT_SERVER_connected;
}

void set_flag_MQTT_SERVER_connected(bool enable)
{
	flag_MQTT_SERVER_connected = enable;
}

int wifi_processing_event_send(uint8_t evt, uint8_t state, uint8_t * msg) {
	wifi_processing_msgq_event_t tmp_wifi_processing_msgq_event;

	tmp_wifi_processing_msgq_event.event = evt;
	tmp_wifi_processing_msgq_event.status = state;
	tmp_wifi_processing_msgq_event.msg = msg;

	return msgq_send(wifi_processing_event_msgq, (unsigned char*) &tmp_wifi_processing_msgq_event);
}

int wizfi360_uart_event_send(uint8_t evt, uint8_t state, uint8_t * msg) {
	uart_wifi_msgq_event_t tmp_uart_wifi_msgq_event;

	tmp_uart_wifi_msgq_event.event = evt;
	tmp_uart_wifi_msgq_event.status = state;
	tmp_uart_wifi_msgq_event.msg = msg;

	return msgq_send(wizfi360_wifi_uart_rx_msgq, (unsigned char*) &tmp_uart_wifi_msgq_event);
}

static void wizfi360_response_event_msq_clear()
{
	msgq_clear(wizfi360_wifi_response_msgq);
}


int wizfi360_response_event_send(uint8_t evt, uint8_t state, uint8_t* msg) {
	uart_wifi_msgq_event_t tmp_uart_wifi_msgq_event;

	tmp_uart_wifi_msgq_event.event = evt;
	tmp_uart_wifi_msgq_event.status = state;
	tmp_uart_wifi_msgq_event.msg = msg;

	return msgq_send(wizfi360_wifi_response_msgq, (unsigned char*) &tmp_uart_wifi_msgq_event);
}

static void wizfi360_uart_rsp_event_handler(nrf_drv_uart_event_t * p_event, void * p_context) {
	//ret_code_t err;
	nrf_drv_uart_t * uart = &wizfi360_uart_instance;

	uint8_t * buf;
	//uint8_t len;

	buf = p_event->data.rxtx.p_data;
	//len = p_event->data.rxtx.bytes;

	switch(p_event->type) {
	case NRF_DRV_UART_EVT_ERROR:
		nrf_drv_uart_errorsrc_get(uart);
		break;

	case NRF_DRV_UART_EVT_RX_DONE:

		wizfi360_uart_rsp_buffer[wizfi360_uart_rsp_buffer_count][wizfi360_uart_rsp_buffer_idx] = *buf;
		wizfi360_uart_rsp_buffer_idx++;

		nrf_drv_uart_rx(&wizfi360_uart_instance, &temp_wifi_uart_buffer, WIZFI360_UART_READ_UNIT_SIZE);

		if(*buf == '\n') {
			wizfi360_uart_event_send(WIZFI360_WIFI_EVENT_RECEIVED, wizfi360_uart_rsp_buffer_count, NULL);

			//Set next Rx buffer point
			wizfi360_uart_rsp_buffer_idx = 0;
			wizfi360_uart_rsp_buffer_count++;
			if(wizfi360_uart_rsp_buffer_count >= UART_WIFI_RSP_BUF_MAX_COUNT)
			{
				wizfi360_uart_rsp_buffer_count = 0;
			}
			break;
		}
		//check Buffer Overflow
		else if(wizfi360_uart_rsp_buffer_idx >= UART_WIFI_RSP_BUF_SIZE)
		{
			//Set next Rx buffer point
			wizfi360_uart_rsp_buffer_idx = 0;
			wizfi360_uart_rsp_buffer_count++;
			if(wizfi360_uart_rsp_buffer_count >= UART_WIFI_RSP_BUF_MAX_COUNT)
			{
				wizfi360_uart_rsp_buffer_count = 0;
			}
		}

		break;

	case NRF_DRV_UART_EVT_TX_DONE:

		break;

	default:
		break;
	}
}

static uint32_t wizfi360_io_pins_init(void) {
	ret_code_t err = NRF_SUCCESS;

	nrf_gpio_cfg_output(WIZFI_RST);
	nrf_gpio_cfg_output(WIZFI_WAKE_UP);

	nrf_gpio_pin_set(WIZFI_WAKE_UP);

	nrf_gpio_pin_clear(WIZFI_RST);
	nrf_delay_ms(500);
	nrf_gpio_pin_set(WIZFI_RST);
	nrf_delay_ms(500);

	return err;
}

static uint32_t wizfi_uart_init(void) {

	ret_code_t err;

	nrf_drv_uart_config_t config;
	config.pseltxd = WIZFI_TX;
	config.pselrxd = WIZFI_RX;
	config.pselcts = WIZFI_CTS;
	config.pselrts = WIZFI_RTS;
	config.p_context = NULL;
	config.hwfc = NRF_UART_HWFC_ENABLED;
	config.parity = NRF_UART_PARITY_EXCLUDED;
	config.baudrate = NRF_UART_BAUDRATE_115200;
	config.interrupt_priority= NVIC_PRIO_LOWEST;

	err = wizfi360_io_pins_init();
	APP_ERROR_CHECK(err);

#if defined(NRF_DRV_UART_WITH_UARTE) && defined(NRF_DRV_UART_WITH_UART)
	config.use_easy_dma = true;
#endif

	err = nrf_drv_uart_init(&wizfi360_uart_instance, &config, wizfi360_uart_rsp_event_handler);
	APP_ERROR_CHECK(err);

	//set Rx Buffer
	err = nrf_drv_uart_rx(&wizfi360_uart_instance, &temp_wifi_uart_buffer, WIZFI360_UART_READ_UNIT_SIZE);
	APP_ERROR_CHECK(err);

	return err;
}

static uint32_t uart_wifi_enable(void) {
	ret_code_t err = NRF_SUCCESS;

	nrf_gpio_pin_set(WIZFI_WAKE_UP);
	APP_ERROR_CHECK(err);

	nrf_gpio_pin_set(WIZFI_RST);
	APP_ERROR_CHECK(err);

	nrf_delay_ms(WIZFI_MODULE_PWR_ON_DELAY_MS);

	return err;
}

static uint32_t uart_wifi_reset(void) {
	ret_code_t err = NRF_SUCCESS;

	nrf_gpio_pin_clear(WIZFI_RST);
	APP_ERROR_CHECK(err);

	nrf_delay_ms(WIZFI_MODULE_PWR_ON_DELAY_MS*10);

	nrf_gpio_pin_set(WIZFI_RST);
	APP_ERROR_CHECK(err);

	nrf_delay_ms(WIZFI_MODULE_PWR_ON_DELAY_MS*10);

	return err;
}

static uint8_t test_YY = 21;
static uint8_t test_MM = 8;
static uint8_t test_DD = 23;
static uint8_t test_hh = 11;
static uint8_t test_mm = 15;
static uint8_t test_ss = 0;

void set_test_time(uint8_t YY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss)
{
	test_YY = YY;
	test_MM = MM;
	test_DD = DD;
	test_hh = hh;
	test_mm = mm;
	test_ss = ss;
}

//Don't use this code!!
static void test_time_task(void * arg) {

	while(1)
	{
		task_sleep(1000);
		test_ss++;
		if(test_ss >= 60)
		{
			test_ss = 0;
			test_mm++;
			if(test_mm >= 60)
			{
				test_mm = 0;
				test_hh++;
				if(test_hh >= 24)
				{
					test_hh=0;
					test_DD++;
					if(test_DD >= 30)
					{
						test_MM++;
						if(test_MM >= 12)
							test_MM = 0;
					}
				}
			}
		}
	}
}

//test code
static void copy_test_time(char* data, uint8_t YY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss)
{
	char temp_time[3];
	memset(temp_time, 0, 3);

	strcat(data,"20");

	if(YY<10)
	{
		strcat(data,"0");
	}
	itoa(YY, temp_time, 10);
	strcat(data,temp_time);
	strcat(data,"-");

	if(MM<10)
	{
		strcat(data,"0");
	}
	itoa(MM, temp_time, 10);
	strcat(data,temp_time);
	strcat(data,"-");

	if(DD<10)
	{
		strcat(data,"0");
	}
	itoa(DD, temp_time, 10);
	strcat(data,temp_time);
	strcat(data," ");

	if(hh<10)
	{
		strcat(data,"0");
	}
	itoa(hh, temp_time, 10);
	strcat(data,temp_time);
	strcat(data,":");

	if(mm<10)
	{
		strcat(data,"0");
	}
	itoa(mm, temp_time, 10);
	strcat(data,temp_time);
	strcat(data,":");

	if(ss<10)
	{
		strcat(data,"0");
	}
	itoa(ss, temp_time, 10);
	strcat(data,temp_time);
}

#define WIZFI360_RESET_MODULE_TIMEOUT	10000

#define WIZFI360_RESET_MODULE_SUCCESS			0
#define WIZFI360_RESET_MODULE_ERROR_ERROR		-1
#define WIZFI360_RESET_MODULE_ERROR_TIMEOUT 	-2
#define WIZFI360_RESET_MODULE_ERROR_UNKNOWN 	-3

static int wizfi360_reset_module()
{
	uint8_t temp_cmd_buf[20] = {0, };
	uint32_t temp_cmd_len = 0;
	int err;

	uart_wifi_msgq_event_t wizfi360_rsp_evt_msg;

	wizfi360_response_event_msq_clear();

	strcpy((char*)temp_cmd_buf, "AT+RST\r\n");
	temp_cmd_len = strlen("AT+RST\r\n");

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}
	nrf_drv_uart_tx(&wizfi360_uart_instance, temp_cmd_buf, temp_cmd_len);

	err = msgq_receive_timed(wizfi360_wifi_response_msgq, (unsigned char*) &wizfi360_rsp_evt_msg, WIZFI360_RESET_MODULE_TIMEOUT);
	switch(err)
	{
	case UBIK_ERR__SUCCESS :
		if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_OK)
		{
			printf("WIFI Setup : WIZFI360_RESET_MODULE_SUCCESS\r\n");
			return WIZFI360_RESET_MODULE_SUCCESS;
		}
		else
		{
			printf("WIFI Setup Error : WIZFI360_RESET_MODULE_ERROR_ERROR\r\n");
			return WIZFI360_RESET_MODULE_ERROR_ERROR;
		}
		break;
	case UBIK_ERR__TIMEOUT :
		printf("WIFI Setup Error : WIZFI360_RESET_MODULE_ERROR_TIMEOUT\r\n");
		return WIZFI360_RESET_MODULE_ERROR_TIMEOUT;
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_RESET_MODULE_ERROR_UNKNOWN\r\n");
		return WIZFI360_RESET_MODULE_ERROR_UNKNOWN;
		break;
	}

	task_sleep(1000);
}

#define WIZFI360_MODE_STATION	1
#define WIZFI360_MODE_SOFTAP	2
#define WIZFI360_MODE_ST_SOFTAP	3
#define WIZFI360_SET_MODE_TIMEOUT	10000

#define WIZFI360_SET_MODE_SUCCESS			0
#define WIZFI360_SET_MODE_ERROR_ERROR		-1
#define WIZFI360_SET_MODE_ERROR_TIMEOUT 	-2
#define WIZFI360_SET_MODE_ERROR_UNKNOWN 	-3
#define WIZFI360_SET_MODE_ERROR_WRONG_PAR	-4

static int wizfi360_set_mode(uint8_t mode)
{
	uint8_t temp_cmd_buf[20] = {0, };
	uint32_t temp_cmd_len = 0;
	int err;

	uart_wifi_msgq_event_t wizfi360_rsp_evt_msg;

	wizfi360_response_event_msq_clear();

	switch(mode)
	{
	case WIZFI360_MODE_STATION :
		strcpy((char*)temp_cmd_buf, "AT+CWMODE_CUR=1\r\n");
		temp_cmd_len = strlen("AT+CWMODE_CUR=1\r\n");
		break;
	case WIZFI360_MODE_SOFTAP :
		strcpy((char*)temp_cmd_buf, "AT+CWMODE_CUR=2\r\n");
		temp_cmd_len = strlen("AT+CWMODE_CUR=2\r\n");
		break;
	case WIZFI360_MODE_ST_SOFTAP :
		strcpy((char*)temp_cmd_buf, "AT+CWMODE_CUR=3\r\n");
		temp_cmd_len = strlen("AT+CWMODE_CUR=3\r\n");
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_SET_MODE_ERROR_WRONG_PAR\r\n");
		return WIZFI360_SET_MODE_ERROR_WRONG_PAR;
	}

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}
	nrf_drv_uart_tx(&wizfi360_uart_instance, temp_cmd_buf, temp_cmd_len);

	err = msgq_receive_timed(wizfi360_wifi_response_msgq, (unsigned char*) &wizfi360_rsp_evt_msg, WIZFI360_SET_MODE_TIMEOUT);
	switch(err)
	{
	case UBIK_ERR__SUCCESS :
		if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_OK)
		{
			printf("WIFI Setup : WIZFI360_SET_MODE_SUCCESS\r\n");
			return WIZFI360_SET_MODE_SUCCESS;
		}
		else
		{
			printf("WIFI Setup Error : WIZFI360_SET_MODE_ERROR_ERROR\r\n");
			return WIZFI360_SET_MODE_ERROR_ERROR;
		}
		break;
	case UBIK_ERR__TIMEOUT :
		printf("WIFI Setup Error : WIZFI360_SET_MODE_ERROR_TIMEOUT\r\n");
		return WIZFI360_SET_MODE_ERROR_TIMEOUT;
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_SET_MODE_ERROR_UNKNOWN\r\n");
		return WIZFI360_SET_MODE_ERROR_UNKNOWN;
		break;
	}
}

#define WIZFI360_DHCP_MODE_SOFTAP		0
#define WIZFI360_DHCP_MODE_STATION		1
#define WIZFI360_DHCP_MODE_ST_SOFTAP	2

#define WIZFI360_DHCP_DISABLE			0
#define WIZFI360_DHCP_ENABLE			1

#define WIZFI360_SET_DHCP_SUCCESS			0
#define WIZFI360_SET_DHCP_ERROR_ERROR		-1
#define WIZFI360_SET_DHCP_ERROR_TIMEOUT 	-2
#define WIZFI360_SET_DHCP_ERROR_UNKNOWN 	-3
#define WIZFI360_SET_DHCP_ERROR_WRONG_PAR	-4

static int wizfi360_enable_DHCP(uint8_t mode, uint8_t enable)
{
	uint8_t temp_cmd_buf[30] = {0, };
	uint32_t temp_cmd_len = 0;
	int err;

	uart_wifi_msgq_event_t wizfi360_rsp_evt_msg;

	wizfi360_response_event_msq_clear();

	switch(mode)
	{
	case WIZFI360_DHCP_MODE_SOFTAP :
		if(enable == WIZFI360_DHCP_ENABLE)
		{
			strcpy((char*)temp_cmd_buf, "AT+CWDHCP_CUR=0,1\r\n");
			temp_cmd_len = strlen("AT+CWDHCP_CUR=0,1\r\n");
		}
		else if(enable == WIZFI360_DHCP_DISABLE)
		{
			strcpy((char*)temp_cmd_buf, "AT+CWDHCP_CUR=0,0\r\n");
			temp_cmd_len = strlen("AT+CWDHCP_CUR=0,0\r\n");
		}
		else
		{
			printf("WIFI Setup Error : WIZFI360_SET_DHCP_ERROR_WRONG_PAR\r\n");
			return WIZFI360_SET_DHCP_ERROR_WRONG_PAR;
		}

		break;
	case WIZFI360_DHCP_MODE_STATION :
		if(enable == WIZFI360_DHCP_ENABLE)
		{
			strcpy((char*)temp_cmd_buf, "AT+CWDHCP_CUR=1,1\r\n");
			temp_cmd_len = strlen("AT+CWDHCP_CUR=1,1\r\n");
		}
		else if(enable == WIZFI360_DHCP_DISABLE)
		{
			strcpy((char*)temp_cmd_buf, "AT+CWDHCP_CUR=1,0\r\n");
			temp_cmd_len = strlen("AT+CWDHCP_CUR=1,0\r\n");
		}
		else
		{
			printf("WIFI Setup Error : WIZFI360_SET_DHCP_ERROR_WRONG_PAR\r\n");
			return WIZFI360_SET_DHCP_ERROR_WRONG_PAR;
		}
		break;
	case WIZFI360_DHCP_MODE_ST_SOFTAP :
		if(enable == WIZFI360_DHCP_ENABLE)
		{
			strcpy((char*)temp_cmd_buf, "AT+CWDHCP_CUR=2,1\r\n");
			temp_cmd_len = strlen("AT+CWDHCP_CUR=2,1\r\n");
		}
		else if(enable == WIZFI360_DHCP_DISABLE)
		{
			strcpy((char*)temp_cmd_buf, "AT+CWDHCP_CUR=2,0\r\n");
			temp_cmd_len = strlen("AT+CWDHCP_CUR=2,0\r\n");
		}
		else
		{
			printf("WIFI Setup Error : WIZFI360_SET_DHCP_ERROR_WRONG_PAR\r\n");
			return WIZFI360_SET_DHCP_ERROR_WRONG_PAR;
		}
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_SET_DHCP_ERROR_WRONG_PAR\r\n");
		return WIZFI360_SET_DHCP_ERROR_WRONG_PAR;
		break;
	}

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}
	nrf_drv_uart_tx(&wizfi360_uart_instance, temp_cmd_buf, temp_cmd_len);

	err = msgq_receive_timed(wizfi360_wifi_response_msgq, (unsigned char*) &wizfi360_rsp_evt_msg, WIZFI360_SET_MODE_TIMEOUT);
	switch(err)
	{
	case UBIK_ERR__SUCCESS :
		if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_OK)
		{
			printf("WIFI Setup : WIZFI360_SET_DHCP_SUCCESS\r\n");
			return WIZFI360_SET_DHCP_SUCCESS;
		}
		else
		{
			printf("WIFI Setup Error : WIZFI360_SET_DHCP_ERROR_ERROR\r\n");
			return WIZFI360_SET_DHCP_ERROR_ERROR;
		}
		break;
	case UBIK_ERR__TIMEOUT :
		printf("WIFI Setup Error : WIZFI360_SET_DHCP_ERROR_TIMEOUT\r\n");
		return WIZFI360_SET_DHCP_ERROR_TIMEOUT;
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_SET_DHCP_ERROR_UNKNOWN\r\n");
		return WIZFI360_SET_DHCP_ERROR_UNKNOWN;
		break;
	}

}

#define WIZFI360_CONN_AP_TIMEOUT 			120000 //2 min

#define WIZFI360_CONN_AP_SUCCESS			0
#define WIZFI360_CONN_AP_ERROR_ERROR		-1
#define WIZFI360_CONN_AP_ERROR_TIMEOUT 		-2
#define WIZFI360_CONN_AP_ERROR_UNKNOWN 		-3
#define WIZFI360_CONN_AP_ERROR_WRONG_PAR	-4

#define WIZFI360_CONN_AP_PAR_SIZE_LIMIT	50
#define WIZFI360_CONN_AP_CMD_SIZE_LIMIT	WIZFI360_CONN_AP_PAR_SIZE_LIMIT*2+20

static int wizfi360_connect_AP(char* ssid, uint8_t ssid_len, char* password, uint8_t password_len)
{
	uint8_t temp_cmd_buf[WIZFI360_CONN_AP_CMD_SIZE_LIMIT] = {0, };
	uint8_t temp_char_buf[WIZFI360_CONN_AP_PAR_SIZE_LIMIT] = {0, };
	uint32_t temp_cmd_len = 0;
	int err;

	uart_wifi_msgq_event_t wizfi360_rsp_evt_msg;

/*
	//check ssid/password len
	if(ssid_len >= WIZFI360_CONN_AP_PAR_SIZE_LIMIT || password_len>=WIZFI360_CONN_AP_PAR_SIZE_LIMIT)
	{
		printf("WIFI Setup Error : WIZFI360_CONN_AP_ERROR_WRONG_PAR\r\n");
		return WIZFI360_CONN_AP_ERROR_WRONG_PAR;
	}
*/
	wizfi360_response_event_msq_clear();

	strcpy((char*)temp_cmd_buf, "AT+CWJAP_CUR=\"");

	memset(temp_char_buf, 0 , WIZFI360_CONN_AP_PAR_SIZE_LIMIT);
	memcpy(temp_char_buf, ssid, ssid_len);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\",\"");

	memset(temp_char_buf, 0 , WIZFI360_CONN_AP_PAR_SIZE_LIMIT);
	memcpy(temp_char_buf, password, password_len);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\"\r\n");

	temp_cmd_len = strlen((char*)temp_cmd_buf);

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}
	nrf_drv_uart_tx(&wizfi360_uart_instance, temp_cmd_buf, temp_cmd_len);

	err = msgq_receive_timed(wizfi360_wifi_response_msgq, (unsigned char*) &wizfi360_rsp_evt_msg, WIZFI360_CONN_AP_TIMEOUT);
	switch(err)
	{
	case UBIK_ERR__SUCCESS :
		if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_OK)
		{
			printf("WIFI Setup : WIZFI360_CONN_AP_SUCCESS\r\n");
			return WIZFI360_CONN_AP_SUCCESS;
		}
		else if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_FAIL)
		{
			printf("WIFI Setup Error : WIZFI360_CONN_AP_ERROR_ERROR\r\n");
			return WIZFI360_CONN_AP_ERROR_ERROR;
		}
		break;
	case UBIK_ERR__TIMEOUT :
		printf("WIFI Setup Error : WIZFI360_CONN_AP_ERROR_TIMEOUT\r\n");
		return WIZFI360_CONN_AP_ERROR_TIMEOUT;
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_CONN_AP_ERROR_UNKNOWN\r\n");
		return WIZFI360_CONN_AP_ERROR_UNKNOWN;
		break;
	}

	return WIZFI360_CONN_AP_SUCCESS;
}

#define WIZFI360_SETUP_MQTT_TIMEOUT 			10000

#define WIZFI360_SETUP_MQTT_SUCCESS				0
#define WIZFI360_SETUP_MQTT_ERROR_ERROR			-1
#define WIZFI360_SETUP_MQTT_ERROR_TIMEOUT 		-2
#define WIZFI360_SETUP_MQTT_ERROR_UNKNOWN 		-3
#define WIZFI360_SETUP_MQTT_ERROR_WRONG_PAR		-4

#define WIZFI360_SETUP_MQTT_PAR_SIZE_LIMIT	20
#define WIZFI360_SETUP_MQTT_CMD_SIZE_LIMIT	WIZFI360_SETUP_MQTT_PAR_SIZE_LIMIT*4+20

static int wizfi360_setup_MQTT(char* user_name, uint8_t user_name_len, char* user_pass, uint8_t user_pass_len,
		char* client_id, uint8_t client_id_len, uint16_t alive_time)
{
	uint8_t temp_cmd_buf[WIZFI360_SETUP_MQTT_CMD_SIZE_LIMIT] = {0, };
	uint8_t temp_char_buf[WIZFI360_SETUP_MQTT_PAR_SIZE_LIMIT] = {0, };
	uint32_t temp_cmd_len = 0;
	int err;

	uart_wifi_msgq_event_t wizfi360_rsp_evt_msg;

	//check ssid/password len
	if(user_name_len >= WIZFI360_SETUP_MQTT_PAR_SIZE_LIMIT || user_pass_len>=WIZFI360_SETUP_MQTT_PAR_SIZE_LIMIT
			||client_id_len>=WIZFI360_SETUP_MQTT_PAR_SIZE_LIMIT)
	{
		printf("WIFI Setup Error : WIZFI360_SETUP_MQTT_ERROR_WRONG_PAR\r\n");
		return WIZFI360_SETUP_MQTT_ERROR_WRONG_PAR;
	}

	wizfi360_response_event_msq_clear();

	strcpy((char*)temp_cmd_buf, "AT+MQTTSET=\"");

	memset(temp_char_buf, 0 , WIZFI360_SETUP_MQTT_PAR_SIZE_LIMIT);
	memcpy(temp_char_buf, user_name, user_name_len);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\",\"");

	memset(temp_char_buf, 0 , WIZFI360_SETUP_MQTT_PAR_SIZE_LIMIT);
	memcpy(temp_char_buf, user_pass, user_pass_len);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\",\"");

	memset(temp_char_buf, 0 , WIZFI360_SETUP_MQTT_PAR_SIZE_LIMIT);
	memcpy(temp_char_buf, client_id, client_id_len);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\",\"");

	memset(temp_char_buf, 0 , WIZFI360_SETUP_MQTT_PAR_SIZE_LIMIT);
	itoa(alive_time, (char*)temp_char_buf, 10);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\"\r\n");

	temp_cmd_len = strlen((char*)temp_cmd_buf);

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}
	nrf_drv_uart_tx(&wizfi360_uart_instance, temp_cmd_buf, temp_cmd_len);

	err = msgq_receive_timed(wizfi360_wifi_response_msgq, (unsigned char*) &wizfi360_rsp_evt_msg, WIZFI360_SETUP_MQTT_TIMEOUT);
	switch(err)
	{
	case UBIK_ERR__SUCCESS :
		if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_OK)
		{
			printf("WIFI Setup : WIZFI360_SETUP_MQTT_SUCCESS\r\n");
			return WIZFI360_SETUP_MQTT_SUCCESS;
		}
		else
		{
			printf("WIFI Setup Error : WIZFI360_SETUP_MQTT_ERROR_ERROR\r\n");
			return WIZFI360_SETUP_MQTT_ERROR_ERROR;
		}
		break;
	case UBIK_ERR__TIMEOUT :
		printf("WIFI Setup Error : WIZFI360_SETUP_MQTT_ERROR_TIMEOUT\r\n");
		return WIZFI360_SETUP_MQTT_ERROR_TIMEOUT;
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_SETUP_MQTT_ERROR_UNKNOWN\r\n");
		return WIZFI360_SETUP_MQTT_ERROR_UNKNOWN;
		break;
	}
}

#define WIZFI360_SETUP_SNTP_TIMEOUT 			10000

#define WIZFI360_SETUP_SNTP_SUCCESS				0
#define WIZFI360_SETUP_SNTP_ERROR_ERROR			-1
#define WIZFI360_SETUP_SNTP_ERROR_TIMEOUT 		-2
#define WIZFI360_SETUP_SNTP_ERROR_UNKNOWN 		-3
#define WIZFI360_SETUP_SNTP_ERROR_WRONG_PAR		-4

#define WIZFI360_SETUP_SNTP_SIZE_LIMIT			50

static int wizfi360_setup_SNTP()
{
	uint8_t temp_cmd_buf[WIZFI360_SETUP_SNTP_SIZE_LIMIT] = {0, };
	uint32_t temp_cmd_len = 0;

	uart_wifi_msgq_event_t wizfi360_rsp_evt_msg;

	wizfi360_response_event_msq_clear();

	strcpy((char*)temp_cmd_buf, "AT+CIPSNTPCFG=1,9\r\n");

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}

	temp_cmd_len = strlen((char*)temp_cmd_buf);

	nrf_drv_uart_tx(&wizfi360_uart_instance, temp_cmd_buf, temp_cmd_len);

	task_sleep(2000);

	int err = 0;

	err = msgq_receive_timed(wizfi360_wifi_response_msgq, (unsigned char*) &wizfi360_rsp_evt_msg, WIZFI360_SETUP_SNTP_TIMEOUT);
	switch(err)
	{
	case UBIK_ERR__SUCCESS :
		if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_OK)
		{
			printf("WIFI Setup : WIZFI360_SETUP_SNTP_SUCCESS\r\n");
			return WIZFI360_SETUP_SNTP_SUCCESS;
		}
		else
		{
			printf("WIFI Setup Error : WIZFI360_SETUP_SNTP_ERROR_ERROR\r\n");
			return WIZFI360_SETUP_SNTP_ERROR_ERROR;
		}
		break;
	case UBIK_ERR__TIMEOUT :
		printf("WIFI Setup Error : WIZFI360_SETUP_SNTP_ERROR_TIMEOUT\r\n");
		return WIZFI360_SETUP_SNTP_ERROR_TIMEOUT;
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_SETUP_SNTP_ERROR_UNKNOWN\r\n");
		return WIZFI360_SETUP_SNTP_ERROR_UNKNOWN;
		break;
	}
}

#define WIZFI360_SETUP_SNTP_TIME_TIMEOUT 		10000

#define WIZFI360_SETUP_SNTP_TIME_SUCCESS				0
#define WIZFI360_SETUP_SNTP_TIME_ERROR_ERROR			-1
#define WIZFI360_SETUP_SNTP_TIME_ERROR_TIMEOUT 			-2
#define WIZFI360_SETUP_SNTP_TIME_ERROR_UNKNOWN 			-3
#define WIZFI360_SETUP_SNTP_TIME_ERROR_WRONG_PAR		-4

#define WIZFI360_CHECK_SNTP_TIME_SIZE_LIMIT			50

static int wizfi360_set_time()
{
	uint8_t temp_cmd_buf[WIZFI360_CHECK_SNTP_TIME_SIZE_LIMIT] = {0, };
	uint32_t temp_cmd_len = 0;

	uart_wifi_msgq_event_t wizfi360_rsp_evt_msg;

	wizfi360_response_event_msq_clear();

	memset(temp_cmd_buf, 0, WIZFI360_CHECK_SNTP_TIME_SIZE_LIMIT);

	strcpy((char*)temp_cmd_buf, "AT+CIPSNTPTIME?\r\n");

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}

	temp_cmd_len = strlen((char*)temp_cmd_buf);

	nrf_drv_uart_tx(&wizfi360_uart_instance, temp_cmd_buf, temp_cmd_len);

	task_sleep(1000);

	int err = 0;

	err = msgq_receive_timed(wizfi360_wifi_response_msgq, (unsigned char*) &wizfi360_rsp_evt_msg, WIZFI360_SETUP_SNTP_TIME_TIMEOUT);
	switch(err)
	{
	case UBIK_ERR__SUCCESS :
		if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_SNTP_TIME)
		{
			printf("WIFI Setup : WIZFI360_SETUP_SNTP_TIME_SUCCESS\r\n");
			return WIZFI360_SETUP_SNTP_TIME_SUCCESS;
		}
		else
		{
			printf("WIFI Setup Error : WIZFI360_SETUP_SNTP_TIME_ERROR_ERROR\r\n");
			return WIZFI360_SETUP_SNTP_TIME_ERROR_ERROR;
		}
		break;
	case UBIK_ERR__TIMEOUT :
		printf("WIFI Setup Error : WIZFI360_SETUP_SNTP_TIME_ERROR_TIMEOUT\r\n");
		return WIZFI360_SETUP_SNTP_TIME_ERROR_TIMEOUT;
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_SETUP_SNTP_TIME_ERROR_UNKNOWN\r\n");
		return WIZFI360_SETUP_SNTP_TIME_ERROR_UNKNOWN;
		break;
	}
}

#define WIZFI360_SETUP_MQTT_TOPIC_TIMEOUT 			10000

#define WIZFI360_SETUP_MQTT_TOPIC_SUCCESS				0
#define WIZFI360_SETUP_MQTT_TOPIC_ERROR_ERROR			-1
#define WIZFI360_SETUP_MQTT_TOPIC_ERROR_TIMEOUT 		-2
#define WIZFI360_SETUP_MQTT_TOPIC_ERROR_UNKNOWN 		-3
#define WIZFI360_SETUP_MQTT_TOPIC_ERROR_WRONG_PAR		-4

#define WIZFI360_SETUP_MQTT_TOPIC_PAR_SIZE_LIMIT	50
#define WIZFI360_SETUP_MQTT_TOPIC_CMD_SIZE_LIMIT	WIZFI360_SETUP_MQTT_TOPIC_PAR_SIZE_LIMIT*2+20



static int wizfi360_setup_MQTT_topic(char* publish_topic, uint8_t publish_topic_len, char* subscribe_topic, uint8_t subscribe_topic_len)
{
	uint8_t temp_cmd_buf[WIZFI360_SETUP_MQTT_TOPIC_CMD_SIZE_LIMIT] = {0, };
	uint8_t temp_char_buf[WIZFI360_SETUP_MQTT_TOPIC_PAR_SIZE_LIMIT] = {0, };
	uint32_t temp_cmd_len = 0;
	int err;

	uart_wifi_msgq_event_t wizfi360_rsp_evt_msg;

	//check ssid/password len
	if(publish_topic_len >= WIZFI360_SETUP_MQTT_TOPIC_PAR_SIZE_LIMIT || subscribe_topic_len>=WIZFI360_SETUP_MQTT_TOPIC_PAR_SIZE_LIMIT)
	{
		printf("WIFI Setup Error : WIZFI360_SETUP_MQTT_TOPIC_ERROR_WRONG_PAR\r\n");
		return WIZFI360_SETUP_MQTT_TOPIC_ERROR_WRONG_PAR;
	}


	wizfi360_response_event_msq_clear();

	strcpy((char*)temp_cmd_buf, "AT+MQTTTOPIC=\"");

	memset(temp_char_buf, 0 , WIZFI360_SETUP_MQTT_TOPIC_PAR_SIZE_LIMIT);
	memcpy(temp_char_buf, publish_topic, publish_topic_len);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\",\"");

	memset(temp_char_buf, 0 , WIZFI360_SETUP_MQTT_TOPIC_PAR_SIZE_LIMIT);
	memcpy(temp_char_buf, subscribe_topic, subscribe_topic_len);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\"\r\n");

	temp_cmd_len = strlen((char*)temp_cmd_buf);

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}
	nrf_drv_uart_tx(&wizfi360_uart_instance, temp_cmd_buf, temp_cmd_len);

	err = msgq_receive_timed(wizfi360_wifi_response_msgq, (unsigned char*) &wizfi360_rsp_evt_msg, WIZFI360_SETUP_MQTT_TOPIC_TIMEOUT);
	switch(err)
	{
	case UBIK_ERR__SUCCESS :
		if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_OK)
		{
			printf("WIFI Setup : WIZFI360_SETUP_MQTT_TOPIC_SUCCESS\r\n");
			return WIZFI360_SETUP_MQTT_TOPIC_SUCCESS;
		}
		else
		{
			printf("WIFI Setup Error : WIZFI360_SETUP_MQTT_TOPIC_ERROR_ERROR\r\n");
			return WIZFI360_SETUP_MQTT_TOPIC_ERROR_ERROR;
		}
		break;
	case UBIK_ERR__TIMEOUT :
		printf("WIFI Setup Error : WIZFI360_SETUP_MQTT_TOPIC_ERROR_TIMEOUT\r\n");
		return WIZFI360_SETUP_MQTT_TOPIC_ERROR_TIMEOUT;
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_SETUP_MQTT_TOPIC_ERROR_UNKNOWN\r\n");
		return WIZFI360_SETUP_MQTT_TOPIC_ERROR_UNKNOWN;
		break;
	}
}

#define WIZFI360_CONN_MQTT_TIMEOUT 			20000

#define WIZFI360_CONN_MQTT_SUCCESS				0
#define WIZFI360_CONN_MQTT_ERROR_ERROR			-1
#define WIZFI360_CONN_MQTT_ERROR_TIMEOUT 		-2
#define WIZFI360_CONN_MQTT_ERROR_UNKNOWN 		-3
#define WIZFI360_CONN_MQTT_ERROR_WRONG_PAR		-4

#define WIZFI360_CONN_MQTT_PAR_SIZE_LIMIT	20
#define WIZFI360_CONN_MQTT_CMD_SIZE_LIMIT	WIZFI360_CONN_MQTT_PAR_SIZE_LIMIT*2+20


static int wizfi360_conn_MQTT(bool authentication, char* broker_ip, uint8_t broker_ip_len, char* broker_port, uint8_t broker_port_len)
{
	uint8_t temp_cmd_buf[WIZFI360_CONN_MQTT_CMD_SIZE_LIMIT] = {0, };
	uint8_t temp_char_buf[WIZFI360_CONN_MQTT_PAR_SIZE_LIMIT] = {0, };
	uint32_t temp_cmd_len = 0;
	int err;

	uart_wifi_msgq_event_t wizfi360_rsp_evt_msg;

	//check ssid/password len
	if(broker_ip_len >= WIZFI360_CONN_MQTT_PAR_SIZE_LIMIT || broker_port_len>=WIZFI360_CONN_MQTT_PAR_SIZE_LIMIT)
	{
		printf("WIFI Setup Error : WIZFI360_SETUP_MQTT_TOPIC_ERROR_UNKNOWN\r\n");
		return WIZFI360_SETUP_MQTT_TOPIC_ERROR_WRONG_PAR;
	}

	wizfi360_response_event_msq_clear();

	if(authentication == true)
	{
		strcpy((char*)temp_cmd_buf, "AT+MQTTCON=1,\"");
	}
	else
	{
		strcpy((char*)temp_cmd_buf, "AT+MQTTCON=0,\"");
	}


	memset(temp_char_buf, 0 , WIZFI360_CONN_MQTT_PAR_SIZE_LIMIT);
	memcpy(temp_char_buf, broker_ip, broker_ip_len);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\",");

	memset(temp_char_buf, 0 , WIZFI360_CONN_MQTT_PAR_SIZE_LIMIT);
	memcpy(temp_char_buf, broker_port, broker_port_len);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\r\n");

	temp_cmd_len = strlen((char*)temp_cmd_buf);

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}
	nrf_drv_uart_tx(&wizfi360_uart_instance, temp_cmd_buf, temp_cmd_len);

	while(1)
	{
		err = msgq_receive_timed(wizfi360_wifi_response_msgq, (unsigned char*) &wizfi360_rsp_evt_msg, WIZFI360_CONN_AP_TIMEOUT);
		switch(err)
		{
		case UBIK_ERR__SUCCESS :
			if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_OK)
			{
				printf("WIFI Setup : WIZFI360_CONN_MQTT_SUCCESS\r\n");
				return WIZFI360_CONN_MQTT_SUCCESS;
			}
			break;
		case UBIK_ERR__TIMEOUT :
			printf("WIFI Setup Error : WIZFI360_CONN_AP_ERROR_TIMEOUT\r\n");
			return WIZFI360_CONN_AP_ERROR_TIMEOUT;
			break;
		default :
			printf("WIFI Setup Error : WIZFI360_CONN_AP_ERROR_UNKNOWN\r\n");
			return WIZFI360_CONN_AP_ERROR_UNKNOWN;
			break;
		}
	}
}

#define WIZFI360_SEND_MQTT_TIMEOUT 			20000

#define WIZFI360_SEND_MQTT_SUCCESS				0
#define WIZFI360_SEND_MQTT_ERROR_ERROR			-1
#define WIZFI360_SEND_MQTT_ERROR_TIMEOUT 		-2
#define WIZFI360_SEND_MQTT_ERROR_UNKNOWN 		-3
#define WIZFI360_SEND_MQTT_ERROR_WRONG_PAR		-4

#define WIZFI360_SEND_MQTT_PAR_SIZE_LIMIT	600
#define WIZFI360_SEND_MQTT_CMD_SIZE_LIMIT	WIZFI360_SEND_MQTT_PAR_SIZE_LIMIT+20

static int wizfi360_send_mqtt_packet(char* packet, uint8_t packet_len)
{
	uint8_t temp_cmd_buf[WIZFI360_SEND_MQTT_CMD_SIZE_LIMIT] = {0, };
	uint8_t temp_char_buf[WIZFI360_SEND_MQTT_CMD_SIZE_LIMIT] = {0, };
	uint32_t temp_cmd_len = 0;
	int err;

	uart_wifi_msgq_event_t wizfi360_rsp_evt_msg;

	wizfi360_response_event_msq_clear();

	//check packet len
	if(packet_len >= WIZFI360_SEND_MQTT_CMD_SIZE_LIMIT)
	{
		printf("WIFI Setup Error : WIZFI360_CONN_AP_ERROR_TIMEOUT\r\n");
		return WIZFI360_SEND_MQTT_ERROR_WRONG_PAR;
	}

	strcpy((char*)temp_cmd_buf, "AT+MQTTPUB=\"");

	memset(temp_char_buf, 0 , WIZFI360_SEND_MQTT_PAR_SIZE_LIMIT);
	memcpy(temp_char_buf, packet, packet_len);
	strcat((char*)temp_cmd_buf, (char*)temp_char_buf);

	strcat((char*)temp_cmd_buf, "\"\r\n");

	temp_cmd_len = strlen((char*)temp_cmd_buf);

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}

	nrf_drv_uart_tx(&wizfi360_uart_instance, (uint8_t*)temp_cmd_buf, temp_cmd_len);

	while (nrf_drv_uart_tx_in_progress(&wizfi360_uart_instance)) {
		task_sleep(10);
	}

	err = msgq_receive_timed(wizfi360_wifi_response_msgq, (unsigned char*) &wizfi360_rsp_evt_msg, WIZFI360_CONN_AP_TIMEOUT);
	switch(err)
	{
	case UBIK_ERR__SUCCESS :
		if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_OK)
		{
			printf("WIFI Setup : WIZFI360_SEND_MQTT_SUCCESS\r\n");
			return WIZFI360_SEND_MQTT_SUCCESS;
		}
		else if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_LINK_IS_NOT_VAILD)
		{
			printf("WIFI Setup Error : WIZFI360_SEND_MQTT_ERROR_ERROR\r\n");
			return WIZFI360_SEND_MQTT_ERROR_ERROR;
		}
		else if(wizfi360_rsp_evt_msg.event == WIZFI360_RSP_EVENT_ERROR)
		{
			printf("MQTT send Error : WIZFI360_SEND_MQTT_ERROR_ERROR\r\n");
			return WIZFI360_SEND_MQTT_ERROR_ERROR;
		}
		break;
	case UBIK_ERR__TIMEOUT :
		printf("WIFI Setup Error : WIZFI360_SEND_MQTT_ERROR_TIMEOUT\r\n");
		return WIZFI360_SEND_MQTT_ERROR_TIMEOUT;
		break;
	default :
		printf("WIFI Setup Error : WIZFI360_SEND_MQTT_ERROR_UNKNOWN\r\n");
		return WIZFI360_SEND_MQTT_ERROR_UNKNOWN;
		break;
	}

	return WIZFI360_SEND_MQTT_SUCCESS;
}

static int send_paar_mqtt_HB_msg()
{
	char temp_packet[PAAR_MQTT_PACKET_LENGTH_LIMIT];
	uint8_t temp_cmd_len = 0;

	memset(temp_packet, 0, PAAR_MQTT_PACKET_LENGTH_LIMIT);

	strcpy(temp_packet, "{\"{TEST_HEART_BEAT}\"}");

	temp_cmd_len = strlen(temp_packet);

	int err;
	err = wizfi360_send_mqtt_packet(temp_packet, temp_cmd_len);
	if(err != WIZFI360_SEND_MQTT_SUCCESS)
	{
		set_wifi_ready(false);
		wifi_processing_event_send(WIFI_PROCESSING_EVENT_ERROR_RESET, 0, NULL);

		return -1;
	}

	return 0;
}

//parameter... data msg
//PAAR ID(4 Bytes) | SERVICE ID(1 Byte) | BODY data Length(1 Byte) | BODY Data (N Bytes)
static int send_paar_mqtt_msg(uint8_t* msg)
{
	uint32_t paar_id;
	uint8_t service_id;

	char temp_id_char[TEMP_ID_BUFFER_SIZE];
	char temp_packet[PAAR_MQTT_PACKET_LENGTH_LIMIT];

	memcpy(&paar_id, &msg[PAAR_MQTT_INDEX_PAAR_ID], PAAR_ID_LEN);

	service_id = msg[PAAR_MQTT_INDEX_SERVCIE_ID];

	memset(temp_packet, 0, PAAR_MQTT_PACKET_LENGTH_LIMIT);

	uint8_t temp_cmd_len = 0;

	//Set Header & Service ID
	strcpy(temp_packet, "{\"HEADER\":{\"PAAR_ID\":\"");

	if(paar_id <= 0x0FFFFFFF )
		strcat(temp_packet, "0");

	memset(temp_id_char, 0, TEMP_ID_BUFFER_SIZE);

	itoa(paar_id, temp_id_char, 16);

	uint8_t i;
	for(i=0; i<strlen(temp_id_char) ;i++)
	{
		if(temp_id_char[i] >= 'a' && temp_id_char[i] <= 'z') {
			temp_id_char[i] = temp_id_char[i]-32;
		}
	}

	strcat(temp_packet, temp_id_char);

	//Set Service ID
	strcat(temp_packet, "\",\"SERVICE_ID\":\"");

	if(service_id <= 0x0F )
		strcat(temp_packet, "0");

	memset(temp_id_char, 0, TEMP_ID_BUFFER_SIZE);

	itoa(service_id, temp_id_char, 16);

	strcat(temp_packet, temp_id_char);

	//Set SH_ID
	strcat(temp_packet, "\",\"SH_ID\":\"");
	strcat(temp_packet, SH_ID);
	strcat(temp_packet, "\",");

	//Set SP_ID
	strcat(temp_packet, "\"SP_ID\":\"");
	strcat(temp_packet, SP_ID);
	strcat(temp_packet, "\",");

	//Set Location
	strcat(temp_packet, "\"LOCATION\":\"");
	strcat(temp_packet, SP_LOCATION);
	strcat(temp_packet, "\",");

	//Set Time
	strcat(temp_packet, "\"TIME\":\"");
	copy_test_time(temp_packet, test_YY, test_MM, test_DD, test_hh, test_mm, test_ss);

	//Set Body Data
	strcat(temp_packet, "\"},\"BODY\":{\"DATA\":\"");

//	uint8_t i;
	uint8_t data_temp;

	if(msg[PAAR_MQTT_INDEX_BODY_DATA_LEN] >= PAAR_MQTT_PACKET_LENGTH_LIMIT-strlen(temp_packet)-1)
		return -1;

	for(i=0; i<msg[PAAR_MQTT_INDEX_BODY_DATA_LEN]; i++)
	{
		memset(temp_id_char, 0, TEMP_ID_BUFFER_SIZE);
		data_temp = msg[PAAR_MQTT_INDEX_BODY_DATA + i];
		itoa(data_temp, temp_id_char, 16);

		if(data_temp <= 0x0F)
			strcat(temp_packet, "0");

		strcat(temp_packet, temp_id_char);
	}

	strcat(temp_packet, "\"}}");

	temp_cmd_len = strlen(temp_packet);

	int err;
	err = wizfi360_send_mqtt_packet(temp_packet, temp_cmd_len);
	if(err != WIZFI360_SEND_MQTT_SUCCESS)
	{
		set_wifi_ready(false);
		wifi_processing_event_send(WIFI_PROCESSING_EVENT_ERROR_RESET, 0, NULL);

		return -1;
	}

	return 0;

	task_sleep(100);
}

static int send_paar_mqtt_msg_env(uint8_t* msg)
{
	uint32_t paar_id;
	uint8_t service_id;

	char temp_id_char[TEMP_ID_BUFFER_SIZE];
	char temp_packet[PAAR_MQTT_PACKET_LENGTH_LIMIT];

	memcpy(&paar_id, &msg[PAAR_MQTT_INDEX_PAAR_ID], PAAR_ID_LEN);

	service_id = msg[PAAR_MQTT_INDEX_SERVCIE_ID];

	memset(temp_packet, 0, PAAR_MQTT_PACKET_LENGTH_LIMIT);

	uint8_t temp_cmd_len = 0;

	//Set Header & Service ID
	strcpy(temp_packet, "{\"HEADER\":{\"PAAR_ID\":\"");

	if(paar_id <= 0x0FFFFFFF )
		strcat(temp_packet, "0");

	memset(temp_id_char, 0, TEMP_ID_BUFFER_SIZE);

	itoa(paar_id, temp_id_char, 16);

	uint8_t i;
	for(i=0; i<strlen(temp_id_char) ;i++)
	{
		if(temp_id_char[i] >= 'a' && temp_id_char[i] <= 'z') {
			temp_id_char[i] = temp_id_char[i]-32;
		}
	}

	strcat(temp_packet, temp_id_char);

	//Set Service ID
	strcat(temp_packet, "\",\"SERVICE_ID\":\"");

	if(service_id <= 0x0F )
		strcat(temp_packet, "0");

	memset(temp_id_char, 0, TEMP_ID_BUFFER_SIZE);

	itoa(service_id, temp_id_char, 16);

	strcat(temp_packet, temp_id_char);

	//Set SH_ID
	strcat(temp_packet, "\",\"SH_ID\":\"");
	strcat(temp_packet, SH_ID);
	strcat(temp_packet, "\",");

	//Set SP_ID
	strcat(temp_packet, "\"SP_ID\":\"");
	strcat(temp_packet, SP_ID);
	strcat(temp_packet, "\",");

	//Set Location
	strcat(temp_packet, "\"LOCATION\":\"");
	strcat(temp_packet, SP_LOCATION);
	strcat(temp_packet, "\",");

	//Set Time
	strcat(temp_packet, "\"TIME\":\"");
	copy_test_time(temp_packet, test_YY, test_MM, test_DD, test_hh, test_mm, test_ss);

	//Set Body Data
	strcat(temp_packet, "\"},\"BODY\":{\"DATA\":{\"CMD\":\"ff\",\"ENV\":\"");

//	uint8_t i;
	uint8_t data_temp;

	for(i=0; i<msg[PAAR_MQTT_INDEX_BODY_DATA_LEN]; i++)
	{
		memset(temp_id_char, 0, TEMP_ID_BUFFER_SIZE);
		data_temp = msg[PAAR_MQTT_INDEX_BODY_DATA + i];
		itoa(data_temp, temp_id_char, 16);

		if(data_temp <= 0x0F)
			strcat(temp_packet, "0");

		strcat(temp_packet, temp_id_char);
	}

	strcat(temp_packet, "\"}}}");

	temp_cmd_len = strlen(temp_packet);

	int err;
	err = wizfi360_send_mqtt_packet(temp_packet, temp_cmd_len);
	if(err != WIZFI360_SEND_MQTT_SUCCESS)
	{
		set_wifi_ready(false);
		wifi_processing_event_send(WIFI_PROCESSING_EVENT_ERROR_RESET, 0, NULL);

		return -1;
	}

	return 0;

	task_sleep(100);
}
#if 0 //MQTT Test MSG
//test code
static void send_test_mqtt_msg(uint8_t cnt)
{
	char temp_packet[250];

	memset(temp_packet, 0, 250);

	uint8_t temp_cmd_len = 0;

	strcpy(temp_packet, "{\"HEADER\":{\"PAAR_ID\":\"");

	//set Service ID
	switch(cnt)
	{
	//수도 Open : 010000E0
	case 0 :
		strcat(temp_packet, "010000E0\",\"SERVICE_ID\":\"12");
		strcat(temp_packet, "\",\"SH_ID\":\"AB001309\",\"SP_ID\":\"010000D4\",\"LOCATION\":\"bedroom\",\"TIME\":\"");
		break;
	//수도 off : 010000E0
	case 1 :
		task_sleep(2000);
		strcat(temp_packet, "010000E0\",\"SERVICE_ID\":\"12");
		strcat(temp_packet, "\",\"SH_ID\":\"AB001309\",\"SP_ID\":\"010000D4\",\"LOCATION\":\"bedroom\",\"TIME\":\"");
		break;
	//선풍기 On : 030000D8
	case 2 :
		task_sleep(10000);
		strcat(temp_packet, "030000D8\",\"SERVICE_ID\":\"17");
		strcat(temp_packet, "\",\"SH_ID\":\"AB001309\",\"SP_ID\":\"010000D4\",\"LOCATION\":\"livingroom\",\"TIME\":\"");
		break;
	//선풍기 Off : 030000D8
	case 3 :
		task_sleep(10000);
		strcat(temp_packet, "030000D8\",\"SERVICE_ID\":\"17");
		strcat(temp_packet, "\",\"SH_ID\":\"AB001309\",\"SP_ID\":\"010000D4\",\"LOCATION\":\"livingroom\",\"TIME\":\"");
		break;
	//냉장고 Open : 010000D4
	case 4 :
		task_sleep(10000);
		strcat(temp_packet, "030000E0\",\"SERVICE_ID\":\"12");
		strcat(temp_packet, "\",\"SH_ID\":\"AB001309\",\"SP_ID\":\"010000D4\",\"LOCATION\":\"livingroom\",\"TIME\":\"");
		break;
	//냉장고 Close : 010000D4
	case 5 :
		task_sleep(2000);
		strcat(temp_packet, "030000E0\",\"SERVICE_ID\":\"12");
		strcat(temp_packet, "\",\"SH_ID\":\"AB001309\",\"SP_ID\":\"010000D4\",\"LOCATION\":\"livingroom\",\"TIME\":\"");
		break;
	default :
		return;
		break;
	}

	copy_test_time(temp_packet, test_YY, test_MM, test_DD, test_hh, test_mm, test_ss);

	strcat(temp_packet, "\"},\"BODY\":{\"DATA\":\"");

	//set Data Field
	switch(cnt)
	{
	//수도꼭지 On
	case 0 :
		strcat(temp_packet, "03040101");
		break;
	//수도꼭지 Off
	case 1 :
		strcat(temp_packet, "03040102");
		break;
	//TV On
	case 2 :
		strcat(temp_packet, "03040101FFFF");
		break;
	//TV Off
	case 3 :
		strcat(temp_packet, "03040102FFFF");
		break;
	//433 PIR On : Location 0x0001
	case 4 :
		strcat(temp_packet, "03050101");
		break;
	//433 PIR Off : Location 0x0001
	case 5 :
		strcat(temp_packet, "03050102");
		break;
	default :
		return;
		break;
	}

	strcat(temp_packet, "\"}}");

	temp_cmd_len = strlen(temp_packet);

	wizfi360_send_mqtt_packet(temp_packet, temp_cmd_len);
}
#endif 


#define WIFI_SETUP_SHORT_DELAY	500

#define WIFI_SETUP_SUCCESS			0
#define WIFI_SETUP_ERROR_RESET		-1
#define WIFI_SETUP_ERROR_RETRY		-2

int setup_wifi_mqtt()
{
	int err = 0;

	err = wizfi360_reset_module();
	if(err != WIZFI360_RESET_MODULE_SUCCESS)
	{
		printf("Error : setup wifi, reset Module. \r\n");
		return WIFI_SETUP_ERROR_RESET;
	}

	task_sleep(1000);

	err = wizfi360_set_mode(WIZFI360_MODE_STATION);
	if(err != WIZFI360_SET_MODE_SUCCESS)
	{
		printf("Error : setup wifi, set mode. \r\n");
		return WIFI_SETUP_ERROR_RESET;
	}

	task_sleep(1000);

	err = wizfi360_enable_DHCP(WIZFI360_DHCP_MODE_STATION, WIZFI360_DHCP_ENABLE);
	if(err != WIZFI360_SET_DHCP_SUCCESS)
	{
		printf("Error : setup wifi, enalbe DHCP. \r\n");
		return WIFI_SETUP_ERROR_RESET;
	}

	task_sleep(1000);

	err = wizfi360_setup_SNTP();
	if(err != WIZFI360_SETUP_SNTP_SUCCESS)
	{
		printf("Error : setup wifi, setup SNTP. \r\n");
		return WIFI_SETUP_ERROR_RESET;
	}

	char AP_ssid[50] = {0, };
	char AP_password[50] = {0, };
	uint8_t ssid_len, password_len;

	ssid_len = strlen(TEST_AP_SSID);
	strcpy(AP_ssid, TEST_AP_SSID);

	password_len = strlen(TEST_AP_PASSWORD);
	strcpy(AP_password, TEST_AP_PASSWORD);

	task_sleep(1000);

	err = wizfi360_connect_AP(AP_ssid, ssid_len, AP_password, password_len);
	switch(err)
	{
	case WIZFI360_CONN_AP_ERROR_TIMEOUT :
		//reset Wifi Device
		printf("Error : setup wifi, connect AP/Timeout. \r\n");
		return WIFI_SETUP_ERROR_RESET;
		break;
	case WIZFI360_CONN_AP_ERROR_ERROR :
		//retry or reset Device
		printf("Error : setup wifi, connect AP/Error. \r\n");
		return WIFI_SETUP_ERROR_RESET;
		break;
	}

	task_sleep(WIFI_SETUP_SHORT_DELAY);

	char MQTT_user_id[20] = {0, };
	char MQTT_user_password[20] = {0, };
	char MQTT_client_id[20] = {0, };
	uint8_t mqtt_user_id_len, mqtt_user_password_len, mqtt_client_id_len;

	mqtt_user_id_len = strlen(TEST_MQTT_USER_NAME);
	strcpy(MQTT_user_id, TEST_MQTT_USER_NAME);

	mqtt_user_password_len = strlen(TEST_MQTT_USER_PASSWORD);
	strcpy(MQTT_user_password, TEST_MQTT_USER_PASSWORD);

	mqtt_client_id_len = strlen(TEST_MQTT_CLIENT_ID);
	strcpy(MQTT_client_id, TEST_MQTT_CLIENT_ID);

	err = wizfi360_setup_MQTT(MQTT_user_id, mqtt_user_id_len, MQTT_user_password, mqtt_user_password_len,
			MQTT_client_id, mqtt_client_id_len, TEST_MQTT_ALIVE_TIME);
	if(err != WIZFI360_SETUP_MQTT_SUCCESS)
	{
		printf("Error : setup wifi, setup_MQTT. \r\n");
		return WIFI_SETUP_ERROR_RESET;
	}
	
	task_sleep(WIFI_SETUP_SHORT_DELAY);

	char MQTT_topic_publish[50] = {0, };;
	char MQTT_topic_subscribe[50] = {0, };;
	uint8_t mqtt_topic_publish_len, mqtt_topic_subscribe_len;

	mqtt_topic_publish_len = strlen(TEST_MQTT_TOPIC_PUBLISH);
	strcpy(MQTT_topic_publish, TEST_MQTT_TOPIC_PUBLISH);

	mqtt_topic_subscribe_len = strlen(TEST_MQTT_TOPIC_SUBSCRIBE);
	strcpy(MQTT_topic_subscribe, TEST_MQTT_TOPIC_SUBSCRIBE);

	err = wizfi360_setup_MQTT_topic(MQTT_topic_publish, mqtt_topic_publish_len, MQTT_topic_subscribe, mqtt_topic_subscribe_len);
	if(err != WIZFI360_SETUP_MQTT_TOPIC_SUCCESS)
	{
		printf("Error : setup wifi, setup_MQTT_topic. \r\n");
		return WIFI_SETUP_ERROR_RESET;
	}

	task_sleep(WIFI_SETUP_SHORT_DELAY);

	char MQTT_broker_ip[20] = {0, };
	char MQTT_broker_port[10] = {0, };
	uint8_t mqtt_broker_ip_len, mqtt_broker_port_len;

	mqtt_broker_ip_len = strlen(TEST_MQTT_BROKER_IP);
	strcpy(MQTT_broker_ip, TEST_MQTT_BROKER_IP);

	mqtt_broker_port_len = strlen(TEST_MQTT_BROKER_PORT);
	strcpy(MQTT_broker_port, TEST_MQTT_BROKER_PORT);

	err = wizfi360_conn_MQTT(false, MQTT_broker_ip, mqtt_broker_ip_len, MQTT_broker_port, mqtt_broker_port_len);
	if(err != WIZFI360_CONN_MQTT_SUCCESS)
	{
		printf("Error : setup wifi, conn_MQTT. \r\n");
		return WIFI_SETUP_ERROR_RESET;
	}

	task_sleep(WIFI_SETUP_SHORT_DELAY);

	wizfi360_set_time();

	set_wifi_ready(true);

	return WIFI_SETUP_SUCCESS;
}

static void mqtt_hb_handler()
{
wifi_processing_event_send(WIFI_PROCESSING_EVENT_SEND_MQTT_HB, 0, NULL);
}

#define MQTT_HB_TICK_EVENT_INTERVAL     APP_TIMER_TICKS(60000)

static void wizfi360_wifi_processing_task(void * arg) {

	int err = 0;
	int mqtt_retry_count = 0;

	ble_stack_init_wait();

	task_sleep(1000);

	uart_wifi_enable();

	task_sleep(1000);

	while(WIFI_SETUP_ERROR_RESET == setup_wifi_mqtt())
	{
		mqtt_retry_count++;
		if(mqtt_retry_count >= MQTT_RESET_MAX_COUNT)
			bsp_abortsystem();
		
		uart_wifi_reset();
	}

	mqtt_retry_count = 0;

	task_sleep(1000);

	wifi_processing_msgq_event_t wifi_processing_evt_msg;

	app_timer_init();

	app_timer_create(&mqtt_hb_timer, APP_TIMER_MODE_SINGLE_SHOT, mqtt_hb_handler);

	app_timer_start(mqtt_hb_timer, MQTT_HB_TICK_EVENT_INTERVAL, NULL);

	while(1)
	{

		err = msgq_receive(wifi_processing_event_msgq, (unsigned char*) &wifi_processing_evt_msg);
		if( err != 0 ) {
			printf("fail at msgq_receive : wifi_processing_event_msgq\r\n");
			APP_ERROR_CHECK(err);
		} else {
			switch( wifi_processing_evt_msg.event ) {
			case WIFI_PROCESSING_EVENT_SEND_MQTT_ENV :
			{
				if(get_wifi_ready() == false)
				{
					break;
				}

				task_sleep(200);
				int r;
				r = send_paar_mqtt_msg_env(wifi_processing_evt_msg.msg);
				if(r != 0)
				{
					uint8_t* temp_buffer = NULL;
					temp_buffer = malloc(wifi_processing_evt_msg.msg[PAAR_MQTT_INDEX_BODY_DATA_LEN] + 5);
					memset(temp_buffer, 0, wifi_processing_evt_msg.msg[PAAR_MQTT_INDEX_BODY_DATA_LEN] + 5);
					if(temp_buffer != NULL)
					{
						memcpy(temp_buffer, wifi_processing_evt_msg.msg, wifi_processing_evt_msg.msg[PAAR_MQTT_INDEX_BODY_DATA_LEN] + 5);
						wifi_processing_event_send(WIFI_PROCESSING_EVENT_SEND_MQTT_ENV, 0, temp_buffer);
					}
				}
				break;
			}
			case WIFI_PROCESSING_EVENT_SEND_MQTT :
			{
				if(get_wifi_ready() == false)
				{
					break;
				}

				task_sleep(200);
				int r;
				r = send_paar_mqtt_msg(wifi_processing_evt_msg.msg);
				if(r != 0)
				{
					uint8_t* temp_buffer = NULL;
					temp_buffer = malloc(wifi_processing_evt_msg.msg[PAAR_MQTT_INDEX_BODY_DATA_LEN] + 5);
					memset(temp_buffer, 0, wifi_processing_evt_msg.msg[PAAR_MQTT_INDEX_BODY_DATA_LEN] + 5);
					if(temp_buffer != NULL)
					{
						memcpy(temp_buffer, wifi_processing_evt_msg.msg, wifi_processing_evt_msg.msg[PAAR_MQTT_INDEX_BODY_DATA_LEN] + 5);
						wifi_processing_event_send(WIFI_PROCESSING_EVENT_SEND_MQTT, 0, temp_buffer);
					}
				}
			}
				break;
			case WIFI_PROCESSING_EVENT_ERROR_RESET :
			{
				app_timer_stop(mqtt_hb_timer);
				set_wifi_ready(false);

				while(WIFI_SETUP_ERROR_RESET == setup_wifi_mqtt())
				{
					mqtt_retry_count++;
					if(mqtt_retry_count >= MQTT_RESET_MAX_COUNT)
						bsp_abortsystem();
					
					uart_wifi_reset();
				}
				
				mqtt_retry_count = 0;

				app_timer_start(mqtt_hb_timer, MQTT_HB_TICK_EVENT_INTERVAL, NULL);
			}
				break;
			case WIFI_PROCESSING_EVENT_SEND_MQTT_HB :

				send_paar_mqtt_HB_msg();

				app_timer_start(mqtt_hb_timer, MQTT_HB_TICK_EVENT_INTERVAL, NULL);

				break;
			default :
				break;
			}
		}


		task_sleep(500);

		if(wifi_processing_evt_msg.msg != NULL)
			free(wifi_processing_evt_msg.msg);
	}
}

static bool is_MQTT_SERVER_CONNECTED = false;

static void wizfi360_uart_rx_task(void * arg) {
	int err;
	uart_wifi_msgq_event_t uart_wifi_evt_msg;

	//int result;

	for(;;) {
		err = msgq_receive(wizfi360_wifi_uart_rx_msgq, (unsigned char*) &uart_wifi_evt_msg);
		if( err != 0 ) {
			printf("fail at msgq_receive : wizfi360_wifi_uart_rx_msgq\r\n");
			APP_ERROR_CHECK(err);
		} else {
			switch( uart_wifi_evt_msg.event ) {
			case WIZFI360_WIFI_EVENT_RECEIVED:

				for (int i = 0; i < UART_WIFI_RSP_BUF_SIZE; i++) {
					printf("%c", wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][i]);
				}

				//response handling
				//"OK"
				if(strcmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][0], "OK\r\n") == 0)
				{
					wizfi360_response_event_send((uint8_t) WIZFI360_RSP_EVENT_OK, 0, (uint8_t*)NULL);
				}
				//"ERROR"
				else if(strcmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][0], "ERROR\r\n") == 0)
				{
					wizfi360_response_event_send(WIZFI360_RSP_EVENT_ERROR, 0, (uint8_t*)NULL);
				}
				//"FAIL"
				else if(strcmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][0], "FAIL\r\n") == 0)
				{
					wizfi360_response_event_send(WIZFI360_RSP_EVENT_FAIL, 0, (uint8_t*)NULL);
				}
				//"SEND OK"
				else if(strcmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][0], "SEND OK\r\n") == 0)
				{
					wizfi360_response_event_send(WIZFI360_RSP_EVENT_SEND_OK, 0, (uint8_t*)NULL);
				}
				else if(strcmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][0], "link is not valid\r\n") == 0)
				{
					wizfi360_response_event_send(WIZFI360_RSP_EVENT_LINK_IS_NOT_VAILD, 0, (uint8_t*)NULL);
				}
				else if(strcmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][0], "WIFI GOT IP\r\n") == 0)
				{
					wizfi360_response_event_send(WIZFI360_RSP_EVENT_WIFI_GOT_IP, 0, (uint8_t*)NULL);
				}
				else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][0], "+CIPSNTPTIME:", 13) == 0)
				{
					//Set time by SNTP time response;
					uint8_t YY, MM, DD, hh, mm, ss;
					char temp_time[5] = {0, };

					if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "Jan", 3) == 0)
					{
						MM = 1;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "Feb", 3) == 0)
					{
						MM = 2;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "Mar", 3) == 0)
					{
						MM = 3;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "Apr", 3) == 0)
					{
						MM = 4;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "May", 3) == 0)
					{
						MM = 5;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "Jun", 3) == 0)
					{
						MM = 6;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "July", 4) == 0)
					{
						MM = 7;

						memcpy(temp_time, &wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][22], 2);
						DD = atoi(temp_time);

						memset(temp_time, 0, 5);
						memcpy(temp_time, &wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][25], 2);
						hh = atoi(temp_time);

						memset(temp_time, 0, 5);
						memcpy(temp_time, &wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][26], 2);
						mm = atoi(temp_time);

						memset(temp_time, 0, 5);
						memcpy(temp_time, &wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][31], 2);
						ss = atoi(temp_time);

						memset(temp_time, 0, 5);
						memcpy(temp_time, &wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][36], 2);
						YY = atoi(temp_time);

						set_test_time( YY, MM, DD, hh, mm, ss);

						memset(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][0], 0, UART_WIFI_RSP_BUF_SIZE);

						break;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "Aug", 3) == 0)
					{
						MM = 8;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "Sep", 3) == 0)
					{
						MM = 9;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "Oct", 3) == 0)
					{
						MM = 10;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "Nov", 3) == 0)
					{
						MM = 11;
					}
					else if(strncmp(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][17], "Dec", 3) == 0)
					{
						MM = 12;
					}
					else
					{
						memset(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][0], 0, UART_WIFI_RSP_BUF_SIZE);

						break;
					}

					memcpy(temp_time, &wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][21], 2);
					DD = atoi(temp_time);

					memset(temp_time, 0, 5);
					memcpy(temp_time, &wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][24], 2);
					hh = atoi(temp_time);

					memset(temp_time, 0, 5);
					memcpy(temp_time, &wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][27], 2);
					mm = atoi(temp_time);

					memset(temp_time, 0, 5);
					memcpy(temp_time, &wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][30], 2);
					ss = atoi(temp_time);

					memset(temp_time, 0, 5);
					memcpy(temp_time, &wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][35], 2);
					YY = atoi(temp_time);


					set_test_time( YY, MM, DD, hh, mm, ss);

					wizfi360_response_event_send(WIZFI360_RSP_EVENT_SNTP_TIME, 0, (uint8_t*)NULL);
				}

				

				memset(&wizfi360_uart_rsp_buffer[uart_wifi_evt_msg.status][0], 0, UART_WIFI_RSP_BUF_SIZE);
				break;

			default:
				break;
			}
		}
	}
}

void uart_wifi_module_task_init(void) {
	ret_code_t err;

	err = wizfi_uart_init();
	if (err != 0) {
		printf("fail at init uart_wifi_io_init\r\n");
		assert(err == 0);
	}

	err = msgq_create(&wizfi360_wifi_uart_rx_msgq, sizeof(uart_wifi_msgq_event_t), 20);
	if(err != 0) {
		printf("fail at msgq_create\r\n");
		assert(err == 0);
	}

	err = msgq_create(&wifi_processing_event_msgq, sizeof(wifi_processing_msgq_event_t), 20);
	if(err != 0) {
		printf("fail at msgq_create\r\n");
		assert(err == 0);
	}

	err = msgq_create(&wizfi360_wifi_response_msgq, sizeof(uart_wifi_msgq_event_t), 20);
	if(err != 0) {
		printf("fail at msgq_create\r\n");
		assert(err == 0);
	}

	err = msgq_create(&wizfi360_wifi_mqtt_data_msgq, sizeof(uart_wifi_msgq_event_t), 20);
	if(err != 0) {
		printf("fail at msgq_create\r\n");
		assert(err == 0);
	}

	err = task_create(NULL, wizfi360_wifi_processing_task, NULL, task_gethighestpriority()-1, 1024, "wizfi360_wifi_processing_task");
	if( err != 0 ) {
		printf("fail at task_create\r\n");
	} else {
		printf("== UART_WIFI_TASK created\r\n");
	}

	//Wizfi360 uart Rx handler
	err = task_create(NULL, wizfi360_uart_rx_task, NULL, task_gethighestpriority()-2, 1024, "wizfi360_uart_rx_task");
	if( err != 0 ) {
		printf("fail at task_create\r\n");
	} else {
		printf("== UART_WIFI_UART_RX_TASK created\r\n");
	}

	//test time generation
	err = task_create(NULL, test_time_task, NULL, task_gethighestpriority()-1, 256, "test_time_task");
	if( err != 0 ) {
		printf("fail at task_create\r\n");
	} else {
		printf("== TEST_TIME_TASK created\r\n");
	}
}
//EOF

#endif /*(CSOS_SAAL__USE_LIB_wifi_wizfi360 == 1)*/
