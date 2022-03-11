
#include "AAT_device_EXT_FLASH.h"

#if (CSOS_SAAL__USE_LIB_AAT_HW_other == 1)

#include <stdbool.h>
#include <stdint.h>

#include "AAT_driver_spi.h"


#define EXT_FLASH_MAN_ID		0xEF
#define EXT_FLASH_DEV_ID		0x13

static int extFlashWaitReady(void);
static void extFlashWriteEnable(void);
bool extFlashRead(size_t offset, size_t length, uint8_t *buf);
bool extFlashWrite(size_t offset, uint32_t length, const uint8_t *buf);
bool extFlashErase(size_t offset, size_t length);
///////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Wait till previous erase/program operation completes.
 * @return Zero when successful.
 */
static int extFlashWaitReady(void) {
	const uint8_t wbuf[1] = { BLS_CODE_READ_STATUS };
	int ret;
	uint8_t buf;

	for (;;) {
		AAT_spi_write_read_Flash(wbuf, 1, &buf, 1);

		if (!(buf & BLS_STATUS_BIT_BUSY)) {
			break;
		}
	}
	return 0;
}

static void extFlashWriteEnable(void) {
	const uint8_t wbuf[1] = { BLS_CODE_WRITE_ENABLE };

	AAT_spi_write_Flash(wbuf, sizeof(wbuf));

}
///////////////////////////////////////////////////////////////////////////////////////////////
bool extFlashRead(size_t offset, size_t length, uint8_t *buf) {
	uint8_t wbuf[4];

	/* Wait till previous erase/program operation completes */
	int ret = extFlashWaitReady();

	/* SPI is driven with very low frequency (1MHz < 33MHz fR spec)
	 * in this temporary implementation.
	 * and hence it is not necessary to use fast read. */
	wbuf[0] = BLS_CODE_READ;
	wbuf[1] = (offset >> 16) & 0xff;
	wbuf[2] = (offset >> 8) & 0xff;
	wbuf[3] = (offset) & 0xff;

	AAT_spi_write_read_Flash(wbuf, sizeof(wbuf), buf, length);

	return ret;
}

bool extFlashWrite(size_t offset, uint32_t length, const uint8_t *buf) {
	uint8_t wbuf[4];

	while (length > 0) {
		/* Wait till previous erase/program operation completes */
		extFlashWaitReady();

		extFlashWriteEnable();

		size_t ilen;

		ilen = BLS_PROGRAM_PAGE_SIZE - (offset % BLS_PROGRAM_PAGE_SIZE);
		if (length < ilen) {
			ilen = length;
		}

		wbuf[0] = BLS_CODE_PROGRAM;
		wbuf[1] = (offset >> 16) & 0xff;
		wbuf[2] = (offset >> 8) & 0xff;
		wbuf[3] = (offset) & 0xff;

		offset += ilen;
		length -= ilen;

		AAT_spi_write_write_Flash(wbuf, sizeof(wbuf), buf, ilen);
		buf += ilen;
	}

	return true;
}

bool extFlashErase(size_t offset, size_t length) {
	uint8_t wbuf[4];
	size_t i, numsectors;

	wbuf[0] = BLS_CODE_SECTOR_ERASE;

	size_t endoffset = offset + length - 1;
	offset = (offset / BLS_ERASE_SECTOR_SIZE) * BLS_ERASE_SECTOR_SIZE;
	numsectors = (endoffset - offset + BLS_ERASE_SECTOR_SIZE - 1) / BLS_ERASE_SECTOR_SIZE;

	for (i = 0; i < numsectors; i++) {
		extFlashWaitReady();

		extFlashWriteEnable();

		wbuf[1] = (offset >> 16) & 0xff;
		wbuf[2] = (offset >> 8) & 0xff;
		wbuf[3] = offset & 0xff;

		AAT_spi_write_Flash(wbuf, sizeof(wbuf));

		offset += BLS_ERASE_SECTOR_SIZE;
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////
bool AAT_falsh_data_all_erase(void){
	return extFlashErase(0x00000, (4 * 1024));
}

bool AAT_flash_test_flash_write(uint8_t* buf) {
	return extFlashWrite(0x00000, sizeof(AAT_flash_lt_t), buf);
}

bool AAT_flash_test_flash_read(uint8_t* buf) {
	return extFlashRead(0x00000, sizeof(AAT_flath_test_t), (uint8_t*) buf);
}

/////////////////////////////////////////////////////////////////////////
bool AAT_flash_LF_test_data_all(void) {
	return extFlashErase(0x00000, (4 * 1024));
}

bool AAT_flash_LF_test_flash_write(uint8_t *buf, uint8_t len) {

	//extFlashWrite(0x00000, sizeof(uint8_t), &len);

	return extFlashWrite(0x00000 + (4 * len), sizeof(AAT_flash_lt_t), buf);
}

//bool AAT_flash_LF_test_flash_size_read(uint8_t *buf) {
//	return extFlashRead(0x00000, sizeof(uint8_t), buf);
//}
bool AAT_flash_LF_test_flash_read(uint8_t* buf, uint8_t len) {
	return extFlashRead(0x00000 + (4 * len), sizeof(AAT_flash_lt_t), buf);
}
////////////////////////////////////////////////////////////////////////////
bool AAT_flash_LOG_data_all_erase(void){
	//
	return extFlashErase(0x00000, (16 * 1024));
}

bool AAT_flash_LOG_data_write(uint8_t * buf, uint8_t len, uint8_t index){

	return extFlashWrite(0x0000 + (16*index) , len ,buf );
}

bool AAT_flash_LOG_data_read(uint8_t * buf, uint8_t len, uint8_t index){

	return extFlashRead(0x0000 + (16*index), len ,buf );

}
/////////////////////////////////////////////////////////////////////////////

#endif /*(CSOS_SAAL__USE_LIB_AAT_HW_other == 1)*/