/*
 *  iap_rp2350.cpp - In-Application Programming (Flash) for RP2350.
 *
 *  Provides the same API as the LPC11xx iap.cpp using
 *  Pico SDK hardware_flash functions.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#ifdef __SBLIB_TARGET_RP2350__

#include <sblib/internal/iap.h>
#include <sblib/platform.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/unique_id.h"

// RP2350 flash is 2MB by default (Pico 2)
#ifndef RP2350_FLASH_SIZE
#define RP2350_FLASH_SIZE (2 * 1024 * 1024)
#endif

unsigned int iapFlashSize()
{
    return RP2350_FLASH_SIZE;
}

unsigned int iapSectorOfAddress(const byte* address)
{
    uintptr_t offset = (uintptr_t)address - (uintptr_t)FLASH_BASE_ADDRESS;
    return offset / FLASH_SECTOR_SIZE;
}

unsigned int iapPageOfAddress(const byte* address)
{
    uintptr_t offset = (uintptr_t)address - (uintptr_t)FLASH_BASE_ADDRESS;
    return offset / FLASH_PAGE_SIZE;
}

uint8_t* iapAddressOfPage(const unsigned int page)
{
    return FLASH_BASE_ADDRESS + page * FLASH_PAGE_SIZE;
}

uint8_t* iapAddressOfSector(const unsigned int sector)
{
    return FLASH_BASE_ADDRESS + sector * FLASH_SECTOR_SIZE;
}

IAP_Status iapEraseSector(const unsigned int sector)
{
    uint32_t flashOffset = sector * FLASH_SECTOR_SIZE;
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(flashOffset, FLASH_SECTOR_SIZE);
    restore_interrupts(ints);
    return IAP_SUCCESS;
}

IAP_Status iapEraseSectorRange(const unsigned int startSector, const unsigned int endSector)
{
    uint32_t flashOffset = startSector * FLASH_SECTOR_SIZE;
    uint32_t count = (endSector - startSector + 1) * FLASH_SECTOR_SIZE;
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(flashOffset, count);
    restore_interrupts(ints);
    return IAP_SUCCESS;
}

IAP_Status iapErasePage(const unsigned int pageNumber)
{
    // RP2350 flash_range_erase requires sector-aligned erase.
    // Erasing a single page requires erasing the entire containing sector.
    uint32_t sectorOffset = (pageNumber * FLASH_PAGE_SIZE / FLASH_SECTOR_SIZE) * FLASH_SECTOR_SIZE;
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(sectorOffset, FLASH_SECTOR_SIZE);
    restore_interrupts(ints);
    return IAP_SUCCESS;
}

IAP_Status iapErasePageRange(const unsigned int startPageNumber, const unsigned int endPageNumber)
{
    uint32_t startSector = startPageNumber * FLASH_PAGE_SIZE / FLASH_SECTOR_SIZE;
    uint32_t endSector = endPageNumber * FLASH_PAGE_SIZE / FLASH_SECTOR_SIZE;
    return iapEraseSectorRange(startSector, endSector);
}

IAP_Status iapProgram(byte* rom, const byte* ram, unsigned int size)
{
    uint32_t flashOffset = (uintptr_t)rom - (uintptr_t)FLASH_BASE_ADDRESS;
    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(flashOffset, ram, size);
    restore_interrupts(ints);
    return IAP_SUCCESS;
}

IAP_Status iapReadUID(byte* uid)
{
    // RP2350/Pico 2 has an 8-byte unique board ID from the flash chip.
    // We pad to 16 bytes (IAP_UID_LENGTH) with zeros.
    pico_unique_board_id_t boardId;
    pico_get_unique_board_id(&boardId);
    memset(uid, 0, IAP_UID_LENGTH);
    memcpy(uid, boardId.id, sizeof(boardId.id));
    return IAP_SUCCESS;
}

IAP_Status iapReadPartID(unsigned int* partId)
{
    // No direct equivalent on RP2350; return a fixed identifier.
    *partId = 0x23500001;  // Synthetic part ID for RP2350
    return IAP_SUCCESS;
}

#endif // __SBLIB_TARGET_RP2350__
