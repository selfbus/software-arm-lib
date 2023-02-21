/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Main Group Description
 * @defgroup SBLIB_SUB_GROUP_1 Sub Group 1 Description
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   memory.h
 * @author Darthyson <darth@maptrack.de>
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef SBLIB_EIB_MEMORY_H_
#define SBLIB_EIB_MEMORY_H_

#include <stdint.h>
#include <sblib/types.h>

/**
 * Class for basic memory operations.
 */
class Memory
{
public:
    Memory() = delete;
    ~Memory() = default;
    Memory(uint32_t  start, uint32_t size);

    /**
     * Access the memory like an ordinary array
     *
     * @param address - the address of the data byte to access.
     * @return The data byte at the given address.
     * @note @ref startAddress will be subtracted from @ref address
     */
    virtual byte& operator[](uint32_t address) = 0;

    /**
     * Gets a byte from the memory at given @ref address
     *
     * @param address   Address to get the byte from
     * @return          Content as byte at the given @ref address
     * @note @ref startAddress will be subtracted from @ref address
     */
    virtual uint8_t getUInt8(uint32_t address) const = 0;

    /**
     * Gets a word from the memory at given @ref address and address + 1
     *
     * @param address   Address to get the byte from
     * @return          Content as word at the @ref address
     * @note @ref startAddress will be subtracted from @ref address
     */
    virtual uint16_t getUInt16(uint32_t address) const = 0;

    /**
     * Checks, if a @ref address is accessible in the memory
     *
     * @param address   Address to check for valid access
     * @return          True if @ref address is accessible, otherwise false
     */
    virtual bool inRange(uint32_t address) const;

    /**
     * Checks, if a given address range is accessible in the memory
     *
     * @param start     Start address of the range to check
     * @param end       End address of the range to check
     * @return          True if range is accesible, otherwise false
     */
    virtual bool inRange(uint32_t start, uint32_t end) const;

    /**
     * Get the start address of the memory
     *
     * @return Start address of the memory
     */
    uint32_t startAddr() const;

    /**
     * Get the end address of the memory
     *
     * @return end address of the memory
     */
    uint32_t endAddr() const;

    /**
     * Get the size of the memory
     *
     * @return size of the memory in bytes
     */
    uint32_t size() const;

protected:
    uint32_t startAddress;  //!< virtual start address of the memory
    uint32_t endAddress;    //!< virtual end address of the memory
    uint32_t sizeTotal;     //!< size in bytes of the memory

    /**
     * Changes the given absolute memory address to the real internal index
     *
     * @param address The absolute memory address
     */
    void normalizeAddress(uint32_t* address) const {(*address -= startAddress);}

private:


};


#endif /* SBLIB_EIB_MEMORY_H_ */
/** @}*/
