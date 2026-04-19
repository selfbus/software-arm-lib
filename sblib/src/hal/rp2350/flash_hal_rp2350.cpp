/*
 *  flash_hal_rp2350.cpp - FlashHAL implementation for RP2350 (Pico SDK).
 *
 *  RP2350 has either internal flash (RP2354) or external QSPI flash.
 *  Both are accessed via the Pico SDK's flash_range_erase / flash_range_program API.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#ifdef __SBLIB_TARGET_RP2350__

#include <sblib/hal/flash_hal.h>

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/unique_id.h"

#include <cstring>

namespace FlashHAL
{

uint32_t sectorSize()
{
    return FLASH_SECTOR_SIZE; // 4096 bytes on RP2350
}

uint32_t pageSize()
{
    return FLASH_PAGE_SIZE; // 256 bytes on RP2350
}

FlashStatus eraseSector(uint32_t sectorNumber)
{
    uint32_t offset = sectorNumber * FLASH_SECTOR_SIZE;
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(offset, FLASH_SECTOR_SIZE);
    restore_interrupts(ints);
    return FLASH_OK;
}

FlashStatus programPage(uint32_t destAddr, const uint8_t* srcData, uint32_t length)
{
    if (length > FLASH_PAGE_SIZE)
        return FLASH_ADDR_ERROR;

    // flash_range_program requires exactly FLASH_PAGE_SIZE bytes, pad with 0xFF if needed
    uint8_t pageBuf[FLASH_PAGE_SIZE];
    std::memset(pageBuf, 0xFF, FLASH_PAGE_SIZE);
    std::memcpy(pageBuf, srcData, length);

    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(destAddr, pageBuf, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
    return FLASH_OK;
}

void readUID(uint8_t* uid)
{
    pico_unique_board_id_t boardId;
    pico_get_unique_board_id(&boardId);
    // Board ID is 8 bytes; pad to 16
    std::memcpy(uid, boardId.id, PICO_UNIQUE_BOARD_ID_SIZE_BYTES);
    std::memset(uid + PICO_UNIQUE_BOARD_ID_SIZE_BYTES, 0,
                16 - PICO_UNIQUE_BOARD_ID_SIZE_BYTES);
}

} // namespace FlashHAL

#endif // __SBLIB_TARGET_RP2350__
