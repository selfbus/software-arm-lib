/*
 *  Copyright (c) 2015 Martin Glueck <martin@mangari.org>
 *  Copyright (c) 2020 Stefan Haller
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include "boot_descriptor_block.h"
#include "crc.h"


/* This is the sanity check as described in 26.3.3 of the UM10398 user guide
*  However, this shouldn't matter since we boot into the BL anyways and just it's vector table
*  needs to be correct.
*  This test get's important if we like to bypass the KNX bootloader.
*  Note that the LPC device as a build in BL ROM for UART ISP which is always started first.
*  This ROM BL checks if the vector table is correct to determine if the KNX BL can be started.
*/
#if 0
unsigned int checkVectorTable(unsigned int start)
{
    unsigned int i;
    unsigned int * address;
    unsigned int cs = 0;
    address = (unsigned int *) start;	// Vector table start always at base address, each entry is 4 byte

    for (i = 0; i < 7; i++)				// Checksum is 2's complement of entries 0 through 6
        cs += address[i];
    //if (address[7])

    return (~cs+1);
    return (address[0]);
}
#else
#define checkVectorTable(s) 1
#endif

unsigned int checkApplication(AppDescriptionBlock * block)
{
    if ((block->startAddress < FIRST_SECTOR) || (block->startAddress > LAST_SECTOR)) // we have just 64k of Flash
        return 0;
    if (block->endAddress > LAST_SECTOR)	// we have just 64k of Flash
        return 0;
    if (block->startAddress >= block->endAddress)
        return 0;

    unsigned int crc = crc32(0xFFFFFFFF, (unsigned char *) block->startAddress,
            block->endAddress - block->startAddress);
    if (crc == block->crc)
    {
        return checkVectorTable(block->startAddress);
    }
    return 0;
}

inline unsigned char * getAppVersion(AppDescriptionBlock * block)
{
    return (unsigned char *) (block->appVersionAddress);
}

// Return start address of application in case of valid descriptor block
// otherwise, base address of firmware area, directly behind bootloader
unsigned char * getFWstartAddress(AppDescriptionBlock * block)
{
    if (checkApplication(block))
    	return (unsigned char *) (block->startAddress);
    else
    	return (unsigned char *) FIRST_SECTOR;
}
