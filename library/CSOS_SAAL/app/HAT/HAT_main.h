/*
 * HAT_main.h
 *
 *  Created on: 2020. 06. 11.
 *      Author: YJPark
 */

#ifndef APP_HAT_MAIN_H_
#define APP_HAT_MAIN_H_

#include <ubinos.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>


enum{
	HAT_EVT_BUTTON1 = 0,

	HAT_APP_ADV_DUMMY_EVT,
	HAT_UART_REV_EVT,
	HAT_UART_ERR_EVT,

	HAT_SAADC_LIMIT_BOUND,		// TMR - PPI - SAADC 에서 전류 변화가 임계값을 넘었음을 나타내는 이벤트
	HAT_SAADC_REQUEST,			// 외부의 요청에 의해 가전의 전원 정보를 보내야 할 때
	HAT_CALIB_REQUEST,			// 캘리브 리퀘스트를 받을 때
	HAT_CRNT_LVL_CHANGE,		// 커런트 레벨이 크게 바뀔 때
	HAT_LVL_ACK,				// 가전 커런트 레벨 전송 후 ACK 신호가 왔을 때
	HAT_REPLY_TMR_EVT,

	HAT_CALIB_DONE,				// 캘리브 완료
	HAT_CALIB_CHECK_WAIT_SUCCESS,// 전원 대기 시간 안에 전원 상승 확인
	HAT_CALIB_CHECK_FAIL,		// 캘리브 이후 체크 시퀀스 실패 시
	HAT_CALIB_CHECK_SUCCES,		// 캘리브 이후 체크 시퀀스 성공

	HAT_APPLIANCE_ID_SET,		// 가전 id 설정
};

enum{
	HAT_DEFAULT_ST
};

typedef struct
{
	uint8_t event;
	uint8_t status;
	uint8_t* msg;
}HATEvt_msgt;


uint8_t Appliance_ID;
uint8_t ble_connect_flag_for_main;

void HAT_main_task_init(void);
int HAT_event_send(uint8_t evt, uint8_t state, uint8_t* msg);

#endif /* APP_HAT_MAIN_H_ */
