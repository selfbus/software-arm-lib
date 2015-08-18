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

#define INVALID_ADDRESS 1
#define NOT_MAPPED      2
#define OUT_OF_MEMORY   4

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

    /**
     * Access the user EEPROM to get a unsigned byte
     *
     * @param idx - the index of the data byte to access.
     * @return The data byte.
     */
    unsigned char getUInt8(int idx);

    /**
     * Access the user EEPROM to get a unsigned short
     *
     * @param idx - the index of the 16 bit data to access.
     * @return The 16bit as unsigned short.
     */
    unsigned short getUInt16(int idx);

    /**
     * Access the user EEPROM to get a unsigned int
     *
     * @param idx - the index of the 32 bit data to access.
     * @return The 32bit as unsigned int.
     */
    unsigned int getUInt32(int idx);

    /**
     * Access the user EEPROM to set a unsigned byte
     *
     * @param idx - the index of the data byte to access.
     * @param data - the value to be written
     * @return error value of flash operation
     */

    int setUInt8(int idx, byte data);

    /**
     * Access the user EEPROM to set a unsigned short
     *
     * @param idx - the index of the 16 bit data to access.
     * @param data - the value to be written
     * @return error value of flash operation
     */
    int setUInt16(int idx, unsigned short data);

    /**
     * Access the user EEPROM to set a unsigned int
     *
     * @param idx - the index of the 32 bit data to access.
     * @param data - the value to be written
     * @return error value of flash operation
     */
    int setUInt32(int idx, unsigned int data);

private:
    bool isValidAddress(int virtAddress);

    byte *flashBase; //memory layout: flashBase + 0 = allocTable, flashBase + 1 = usableMemory
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
