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
#include <string.h>

MemMapper::MemMapper(byte *flashBase = (byte*) 0xf000, unsigned int flashSize =
        4096) :
        flashBase(flashBase), flashSize(flashSize)
{
    allocTable = new byte[FLASH_PAGE_SIZE];
    writeBuf = new byte[FLASH_PAGE_SIZE];
    flashSizePages = flashSize / FLASH_PAGE_SIZE;
    flashBasePage = ((unsigned int) flashBase) / FLASH_PAGE_SIZE;
    lastAllocated = 0; // means: nothing allocated in this run
    writePage = 0;

    memcpy(allocTable, flashBase, FLASH_PAGE_SIZE);
}

int MemMapper::doFlash(void)
{
    int ret = 0;
    if (allocTableModified)
    {
        if (iapErasePage(flashBasePage) != IAP_SUCCESS)
        {
            fatalError();
        }
        if (iapProgram(flashBase, allocTable, FLASH_PAGE_SIZE) != IAP_SUCCESS)
        {
            fatalError();
        }
        allocTableModified = false;
        ret |= 1;
    }
    if (flashMemModified)
    {
        if (iapErasePage(writePage) != IAP_SUCCESS)
        {
            fatalError();
        }
        if (iapProgram((byte *) (writePage << 8), writeBuf, FLASH_PAGE_SIZE)
                != IAP_SUCCESS)
        {
            fatalError();
        }
        flashMemModified = false;
        ret |= 2;
    }
    return ret;
}

bool MemMapper::isValidAddress(int virtAddress)
{
    int virtPage = virtAddress >> 8;
    if (virtPage >= FLASH_PAGE_SIZE)
    {
        return false;
    }
    return true;
}

int MemMapper::writeMem(int virtAddress, byte data)
{
    int virtPage = virtAddress >> 8;

    if (!isValidAddress(virtPage))
    {
        return INVALID_ADDRESS;
    }

    byte flashPageNum = allocTable[virtPage] ^ 0xff;

    if (writePage != flashPageNum)
    {
        doFlash();
        writePage = flashPageNum;
        if (writePage != 0)
        { // swap flash page into write buffer
            memcpy(writeBuf, flashBase + (writePage << 8), FLASH_PAGE_SIZE);
        }
    }

    if (flashPageNum == 0)
    { // not yet allocated in flash memory
        if (lastAllocated == 0)
        { // not yet found the highest used entry
            for (int i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                unsigned int entry = allocTable[i] ^ 0xff;
                if (entry > lastAllocated)
                {
                    lastAllocated = entry;
                }
            }
        }
        if (lastAllocated == (flashBasePage + flashSizePages - 1))
        {
            return OUT_OF_MEMORY; // we are out of memory
        }
        if (lastAllocated == 0)
        {  // no pages allocated yet.
            writePage = flashBasePage + 1;
        } else
        {
            lastAllocated++;
            writePage = lastAllocated;
        }
        memset(writeBuf, 0, FLASH_PAGE_SIZE);

        allocTable[virtPage] = writePage ^ 0xff;
        allocTableModified = true;
    }
    writeBuf[(virtAddress & 0xff)] = data;
    flashMemModified = true;

    return 0;
}

int MemMapper::readMem(int virtAddress, byte &data)
{
    int virtPage = virtAddress >> 8;

    if (!isValidAddress(virtPage))
    {
        return INVALID_ADDRESS;
    }

    byte flashPageNum = allocTable[virtPage] ^ 0xff;

    if (flashPageNum == 0)
    {
        data = 0x00;
        return NOT_MAPPED;
    } else if (flashPageNum == writePage)
    {
        data = writeBuf[virtAddress & 0xff];
    } else
    {
        data = ((byte*) (flashPageNum << 8))[virtAddress & 0xff];
    }
    return 0;
}

unsigned char MemMapper::getUInt8(int idx)
{
    byte ret;
    readMem(idx, ret);
    return ret;
}

unsigned short MemMapper::getUInt16(int idx)
{
    byte b1;
    byte b2;
    readMem(idx, b1);
    readMem(idx + 1, b2);
    return (b1 << 8) | b2;
}

unsigned int MemMapper::getUInt32(int idx)
{
    byte b1;
    byte b2;
    byte b3;
    byte b4;
    readMem(idx, b1);
    readMem(idx + 1, b2);
    readMem(idx + 2, b3);
    readMem(idx + 3, b4);
    return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

int MemMapper::setUInt8(int idx, byte data)
{
    return writeMem(idx, data);
}

int MemMapper::setUInt16(int idx, unsigned short data)
{
    int ret=writeMem(idx, data>>8);
    ret|=writeMem(idx +1, data);
    return ret;
}

int MemMapper::setUInt32(int idx, unsigned int data)
{
    int ret=writeMem(idx, data>>24);
    ret|=writeMem(idx + 1, data>>16);
    ret|=writeMem(idx + 2, data>>8);
    ret|=writeMem(idx + 3, data);
    return ret;
}

