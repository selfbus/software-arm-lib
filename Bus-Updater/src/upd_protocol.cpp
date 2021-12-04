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
        switch (cmd)
        {
            case UPD_ERASE_SECTOR: d1("UPD_ERASE_SECTOR"); break;
            case UPD_SEND_DATA: d1("UPD_SEND_DATA"); break;
            case UPD_PROGRAM: d1("UPD_PROGRAM"); break;
            case UPD_UPDATE_BOOT_DESC: d1("UPD_UPDATE_BOOT_DESC"); break;
            case UPD_SEND_DATA_TO_DECOMPRESS: d1("UPD_SEND_DATA_TO_DECOMPRESS"); break;
            case UPD_PROGRAM_DECOMPRESSED_DATA: d1("UPD_PROGRAM_DECOMPRESSED_DATA"); break;
            case UPD_ERASE_COMPLETE_FLASH: d1("UPD_ERASE_COMPLETE_FLASH"); break;
            case UPD_ERASE_ADDRESSRANGE: d1("UPD_ERASE_ADDRESSRANGE"); break;
            case UPD_REQ_DATA: d1("UPD_REQ_DATA"); break;
            case UPD_GET_LAST_ERROR: d1("UPD_GET_LAST_ERROR"); break;
            case UPD_SEND_LAST_ERROR: d1("UPD_SEND_LAST_ERROR"); break;
            case UPD_UNLOCK_DEVICE: d1("UPD_UNLOCK_DEVICE"); break;
            case UPD_REQUEST_UID: d1("UPD_REQUEST_UID"); break;
            case UPD_RESPONSE_UID: d1("UPD_RESPONSE_UID"); break;
            case UPD_APP_VERSION_REQUEST: d1("UPD_APP_VERSION_REQUEST"); break;
            case UPD_APP_VERSION_RESPONSE: d1("UPD_APP_VERSION_RESPONSE"); break;
            case UPD_RESET: d1("UPD_RESET"); break;
            case UPD_REQUEST_BOOT_DESC: d1("UPD_REQUEST_BOOT_DESC"); break;
            case UPD_RESPONSE_BOOT_DESC: d1("UPD_RESPONSE_BOOT_DESC"); break;
            case UPD_REQUEST_BL_IDENTITY: d1("UPD_REQUEST_BL_IDENTITY"); break;
            case UPD_RESPONSE_BL_IDENTITY: d1("UPD_RESPONSE_BL_IDENTITY"); break;
            case UPD_SET_EMULATION: d1("UPD_SET_EMULATION"); break;
            default:  d3(serial.print("UPD_Command unknown", cmd)); break;
        }
        d1(" ");
    );
}

void updResult2Serial(UDP_State result)
{
    d3(
        switch (result)
        {
            case UDP_IAP_SUCCESS: d1("IAP OK"); break;
            case UDP_IAP_INVALID_COMMAND: d1("IAP invalid command"); break;
            case UDP_IAP_SRC_ADDR_ERROR: d1("IAP source address error"); break;
            case UDP_IAP_DST_ADDR_ERROR: d1("IAP destination address error"); break;
            case UDP_IAP_SRC_ADDR_NOT_MAPPED: d1("IAP source address not mapped"); break;
            case UDP_IAP_DST_ADDR_NOT_MAPPED: d1("IAP destination address not mapped"); break;
            case UDP_IAP_COUNT_ERROR: d1("IAP count error"); break;
            case UDP_IAP_INVALID_SECTOR: d1("IAP invalid sector error"); break;
            case UDP_IAP_SECTOR_NOT_BLANK: d1("IAP sector not blank"); break;
            case UDP_IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION: d1("IAP sector not prepared for write operation"); break;
            case UDP_IAP_COMPARE_ERROR: d1("IAP compare error"); break;
            case UDP_IAP_BUSY: d1("IAP busy"); break;
            case UDP_UNKNOWN_COMMAND: d1("received command is not defined"); break;
            case UDP_CRC_ERROR: d1("CRC calculated on the device and by the PC Updater tool don't match"); break;
            case UDP_ADDRESS_NOT_ALLOWED_TO_FLASH: d1("specifed address cannot be programmed"); break;
            case UDP_SECTOR_NOT_ALLOWED_TO_ERASE: d1("the specified sector cannot be erased"); break;
            case UDP_RAM_BUFFER_OVERFLOW: d1("internal buffer for storing the data would overflow"); break;
            case UDP_WRONG_DESCRIPTOR_BLOCK: d1("the boot descriptor block does not exist"); break;
            case UDP_APPLICATION_NOT_STARTABLE: d1("the programmed application is not startable"); break;
            case UDP_DEVICE_LOCKED: d1("the device is locked"); break;
            case UDP_UID_MISMATCH: d1("UID sent to unlock the device is invalid"); break;
            case UDP_ERASE_FAILED: d1("page erase failed"); break;
            case UDP_FLASH_ERROR: d1("page program (flash) failed"); break;
            case UDP_PAGE_NOT_ALLOWED_TO_ERASE: d1("page not allowed to erase"); break;
            case UDP_ADDRESS_RANGE_NOT_ALLOWED_TO_ERASE: d1("address range not allowed to erase"); break;
            case UDP_NOT_IMPLEMENTED: d1("command is not yet implemented"); break;
            default:  serial.print("UDP_State unknown ", result); break;
        }
    );
}



/** @}*/
