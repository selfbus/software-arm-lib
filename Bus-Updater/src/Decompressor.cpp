/* This is an extended version of the knxduino diff updater from
 * https://github.com/pavkriz/knxduino/tree/master/bootloader/src
 */


#include "Decompressor.h"

#include <string.h>
#include "crc.h"
#include <sblib/internal/iap.h>

#ifdef DUMP_TELEGRAMS_LVL1
#include <sblib/serial.h>
#define d1(x) {serial.print(x);}
#define d2(u,v,w) {serial.print(u,v,w);}
#else
#define d1(x)
#define d2(u,v,w)
#endif


#define CMD_RAW 0
#define CMD_COPY 0b10000000
#define FLAG_LONG 0b01000000
#define ADDR_FROM_ROM 0
#define ADDR_FROM_RAM 0b10000000

Decompressor::Decompressor(AppDescriptionBlock* BaseAddress)
{
	startAddrOfFlash = getFWstartAddress(BaseAddress);
	startAddrOfPageToBeFlashed = getFWstartAddress(BaseAddress);

	//startAddrOfFlash = (uint8_t*)startAddrOfFirstPageToBeFlashed;
	//startAddrOfPageToBeFlashed = (uint8_t*)startAddrOfFirstPageToBeFlashed;
}

int Decompressor::getLength()
{
	if ((cmdBuffer[0] & FLAG_LONG) == FLAG_LONG)
	{
		return ((cmdBuffer[0] & 0b111111) << 8) | (cmdBuffer[1] & 0xff);
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
		return (cmdBuffer[2] & ADDR_FROM_RAM) == ADDR_FROM_RAM;
	}
	else
	{
		return (cmdBuffer[1] & ADDR_FROM_RAM) == ADDR_FROM_RAM;
	}
}

int Decompressor::getCopyAddress()
{
	if ((cmdBuffer[0] & FLAG_LONG) == FLAG_LONG)
	{
		return ((cmdBuffer[2] & 0b1111111) << 16) | ((cmdBuffer[3] & 0xff) << 8) | (cmdBuffer[4] & 0xff);
	}
	else
	{
		return ((cmdBuffer[1] & 0b1111111) << 16) | ((cmdBuffer[2] & 0xff) << 8) | (cmdBuffer[3] & 0xff);
	}
}

void Decompressor::resetStateMachine()
{
	state = State::EXPECT_COMMAND_BYTE;
}

int Decompressor::pageCompletedDoFlash()
{
	// backup old page content, flash new content from scratchpad RAM to flash
	static unsigned int lastError = 0;

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
	lastError = memcmp(startAddrOfPageToBeFlashed, scratchpad, bytesToFlash);
	if (lastError)
	{
		// erase the page to be flashed
		d1(" different, Erase Page");
		//d2(getFlashPageNumberToBeFlashed(), DEC,2);

		lastError = iapErasePage(getFlashPageNumberToBeFlashed());
		//lastError = IAP_SUCCESS; // Dry RUN! for debug
		if (lastError)
		{
			d1(" Failed!\n\r")
			return lastError;
		}
		else
			d1(" OK\n\r");

		// proceed to flash the decompressed page stored in the scratchpad RAM
		d1("Diff - Program Page at Address 0x");
		d2((unsigned int)startAddrOfPageToBeFlashed, HEX,4);
		lastError = iapProgram(startAddrOfPageToBeFlashed, scratchpad, FLASH_PAGE_SIZE);
		//lastError = IAP_SUCCESS; // Dry RUN! for debug
		if(lastError)
		{
			d1(" Failed!\n\r");
		}
		else
		{
			d1(" OK\n\r");
		}
	}
	else
	{
		d1("  equal, skipping!\n\r");
	}
	// Equal, skip this page and
	// move to next page
	startAddrOfPageToBeFlashed += FLASH_PAGE_SIZE;
	// reinitialize scratchpad
	bytesToFlash = 0;
	memset(scratchpad, 0, sizeof scratchpad);
	//d1("LAST_ERROR = 0x");
	//d2(lastError,HEX,2);
	//d1("\n\r");
	return lastError;
}

void Decompressor::putByte(uint8_t data)
{
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
	//UART_printf("\n");
}

uint32_t Decompressor::getCrc32() {
	uint32_t ccrc = crc32(0xFFFFFFFF, scratchpad, bytesToFlash);

	//UART_printf("# CRC req scratchpad content l=%d", bytesToFlash);
	for (int i = 0; i < bytesToFlash; i++) {
		if (i % 16 == 0) {
			//UART_printf("\n  ");
		}
		//UART_printf("%02X ", scratchpad[i]);
	}
	//UART_printf("\n");


	return ccrc;
}

uint32_t Decompressor::getStartAddrOfPageToBeFlashed() {
	return (uint32_t)startAddrOfPageToBeFlashed;
}

uint32_t Decompressor::getBytesCountToBeFlashed() {
	return bytesToFlash;
}

uint8_t Decompressor::getFlashPageNumberToBeFlashed() {
	return (uint8_t)((uint32_t)startAddrOfPageToBeFlashed / FLASH_PAGE_SIZE);
}
