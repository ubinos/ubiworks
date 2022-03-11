/* Instruction codes */

#include "ubinos.h"

#if (CSOS_SAAL__USE_LIB_AAT_HW_other == 1)

#include "stdint.h"
#include "stdio.h"
#include <stdbool.h>

/************************************************/
/**\name	REGISTER ADDRESS DEFINITION       */
/***********************************************/
#define BLS_CODE_PROGRAM          0x02 /**< Page Program */
#define BLS_CODE_READ             0x03 /**< Read Data */
#define BLS_CODE_READ_STATUS      0x05 /**< Read Status Register */
#define BLS_CODE_WRITE_ENABLE     0x06 /**< Write Enable */
#define BLS_CODE_SECTOR_ERASE     0x20 /**< Sector Erase */
#define BLS_CODE_MDID             0x90 /**< Manufacturer Device ID */

#define BLS_CODE_DP               0xB9 /**< Power down */
#define BLS_CODE_RDP              0xAB /**< Power standby */

/* Erase instructions */
#define BLS_CODE_ERASE_4K         0x20 /**< Sector Erase */
#define BLS_CODE_ERASE_32K        0x52
#define BLS_CODE_ERASE_64K        0xD8
#define BLS_CODE_ERASE_ALL        0xC7 /**< Mass Erase */

/* Bitmasks of the status register */
#define BLS_STATUS_SRWD_BM        0x80
#define BLS_STATUS_BP_BM          0x0C
#define BLS_STATUS_WEL_BM         0x02
#define BLS_STATUS_WIP_BM         0x01

#define BLS_STATUS_BIT_BUSY       0x01 /**< Busy bit of the status register */

/* Part specific constants */
//#define BLS_PROGRAM_PAGE_SIZE     256

//for nRF SPI Driver : Size of SPI Write unit is (char = 256 byte)
#define BLS_PROGRAM_PAGE_SIZE     128

#define BLS_ERASE_SECTOR_SIZE     4096
#define BLS_ERASE_BLOCK_SIZE      65536

typedef struct{
	uint8_t AAT_paar_id[4];
}AAT_flath_test_t;

typedef struct{
	uint8_t data[4];
/*	uint8_t rssi;
	uint8_t rssiDB;*/
}AAT_flash_lt_t;

/************************************************/
/**\name	 FUNCTION DEFINITION       */
/***********************************************/
bool AAT_falsh_data_all_erase(void);
bool AAT_flash_test_data_all(void);
bool AAT_flash_test_flash_write(uint8_t* buf) ;
bool AAT_flash_test_flash_read(uint8_t* buf);

bool AAT_flash_LF_test_flash_write(uint8_t *buf, uint8_t len);
bool AAT_flash_LF_test_flash_size_read(uint8_t *buf);
bool AAT_flash_LF_test_flash_read(uint8_t* buf, uint8_t len);

bool AAT_flash_LOG_data_all_erase(void);
bool AAT_flash_LOG_data_write(uint8_t * buf, uint8_t len, uint8_t index);
bool AAT_flash_LOG_data_read(uint8_t * buf, uint8_t len, uint8_t index);

#endif /*(CSOS_SAAL__USE_LIB_AAT_HW_other == 1)*/