/*
 *  Copyright (c) 2014 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef sblib_iap_h
#define sblib_iap_h

#include <sblib/platform.h>
#include <sblib/types.h>

#define IAP_UID_LENGTH (16) //!< number of bytes iapReadUID wants as buffer

/**
 * Status code of IAP (In Application Programming (Flash)) commands
 */
enum IAP_Status
{
    IAP_SUCCESS,                                //!< successful
    IAP_INVALID_COMMAND,                        //!< invalid command
    IAP_SRC_ADDR_ERROR,                         //!< source address error
    IAP_DST_ADDR_ERROR,                         //!< destination address error
    IAP_SRC_ADDR_NOT_MAPPED,                    //!< source address not mapped
    IAP_DST_ADDR_NOT_MAPPED,                    //!< destination address not mapped
    IAP_COUNT_ERROR,                            //!< count error
    IAP_INVALID_SECTOR,                         //!< invalid sector error
    IAP_SECTOR_NOT_BLANK,                       //!< sector not blank
    IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION,//!< sector not prepared for write operation
    IAP_COMPARE_ERROR,                          //!< compare error
    IAP_BUSY                                    //!< busy
};


/**
 * Get the index of the FLASH sector for the passed address.
 *
 * @param address - the address inside the FLASH
 * @return The sector index of the address.
 */
int iapSectorOfAddress(const byte* address);

/**
 * Get the index of the FLASH sector for the passed address.
 *
 * @param address - the address inside the FLASH
 * @return The sector index of the address.
 */
int iapSectorOfAddress(const unsigned int address);

/**
 * Get the index of the FLASH page for the passed address.
 *
 * @param address - the address inside the FLASH
 * @return The sector index of the address.
 */
int iapPageOfAddress(const byte* address);

/**
 * Get the index of the FLASH page for the passed address.
 *
 * @param address - the address inside the FLASH
 * @return The sector index of the address.
 */
int iapPageOfAddress(const unsigned int address);

/**
 * Erase the specified sector.
 *  @param sector       index of the sector which should be erased
 *  @return             @ref IAP_Status status code (0 == OK)
 *  @warning             The function calls no_interrupts().
 */
IAP_Status iapEraseSector(int sector);

/**
 * @brief Erase the specified sector range.
 *
 * @param startSector   first sector to be erased
 * @param endSector     last sector to be erased, must be equal or greater then startSector
 * @return              @ref IAP_Status status code (0 == OK)
 * @warning             The function calls no_interrupts().
 */
IAP_Status iapEraseSectorRange(int startSector, int endSector);

/**
 * Erase the specified page.
 *  @param pageNumber   index of the page which should be erased
 *  @return             @ref IAP_Status status code (0 == OK)
 *  @warning            The function calls no_interrupts().
 */
IAP_Status iapErasePage(int pageNumber);

/**
 * @brief Erase the specified page range.
 *
 * @param startPageNumber first page to be erased
 * @param endPageNumber   last page to be erased, must be equal or greater then startPageNumber
 * @return                @ref IAP_Status status code (0 == OK)
 * @warning               The function calls no_interrupts().
 */
IAP_Status iapErasePageRange(const unsigned int startPageNumber, const unsigned int endPageNumber);

/**
 * Programs the specified number of bytes from the RAM to the specified location
 * inside the FLASH.
 * @param rom           start address of inside the FLASH
 * @param ram           start address of the buffer
 * @param size          number of bytes to program
 * @return              @ref IAP_Status status code
 * @warning             The function calls no_interrupts().
 */
IAP_Status iapProgram(byte* rom, const byte* ram, unsigned int size);

/**
 * Read the unique ID of the CPU. The ID is 16 bytes long.
 *
 * @param uid   will contain the 16 byte UID after the call.
 *
 * @return      @ref IAP_Status status code (0 == OK)
 * @warning     The function calls no_interrupts().
 */
IAP_Status iapReadUID(byte* uid);

/**
 * Read the 32 bit part identification number of the CPU.
 *
 * @param partId    will contain the 32 bit part identification number after the call.
 *
 * @return          @ref IAP_Status status code (0 == OK)
 */
IAP_Status iapReadPartID(unsigned int* partId);

/**
 * Get the size of the flash memory. This is done by probing the flash sectors
 * until an error is encountered.
 *
 * @return the size of the flash memory.
 */
int iapFlashSize();


#endif /* sblib_iap_h */
