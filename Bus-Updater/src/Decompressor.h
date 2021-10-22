/* This is an extended version of the knxduino diff updater from
 * https://github.com/pavkriz/knxduino/tree/master/bootloader/src
 */

#ifndef _DECOMPRESSOR_H_
#define _DECOMPRESSOR_H_

#include <stdint.h>
#include "boot_descriptor_block.h"
// #include "../src/platform.h"

#define REMEMBER_OLD_PAGES_COUNT 2	// There seems to be a RAM leak/overflow somewhere, 2 seems to work for the moment!


/**
 * Apply diff stream and produce one page to be flashed
 * (based on diff stream, original ROM content, and RAM buffer to store some latest ROM pages already flashed)
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
