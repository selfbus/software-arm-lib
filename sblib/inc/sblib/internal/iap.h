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

#define IAP_UID_LENGTH (16)                  //!< number of bytes iapReadUID wants as buffer
#define PAGE_ALIGNMENT (FLASH_PAGE_SIZE - 1) //!< page alignment which is allowed to flash

/**
 * Status code of IAP (In Application Programming (Flash)) commands
 */
enum IAP_Status
{
    IAP_SUCCESS = 0,                                 //!< Command is executed successfully.
    IAP_INVALID_COMMAND = 1,                         //!< Invalid command.
    IAP_SRC_ADDR_ERROR = 2,                          //!< Source address is not on a word boundary.
    IAP_DST_ADDR_ERROR = 3,                          //!< Destination address is not on a correct boundary.
    IAP_SRC_ADDR_NOT_MAPPED = 4,                     //!< Source address is not mapped in the memory map. Count value is taken in to consideration where applicable.
    IAP_DST_ADDR_NOT_MAPPED = 5,                     //!< Destination address is not mapped in the memory map. Count value is taken in to consideration where applicable.
    IAP_COUNT_ERROR = 6,                             //!< Byte count is not multiple of 4 or 256 | 512 | 1024 | 4096
    IAP_INVALID_SECTOR = 7,                          //!< Sector number is invalid.
    IAP_SECTOR_NOT_BLANK = 8,                        //!< Sector is not blank.
    IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION = 9, //!< Command to prepare sector for write operation was not executed.
    IAP_COMPARE_ERROR = 10,                          //!< Source and destination data is not same.
    IAP_BUSY = 11                                    //!< Flash programming hardware interface is busy.
};


/**
 * Get the index of the FLASH sector for the passed address.
 *
 * @param address - the address inside the FLASH
 * @return The sector index of the address.
 */
unsigned int iapSectorOfAddress(const byte* address);

/**
 * Get the index of the FLASH sector for the passed address.
 *
 * @param address - the address inside the FLASH
 * @return The sector index of the address.
 */
unsigned int iapSectorOfAddress(const unsigned int address);

/**
 * Get the index of the FLASH page for the passed address.
 *
 * @param address - the address inside the FLASH
 * @return The sector index of the address.
 */
unsigned int iapPageOfAddress(const byte* address);

/**
 * Get the index of the FLASH page for the passed address.
 *
 * @param address - the address inside the FLASH
 * @return The sector index of the address.
 */
unsigned int iapPageOfAddress(const unsigned int address);

/**
 * Get the address of the given FLASH page index.
 *
 * @param page - the page index inside the FLASH
 * @return The address of the given page index.
 */
unsigned int iapAddressOfPage(const unsigned int page);

/**
 * Get the address of the given FLASH sector index.
 *
 * @param sector - the sector index inside the FLASH
 * @return The address of the given sector index.
 */
unsigned int iapAddressOfSector(const unsigned int sector);

/**
 * Erase the specified sector.
 *  @param sector       index of the sector which should be erased
 *  @return             @ref IAP_Status status code (0 == OK)
 *  @warning             The function calls no_interrupts().
 */
IAP_Status iapEraseSector(const unsigned int sector);

/**
 * @brief Erase the specified sector range.
 *
 * @param startSector   first sector to be erased
 * @param endSector     last sector to be erased, must be equal or greater then startSector
 * @return              @ref IAP_Status status code (0 == OK)
 * @warning             The function calls no_interrupts().
 */
IAP_Status iapEraseSectorRange(const unsigned int startSector, const unsigned int endSector);

/**
 * Erase the specified page.
 *  @param pageNumber   index of the page which should be erased
 *  @return             @ref IAP_Status status code (0 == OK)
 *  @warning            The function calls no_interrupts().
 */
IAP_Status iapErasePage(const unsigned int pageNumber);

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
 * @param ram           start address of the buffer (must be word aligned, use __attribute__ ((aligned (4))) to force it)
 * @param size          number of bytes to program
 * @return              @ref IAP_Status status code
 * @note                Address of ram must be word aligned. Otherwise you'll run into a @ref IAP_SRC_ADDR_ERROR
 *                      Use '__attribute__ ((aligned (4)))' to force correct alignment even with compiler optimization -Ox
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
unsigned int iapFlashSize();


#endif /* sblib_iap_h */
