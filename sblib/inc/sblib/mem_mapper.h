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

#define MEM_MAPPER_SUCCESS         0
#define MEM_MAPPER_INVALID_ADDRESS -1
#define MEM_MAPPER_NOT_MAPPED      -2
#define MEM_MAPPER_OUT_OF_MEMORY   -4
#define MEM_MAPPER_INVALID_LENGTH  -8

class MemMapper
{
public:
    /**
     * Creates a MemMapper instance with flash base address and size
     *
     *
     * @param flashBase - must be a page aligned address within 16 bit address space
     * @param flashSize - must be a page aligned size in bytes
     * @param autoAddPage - when set to true non existing pages are allocated automatically
     */
    MemMapper(unsigned int flashBase = 0xf000, unsigned int flashSize = 0x1000,
            bool autoAddPage = false);

    /**
     * Write a single byte to virtual address
     *
     *
     * @param virtAddress - a 16 bit virtual address
     * @param data - a byte that should be written to the address
     * @return 0 on success, else error
     */
    int writeMem(int virtAddress, byte data);

    /**
     * Write an array of byte to virtual address
     *
     *
     * @param virtAddress - a 16 bit virtual address
     * @param data - bytes that should be written to the address
     * @param length - number of bytes to write
     * @return 0 on success, else error
     */
    int writeMemPtr(int virtAddress, byte *data, int length);

    /**
     * Read a single byte from virtual address
     *
     *
     * @param virtAddress - a 16 bit virtual address
     * @param data - a reference to a byte that should contain the read data
     * @param forceFlash - force pending data to be flashed before operation
     * @return 0 on success, else error
     */
    int readMem(int virtAddress, byte &data, bool forceFlash = false);

    /**
     * Read a single byte from virtual address
     *
     *
     * @param virtAddress - a 16 bit virtual address
     * @param data - a reference to a byte that should contain the read data
     * @param length - number of bytes to read
     * @param forceFlash - force pending data to be flashed before operation
     * @return 0 on success, else error
     */
    int readMemPtr(int virtAddress, byte *data, int length, bool forceFlash =
            false);

    /**
     * Add a prereserved range
     *
     *
     * @param virtAddress - a 16 bit virtual address
     * @param length - the size of the range
     * @return 0 on success, else error
     */
    int addRange(int virtAddress, int length);

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
     * @param virtAddress - the virtual address of the data byte to access.
     * @return The data byte.
     */
    unsigned char getUInt8(int virtAddress);

    /**
     * Access the user EEPROM to get a unsigned short
     *
     * @param virtAddress - the virtual address of the 16 bit data to access.
     * @return The 16bit as unsigned short.
     */
    unsigned short getUInt16(int virtAddress);

    /**
     * Access the user EEPROM to get a unsigned int
     *
     * @param virtAddress - the virtual address of the 32 bit data to access.
     * @return The 32bit as unsigned int.
     */
    unsigned int getUInt32(int virtAddress);

    /**
     * Access the user EEPROM to set a unsigned byte
     *
     * @param virtAddress - the virtual address of the data byte to access.
     * @param data - the value to be written
     * @return error value of flash operation
     */

    int setUInt8(int virtAddress, byte data);

    /**
     * Access the user EEPROM to set a unsigned short
     *
     * @param virtAddress - the virtual address of the 16 bit data to access.
     * @param data - the value to be written
     * @return error value of flash operation
     */
    int setUInt16(int virtAddress, unsigned short data);

    /**
     * Access the user EEPROM to set a unsigned int
     *
     * @param virtAddress - the virtual address of the 32 bit data to access.
     * @param data - the value to be written
     * @return error value of flash operation
     */
    int setUInt32(int virtAddress, unsigned int data);

    /**
     * Access the user EEPROM as a pointer
     *
     * @param virtAddresss - the virtual address of the data block.
     * @param forceFlash - force pending data to be flashed before operation
     * @return a pointer to the desired data
     */
    byte* memoryPtr(int virtAddress, bool forceFlash = true);

    /**
     * Query about mapping
     *
     * @param virtAddresss - the virtual address of the data block.
     * @return true if virtual address is mapped
     */
    bool isMapped(int virtAddress);

private:
    int allocatePage(int virtPage);
    int getFlashPageNum(int virtAddress);

    unsigned int flashBase; //memory layout: flashBase + 0 = allocTable, flashBase + 1 = usableMemory
    unsigned int flashBasePage;

    unsigned int flashSize;
    unsigned int flashSizePages;

    byte allocTable[FLASH_PAGE_SIZE];

    byte writeBuf[FLASH_PAGE_SIZE];
    unsigned int writePage;

    unsigned int lastAllocated;

    bool autoAddPage;
    bool flashMemModified;
    bool allocTableModified;
};

#endif /* SBLIB_MEM_MAPPER_H_ */
