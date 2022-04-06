/**************************************************************************//**
 * @addtogroup SBLIB Selfbus library
 * @defgroup
 * @brief
 * @details
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
#include <sblib/utils.h>

ApciCommand apciCommand(unsigned char *telegram)
{
    unsigned short apci = (unsigned short)(((telegram[APCI_HIGH_BYTE] & 0x03) << 8) | telegram[APCI_LOW_BYTE]);
    unsigned short shortCommand = apci & APCI_GROUP_MASK;
    switch (shortCommand)
    {
        case APCI_ADC_READ_PDU:
        case APCI_MEMORY_READ_PDU:
        case APCI_MEMORY_WRITE_PDU:
        case APCI_DEVICEDESCRIPTOR_READ_PDU:
            return ((ApciCommand) shortCommand);

        default:
            return ((ApciCommand) apci);
    }
}

void setApciCommand(unsigned char *telegram, ApciCommand newApciCommand, byte additionalData)
{
    telegram[APCI_HIGH_BYTE] = HIGH_BYTE(newApciCommand);
    telegram[APCI_LOW_BYTE] = LOW_BYTE(newApciCommand);
    telegram[APCI_LOW_BYTE] |= additionalData;
}

bool checkApciForMagicWord(byte eraseCode, byte channelNumber)
{
    // special version of APCI_MASTER_RESET_PDU used by Selfbus bootloader
    // restart with parameters, special meaning of erase=7 and channel=255 for bootloader mode
    return ((eraseCode == BOOTLOADER_MAGIC_ERASE) && (channelNumber == BOOTLOADER_MAGIC_CHANNEL));
}


/** @}*/
