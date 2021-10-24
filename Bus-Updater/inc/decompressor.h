/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_BOOTLOADER_DECOMPRESSOR Decompressor
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Differential decompressor
 * @details This is an extended version of the knxduino differential updater from
 *          https://github.com/pavkriz/knxduino/tree/master/bootloader/src
 *
 * @{
 *
 * @file   decompressor.h
 * @author Pavel Kriz <https://github.com/pavkriz> Copyright (c) 2019
 * @author Stefan Haller Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#ifndef _DECOMPRESSOR_H_
#define _DECOMPRESSOR_H_

#include <stdint.h>
#include "boot_descriptor_block.h"
// #include "../src/platform.h"

#define REMEMBER_OLD_PAGES_COUNT 2	// There seems to be a RAM leak/overflow somewhere, 2 seems to work for the moment!


/**
 * Apply differential stream and produce one page to be flashed
 * (based on differential stream, original ROM content, and RAM buffer to store some latest ROM pages already flashed)
 */
class Decompressor
{

	private:
		enum class State
		{
			EXPECT_COMMAND_BYTE,
			EXPECT_COMMAND_PARAMS,
			EXPECT_RAW_DATA
		};

	private:
		uint8_t cmdBuffer[5] = {0};
		int expectedCmdLength = 0;
		int cmdBufferLength = 0;
		uint8_t scratchpad[FLASH_PAGE_SIZE] = {0};
		uint8_t oldPages[FLASH_PAGE_SIZE * REMEMBER_OLD_PAGES_COUNT] = {0};
		int bytesToFlash = 0;
		int rawLength = 0;
		State state = State::EXPECT_COMMAND_BYTE;
		uint8_t* startAddrOfPageToBeFlashed = 0;
		uint8_t* startAddrOfFlash = 0;

	public:
		Decompressor(AppDescriptionBlock* BaseAddress);

	private:
		int getLength();

		bool isCopyFromRam();

		int getCopyAddress();

		void resetStateMachine();

	public:

		void putByte(uint8_t data);

		int pageCompletedDoFlash();

		uint32_t getCrc32();

		uint32_t getStartAddrOfPageToBeFlashed();

		uint32_t getBytesCountToBeFlashed();

		uint8_t getFlashPageNumberToBeFlashed();

};

#endif

/** @}*/
