/*
 *  flash_hal.h - Hardware Abstraction Layer for on-chip flash operations.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */
#ifndef SBLIB_HAL_FLASH_HAL_H
#define SBLIB_HAL_FLASH_HAL_H

#include <stdint.h>

/**
 * Return codes for flash operations.
 */
enum FlashStatus : int
{
    FLASH_OK              =  0,
    FLASH_INVALID_COMMAND = -1,
    FLASH_ADDR_ERROR      = -2,
    FLASH_BUSY            = -3,
    FLASH_ERROR           = -4,
};

/**
 * Hardware Abstraction for on-chip flash programming.
 *
 * Different MCUs have different flash sector/page sizes and erase/program APIs.
 * This interface unifies access for the EIB stack's EEPROM emulation.
 */
namespace FlashHAL
{
    /** Size of one flash sector in bytes. */
    uint32_t sectorSize();

    /** Size of one flash page (minimum write unit) in bytes. */
    uint32_t pageSize();

    /**
     * Erase a flash sector.
     * @param sectorNumber  Sector index.
     * @return FLASH_OK on success.
     */
    FlashStatus eraseSector(uint32_t sectorNumber);

    /**
     * Program a page of flash.
     * Interrupts are typically disabled internally during the operation.
     *
     * @param destAddr  Destination address in flash (must be page-aligned).
     * @param srcData   Source data buffer.
     * @param length    Number of bytes to write (must be <= pageSize()).
     * @return FLASH_OK on success.
     */
    FlashStatus programPage(uint32_t destAddr, const uint8_t* srcData, uint32_t length);

    /**
     * Read the MCU's unique serial number / UID.
     * @param uid  Buffer of at least 16 bytes to receive the UID.
     */
    void readUID(uint8_t* uid);
}

#endif /* SBLIB_HAL_FLASH_HAL_H */
