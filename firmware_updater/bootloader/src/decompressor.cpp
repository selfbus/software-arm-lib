/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_BOOTLOADER_DECOMPRESSOR Decompressor
 * @ingroup SBLIB_BOOTLOADER
 *
 * @{
 *
 * @file   decompressor.cpp
 * @author Pavel Kriz <https://github.com/pavkriz> Copyright (c) 2019
 * @author Stefan Haller Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#include <string.h>
#include "flash.h"
#include "crc.h"
#include "decompressor.h"
#include "dump.h"

#define CMD_RAW 0
#define CMD_COPY 0b10000000
#define FLAG_LONG 0b01000000
#define ADDR_FROM_ROM 0
#define ADDR_FROM_RAM 0b10000000

Decompressor::Decompressor(AppDescriptionBlock* BaseAddress)
{
    startAddrOfFlash = getFirmwareStartAddress(BaseAddress);
	startAddrOfPageToBeFlashed = startAddrOfFlash;
}

int Decompressor::getLength()
{
	if ((cmdBuffer[0] & FLAG_LONG) == FLAG_LONG)
	{
		return (((cmdBuffer[0] & 0b111111) << 8) | (cmdBuffer[1] & 0xff));
	}
	else
	{
		return (cmdBuffer[0] & 0b111111);
	}
}

bool Decompressor::isCopyFromRam()
{
	if ((cmdBuffer[0] & FLAG_LONG) == FLAG_LONG)
	{
		return ((cmdBuffer[2] & ADDR_FROM_RAM) == ADDR_FROM_RAM);
	}
	else
	{
		return ((cmdBuffer[1] & ADDR_FROM_RAM) == ADDR_FROM_RAM);
	}
}

int Decompressor::getCopyAddress()
{
	if ((cmdBuffer[0] & FLAG_LONG) == FLAG_LONG)
	{
		return (((cmdBuffer[2] & 0b1111111) << 16) | ((cmdBuffer[3] & 0xff) << 8) | (cmdBuffer[4] & 0xff));
	}
	else
	{
		return (((cmdBuffer[1] & 0b1111111) << 16) | ((cmdBuffer[2] & 0xff) << 8) | (cmdBuffer[3] & 0xff));
	}
}

void Decompressor::resetStateMachine()
{
	state = State::EXPECT_COMMAND_BYTE;
}

UDP_State Decompressor::pageCompletedDoFlash()
{
	// backup old page content, flash new content from scratchpad RAM to flash
    UDP_State result = UDP_IAP_SUCCESS;

	// Keep a copy of the last couple of flash pages in a RAM buffer for the differ
	// RAM buffer size is set by REMEMBER_OLD_PAGES_COUNT * FLASH_PAGE_SIZE
	// this is a FIFO buffer, oldest pages gets dropped
	// shift data one flash page forward to make space for new page at the end
	memcpy(oldPages, oldPages + FLASH_PAGE_SIZE, sizeof(oldPages) - FLASH_PAGE_SIZE);
	// add latest (current) page (this may not be whole page when EOF) at the end
	memcpy(oldPages + (FLASH_PAGE_SIZE * (REMEMBER_OLD_PAGES_COUNT-1)), startAddrOfPageToBeFlashed, FLASH_PAGE_SIZE);

	// Check if flash page is identical or if we need to flash it
	d1("Diff - Compare Page ");
	d2(getFlashPageNumberToBeFlashed(), DEC,2);
	if (memcmp(startAddrOfPageToBeFlashed, scratchpad, bytesToFlash) != 0)
	{
		// erase the page to be flashed
		d1(" different, Erase Page");
		//d2(getFlashPageNumberToBeFlashed(), DEC,2);

		result = erasePageRange(getFlashPageNumberToBeFlashed(), getFlashPageNumberToBeFlashed());
		//result = UDP_IAP_SUCCESS; // Dry RUN! for debug

		// proceed to flash the decompressed page stored in the scratchpad RAM
		d1("Diff - Program Page at Address 0x");
		d2ptr(startAddrOfPageToBeFlashed);
		result = executeProgramFlash(startAddrOfPageToBeFlashed, scratchpad, FLASH_PAGE_SIZE);
		//result = UDP_IAP_SUCCESS; // Dry RUN! for debug
	}
	else
	{
	    dline("  equal, skipping!");
	}
	// Equal, skip this page and
	// move to next page
	startAddrOfPageToBeFlashed += FLASH_PAGE_SIZE;
	// reinitialize scratchpad
	bytesToFlash = 0;
	memset(scratchpad, 0, sizeof(scratchpad));
	//d1("result = 0x");
	//d2(result,HEX,2);
	//dline("");
	return (result);
}

void Decompressor::putByte(uint8_t data)
{
    ///\todo Implement buffer overflow protection for cmdBuffer, scratchpad, oldPages

	//System.out.println("Decompressor processing new byte " + (data & 0xff) + ", state=" + state);
	//UART_printf("@ b=%02X s=%02X i=%d", data, state, bytesToFlash);
	switch (state)
	{
		case State::EXPECT_COMMAND_BYTE:
			cmdBuffer[0] = data;
			cmdBufferLength = 1;
			expectedCmdLength = 1;
			if ((data & CMD_COPY) == CMD_COPY)
			{
				expectedCmdLength += 3; // 3 more bytes of source address
				//d1(" CMD_COPY");
			}
			//else
			//{
				//d1(" CMD_RAW");
			//}
			if ((data & FLAG_LONG) == FLAG_LONG)
			{
				//d1(" FLAG_LONG");
				expectedCmdLength += 1; // 1 more byte for longer length
			}
			if (expectedCmdLength > 1)
			{
				state = State::EXPECT_COMMAND_PARAMS;
			}
			else
			{
				state = State::EXPECT_RAW_DATA;
				rawLength = 0;
			}
			break;
		case State::EXPECT_COMMAND_PARAMS:
			//d1(" params");
			cmdBuffer[cmdBufferLength++] = data;
			if (cmdBufferLength >= expectedCmdLength)
			{
				// we have all params of the command
				if ((cmdBuffer[0] & CMD_COPY) == CMD_COPY)
				{
					// perform copy
					if (isCopyFromRam())
					{
						//UART_printf(" DO COPY FROM RAM l=%d sa=%08X", getLength(), getCopyAddress());
						d1("\n\rCopy from RAM with length ");
						d2(getLength(),DEC,4);
						d1(", address offset 0x")
						d2(getCopyAddress(),HEX,4);
						d1("\n\r");
						//System.out.println("COPY FROM RAM index=" + scratchpadIndex + " length=" + getLength() + " from addr=" + getCopyAddress());
						//System::arraycopy(oldPagesRam->getOldBinData(), getCopyAddress(), scratchpad, scratchpadIndex, getLength());
						memcpy(scratchpad + bytesToFlash, oldPages + getCopyAddress(), getLength());
						//for (int i = 0; i < getLength(); i++)
						//{
						//	if (i % 16 == 0) {
						//		d1("\n\r ");
						//	}
							//UART_printf("%02X ", oldPages[getCopyAddress() + i]);
							//d2(oldPages[getCopyAddress() + i],HEX,4);
							//d1(" ");
						//}
					}
					else
					{
						//UART_printf(" DO COPY FROM ROM l=%d sa=%08X", getLength(), getCopyAddress());
						d1("\n\rCopy from ROM with length ");
						d2(getLength(),DEC,4);
						d1(", address offset 0x")
						d2(getCopyAddress(),HEX,4);
						d1("\n\r");
						//d1(" FlashStart ");
						//d2(startAddrOfFlash,HEX,6)
						//System.out.println("COPY FROM ROM index=" + scratchpadIndex + " length=" + getLength() + " from addr=" + getCopyAddress());
						//System.out.println(rom.getBinData()[getCopyAddress()] & 0xff);
						//System.out.println(rom.getBinData()[getCopyAddress()+1] & 0xff);
						//System.out.println(rom.getBinData()[getCopyAddress()+2] & 0xff);
						//System::arraycopy(rom->getBinData(), getCopyAddress(), scratchpad, scratchpadIndex, getLength());
						memcpy(scratchpad + bytesToFlash, startAddrOfFlash + getCopyAddress(), getLength());
						//for (int i = 0; i < getLength(); i++)
						//{
						//	if (i % 16 == 0) {
						//		d1("\n\r ");
						//	}
							//UART_printf("%02X ", startAddrOfFlash[getCopyAddress() + i]);
							//d2(startAddrOfFlash[getCopyAddress() + i],HEX,2);
							//d1(" ");
						//}
					}
					bytesToFlash += getLength();
					// and finish command
					resetStateMachine();
				}
				else
				{
					// next, read raw data
					state = State::EXPECT_RAW_DATA;
					rawLength = 0;
				}
			} // else expect more params of the command
			break;
		case State::EXPECT_RAW_DATA:
			//d1(" raw");
			// store data read to scratchpad
			scratchpad[bytesToFlash++] = data;
			rawLength++;
			if (rawLength >= getLength())
			{
				// we have all RAW data, reset state machine
				resetStateMachine();
			}
	}
	//UART_printf("\n\r");
}

uint32_t Decompressor::getCrc32() {
	uint32_t ccrc = crc32(0xFFFFFFFF, scratchpad, bytesToFlash);

	//UART_printf("# CRC req scratchpad content l=%d", bytesToFlash);
	for (int i = 0; i < bytesToFlash; i++) {
		if (i % 16 == 0) {
			//UART_printf("\n\r  ");
		}
		//UART_printf("%02X ", scratchpad[i]);
	}
	//UART_printf("\n\r");


	return (ccrc);
}

uint8_t * Decompressor::getStartAddrOfPageToBeFlashed() {
	return (startAddrOfPageToBeFlashed);
}

uint32_t Decompressor::getBytesCountToBeFlashed() {
	return (bytesToFlash);
}

uint8_t Decompressor::getFlashPageNumberToBeFlashed() {
	return ((uint8_t)((uint32_t)startAddrOfPageToBeFlashed / FLASH_PAGE_SIZE));
}

/** @}*/
