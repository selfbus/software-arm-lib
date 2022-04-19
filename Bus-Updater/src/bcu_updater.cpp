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
#include "bcu_updater.h"
#include "dump.h"


// The EIB bus access object
// Bus bus(timer16_1, PIN_EIB_RX, PIN_EIB_TX, CAP0, MAT0);

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
    switch(apciCmd)
    {
        case APCI_MEMORY_WRITE_PDU:
#ifdef DEBUG
            if (checkCountToFail())
            {
                sendConControlTelegram(T_DISCONNECT_PDU, senderAddr, 0);
                *sendResponse = false;
                return (0);
            }
#endif
            *sendResponse = true;
            return (handleApciMemoryWriteRequest(&telegram[7])); ///\todo this 7 is not consistent with other telegram handling in sblib

        case APCI_BASIC_RESTART_PDU:
            dump2(serial.println("APCI_BASIC_RESTART_PDU"));
            d3(
                serial.println();serial.println();serial.println();
                serial.println("disconnectCount ", disconnectCount);

                ///\todo remove after fix and on release
                serial.println("repeated ", repeatedTelegramCount);
                serial.println("ignored  ", repeatedIgnoredTelegramCount);
                ///\todo end of remove after fix and on release

                serial.println();serial.println();serial.println();
                serial.flush();  // give time to send serial data
            );
            NVIC_SystemReset();
            return (T_ACK_PDU);

        default:
            return (T_NACK_PDU);
    }
}
/*
void BcuUpdate::loop()
{
    if (!enabled)
        return;
    //TLayer4::loop();
    BcuBase::loop();
    ///\todo MERGE
*/
/* partly already done in BcuBase::loop
    if (bus->telegramReceived() && (!bus->sendingTelegram()) && (bus->state == Bus::IDLE))
    {
        processTelegram(&bus->telegram[0], (uint8_t)bus->telegramLen); // if processed successfully, received telegram will be discarded by processTelegram()
    }

    if (progPin)
    {
        // Detect the falling edge of pressing the prog button
        pinMode(progPin, INPUT|PULL_UP);
        int oldValue = progButtonDebouncer.value();
        if (!progButtonDebouncer.debounce(digitalRead(progPin), 50) && oldValue)
        {
            userRam->status ^= 0x81;  // toggle programming mode and checksum bit
        }
        pinMode(progPin, OUTPUT);
        digitalWrite(progPin, (userRam->status & BCU_STATUS_PROG) ^ progPinInv);
    }
    */
//}

void BcuUpdate::begin()
{
    userRam->status = BCU_STATUS_LL | BCU_STATUS_TL | BCU_STATUS_AL | BCU_STATUS_USR;
    userRam->runState = 1;
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
