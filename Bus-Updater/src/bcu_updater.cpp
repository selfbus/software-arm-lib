/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_BOOTLOADER_BCU Bus coupling unit (BCU)
 * @ingroup SBLIB_BOOTLOADER
 *
 * @{
 *
 * @file   bcu_updater.cpp
 * @author Martin Glueck <martin@mangari.org> Copyright (c) 2015
 * @author Stefan Haller Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#if defined(DUMP_TELEGRAMS_LVL2) && defined(DEBUG)
    #include <sblib/serial.h>
#endif

#include "bcu_updater.h"


// The EIB bus access object
Bus bus(timer16_1, PIN_EIB_RX, PIN_EIB_TX, CAP0, MAT0);

#if defined(DUMP_TELEGRAMS_LVL2) && defined(DEBUG)
#   define dump2(code) code
#else
#   define dump2(x)
#endif

void BcuUpdate::dumpTicks()
{
    dump2(
        unsigned int ticks = systemTime - lastTick;
        serial.print(telegramCount, DEC, 6);
        serial.print(" ");
        serial.print("t:", ticks, DEC, 6);
        serial.print(" ");
        if (ticks > 200)
        {
            serial.print("HIGH ");
        }
        lastTick = systemTime;
    );
}

void BcuUpdate::processTelegram()
{
    telegramCount++;
    /*
    dump2(
        serial.print("RX: ");
        for (volatile int i = 0; i < bus.telegramLen; i++)
        {
            serial.print(bus.telegram[i],HEX,2);
            serial.print(" ");
        }
        serial.print(" LEN: ");
        serial.println(bus.telegramLen,DEC,4);
    );
    */

    unsigned short destAddr = (unsigned short)((bus.telegram[3] << 8) | bus.telegram[4]);
    unsigned char tpci = bus.telegram[6] & 0xc3; // Transport control field (see KNX 3/3/4 p.6 TPDU)
    unsigned short apci = (unsigned short)(((bus.telegram[6] & 3) << 8) | bus.telegram[7]);
    dump2(byte isSequenced = (byte)((bus.telegram[6] >> 6) & 0x01););

    if ((bus.telegram[5] & 0x80) != 0) // discard non physical destination addresses
    {
        bus.discardReceivedTelegram();
        return;
    }

    if (destAddr != bus.ownAddress()) // discard telegrams not for us
    {
        bus.discardReceivedTelegram();
        return;
    }

    dump2(
        dumpTicks();
        if (isSequenced != 0)
        {
            serial.print("#", (bus.telegram[6] & 0b00111100) >> 2, DEC, 2);
            serial.print(" ");
        }
    );

    if (tpci & 0x80)  // A connection control command
    {
        processConControlTelegram(bus.telegram[6]);
    }
    else
    {
        processDirectTelegram(apci);
    }
    // At the end: discard the received telegram
    bus.discardReceivedTelegram();
}

void BcuUpdate::processDirectTelegram(int apci)
{
    dump2(serial.print("procDirectT "));
    const int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];
    const int senderSeqNo = bus.telegram[6] & 0x3c; // 0b00111100 // sequence number
    unsigned char sendAckTpu = 0;
    bool sendTel = false;

    if (connectedAddr != senderAddr) // ensure that the sender is correct
    {
        dump2(
                serial.print("connectedAddr 0x");
                serial.print(connectedAddr, HEX, 4);
                serial.print(" != 0x");
                serial.print(senderAddr, HEX, 4);
                serial.println(" senderAddr");
             );
        return;
    }

    connectedTime = systemTime;
    sendTelegram[6] = 0;

    int apciCommand = apci & APCI_GROUP_MASK;
    if ((apciCommand == APCI_MEMORY_READ_PDU) | (apciCommand == APCI_MEMORY_WRITE_PDU))
    {
        sendAckTpu = handleMemoryRequests(apciCommand, &sendTel, &bus.telegram[7]);
    }
    else if (apciCommand == APCI_RESTART_PDU)
    {
        // attention we check acpi not like before apciCommand!
        if (checkApciForMagicWord(apci, bus.telegram[8], bus.telegram[9]))
        {
            // special version of APCI_RESTART_TYPE1_PDU  used by Selfbus bootloader
            // restart with parameters, we need to start in flashmode
            dump2(serial.print("MAGIC "));
            unsigned int * magicWord = BOOTLOADER_MAGIC_ADDRESS;
            *magicWord = BOOTLOADER_MAGIC_WORD;
        }
        dump2(serial.println("APCI_RESTART_PDU"));
        sendAckTpu = T_ACK_PDU;
        sendRestartResponseControlTelegram(APCI_RESTART_RESPONSE_PDU, senderSeqNo, 0, 1); // we need 1s reset time
        restartRequest(RESET_DELAY_MS); // Software Reset
    }
    else
    {
        dump2(serial.println("APCI_UNKNOWN 0x", apciCommand, HEX, 4));
        sendAckTpu = T_NACK_PDU;  // Command not supported
        //XXX in case we run into this, maybe Reset?
    }

    if (sendAckTpu)
    {
        sendConControlTelegram(sendAckTpu, senderSeqNo);
    }
    else
    {
        sendCtrlTelegram[0] = 0;
        dump2(
            dumpTicks();
            serial.print("#", senderSeqNo >> 2, DEC, 2);
            serial.println(" sendAckTpu=false");
        );
    }

    if (sendTel)
    {
        telegramCount++;
        dump2(
            if (sendTelegram[6] & 0x40)
            {
                dumpTicks();
                serial.print("#", senderSeqNo >> 2, DEC, 2);
                serial.print(" ");
            }
            serial.println("TX-sendTel");
        );
        sendTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
        // 1+2 contain the sender address, which is set by bus.sendTelegram()
        sendTelegram[3] = static_cast<byte>(connectedAddr >> 8);
        sendTelegram[4] = static_cast<byte>(connectedAddr);

        if (sendTelegram[6] & 0x40) // Add the sequence number if applicable
        {
            sendTelegram[6] &= static_cast<byte>(~0x3c);
            sendTelegram[6] |= static_cast<byte>(connectedSeqNo);
            incConnectedSeqNo = true;
        }
        else
            incConnectedSeqNo = false;
        bus.sendTelegram(sendTelegram, telegramSize(sendTelegram));
    }
    else
    {
        dump2(
            dumpTicks();
            serial.print("#", senderSeqNo >> 2, DEC, 2);
            serial.println(" sendTel=false");
        );
    }
}

