/*
 * RT_uart_433_3.c
 *
 *  Created on: 2021. 1. 21.
 *      Author: HMKANG
 */


/*
 * To see details of UART Driver,
 * https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v17.0.2%2Fhardware_driver_uart.html
 *
 */

#include <ubinos/bsp.h>
#include <ubinos/bsp/arch.h>
#include <ubinos/bsp_ubik.h>

#if (CSOS_SAAL__USE_LIB_433 == 1)

#include "sh_uart_433_module.h"
#include "twi_sensor_module.h"
#include "ubinos.h"
#include "assert.h"
#include "math.h"

#include "nrf_drv_uart.h"
#include "bsp.h"

#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"

#include "hw_config.h"
#include "../lib_bluetooth_csos/ble_stack.h"
//#include "../lib_bluetooth_csos/LAP_main.h"
#include "../lib_bluetooth_csos/LAP_api.h"




static uart433_t _g_uart433;


static bool _uart_receive_flag = true;

static msgq_pt 	uart433_module_task_msgq_event;

static nrf_drv_uart_t _g_uart433_uart_instance = NRF_DRV_UART_INSTANCE(1);

uint8_t _g_uart433_data_read_buffer[UART433_READ_DATA_BUF_SIZE];
uint16_t _g_uart433_data_read_buffer_idx = 0;

//room, sensor, sensor_func, device_ID
sensor_433_profile _g_sensor_433_profile_arr[UART433_SENSOR_NUM_MAX];
static uint8_t _g_sensor_433_profile_index;
static bool _sensor_433_profile_init_flag = false;

/////////////////////////////////////////////////
/////////////////////////////////////////////////

void set_uart433_receive_flag_init() {
	_uart_receive_flag = true;
}
void uart433_receive_flag_setting(bool set) {
	_uart_receive_flag = set;
}
bool is_uart_receive_flag_set() {

	return _uart_receive_flag;
}

void set_uart_433_profile_flag_init() {
	_sensor_433_profile_init_flag = true;
}
void uart_433_profile_flag_setting(bool set) {
	_sensor_433_profile_init_flag = set;
}
bool is_uart_profile_flag_set() {

	return _sensor_433_profile_init_flag;
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////

static uint32_t rt_uart_433_sensor_profile_arr_init() {
	for(int i=0; i<UART433_SENSOR_NUM_MAX; i++) {
		_g_sensor_433_profile_arr[i].room = 0;
		_g_sensor_433_profile_arr[i].sensor = 0;
		_g_sensor_433_profile_arr[i].sensor_func = 0;
		_g_sensor_433_profile_arr[i].device_ID = UART433_SENSOR_PROFILE_DEVICE_ID_DEFAULT;
	}
	_g_sensor_433_profile_index = 0;
	return NRF_SUCCESS;
}

static uint32_t rt_uart_is_device_new(uint32_t device_id) {
	for(int i=0; i<UART433_SENSOR_NUM_MAX; i++) {
		if( device_id == _g_sensor_433_profile_arr[i].device_ID ) {
			return 0;
		}
	}
	return 1;
}

static uint32_t rt_uart_register_433_sensor(uint32_t device_id) {

	if( rt_uart_is_device_new(device_id) ) {
		_g_sensor_433_profile_arr[_g_sensor_433_profile_index++].device_ID = device_id;
		printf("New Device registerd [%ld]\r\n", device_id);
	} else {
		printf("Device already registerd [%ld]\r\n", device_id);
	}


	return NRF_SUCCESS;
}

static uint32_t rt_uart433_device_register_handler(uint8_t * global_buffer, uint16_t buffer_length) {
	char temp_buf[128];
	uint32_t device_id;
	for(int i=0; i<buffer_length; i++) {
		temp_buf[i] = (char)global_buffer[i];
	}
	device_id = atoi(temp_buf);

	rt_uart_register_433_sensor(device_id);

	return NRF_SUCCESS;
}


/////////////////////////////////////////////////
/////////////////////////////////////////////////

static void uart433_data_receive_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
	if(nrf_drv_gpiote_in_is_set(pin) == false) {
		//gpio interrupt from atmel7810 uart433 module
//		uart433_event_send(UART433_MODULE_DATA_RECEIVED_INT, 0, NULL);
//		LED_toggle(PIN_LED1);
	}
}

