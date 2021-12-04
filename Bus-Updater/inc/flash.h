/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_UPD_UDP_FLASH_1 Flash access utilities
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Provides several function for accessing and flashing the MCU's flash
 * @details 
 *
 *
 * @{
 *
 * @file   flash.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef FLASH_H_
#define FLASH_H_

#include "upd_protocol.h"

/**
 * @brief Erases if allowed the requested page range, startPage must be lower or equal endPage.
 *
 * @param startPage Start page number to be erased
 * @param endPage   End page number to be erased
 * @return          @ref UDP_IAP_SUCCESS if successful, otherwise @ref UDP_PAGE_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status
 */
UDP_State erasePageRange(unsigned int startPage, unsigned int endPage);


/**
 * @brief   Erases the entire flash area starting behind the bootloader itself
 *
 * @return  @ref UDP_IAP_SUCCESS if successful, otherwise a @ref UDP_State
 * @warning Function can take up to ~2 seconds to return.
 *          It works on a page and sector base. Page erases are very slow ~100ms for one page
 */
UDP_State eraseFullFlash();

/**
 * @brief Erases if allowed the requested address range.
 *
 * @param startAddress  start address of flash range to erase
 * @param endAddress    end address of flash range to erase
 * @param rangeCheck    set to true to check if the address range is allowed to be accessed
 * @return              @ref UDP_IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status
 * @warning             Function can take up to ~3.5 seconds to return.
 *                      It works on a page and sector base. Page erases are very slow ~100ms for one page
 */
UDP_State eraseAddressRange(unsigned int startAddress, const unsigned int endAddress, const bool rangeCheck = true);

/**
 * @brief Checks if the address range is allowed to be programmed
 *
 * @param start            start of the address range to check
 * @param length           length of the address range
 * @param isBootDescriptor set true to check range of the application boot descriptor
 *                         or false to check range of the application itself
 * @return                 true if programming is allowed, otherwise false
 */
bool addressAllowedToProgram(unsigned int start, unsigned int length, bool isBootDescriptor = false);

/**
 * @brief Programs the specified number of bytes from the RAM to the specified location
 *        inside the FLASH.
 * @param address start address of inside the FLASH
 * @param ram     start address of the buffer containing the data
 * @param size    number of bytes to program
 * @param isBootDescriptor  Set true to program an @ref AppDescriptionBlock, default false
 *
 * @return        UDP_IAP_SUCCESS if successful, otherwise UDP_ADDRESS_NOT_ALLOWED_TO_FLASH
 *                or a @ref IAP_Status
 * @warning       The function calls iap_Program which by itself calls no_interrupts().
 */
UDP_State executeProgramFlash(unsigned int address, const byte* ram, unsigned int size, bool isBootDescriptor = false);



#endif /* FLASH_H_ */
/** @}*/
