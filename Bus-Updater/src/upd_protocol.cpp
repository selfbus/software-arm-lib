/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_UPD_UDP_PROTOCOL_1 UPD/UDP protocol
 * @ingroup SBLIB_BOOTLOADER
 *
 * @brief   Definition of the UPD/UDP bootloader protocol
 * @details 
 *
 *
 * @{
 *
 * @file   upd_protocol.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include "upd_protocol.h"
#include "dump.h"

void updCommand2Serial(byte cmd)
{
    d3(
        d1("UPD_");
        switch (cmd)
        {
            case UPD_ERASE_SECTOR: d1("ERASE_SECTOR"); break;
            case UPD_SEND_DATA: d1("SEND_DATA"); break;
            case UPD_PROGRAM: d1("PROGRAM"); break;
            case UPD_UPDATE_BOOT_DESC: d1("UPDATE_BOOT_DESC"); break;
            case UPD_SEND_DATA_TO_DECOMPRESS: d1("SEND_DATA_TO_DECOMPRESS"); break;
            case UPD_PROGRAM_DECOMPRESSED_DATA: d1("PROGRAM_DECOMPRESSED_DATA"); break;
            case UPD_ERASE_COMPLETE_FLASH: d1("ERASE_COMPLETE_FLASH"); break;
            case UPD_ERASE_ADDRESSRANGE: d1("ERASE_ADDRESSRANGE"); break;
            case UPD_REQ_DATA: d1("REQ_DATA"); break;
            case UPD_DUMP_FLASH: d1("DUMP_FLASH"); break;
            case UPD_REQUEST_STATISTIC: d1("REQUEST_STATISTIC"); break;
            case UPD_RESPONSE_STATISTIC: d1("RESPONSE_STATISTIC"); break;
            case UPD_GET_LAST_ERROR: d1("GET_LAST_ERROR"); break;
            case UPD_SEND_LAST_ERROR: d1("SEND_LAST_ERROR"); break;
            case UPD_UNLOCK_DEVICE: d1("UNLOCK_DEVICE"); break;
            case UPD_REQUEST_UID: d1("REQUEST_UID"); break;
            case UPD_RESPONSE_UID: d1("RESPONSE_UID"); break;
            case UPD_APP_VERSION_REQUEST: d1("APP_VERSION_REQUEST"); break;
            case UPD_APP_VERSION_RESPONSE: d1("APP_VERSION_RESPONSE"); break;
            case UPD_RESET: d1("RESET"); break;
            case UPD_REQUEST_BOOT_DESC: d1("REQUEST_BOOT_DESC"); break;
            case UPD_RESPONSE_BOOT_DESC: d1("RESPONSE_BOOT_DESC"); break;
            case UPD_REQUEST_BL_IDENTITY: d1("REQUEST_BL_IDENTITY"); break;
            case UPD_RESPONSE_BL_IDENTITY: d1("RESPONSE_BL_IDENTITY"); break;
            case UPD_RESPONSE_BL_VERSION_MISMATCH: d1("RESPONSE_BL_VERSION_MISMATCH"); break;
            case UPD_SET_EMULATION: d1("SET_EMULATION"); break;
            default: serial.print("Command unknown", (unsigned int)cmd); break;
        }
        d1(" ");
    );
}

void updResult2Serial(UDP_State result)
{
    d3(
        d1("UPD_");
        switch (result)
        {
            case UDP_IAP_SUCCESS: d1("IAP OK"); break;
            case UDP_IAP_INVALID_COMMAND: d1("IAP_INVALID_COMMAND."); break;
            case UDP_IAP_SRC_ADDR_ERROR: d1("IAP_SRC_ADDR_ERROR"); break;
            case UDP_IAP_DST_ADDR_ERROR: d1("IAP_DST_ADDR_ERROR"); break;
            case UDP_IAP_SRC_ADDR_NOT_MAPPED: d1("IAP_SRC_ADDR_NOT_MAPPED"); break;
            case UDP_IAP_DST_ADDR_NOT_MAPPED: d1("IAP_DST_ADDR_NOT_MAPPED"); break;
            case UDP_IAP_COUNT_ERROR: d1("IAP_COUNT_ERROR"); break;
            case UDP_IAP_INVALID_SECTOR: d1("IAP_INVALID_SECTOR"); break;
            case UDP_IAP_SECTOR_NOT_BLANK: d1("IAP_SECTOR_NOT_BLANK"); break;
            case UDP_IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION: d1("IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION"); break;
            case UDP_IAP_COMPARE_ERROR: d1("IAP_COMPARE_ERRO"); break;
            case UDP_IAP_BUSY: d1("IAP_BUSY"); break;
            case UDP_UNKNOWN_COMMAND: d1("UNKNOWN_COMMAND"); break;
            case UDP_CRC_ERROR: d1("CRC_ERROR"); break;
            case UDP_ADDRESS_NOT_ALLOWED_TO_FLASH: d1("ADDRESS_NOT_ALLOWED_TO_FLASH"); break;
            case UDP_SECTOR_NOT_ALLOWED_TO_ERASE: d1("SECTOR_NOT_ALLOWED_TO_ERASE"); break;
            case UDP_RAM_BUFFER_OVERFLOW: d1("RAM_BUFFER_OVERFLOW"); break;
            case UDP_WRONG_DESCRIPTOR_BLOCK: d1("WRONG_DESCRIPTOR_BLOCK"); break;
            case UDP_APPLICATION_NOT_STARTABLE: d1("APPLICATION_NOT_STARTABLE"); break;
            case UDP_DEVICE_LOCKED: d1("DEVICE_LOCKED"); break;
            case UDP_UID_MISMATCH: d1("UID_MISMATCH"); break;
            case UDP_ERASE_FAILED: d1("ERASE_FAILED"); break;
            case UDP_FLASH_ERROR: d1("FLASH_ERROR"); break;
            case UDP_PAGE_NOT_ALLOWED_TO_ERASE: d1("PAGE_NOT_ALLOWED_TO_ERASE"); break;
            case UDP_ADDRESS_RANGE_NOT_ALLOWED_TO_ERASE: d1("ADDRESS_RANGE_NOT_ALLOWED_TO_ERASE"); break;
            case UDP_NOT_IMPLEMENTED: d1("NOT_IMPLEMENTED"); break;
            default: serial.print("State unknown ", (unsigned int)result); break;
        }
    );
}



/** @}*/
