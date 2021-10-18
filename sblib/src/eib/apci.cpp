/**************************************************************************//**
 * @addtogroup SBLIB Selfbus library
 * @defgroup EIB protocol
 * @brief   Magic word check for Bootloader and BCU
 * @details checks a APCI_RESTART_RESPONSE_PDU for the magic
 *          
 * @{
 *
 * @file   apci.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/eib/apci.h>

const bool checkApciForMagicWord(const int apci, byte eraseCode, byte channelNumber)
{
    if (!(apci & APCI_RESTART_RESPONSE_PDU))
    {
        return false;
    }

    // special version of APCI_RESTART_RESPONSE_PDU used by Selfbus bootloader
    // restart with parameters, special meaning of erase=0 and channel=255 for update mode
    return ((eraseCode == BOOTLOADER_MAGIC_ERASE) && (channelNumber == BOOTLOADER_MAGIC_CHANNEL));
}


/** @}*/
