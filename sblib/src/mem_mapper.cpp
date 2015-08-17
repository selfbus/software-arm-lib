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

    for (int i = 0; i < FLASH_PAGE_SIZE; i++)
    {
        allocTable[i] = flashBase[i];
    }
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
        return -1;
    }

    byte flashPageNum = allocTable[virtPage] ^ 0xff;

    if (writePage != flashPageNum)
    {
        doFlash();
        writePage = flashPageNum;
        if (writePage != 0)
        { // swap flash page into write buffer
            for (int i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                writeBuf[i] = flashBase[i | (writePage << 8)];
            }
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
            return -1; // we are out of memory
        }
        if (lastAllocated == 0)
        {  // no pages allocated yet.
            writePage = flashBasePage + 1;
        } else
        {
            lastAllocated++;
            writePage = lastAllocated;
        }
        for (int i = 0; i < FLASH_PAGE_SIZE; i++)
        {
            writeBuf[i] = 0x00;
        }

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
        return -1;
    }

    byte flashPageNum = allocTable[virtPage] ^ 0xff;

    if (flashPageNum == 0)
    {
        data = 0xff;
    } else if (flashPageNum == writePage)
    {
        data = writeBuf[virtAddress & 0xff];
    } else
    {
        data = ((byte*) (flashPageNum << 8))[virtAddress & 0xff];
    }
    return 0;
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

