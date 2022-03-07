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

#include <sblib/eib/knx_tpdu.h>
#include <sblib/eib/apci.h>
#include "bcu_updater.h"
#include "dump.h"


// The EIB bus access object
Bus bus(timer16_1, PIN_EIB_RX, PIN_EIB_TX, CAP0, MAT0);

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

unsigned char BcuUpdate::processApci(int apci, const int senderAddr, const int senderSeqNo, bool *sendResponse, unsigned char *telegram, unsigned short telLength)
{
    const unsigned short apciCommand = apci & APCI_GROUP_MASK;
    switch(apciCommand)
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
            return (handleMemoryRequests(apciCommand, sendResponse, &telegram[7])); ///\todo this 7 is not consistent with other telegram handling in sblib

        case APCI_BASIC_RESTART_PDU:
            dump2(serial.println("APCI_BASIC_RESTART_PDU"));
            restartRequest(RESET_DELAY_MS); // Software Reset
            return (T_ACK_PDU);

        default:
            return (T_NACK_PDU);
    }
}

void BcuUpdate::resetConnection()
{
    BcuBase::resetConnection();
}

bool BcuUpdate::processGroupAddressTelegram(unsigned char *telegram, unsigned short telLength)
{
    bus.discardReceivedTelegram();
    return (true);
}

bool BcuUpdate::processBroadCastTelegram(unsigned char *telegram, unsigned short telLength)
{
    bus.discardReceivedTelegram();
    return (true);
}


/** @}*/
