/*
 * mem_mapper.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: Deti Fliegl <deti@fliegl.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 *
 */

#include <sblib/platform.h>
#include <sblib/internal/iap.h>
#include <sblib/utils.h>
#include <sblib/mem_mapper.h>
#include <sblib/serial.h>
#include <sblib/ioports.h>

MemMapper:: MemMapper(byte *flashBase=(byte*)0xf000, unsigned int flashSize=4096):flashBase(flashBase), flashSize(flashSize)
{
    allocTable = new byte[FLASH_PAGE_SIZE];
    writeBuf = new byte[FLASH_PAGE_SIZE];
    flashSizePages = flashSize / FLASH_PAGE_SIZE;
    flashBasePage = ((unsigned int)flashBase) / FLASH_PAGE_SIZE;
    lastAllocated = 0; // means: nothing allocated in this run
    writePage = 0;

    for(int i=0; i<FLASH_PAGE_SIZE; i++) {
        allocTable[i] = flashBase[i];
    }
}

bool MemMapper::isValidAddress(int virtAddress)
{
    int virtPage = virtAddress>>8;
    if(virtPage >= FLASH_PAGE_SIZE) {
        return false;
    }
    return true;
}

int MemMapper::writeMem(int virtAddress, byte data)
{
    int virtPage = virtAddress>>8;
#ifdef DEBUG_MEMMAPPER
    serial.print("writeMem:");
    serial.print(virtAddress, HEX, 4);
    serial.print(" Data:");
    serial.print(data, HEX, 2);
#endif

    if(!isValidAddress(virtPage)) {
        return -1;
    }

    byte flashPageNum = allocTable[virtPage]^0xff;

    if(writePage != flashPageNum) {
        doFlash();
        writePage = flashPageNum;
        if(writePage != 0) { // swap flash page into write buffer
            for(int i=0; i < FLASH_PAGE_SIZE; i++) {
                writeBuf[i] = flashBase[i | (writePage<<8)];
            }
        }
    }

    if( flashPageNum == 0) { // not yet allocated in flash memory
#ifdef DEBUG_MEMMAPPER
        serial.print(" allocate ");
#endif
        if( lastAllocated == 0) { // not yet found the highest used entry
            for(int i=0; i < FLASH_PAGE_SIZE; i++) {
                unsigned int entry = allocTable[i]^0xff;
                 if(entry > lastAllocated) {
                    lastAllocated = entry;
                }
            }
        }
        if(lastAllocated == (flashBasePage + flashSizePages - 1)) {
#ifdef DEBUG_MEMMAPPER
            serial.print(" out of memory ");
#endif
            return -1; // we are out of memory
        }
        if(lastAllocated == 0) {  // no pages allocated yet.
            writePage = flashBasePage+1;
        } else {
            lastAllocated++;
            writePage = lastAllocated;
        }
        for(int i=0; i < FLASH_PAGE_SIZE; i++) {
            writeBuf[i] = 0x00;
        }

        allocTable[virtPage] = writePage^0xff;
        allocTableModified = true;
    }
#ifdef DEBUG_MEMMAPPER
    serial.print(" page:");
    serial.print(writePage, HEX, 4);
#endif
    writeBuf[(virtAddress & 0xff)] = data;
    flashMemModified = true;
#ifdef DEBUG_MEMMAPPER
    serial.println("");
#endif
    return 0;
}

int MemMapper::readMem(int virtAddress, byte &data)
{
    int virtPage = virtAddress>>8;
#ifdef DEBUG_MEMMAPPER
    serial.print("readMem:");
    serial.print(virtAddress, HEX, 4);
#endif
    if(!isValidAddress(virtPage)) {
        return -1;
    }

    byte flashPageNum = allocTable[virtPage]^0xff;
#ifdef DEBUG_MEMMAPPER
    serial.print(" page:");
    serial.print(flashPageNum, HEX, 4);
#endif
    if( flashPageNum == 0) {
        data = 0xff;
    } else if( flashPageNum == writePage) {
#ifdef DEBUG_MEMMAPPER
        serial.print(" cache ");
#endif
        data=writeBuf[virtAddress&0xff];
    } else {
#ifdef DEBUG_MEMMAPPER
        serial.print(" flash ");
#endif
        data=((byte*)(flashPageNum<<8))[virtAddress&0xff];
    }
#ifdef DEBUG_MEMMAPPER
    serial.print(" Data:");
    serial.print(data, HEX, 2);
    serial.println("");
#endif
    return 0;
}

int MemMapper::doFlash(void)
{
    int ret = 0;
    if(allocTableModified) {
#ifdef DEBUG_MEMMAPPER
        serial.print(" allocTableModified ");
#endif
        if(iapErasePage(flashBasePage) != IAP_SUCCESS) {
            fatalError();
        }
        if(iapProgram(flashBase, allocTable, FLASH_PAGE_SIZE) != IAP_SUCCESS) {
            fatalError();
        }
        allocTableModified = false;
        ret |= 1;
    }
    if(flashMemModified) {
#ifdef DEBUG_MEMMAPPER
        serial.print(" flashMemModified page: ");
        serial.print(writePage, HEX, 4);
        serial.print(" ");
#endif
        if(iapErasePage(writePage) != IAP_SUCCESS) {
            fatalError();
        }
        if(iapProgram((byte *)(writePage<<8), writeBuf, FLASH_PAGE_SIZE) != IAP_SUCCESS) {
            fatalError();
        }
        flashMemModified = false;
        ret |= 2;
    }
    return ret;
}