static void uart433_data_receive_gpio_pin_init(nrf_drv_gpiote_evt_handler_t handler) {
	int r = NRF_SUCCESS;

	nrf_gpio_cfg_output(UART_433_RF_POWER_CTRL);

	nrf_gpio_pin_set(UART_433_RF_POWER_CTRL);
//	nrf_gpio_pin_clear(UART_433_RF_POWER_CTRL);

	r = nrf_drv_gpiote_init();
	if( r != 0 ) {
		printf("nrf drv gpiote init error : %d\r\n", r);
		assert(r == 0);
	}

	nrf_drv_gpiote_in_config_t in_config;
	in_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
	in_config.pull = NRF_GPIO_PIN_PULLUP;
	in_config.is_watcher = false;
	in_config.hi_accuracy = false;
	in_config.skip_gpio_setup = false;


	r = nrf_drv_gpiote_in_init(UART_DATA_SEND_ENABLE_PIN, &in_config,
			(nrf_drv_gpiote_evt_handler_t) uart433_data_receive_event_handler);
	if( r != 0 ) {
		printf("nrf drv gpiote in init error : %d\r\n", r);
		assert(r == 0);
	}
	nrf_drv_gpiote_in_event_enable(UART_DATA_SEND_ENABLE_PIN, true);
	//0406 - should change from [In Event] to [Out Signal] mode

//	LED_init();
}

static void uart433_io_event_handler(nrf_drv_uart_event_t * p_event, void * p_context) {

	uint8_t * buf;
//	uint8_t * buf_throw;
	uint32_t len;

	nrf_drv_uart_t *uart = &_g_uart433_uart_instance;

//	cbuf_pt rbuf = _g_uart433.io_read_buf;
//	cbuf_pt wbuf = _g_uart433.io_write_buf;
//	sem_pt rsem = _g_uart433.io_read_sem;
//	sem_pt wsem = _g_uart433.io_write_sem;
	char tmp_buf;
	
//	uint32_t need_signal = 0;

	switch(p_event->type) {
		case NRF_DRV_UART_EVT_ERROR:
		{
//			ret_code_t r;
			nrf_drv_uart_errorsrc_get(uart);
			break;
		}
		case NRF_DRV_UART_EVT_RX_DONE:

			//0415 전역 변수를 이용해서 출력 가능한 부분
			//cbuf.h 를 이용해 순환 큐에 값을 집어 넣어서 하기
			buf = p_event->data.rxtx.p_data;
			len = p_event->data.rxtx.bytes;
			tmp_buf = *buf;
			_g_uart433_data_read_buffer[_g_uart433_data_read_buffer_idx] = *buf;
			_g_uart433_data_read_buffer_idx++;
			nrf_drv_uart_rx(uart, buf, len);
			//nrf_drv_uart_rx의 의미가 정확히 무엇인지 포럼이나 선배님께 물어보기
			//이벤트 핸들러에서 값이 올라오는데 굳이 rx 함수를 호출해야 하는 이유? rx를 쓰면 한 byte씩 뒤로 밀리는 이유?
			if(tmp_buf == '\r') {

				uart433_event_send(UART433_MODULE_DATA_RECEIVE_DONE, 0, NULL);

				break;

			}
			break;

		case NRF_DRV_UART_EVT_TX_DONE:
//			if( p_event->data.rxtx.bytes > 0 ) {
//				cbuf_read(wbuf, NULL, p_event->data.rxtx.bytes, NULL);
//			}
//
//			if( cbuf_get_len(wbuf) > 0 ) {
//				buf = cbuf_get_head_addr(wbuf);
//				len = 1;
//				nrf_drv_uart_tx(uart, buf, len);
//			}
//			else {
//				_g_uart433.tx_busy = 0;
//				sem_give(wsem);
//			}

			break;

		default:
			break;
	}
}

static uart433_err_t uart433_io_init(uart433_t * uart433) {
	uart433_err_t uart433_3_err;
	ubi_err_t ubi_err;
	ret_code_t nrf_err;
	uint32_t r;

	uint8_t data;
	uint32_t len;

	uart433->rx_overflow_count = 0;
	uart433->tx_busy = 0;
	ubi_err = cbuf_create(&uart433->io_read_buf, UART433_READ_BUF_SIZE);
	assert(ubi_err == UBI_ERR_OK);
	ubi_err = cbuf_create(&uart433->io_write_buf, UART433_WRITE_BUF_SIZE);
	assert(ubi_err == UBI_ERR_OK);
	r = mutex_create(&uart433->io_mutex);
	assert(r == 0);
	r = semb_create(&uart433->io_read_sem);
	assert(r == 0);
	r = semb_create(&uart433->io_write_sem);
	assert(r == 0);

	nrf_drv_uart_config_t config;

    config.pseltxd = NRF_GPIO_PIN_MAP(0,10);
    config.pselrxd = NRF_GPIO_PIN_MAP(0,9);
    config.pselcts = CTS_PIN_NUMBER;
    config.pselrts = RTS_PIN_NUMBER;
    config.p_context = NULL;
    config.hwfc = NRF_UART_HWFC_DISABLED;
    config.parity = NRF_UART_PARITY_EXCLUDED;
    config.baudrate = NRF_UART_BAUDRATE_115200;
    config.interrupt_priority = NVIC_PRIO_LOWEST;

#if defined(NRF_DRV_UART_WITH_UARTE) && defined(NRF_DRV_UART_WITH_UART)
    config.use_easy_dma = true;
#endif

    nrf_err = nrf_drv_uart_init(&_g_uart433_uart_instance, &config, uart433_io_event_handler);
    assert(nrf_err == NRF_SUCCESS);

    len = 1;
    r = nrf_drv_uart_rx(&_g_uart433_uart_instance, &data, len);	//1byte를 받아야 제대로 init이 된다. 왜그런지는 나중에 선배님께 여쭤보자.

    uart433_3_err = UART433_ERR_OK;

    return uart433_3_err;
}

