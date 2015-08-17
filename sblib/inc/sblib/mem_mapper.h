/*
 * mem_mapper.h
 *
 *  Created on: Aug 16, 2015
 *      Author: Deti Fliegl <deti@fliegl.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 *
 */

#ifndef SBLIB_MEM_MAPPER_H_
#define SBLIB_MEM_MAPPER_H_

class MemMapper
{
    public:
    /**
     * Creates a MemMapper instance with flash base address and size
     *
     *
     * @param flashBase - must be a page aligned address within 16 bit address space
     * @param flashSize - must be a page aligned size in bytes
     */
    MemMapper(byte *flashBase, unsigned int flashSize);

    /**
     * Write a single byte to virtual address
     *
     *
     * @param virtAddress - a 16 bit virtual address
     * @param data - a byte that should be written to the address
     * @return 0 on success, < 0 on error
     */
    int writeMem(int virtAddress, byte data);

    /**
     * Read a single byte from virtual address
     *
     *
     * @param virtAddress - a 16 bit virtual address
     * @param data - a reference to a byte that should contain the read data
     * @return 0 on success, < 0 on error
     */
    int readMem(int virtAddress, byte &data);

    /**
     * Force writing all pending data to flash
     *
     *
     * @return 0 nothing flashed, 1 allocation table flashed, 2 data page flashed
     */
    int doFlash(void);

    private:
    bool isValidAddress(int virtAddress);
    bool cacheMem(unsigned int flashPage);

    byte *flashBase;  //memory layout: flashBase + 0 = allocTable, flashBase + 1 = usableMemory
    unsigned int flashBasePage;

    unsigned int flashSize;
    unsigned int flashSizePages;

    byte *allocTable;

    byte *writeBuf;
    unsigned int writePage;

    unsigned int lastAllocated;

    bool flashMemModified;
    bool allocTableModified;
};

#endif /* SBLIB_MEM_MAPPER_H_ */
