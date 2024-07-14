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
 * @file   flash.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/internal/iap.h>
#include "flash.h"
#include "boot_descriptor_block.h"
#include "dump.h"

/**
 * @brief Checks if the pointer is aligned.
 *
 * @param  ptr       Pointer to check
 * @param  alignment Expected alignment, needs to be a power of 2
 * @return           true if pointer is aligned to alignment, otherwise false
 */
static inline bool is_aligned(const uint8_t * ptr, const uint32_t alignment)
{
    // See https://stackoverflow.com/a/1898487 and https://stackoverflow.com/a/28760180 for reasoning.
    return (((uintptr_t)(const void *)ptr) & (alignment - 1)) == 0;
}

/**
 * @brief Erases if allowed the requested sector.
 * @param sector  Sector number to be erased
 * @return        @ref UDP_IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status
 */
/*
static UDP_State eraseSector(unsigned int sector)
{
    return (eraseSectorRange(sector, sector));
}
*/

bool addressAllowedToProgram(uint8_t * start, unsigned int length, bool isBootDescriptor)
{
    if (!is_aligned(start, FLASH_PAGE_SIZE) || !length) // not aligned to page or 0 length
    {
        return (0);
    }
    uint8_t * end = start + length - 1;
    if (isBootDescriptor)
    {
        return ((start >= bootDescriptorBlockAddress()) && (end < applicationFirstAddress()));
    }
    else
    {
        return ((start >= applicationFirstAddress()) && (end <= flashLastAddress()));
    }
}

/**
 * @brief Checks if the requested page is allowed to be erased.
 *
 * @param  pageNumber Page number to check erase is allowed
 * @return            true if page is allowed to erase, otherwise false
 */
static bool pageAllowedToErase(const unsigned int pageNumber)
{
    return ((pageNumber > iapPageOfAddress(bootLoaderLastAddress())) &&
            ( pageNumber <= iapPageOfAddress(flashLastAddress())));
}

/**
 * @brief Checks if the requested sector is allowed to be erased.
 *
 * @param  sectorNumber Sector number to check erase is allowed
 * @return              true if sector is allowed to erase, otherwise false
 */
static bool sectorAllowedToErase(const unsigned int sectorNumber)
{
    return ((sectorNumber > iapSectorOfAddress(bootLoaderLastAddress())) &&
            ( sectorNumber <= iapSectorOfAddress(flashLastAddress())));
}

UDP_State erasePageRange(unsigned int startPage, unsigned int endPage)
{
    UDP_State result = UDP_PAGE_NOT_ALLOWED_TO_ERASE;
    d3(serial.print("page   0x", startPage, HEX, 2));
    d3(serial.print(" - 0x", endPage, HEX, 2));
    d3(serial.print(" "));
    if ((!pageAllowedToErase(startPage)) || (!pageAllowedToErase(endPage)))
    {
        dline("not allowed!");
        return (UDP_PAGE_NOT_ALLOWED_TO_ERASE);
    }

    result = iapResult2UDPState(iapErasePageRange(startPage, endPage));
    d3(
        if (result != UDP_IAP_SUCCESS)
        {
            updResult2Serial(result);
            dline(" iapErasePageRange failed!");
        }
        else
        {
            dline("OK");
        }
    );
    return (result);
}

/**
 * @brief Erases if allowed the requested sector.
 *
 * @param startSector Start sector number to be erased
 * @param endSector   End sector number to be erased
 * @return  @ref UDP_IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status
 */
static UDP_State eraseSectorRange(unsigned int startSector, unsigned int endSector)
{
    UDP_State result = UDP_SECTOR_NOT_ALLOWED_TO_ERASE;
    d3(serial.print("sector 0x", startSector, HEX, 2));
    d3(serial.print(" - 0x", endSector, HEX, 2));
    if ((!sectorAllowedToErase(startSector)) || (!sectorAllowedToErase(endSector)))
    {
        dline(" not allowed!");
        return (UDP_SECTOR_NOT_ALLOWED_TO_ERASE);
    }

    result = iapResult2UDPState(iapEraseSectorRange(startSector, endSector));
    d3(
        if (result != UDP_IAP_SUCCESS)
        {
            updResult2Serial(result);
            dline(" iapEraseSectorRange failed!");
        }
        else
        {
            dline(" OK");
        }
    );
    return (result);
}

UDP_State eraseAddressRange(uint8_t * startAddress, const uint8_t * endAddress, const bool rangeCheck)
{
    UDP_State result = UDP_ADDRESS_RANGE_NOT_ALLOWED_TO_ERASE;
    d3(
        serial.print(" eraseAddressRange 0x", startAddress);
        serial.println("-0x", endAddress);
    );

    if (rangeCheck && (!addressAllowedToProgram(startAddress, endAddress - startAddress + 1, false)))
    {
        dline(" not allowed!");
        return (UDP_ADDRESS_RANGE_NOT_ALLOWED_TO_ERASE);
    }

    unsigned int start;
    unsigned int end;
    unsigned int startSector;
    unsigned int endSector;
    unsigned int startPage;
    const unsigned int endPage = iapPageOfAddress(endAddress);

    startSector = iapSectorOfAddress(startAddress);
    endSector = iapSectorOfAddress(endAddress);
    startPage = iapPageOfAddress(startAddress);

    const bool lessThenOneSector = (endPage - startPage + 1) < (FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE);
    if (lessThenOneSector)
    {
        result = erasePageRange(startPage, endPage); // this is slow and can take up to 15*100ms = ~1,5s
        return (result);
    }

    if (!is_aligned(startAddress, FLASH_SECTOR_SIZE))
    {
        // start address is not sector aligned, lets erase on a page level
        start = startPage;
        startSector++;
        // from start to last page of the sector
        uint8_t * nextSectorStartAddress = FLASH_BASE_ADDRESS + (startSector * FLASH_SECTOR_SIZE);
        end = iapPageOfAddress(nextSectorStartAddress) - 1;
        result = erasePageRange(start, end); // this is slow and can take up to 15*100ms = ~1,5s
        if (result != UDP_IAP_SUCCESS)
        {
            return (result);
        }
        startAddress = nextSectorStartAddress; // set new startAddress
    }

    startSector = iapSectorOfAddress(startAddress);

    start = (endSector * FLASH_SECTOR_SIZE) / FLASH_PAGE_SIZE;
    const bool lastPageInSector = ((endPage - start + 1) ==  ((FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE)));
    if (!(lastPageInSector))
    {
        // end address is not sector aligned, lets erase on a page level
        end = iapPageOfAddress(endAddress);

        result = erasePageRange(start, end); //  this is slow and can take up to 15*100ms = ~1,5s
        if (result != UDP_IAP_SUCCESS)
        {
            return (result);
        }
        endSector--;
    }

    if (startSector <= endSector)
    {
        result = eraseSectorRange(startSector, endSector); // this is fast and should always take ~100ms
    }

    return (result);
}

UDP_State eraseFullFlash()
{
    unsigned int page = iapPageOfAddress(bootLoaderLastAddress());
    page++;
    return eraseAddressRange(iapAddressOfPage(page), flashLastAddress(), false);
}

UDP_State executeProgramFlash(uint8_t * address, const uint8_t * ram, unsigned int size, bool isBootDescriptor)
{
    UDP_State result = UDP_ADDRESS_NOT_ALLOWED_TO_FLASH;
    if (!addressAllowedToProgram(address, size, isBootDescriptor))
    {
        return (UDP_ADDRESS_NOT_ALLOWED_TO_FLASH);
    }

    result = iapResult2UDPState(iapProgram(address, ram, size));
    return (result);
}


/** @}*/
