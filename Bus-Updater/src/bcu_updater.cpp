/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_BOOTLOADER_BCU Bus coupling unit (BCU)
 * @ingroup SBLIB_BOOTLOADER
 *
 * @{
 *
 * @brief Implements the Bootloader's bus coupling unit (BCU 1)
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

unsigned char BcuUpdate::processApci(int apci, const int senderAddr, const int senderSeqNo, bool * sendTel, unsigned char * data)
{
    unsigned char sendAckTpu = 0;
    int apciCommand = apci & APCI_GROUP_MASK;
    switch(apciCommand)
    {
        case APCI_MEMORY_READ_PDU:
        case APCI_MEMORY_WRITE_PDU:
            sendAckTpu = handleMemoryRequests(apciCommand, sendTel, &bus.telegram[7]);
#ifdef DEBUG
            if (checkCountToFail())
            {
                sendConControlTelegram(T_DISCONNECT_PDU, senderAddr, 0);
                *sendTel = false;
                return (0);
            }
#endif
            break;

        case APCI_DEVICEDESCRIPTOR_READ_PDU:
            *sendTel = processDeviceDescriptorReadTelegram(apci & 0x3f);
            if (!sendTel)
            {
                sendAckTpu = T_NACK_PDU; ///\todo this is not correct, it must be always T_ACK_PDU
            }
            break;

        default:
            // attention we check acpi not like before apciCommand!
            switch (apci)
            {
                case APCI_BASIC_RESTART_PDU:
                    dump2(serial.println("APCI_BASIC_RESTART_PDU"));
                    restartRequest(RESET_DELAY_MS); // Software Reset
                    break;

                case APCI_MASTER_RESET_PDU:
                    dump2(serial.println("APCI_MASTER_RESET_PDU"));
                    if (processApciMasterResetPDU(apci, senderSeqNo, bus.telegram[8], bus.telegram[9]))
                    {
                        restartRequest(RESET_DELAY_MS); // Software Reset
                    }
                    // APCI_MASTER_RESET_PDU was not processed successfully send prepared response telegram
                    *sendTel = true;
                    break;
                    sendAckTpu = T_ACK_PDU;

                default:
                    dump2(serial.println("APCI_UNKNOWN 0x", apciCommand, HEX, 4));
                    sendAckTpu = T_NACK_PDU;  // Command not supported
            }
    }
    return (sendAckTpu);
}

void BcuUpdate::resetConnection()
{
    TLayer4::resetConnection();
}

bool BcuUpdate::processApciMasterResetPDU(int apci, const int senderSeqNo, byte eraseCode, byte channelNumber)
{
    // create the APCI_MASTER_RESET_RESPONSE_PDU
    sendTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    sendTelegram[3] = connectedTo() >> 8;
    sendTelegram[4] = connectedTo();
    sendTelegram[5] = 0x64; // length of the telegram is 4 bytes
    sendTelegram[6] = 0x40 | (APCI_MASTER_RESET_RESPONSE_PDU >> 8); // set first byte of apci
    sendTelegram[7] = APCI_MASTER_RESET_RESPONSE_PDU & 0xff; // set second byte of apci
    sendTelegram[7] |= 1; // set restart type to 1

    sendTelegram[8] = T_RESTART_UNSUPPORTED_ERASE_CODE; // set no error response
    // restart process time 2 byte unsigned integer value expressed in seconds
    // DPT_TimePeriodSec / DPT7.005
    sendTelegram[9] = 0;
    sendTelegram[10] = 6;

    if (apci != APCI_MASTER_RESET_PDU)
    {
        return (false);
    }
    ///\todo implement proper handling of APCI_MASTER_RESET_PDU for all other Erase Codes

    if (checkApciForMagicWord(apci, eraseCode, channelNumber))
    {
        // special version of APCI_MASTER_RESET_PDU used by Selfbus bootloader
        // set magicWord to start after reset in bootloader mode
        unsigned int * magicWord = BOOTLOADER_MAGIC_ADDRESS;
        *magicWord = BOOTLOADER_MAGIC_WORD;

        // Add the sequence number
        sendTelegram[6] &= ~0x3c;
        sendTelegram[6] |= sequenceNumberSend();
        // set no error
        sendTelegram[8] = T_RESTART_NO_ERROR;

        // send transport layer 4 ACK
        sendConControlTelegram(T_ACK_PDU, connectedTo(), sequenceNumberReceived());
        while (!bus.idle())
            ;
        // send APCI_MASTER_RESET_RESPONSE_PDU
        bus.sendTelegram(sendTelegram, telegramSize(sendTelegram));
        while (!bus.idle())
                    ;
        // send disconnect
        sendConControlTelegram(T_DISCONNECT_PDU, connectedTo(), 0);
        while (!bus.idle())
            ;
        NVIC_SystemReset();// Software Reset
    }

    return (false);
}

bool BcuUpdate::processDeviceDescriptorReadTelegram(int id)
{
    switch (id)
    {
    case 0:
        {
            int version = maskVersion();
            sendTelegram[5] = 0x63;
            sendTelegram[6] = 0x43;
            sendTelegram[7] = 0x40;
            sendTelegram[8] = version >> 8;
            sendTelegram[9] = version;

            return true;
        }

    default:
        return false; // unknown device descriptor
    }
    return false; // unknown device descriptor
}


/** @}*/
