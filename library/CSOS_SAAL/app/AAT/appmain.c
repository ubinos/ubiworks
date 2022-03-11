#include <lib_wifi_wizfi360/wiz360_uart_wifi_module.h>
#include <ubinos.h>

#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
*/

#include "lib_bluetooth_csos/ble_process.h"
#include "lib_bluetooth_csos/ble_profile.h"
#include "lib_bluetooth_csos/ble_stack.h"
#include "lib_bluetooth_csos/LAP_api.h"
#include "LAP_main.h"
#include "AAT_main.h"

int appmain(int argc, char *argv[]) {

//	Set Pin9, Pin10 NFC ANT -> GPIO
//	Disable NFC function
	if ((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk)
			== (UICR_NFCPINS_PROTECT_NFC << UICR_NFCPINS_PROTECT_Pos)) {
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
		}
		NRF_UICR->NFCPINS &= ~UICR_NFCPINS_PROTECT_Msk;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
		}
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
		}
		NVIC_SystemReset();
	}

	printf("\r\n\n\n");
	printf("================================================================================\r\n");
	printf("SMART_HUB_BOARD_1.0 (build time: %s %s)\r\n", __TIME__, __DATE__);
	printf("================================================================================\r\n");
	printf("\r\n\n");

	BLE_stack_task_init();

	BLE_process_task_init();

	LAP_main_task_init();

	AAT_main_task_init();

	ubik_comp_start();

	return 0;
}

