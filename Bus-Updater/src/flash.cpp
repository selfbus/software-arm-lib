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

#if defined(DUMP_TELEGRAMS_LVL1) || defined(DEBUG)
#   include <sblib/serial.h>

#   define d1(x) {serial.print(x);} //!< \todo
#   define dline(x) {serial.println(x);} //!< \todo
#   define d2(u,v,w) {serial.print(u,v,w);} //!< \todo
#   define d3(x) {x;} //!< \todo
#else
#   define d1(x) //!< \todo
#   define d2(u,v,w) //!< \todo
#   define d3(x) //!< \todo
#   define dline(x) //!< \todo
#endif

#define ADDRESS_TO_SECTOR(a) ((a + FLASH_SECTOR_SIZE) / FLASH_SECTOR_SIZE) //!< address to sector conversion based on flash sector size
#define ADDRESS_TO_PAGE(a) ((a + FLASH_PAGE_SIZE) / FLASH_PAGE_SIZE)       //!< address to page conversion based on flash page size

#define PAGE_ALIGNMENT 0xff                      //!< page alignment which is allowed to flash

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

bool addressAllowedToProgram(unsigned int start, unsigned int length, bool isBootDescriptor)
{
    if ((start & (FLASH_PAGE_SIZE - 1)) || !length) // not aligned to page or 0 length
    {
        return (0);
    }
    unsigned int end = start + length - 1;
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
static bool pageAllowedToErase(unsigned int pageNumber)
{
    return ((pageNumber >= ADDRESS_TO_PAGE(bootLoaderLastAddress() + 1)) &&
            ( pageNumber < ADDRESS_TO_PAGE(flashLastAddress())));
}

/**
 * @brief Checks if the requested sector is allowed to be erased.
 *
 * @param  sectorNumber Sector number to check erase is allowed
 * @return              true if sector is allowed to erase, otherwise false
 */
static bool sectorAllowedToErase(unsigned int sectorNumber)
{
    return ((sectorNumber >= ADDRESS_TO_SECTOR(bootLoaderLastAddress() + 1)) &&
            ( sectorNumber < ADDRESS_TO_SECTOR(flashLastAddress())));
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

    result = (UDP_State)iapErasePageRange(startPage, endPage);
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

    result = (UDP_State)iapEraseSectorRange(startSector, endSector);
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


/**
 * @brief Erases if allowed the requested page.
 * @param page  Page number to be erased
 * @return      UDP_IAP_SUCCESS if successful, otherwise @ref UDP_PAGE_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status
 */
UDP_State erasePage(unsigned int page)
{
    return (erasePageRange(page, page));
}

UDP_State eraseAddressRange(unsigned int startAddress, const unsigned int endAddress)
{
    d3(
        serial.print(" eraseAddressRange 0x", startAddress, HEX, 4);
        serial.println("-0x", endAddress, HEX, 4);
    );

    if (!addressAllowedToProgram(startAddress, endAddress - startAddress + 1, false))
    {
        dline(" not allowed!");
        return (UDP_ADDRESS_RANGE_NOT_ALLOWED_TO_ERASE);
    }

    UDP_State result;
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

    const bool addressSectorAligned = !(startAddress & (FLASH_SECTOR_SIZE  - 1));
    if (!addressSectorAligned)
    {
        // start address is not sector aligned, lets erase on a page level
        start = startPage;
        startSector++;
        // from start to last page of the sector
        end = iapPageOfAddress(((startSector * FLASH_SECTOR_SIZE) - 1)); ///\todo there must be a better version
        result = erasePageRange(start, end); // this is slow and can take up to 15*100ms = ~1,5s
        if (result != UDP_IAP_SUCCESS)
        {
            return (result);
        }
        startAddress = startSector * FLASH_SECTOR_SIZE; // set new startAddress
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

#ifdef DEBUG
unsigned int testEraseAddressRange() ///\todo remove on release
{
    /*
    eraseAddressRange(0x1A00, 0x6FFF);
    eraseAddressRange(0x6B00, 0x6FFF);
    eraseAddressRange(0x7100, 0x8FFF);
    eraseAddressRange(0x7F00, 0xFFFF);
    eraseAddressRange(0x7F00, 0x7FFF);
    eraseAddressRange(0x7E00, 0x9AFF);
    eraseAddressRange(0x7F00, 0x9EFF);
    */
    return (UDP_IAP_SUCCESS);
}
#endif

UDP_State executeProgramFlash(unsigned int address, const byte* ram, unsigned int size, bool isBootDescriptor)
{
    UDP_State result = UDP_ADDRESS_NOT_ALLOWED_TO_FLASH;
    if (!addressAllowedToProgram(address, size, isBootDescriptor))
    {
        return (UDP_ADDRESS_NOT_ALLOWED_TO_FLASH);
    }

    result = (UDP_State)iapProgram((byte *) address, ram, size);
    return (result);
}


/** @}*/
