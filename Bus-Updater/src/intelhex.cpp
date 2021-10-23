/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_INTEL_HEX Intel(R) hex file format
 * @ingroup SBLIB_BOOTLOADER
 *
 * @{
 *
 * @file   intelhex.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include "intelhex.h"

#define HEX_STARTCODE ":"
#define HEX_DATA_RECORD (00)
#define HEX_END_OF_FILE "00000001FF"
#define HEX_START_SEGMENT_RECORD "04000003"
#define HEX_START_SEGMENT_RECORD_CHKSM (0x07)  // 04 + 00 + 00 + 03

// short example hex file with 16 byte of data (actual hex file has no spaces)
// :10 7000 00 001F001049710000CD710000A5C20000 F2
// :04 000003 0000 7149 3F
// :00000001FF

void dumpToSerialinIntelHex(Serial* serialPort, unsigned char* data, unsigned int count, unsigned int bytesPerLine)
{
    unsigned char x;
    unsigned int checkSum;
    unsigned int address;
    unsigned int i = 0;
    unsigned int bytesToWrite = bytesPerLine;
    byte hexType = HEX_DATA_RECORD;
    unsigned int startAddress = (unsigned int)data;

    while (i < count)
    {
        bytesToWrite = bytesPerLine;
        // write line header/command, excluded from checksum
        serialPort->print(HEX_STARTCODE);
        checkSum = 0;

        if ((count - i) < bytesPerLine)
        {
            bytesToWrite = count - i;
        }

        // # of bytes
        checkSum += bytesToWrite;
        serialPort->print(bytesToWrite, HEX, 2);

        // address
        address = (unsigned int)data;
        checkSum += ((address >> 8) & 0xff) + (address & 0xff);
        serialPort->print(address, HEX, 4);

        // type/command
        serialPort->print(hexType, HEX, 2);
        checkSum += hexType;

        // write bytes
        for (unsigned int j = 0; j < bytesToWrite; j++)
        {
            x = *data++;
            serialPort->print(x, HEX, 2);
            checkSum += x;
            i++;
        }
        checkSum &= 0xff;
        checkSum = ((byte)(checkSum ^ 0xff) + 1) & 0xff;
        // write line checksum
        serialPort->println(checkSum, HEX, 2);
    }

    // Start segment Address record
    serialPort->print(HEX_STARTCODE);
    // "04000003"
    serialPort->print(HEX_START_SEGMENT_RECORD);
    checkSum = HEX_START_SEGMENT_RECORD_CHKSM;

    // miss use of Segment to send start address
    checkSum += ((startAddress >> 8) & 0xff) + (startAddress & 0xff);
    serialPort->print(startAddress, HEX, 4);

    // miss use of Offset to send end address
    address = (unsigned int)data;
    checkSum += ((address >> 8) & 0xff) + (address & 0xff);
    serialPort->print(address, HEX, 4);
    checkSum &= 0xff;
    checkSum = ((byte)(checkSum ^ 0xff) + 1) & 0xff;

    // write line checksum
    serialPort->println(checkSum, HEX, 2);
    // end of file record
    serialPort->print(HEX_STARTCODE);
    serialPort->println(HEX_END_OF_FILE);
}

/** @}*/