void BcuUpdate::processConControlTelegram(int tpci)
{
    dump2(serial.print("procControlT "));
    int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];

    if (tpci & 0x40)  // An acknowledgement
    {
        tpci &= 0xc3;
        if (tpci == T_ACK_PDU) // A positive acknowledgement
        {
            int curSeqNo = bus.telegram[6] & 0x3c;
            if (incConnectedSeqNo && connectedAddr == senderAddr && lastAckSeqNo !=  curSeqNo)
            {
                dump2(serial.println("RX-ACK"));
                connectedSeqNo += 4;
                connectedSeqNo &= 0x3c;
                incConnectedSeqNo = false;
                lastAckSeqNo = curSeqNo;
            }
            else
            {
                dump2(serial.println("RX-ACK error?"));
            }
        }
        else if (tpci == T_NACK_PDU)  // A negative acknowledgement
        {
            if (connectedAddr == senderAddr)
            {
                dump2(serial.println("RX-NACK"));
                sendConControlTelegram(T_DISCONNECT_PDU, 0);
                connectedAddr = 0;
                incConnectedSeqNo = false;
            }
        }

        incConnectedSeqNo = true;
    }
    else  // A connect/disconnect command
    {
        if (tpci == T_CONNECT_PDU)  // Open a direct data connection
        {
            dump2(serial.print("T_CONNECT_PDU "));
            if (connectedAddr == 0 || connectedAddr == senderAddr)
            {
                connectedTime = systemTime;
                lastTick = connectedTime;
                connectedAddr = senderAddr;
                telegramCount = 1;
                connectedSeqNo = 0;
                incConnectedSeqNo = false;
                dump2(serial.println("RX-CON"));
                bus.setSendAck(SB_BUS_ACK);
            }
            else
            {
                dump2(serial.println("error"));
            }
        }
        else if (tpci == T_DISCONNECT_PDU)  // Close the direct data connection
        {
            dump2(serial.print("T_DISCONNECT_PDU "));
            if (connectedAddr == senderAddr)
            {
                connectedAddr = 0;
                bus.setSendAck(SB_BUS_ACK);
                dump2(serial.println("RX-DIS"));
            }
            else
            {
                dump2(serial.println("error"));
            }
        }
        else
        {
            dump2(
                  serial.print("processConControlTelegram unknown tpci 0x");
                  serial.println(tpci, HEX, 4);
                  );
        }
    }
}

