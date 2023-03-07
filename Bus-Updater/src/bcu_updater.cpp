/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_BOOTLOADER_BCU Bus coupling unit (BCU)
 * @ingroup SBLIB_BOOTLOADER
 *
 * @{
 *
 * @brief Implementation of the Bootloader's bus coupling unit (BCU 1)
 *
 * @file   bcu_updater.cpp
 * @author Martin Glueck <martin@mangari.org> Copyright (c) 2015
 * @author Stefan Haller Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/knx_tpdu.h>
#include <sblib/eib/apci.h>
#include <sblib/digital_pin.h>
#include <sblib/interrupt.h>
#include "bcu_updater.h"
#include "dump.h"

#ifdef DEBUG
#   define DEFAULT_COUNT_TO_FAIL (30)
    int defaultCountToFail = DEFAULT_COUNT_TO_FAIL;
    int countToFail = defaultCountToFail;

    bool checkCountToFail()
    {
        return (false); ///\done uncomment on release
        // ok lets drop connection for debugging
        countToFail--;
        if (countToFail)
        {
            return (false);
        }
        defaultCountToFail++;
        countToFail = defaultCountToFail;
        return (true);
    }
#endif

BcuUpdate::BcuUpdate() : BcuUpdate(new UserRamUpdater())
{
}

BcuUpdate::BcuUpdate(UserRamUpdater* userRamUpdater) :
    BcuBase(userRamUpdater, nullptr)
{
}

unsigned char BcuUpdate::processApci(ApciCommand apciCmd, const uint16_t senderAddr, const int8_t senderSeqNo,
        bool * sendResponse, unsigned char * telegram, uint8_t telLength)
{
    uint32_t offset = 7; ///\todo this offset is not consistent with other telegram handling in sblib
    uint32_t dataLength = telLength - offset - 1; // -1 exclude knx checksum
    uint32_t endDelay;

    switch(apciCmd)
    {
        case APCI_MEMORY_WRITE_PDU:
            *sendResponse = true;
            return (handleDeprecatedApciMemoryWrite());
        case APCI_USERMSG_MANUFACTURER_0:
            offset++;
            dataLength--;
            *sendResponse = true;
            return (handleApciUsermsgManufacturer(&telegram[offset], dataLength));

        case APCI_BASIC_RESTART_PDU:
            dump2(serial.println("APCI_BASIC_RESTART_PDU"));
            d3(
                serial.println();serial.println();serial.println();
                serial.println("disconnectCount ", disconnectCount);

                ///\todo remove after fix and on release
                serial.println("ignored  ", repeatedIgnoredTelegramCount);
                ///\todo end of remove after fix and on release

                serial.println();serial.println();serial.println();
                serial.flush();  // give time to send serial data
            );
            endDelay = millis() + RESET_DELAY_MS;
            while (millis() < endDelay)
            {
                waitForInterrupt();
            }
            NVIC_SystemReset();
            return (T_ACK_PDU);

        default:
            return (T_NACK_PDU);
    }
}

void BcuUpdate::begin()
{
    userRam->status() = BCU_STATUS_LINK_LAYER | BCU_STATUS_TRANSPORT_LAYER | BCU_STATUS_APPLICATION_LAYER | BCU_STATUS_USER_MODE;
    userRam->runState() = 1;
    BcuBase::_begin();
}

bool BcuUpdate::processBroadCastTelegram(ApciCommand apciCmd, unsigned char *telegram, uint8_t telLength)
{
    if (apciCmd == APCI_INDIVIDUAL_ADDRESS_READ_PDU)
    {
        sendApciIndividualAddressReadResponse();
    }
    return (true);
}

bool BcuUpdate::processGroupAddressTelegram(ApciCommand apciCmd, uint16_t groupAddress, unsigned char *telegram, uint8_t telLength)
{
    return (true);
}

/** @}*/
