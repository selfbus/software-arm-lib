/*
 *  Copyright (c) 2014 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/internal/iap.h>

#include <sblib/interrupt.h>
#include <sblib/platform.h>
#include <string.h>

// The maximum memory that is tested when searching for the flash size, in bytes
#define MAX_FLASH_SIZE 0x80000 // (524kB)

// The increments when searching for the flash size
#define FLASH_SIZE_SEARCH_INC 0x2000


/**
 * IAP command codes.
 */
enum IAP_Command
{
    CMD_PREPARE = 50,           //!< Prepare sector(s) for write
    CMD_COPY_RAM2FLASH = 51,    //!< Copy RAM to Flash
    CMD_ERASE = 52,             //!< Erase sector(s)
    CMD_BLANK_CHECK = 53,       //!< Blank check sector(s)
    CMD_READ_PART_ID = 54,      //!< Read chip part ID
    CMD_READ_BOOT_VER = 55,     //!< Read chip boot code version
    CMD_COMPARE = 56,           //!< Compare memory areas
    CMD_REINVOKE_ISP = 57,      //!< Reinvoke ISP
    CMD_READ_UID = 58,          //!< Read unique ID
    CMD_ERASE_PAGE = 59         //!< Erase page(s)
};

/**
 * A container for the interface to the IAP function calls.
 */
struct IAP_Parameter
{
    uintptr_t cmd;         //!< Command
    uintptr_t par[4];      //!< Parameters
    uintptr_t stat;        //!< Status
    uintptr_t res[4];      //!< Result
};

// The size of the flash in bytes. Use iapFlashSize() to get the flash size.
unsigned int iapFlashBytes = 0;


/** 
 * IAP call function (DO NOT USE UNLESS YOU KNOW WHAT YOU ARE DOING!)
 * use instead: IAP_Call_InterruptSafe()
 */
typedef void (*IAP_Func)(uintptr_t * cmd, uintptr_t * stat);

#ifndef IAP_EMULATION
#  if defined(__LPC11XX__) || defined(__LPC11UXX__) || defined(__LPC13XX__) || defined(__LPC17XX__)
#    define IAP_LOCATION      0x1FFF1FF1
#  elif defined(__LPC2XXX__)
#    define IAP_LOCATION      0x7FFFFFF1
#  else
#    error "Unsupported processor"
#  endif
#  define IAP_Call ((IAP_Func) IAP_LOCATION)
#else
   extern "C" void IAP_Call (uintptr_t * cmd, uintptr_t * stat);
#endif


/**
 * IAP_Call_InterruptSafe(): interrupt-safe IAP_Call function
 *
 * ATTENTION: interrupts shall be blocked during an IAP_Call()!
 *
 * Reason: during an IAP_Call() with flash access the flash is inaccessible for
 *         the user application. When an interrupt occurs and the Interrupt
 *         Vector Table is located in the Flash this will fail and raise a
 *         non-handled HardFault condition.
 */
inline void IAP_Call_InterruptSafe(uintptr_t * cmd, uintptr_t * stat, const bool getLock = true)
{
    if (getLock)
    {
        noInterrupts();
    }

    IAP_Call(cmd, stat);

    if (getLock)
    {
        interrupts();
    }
}

static IAP_Status _prepareSectorRange(const unsigned int startSector, const unsigned int endSector, const bool getLock = true)
{
    IAP_Parameter p;

    p.cmd = CMD_PREPARE;
    p.par[0] = startSector;
    p.par[1] = endSector;
    IAP_Call_InterruptSafe(&p.cmd, &p.stat, getLock);

    return (IAP_Status) p.stat;
}

IAP_Status iapEraseSector(const unsigned int sector)
{
    return iapEraseSectorRange(sector, sector);
}

IAP_Status iapEraseSectorRange(const unsigned int startSector, const unsigned int endSector)
{
    IAP_Parameter p;

    p.stat = _prepareSectorRange(startSector, endSector);

    if (p.stat == IAP_SUCCESS)
    {
        p.cmd = CMD_ERASE;
        p.par[0] = startSector;
        p.par[1] = endSector;
        p.par[2] = SystemCoreClock / 1000;
        IAP_Call_InterruptSafe(&p.cmd, &p.stat);

        if (p.stat == IAP_SUCCESS)
        {
            p.cmd = CMD_BLANK_CHECK;
            p.par[0] = startSector;
            p.par[1] = endSector;
            IAP_Call_InterruptSafe(&p.cmd, &p.stat);
        }
    }
    return (IAP_Status) p.stat;
}

IAP_Status iapErasePage(const unsigned int pageNumber)
{
    return iapErasePageRange(pageNumber, pageNumber);
}