void BcuUpdate::sendConControlTelegram(int cmd, int senderSeqNo)
{
    telegramCount++;
    dump2(
        if (cmd & 0x40)
        {
            dumpTicks();
            serial.print("#", senderSeqNo >> 2, DEC, 2);
            serial.print(" ");
        }
        serial.print("sendConControl ");
        switch (cmd)
        {
           case T_ACK_PDU:
               serial.println("TX-ACK");
               break;
           case T_NACK_PDU:
               serial.println("TX-NACK");
               break;
           case T_CONNECT_PDU:
               serial.println("TX-CONNECT");
               break;
           case T_DISCONNECT_PDU:
               serial.println("TX-DISCONNECT");
               break;
           default:
               serial.println("TX-unknown");
        }
    );

    if (cmd & 0x40)  // Add the sequence number if the command shall contain it
        cmd |= senderSeqNo & 0x3c;

    sendCtrlTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    sendCtrlTelegram[3] = (byte)(connectedAddr >> 8);
    sendCtrlTelegram[4] = (byte)connectedAddr;
    sendCtrlTelegram[5] = (byte)0x60;
    sendCtrlTelegram[6] = (byte)cmd;

    bus.sendTelegram(sendCtrlTelegram, 7);
}

void BcuUpdate::sendRestartResponseControlTelegram(int senderSeqNo, int cmd, byte errorCode, unsigned int processTime)
{
    byte restartResponseTelegram[10]; // we need a longer buffer for connection control telegrams
    if (cmd & 0x40)  // Add the sequence number if the command shall contain it
        cmd |= senderSeqNo & 0x3c;

    restartResponseTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    restartResponseTelegram[3] = (byte)(connectedAddr >> 8);
    restartResponseTelegram[4] = (byte)connectedAddr;
    restartResponseTelegram[5] = (byte)(cmd >> 8);
    restartResponseTelegram[6] = (byte)(cmd & 0xff);
    restartResponseTelegram[7] = errorCode;
    restartResponseTelegram[8] = (byte)(processTime >> 8);
    restartResponseTelegram[9] = processTime & 0xff;
    telegramCount++;
    bus.sendTelegram(restartResponseTelegram, sizeof(restartResponseTelegram)/sizeof(restartResponseTelegram[0]));
}

void BcuUpdate::loop()
{
    if (!enabled)
        return;
    // Send a disconnect after 6 seconds inactivity
    if (connectedAddr && (elapsed(connectedTime) > BCU_DIRECT_CONNECTION_TIMEOUT_MS))
    {
        sendConControlTelegram(T_DISCONNECT_PDU, 0);
        connectedAddr = 0;
        resetProtocol();
    }
    BcuBase::loop();
}

/** @}*/