static uart433_err_t uart433_init(uart433_t * uart433) {

	uint32_t err;
	assert(uart433->cmd_mutex == NULL);

	do {
		err = mutex_create(&uart433->cmd_mutex);
		if(err != 0) {
			err = UART433_ERR_ERROR;
			break;
		}

		err = uart433_io_init(uart433);
		if( err != UART433_ERR_OK) {
			break;
		}
		err = UART433_ERR_OK;
		break;
	} while(1);

	if(err != UART433_ERR_OK) {
		if(uart433->cmd_mutex != NULL) {
			mutex_delete(&(uart433->cmd_mutex));
		}
	}

	return err;
}

static void uart433_module_task(void * arg) {
	int r;
	uart433_err_t uart_err = 0;
	uart433_msgq_event_t uart433_evt_msg;

	ble_stack_init_wait();

	task_sleepms(500);


	//uint8_t byte = '\r';
	for(;;) {
		if(nrf_drv_gpiote_in_is_set(UART_DATA_SEND_ENABLE_PIN) == false) {
//			LED_toggle(PIN_LED4);
		}
		r = msgq_receive(uart433_module_task_msgq_event, (unsigned char*) &uart433_evt_msg);
		if( r != 0 ) {
			printf("fail at msgq receive\r\n");
		} else {
			switch( uart433_evt_msg.event ) {

			case UART433_MODULE_DATA_RECEIVE_INT:
				if(uart_err != UART433_ERR_OK) {
					printf("UART IO READ ERROR\r\n");
					assert(uart_err == 0);
				}
				break;

			case UART433_MODULE_DATA_RECEIVE_DONE:
				rt_uart433_device_register_handler(_g_uart433_data_read_buffer, _g_uart433_data_read_buffer_idx);
				_g_uart433_data_read_buffer_idx = 0;

				break;

			case UART433_MODULE_SENSOR_PAIRING_MODE:

				break;

			case UART433_MODULE_SEND_CMD_TO_OTHER_HUB:
				//0426
				//현재는 아두이노와 nrf간의 동작 전압이 똑같지 않아서 NRF->아두이노 는 전달이 안되는 상황


				break;


			default:
				break;
			} //swtich end
		} //msgq_end

		task_sleepms(500);
	} // end of uart433_module_task for(;;)
}

void uart433_module_task_init(void) {
	int r;

	r = uart433_init(&_g_uart433);
	if ( r != 0 ) {
		printf("uart433_init error : %d\n\r", r);
		assert(r == 0);
	}
	uart433_data_receive_gpio_pin_init(uart433_data_receive_event_handler);
	set_uart433_receive_flag_init();
	rt_uart_433_sensor_profile_arr_init();

	r = msgq_create(&uart433_module_task_msgq_event, sizeof(uart433_msgq_event_t), 20);
	if( 0 != r ) {
		printf("fail at msgq_create\r\n");
		assert(r == 0);
	}

	r = task_create(NULL, uart433_module_task, NULL, task_getmiddlepriority(), 256, "uart433_module_task");
	if( 0 != r ) {
		printf("fail at task_create\r\n");
	} else {
		printf("== UART_MODULE_TASK created\r\n");
	}
}

int uart433_event_send(uint8_t evt, uint8_t state, uint8_t *msg) {
	uart433_msgq_event_t tmp_uart433_msgq_event;

	tmp_uart433_msgq_event.event = evt;
	tmp_uart433_msgq_event.status = state;
	tmp_uart433_msgq_event.msg = msg;

	return msgq_send(uart433_module_task_msgq_event, (unsigned char*) &tmp_uart433_msgq_event);
}



uint32_t u_atoi(char * arr, uint32_t len) {
	uint32_t data = 0;
	uint8_t temp;
	for(int i=0; i<len; i++) {
		temp = (uint8_t)arr[i];
		data = data << 8;
		data = data + temp;
	}

	return data;
}


#endif /* (CSOS_SAAL__USE_LIB_433 == 1) */













//EOF