IAP_Status iapErasePageRange(const unsigned int startPageNumber, const unsigned int endPageNumber)
{
    unsigned int startSector = startPageNumber / (FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE); // each sector has 16 pages
    unsigned int endSector = endPageNumber / (FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE); // each sector has 16 pages
    IAP_Parameter p;

    p.stat = _prepareSectorRange(startSector, endSector); // even not mentioned in manual, this prepare is needed

    if (p.stat == IAP_SUCCESS)
    {
        p.cmd = CMD_ERASE_PAGE;
        p.par[0] = startPageNumber;
        p.par[1] = endPageNumber;
        p.par[2] = SystemCoreClock / 1000;
        IAP_Call_InterruptSafe(&p.cmd, &p.stat);
    }
    return (IAP_Status) p.stat;
}

IAP_Status iapProgram(uint8_t * rom, const uint8_t * ram, unsigned int size)
{
    // IMPORTANT: Address of ram must be word aligned. Otherwise you'll run into a IAP_SRC_ADDR_ERROR
    // Use '__attribute__ ((aligned (FLASH_PAGE_ALIGNMENT)))' to force correct alignment even with compiler optimization -Ox

    IAP_Parameter p;
    unsigned int sector = iapSectorOfAddress(rom);

    // in order to access flash we need to disable all interrupts
    noInterrupts();
    // first we need to unlock the sector
    p.stat = _prepareSectorRange(sector, sector, false);

    if (p.stat != IAP_SUCCESS)
    {
        interrupts();
        return (IAP_Status) p.stat;
    }

    // then we can copy the RAM content to the FLASH
    p.cmd = CMD_COPY_RAM2FLASH;
    p.par[0] = (uintptr_t) rom;
    p.par[1] = (uintptr_t) ram;
    p.par[2] = size;
    p.par[3] = SystemCoreClock / 1000;
    IAP_Call_InterruptSafe(&p.cmd, &p.stat, false);

    if (p.stat != IAP_SUCCESS)
    {
        interrupts();
        return (IAP_Status) p.stat;
    }

    // now we check that RAM and FLASH have the same content
    p.cmd = CMD_COMPARE;
    p.par[0] = (uintptr_t) rom;
    p.par[1] = (uintptr_t) ram;
    p.par[2] = size;
    IAP_Call_InterruptSafe(&p.cmd, &p.stat, false);
    interrupts();
    return (IAP_Status) p.stat;
}

IAP_Status iapReadUID(byte* uid)
{
    IAP_Parameter p;
    p.cmd = CMD_READ_UID;

    IAP_Call_InterruptSafe(&p.cmd, &p.stat);
    memcpy(uid, p.res, 16);

    return (IAP_Status) p.stat;
}

IAP_Status iapReadPartID(unsigned int* partId)
{
    IAP_Parameter p;
    p.cmd = CMD_READ_PART_ID;

    IAP_Call_InterruptSafe(&p.cmd, &p.stat);
    *partId = p.res[0];

    return (IAP_Status) p.stat;
}

unsigned int iapSectorOfAddress(const uint8_t * address)
{
    return (unsigned int)((address - FLASH_BASE_ADDRESS) / FLASH_SECTOR_SIZE);
}

unsigned int iapPageOfAddress(const uint8_t * address)
{
    return (unsigned int)((address - FLASH_BASE_ADDRESS) / FLASH_PAGE_SIZE);
}

uint8_t * iapAddressOfPage(const unsigned int page)
{
    return (page * FLASH_PAGE_SIZE) + FLASH_BASE_ADDRESS;
}

uint8_t * iapAddressOfSector(const unsigned int sector)
{
    return (sector * FLASH_SECTOR_SIZE) + FLASH_BASE_ADDRESS;
}

unsigned int iapFlashSize()
{
    if (iapFlashBytes)
        return iapFlashBytes;

    IAP_Parameter p;
    p.cmd = CMD_BLANK_CHECK;

    const int sectorInc = FLASH_SIZE_SEARCH_INC / FLASH_SECTOR_SIZE;
    unsigned int sector = sectorInc;
    const int maxSector = MAX_FLASH_SIZE / FLASH_SECTOR_SIZE;

    while (sector < maxSector)
    {
        p.par[0] = sector;
        p.par[1] = sector;
        IAP_Call_InterruptSafe(&p.cmd, &p.stat);

        if (p.stat == IAP_INVALID_SECTOR)
            break;

        sector += sectorInc;
    }

    iapFlashBytes = sector * FLASH_SECTOR_SIZE;
    return iapFlashBytes;
}
