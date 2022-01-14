/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Main Group Description
 * @defgroup SBLIB_SUB_GROUP_1 Sub Group 1 Description
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   bcu_layer4.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/eib/apci.h>
#include <sblib/timeout.h>
#include "bcu_layer4.h"
#include "dump.h"

dump2(
    unsigned int lastTick = 0;      //!< last systemtick a telegram was received or sent
    unsigned int telegramCount = 0; //!< number of telegrams received in one "connect" session
);

byte getSequenceNumber(byte tpci)
{
    if (tpci != 0xFF)
    {
        return ((tpci & T_SEQUENCE_NUMBER_Msk) >> T_SEQUENCE_NUMBER_FIRST_BIT_Pos);
    }
    else
    {
        return (-1);
    }
}

void dumpTelegram(bool tx, const unsigned char * telegram, const unsigned int length, const bool newLine = true)
{
    dump2(
        if (tx)
        {
            serial.print(" TX: ");
        }
        else
        {
            serial.print(" RX: ");
        }

        for (unsigned int i = 0; i < length; i++)
        {
            serial.print(telegram[i], HEX, 2);
            serial.print(" ");
        }

        serial.print(" #");
        serial.print(length, DEC);

        if (newLine)
        {
            serial.println();
        }
    );
}

void dumpState(BcuLayer4::TL4State dumpState)
{
    dump2(
        switch (dumpState)
        {
            case BcuLayer4::CLOSED:
                serial.print("CLOSED ");
                break;

            case BcuLayer4::OPEN_IDLE:
                serial.print("OPEN_IDLE ");
                break;

            case BcuLayer4::OPEN_WAIT:
                serial.print("OPEN_WAIT ");
                break;

            default:
                serial.print("ERROR should never land here ");
        }
    );
}

void dumpTicks()
{
    dump2(
        unsigned int ticks = systemTime - lastTick;
        serial.print(telegramCount, DEC, 6);
        serial.print(" ");
        serial.print("t:", ticks, DEC, 5);
        serial.print(" ");
        if (ticks > LONG_PAUSE_THRESHOLD_MS)
        {
            serial.print("HIGH ");
        }
        lastTick = systemTime;
    );
}

BcuLayer4::BcuLayer4()
{

}

void BcuLayer4::processTelegram()
{
    if ((bus.telegram[5] & T_GROUP_ADDRESS_FLAG_Msk))
    {
        // discard non physical destination addresses
        bus.discardReceivedTelegram();
        return;
    }

    unsigned short destAddr = (unsigned short)((bus.telegram[3] << 8) | bus.telegram[4]);
    if (destAddr != bus.ownAddress())
    {
        // discard telegrams not for us
        bus.discardReceivedTelegram();
        return;
    }

    dump2(telegramCount++;);
    unsigned char tpci = bus.telegram[6] & 0xc3; // Transport control field (see KNX 3/3/4 p.6 TPDU)
    unsigned short apci = (unsigned short)(((bus.telegram[6] & 3) << 8) | bus.telegram[7]);
    unsigned int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];

    dump2(
        dumpTicks();
        bool isSequenced = (bool)(bus.telegram[6] & T_IS_SEQUENCED_Msk);
        if (isSequenced != 0)
        {
            serial.print("#", getSequenceNumber(bus.telegram[6]), DEC, 2);
            serial.print(" ");
        }
    );

    if (tpci & T_CONNECTION_CTRL_COMMAND_Msk)  // A connection control command
    {
        processConControlTelegram(senderAddr, bus.telegram[6]);
    }
    else
    {
        processDirectTelegram(apci);
    }
    // At the end: discard the received telegram
    bus.discardReceivedTelegram();
}

void BcuLayer4::processConControlTelegram(const unsigned int senderAddr, const unsigned int tpci)
{
    dump2(
          serial.print("controlT from ");
          dumpKNXAddress(senderAddr);
          serial.print(" ");
    );

    bool result = false;
    switch (tpci)
    {
        case T_CONNECT_PDU: // open a TP layer 4 direct data connection
            result = processConControlConnectPDU(senderAddr);
            break;

        case T_DISCONNECT_PDU: // close the TP layer 4 direct data connection
            result = processConControlDisconnectPDU(senderAddr);
            break;

        default:
            result = processConControlAcknowledgmentPDU(senderAddr, tpci);
    }


    if (!result)
    {
        dump2(
            dumpTelegram(false, &bus.telegram[0], bus.telegramLen, true);
            serial.println("ERROR");
            serial.println("tpci              0x", tpci, HEX, 2);
              serial.print("connectedAddr     ");
            dumpKNXAddress(connectedAddr);
            serial.println();
              serial.print("senderAddr        ");
            dumpKNXAddress(senderAddr);
            serial.println();
            serial.println("seqNoSend        #", seqNoSend, DEC, 2);
            serial.println("seqNoRcv         #", seqNoRcv , DEC, 2);
            if ((tpci != T_CONNECT_PDU) && (tpci !=T_DISCONNECT_PDU))
            {
                serial.println("curSeqNo         #", getSequenceNumber(bus.telegram[6]), DEC, 2);
            }
        );
    }
}

bool BcuLayer4::processConControlConnectPDU(int senderAddr)
{
    // event E00 and E01 handling
    dump2(serial.print("T_CONNECT_PDU "));
    switch (state)
    {
        case BcuLayer4::CLOSED:
            setTL4State(BcuLayer4::OPEN_IDLE);
            actionA01Connect(senderAddr);
            return (true);
            break;

        case BcuLayer4::OPEN_IDLE:
        case BcuLayer4::OPEN_WAIT:
            if (connectedAddr == senderAddr)
            {
                // event E00
                setTL4State(BcuLayer4::CLOSED);
                actionA06Disconnect();
            }
            else
            {
                // event E01
                actionA10Disconnect(senderAddr);
            }
            break;

        default:
            dump2(serial.print("ERROR processConControlConnectPDU should never land here"));
    }
    return (false);
}

bool BcuLayer4::processConControlDisconnectPDU(int senderAddr)
{
    dump2(serial.print("T_DISCONNECT_PDU "));
    // event E02 and E03 handling
    switch (state)
    {
        case BcuLayer4::CLOSED:
            actionA00Nothing();
            break;

        case BcuLayer4::OPEN_IDLE:
        case BcuLayer4::OPEN_WAIT:
            if (senderAddr == connectedAddr)
            {
                // event E02
                setTL4State(BcuLayer4::CLOSED);
                actionA05DisconnectUser();
            }
            else
            {
                // event E03
                actionA00Nothing();
            }
            break;

        default:
            dump2(serial.print("ERROR processConControlDisconnectPDU should never land here"));
            return (false);
    }
    return (true);
}

bool BcuLayer4::processConControlAcknowledgmentPDU(int senderAddr, int tpci)
{
    if (!(tpci & T_ACKNOWLEDGE_Msk))
    {
        // not an acknowledgment
        dump2(
              serial.print("unknown tpci 0x");
              serial.print(tpci, HEX, 4);
              );
        return (false);
    }

    // from here on, it can only be a T_ACK_PDU or T_NACK_PDU
    const bool isACK = (!(tpci & T_NACK_Msk));

    if (isACK)
    {
        // it's a T_ACK_PDU
        dump2(serial.print("T_ACK "));

        // check for event E10
        if (connectedAddr != senderAddr)
        {
            actionA10Disconnect(senderAddr);
            return (false);
        }

        // check CLOSED state for E08, E09
        if (state == BcuLayer4::CLOSED)
        {
            actionA10Disconnect(senderAddr);
            return (false);
        }

        const byte curSeqNo = getSequenceNumber(bus.telegram[6]);
        // check if we got correct sequence number
        if (curSeqNo != seqNoSend)
        {
            // event E09
            actionA06Disconnect();
            setTL4State(BcuLayer4::CLOSED);
            return (false);
        }

        // check correct state for event E08
        if (state == BcuLayer4::OPEN_WAIT)
        {
            actionA08IncrementSequenceNumber();
            setTL4State(BcuLayer4::OPEN_IDLE);
            dump2(serial.println(););
            return (true);
        }
        else
        {
            actionA06Disconnect();
            setTL4State(BcuLayer4::CLOSED);
            return (false);
        }
    }
    else
    {
        // it's a T_NACK_PDU
        dump2(serial.print("T_NACK ERROR "));

        // check for event E14
        if (connectedAddr != senderAddr)
        {
            actionA10Disconnect(senderAddr);
            return (false);
        }

        // event E11b
        if (state == BcuLayer4::CLOSED)
        {
            actionA10Disconnect(connectedAddr);
            return (false);
        }
        else
        {
            actionA06Disconnect();
            setTL4State(BcuLayer4::CLOSED);
            return (false);
        }
    }
}

void BcuLayer4::sendConControlTelegram(int cmd, unsigned int address, int senderSeqNo)
{
    dump2(
        telegramCount++;
        if (cmd & 0x40)
        {
            dumpTicks();
            serial.print("#", senderSeqNo, DEC, 2);
            serial.print(" ");
        }
        serial.print("controlT to   "); // spaces for better format in log
        dumpKNXAddress(address);
        serial.print(" ");
    );

    sendCtrlTelegram[0] = 0xb0; // connection controls always in system priority
    switch (cmd)
    {
        case T_ACK_PDU:
           dump2(serial.print("T_ACK"););
           break;
        case T_NACK_PDU:
           dump2(serial.print("T_NACK"););
           break;
        case T_CONNECT_PDU:
           dump2(serial.print("T_CONNECT"););
           break;
        case T_DISCONNECT_PDU:
           dump2(serial.print("T_DISCONNECT"););
           break;
        default:
           dump2(serial.print("TX-unknown"););
    }

    if (cmd & 0x40)  // Add the sequence number if the command shall contain it
    {
        cmd |= (senderSeqNo << T_SEQUENCE_NUMBER_FIRST_BIT_Pos) & T_SEQUENCE_NUMBER_Msk;
    }

    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    sendCtrlTelegram[3] = (byte)(address >> 8);
    sendCtrlTelegram[4] = (byte)address;
    sendCtrlTelegram[5] = (byte)0x60;
    sendCtrlTelegram[6] = (byte)cmd;

    dump2(
        if (address != (unsigned int)connectedAddr)
        {
            serial.print(" ERROR");
        }
        unsigned int lengthCtrlTelegram = sizeof(sendCtrlTelegram) / sizeof(sendCtrlTelegram[0]);
        dumpTelegram(true, &sendCtrlTelegram[0], lengthCtrlTelegram);
    );

    bus.sendTelegram(sendCtrlTelegram, 7);
}

void BcuLayer4::processDirectTelegram(int apci)
{
    const int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];
    dump2(
          serial.print("directT  from ");  // double space for better format in log
          dumpKNXAddress(senderAddr);
          serial.print(" ");
    );

    const byte seqNo = getSequenceNumber(bus.telegram[6] & T_SEQUENCE_NUMBER_Msk);

    // check for event E07 or CLOSED state of events E04, E05, E06
    if ((connectedAddr != senderAddr) || (state == BcuLayer4::CLOSED))
    {
        // event E07 or state CLOSED -> always action A10
        dump2(
                serial.print("ERROR");
                serial.print(" connectedAddr ");
                dumpKNXAddress(connectedAddr);
                serial.print(" != ");
                dumpKNXAddress(senderAddr);
                serial.print(" senderAddr");
                dumpTelegram(false, &bus.telegram[0], bus.telegramLen);
             );
        actionA10Disconnect(senderAddr);
        return;
    }

    if (seqNo == seqNoRcv)
    {
        // event E04
        if (state == BcuLayer4::OPEN_IDLE)
        {
            telegramReadyToSend = actionA02sendAckPduAndProcessApci(apci, seqNo, &bus.telegram[7]);
        }
        else
        {
            // according to KNX Spec 2.1, we should execute action A02
            // but that can lead to a infinite loop, instead we execute A06 to disconnect the connection
            dump2(serial.println("ERROR DirectTelegram while waiting for T_ACK"););
            actionA06Disconnect();
            setTL4State(BcuLayer4::CLOSED);
        }
        return;
    }

    if (seqNo == ((seqNoRcv-1) & 0x0F))
    {
        // event E05
        actionA03sendAckPduAgain(seqNo);
        return;
    }

    // can only be event E06 ((SeqNo_of_PDU != SeqNoRcv ) and (SeqNo_of_PDU !=((SeqNoRcv-1)&Fh))
    dump2(
          serial.print("ERROR out of sequence ");
          dumpTelegram(false, &bus.telegram[0], bus.telegramLen);
    );
    actionA06Disconnect();
    setTL4State(BcuLayer4::CLOSED);
}

unsigned char BcuLayer4::processApci(int apci, const int senderAddr, const int senderSeqNo, bool * sendTel, unsigned char * data)
{
    dump2(serial.println("processApci"));
    *sendTel = false;
    return (0); // we return nothing
}

void BcuLayer4::actionA00Nothing()
{
    dump2(serial.println("A00Nothing"));
}

void BcuLayer4::actionA01Connect(unsigned int address)
{
    dump2(serial.print("A01Connect with ");
          dumpKNXAddress(address);
          serial.println();
    );
    resetConnection();
    connectedAddr = address;
    seqNoSend = 0;
    seqNoRcv = 0;
    connectedTime = systemTime; // "start connection timeout timer"
    dump2(lastTick = connectedTime;);   // for debug logging
}

bool BcuLayer4::actionA02sendAckPduAndProcessApci(int apci, const int seqNo, unsigned char * data)
{
    dump2(serial.print("actionA02 "));
    bool sendTel = false;
    processApci(apci, connectedAddr, seqNo, &sendTel, data);
    sendConControlTelegram(T_ACK_PDU, connectedAddr, seqNo);
    seqNoRcv++;
    seqNoRcv &= 0x0F;
    connectedTime = systemTime; // "restart the connection timeout timer"
    return sendTel;
}

void BcuLayer4::actionA03sendAckPduAgain(const int seqNo)
{
    dump2(serial.println("ERROR A03sendAckPduAgain"));
    sendConControlTelegram(T_ACK_PDU, connectedAddr, seqNo);
    connectedTime = systemTime; // "restart the connection timeout timer"
}

void BcuLayer4::actionA05DisconnectUser()
{
    dump2(serial.println("A05DisconnectUser"));
    resetConnection();
}

void BcuLayer4::actionA06Disconnect()
{
    dump2(serial.println("A06Disconnect"));
    sendConControlTelegram(T_DISCONNECT_PDU, connectedAddr, 0);
    resetConnection();
}

void BcuLayer4::actionA07SendDirectTelegram()
{
    if ((state != OPEN_IDLE) || (!telegramReadyToSend))
    {
        dump2(serial.println("ERROR A07SendTelegram"););
        return;
    }

    dump2(
        telegramCount++;
        if (sendTelegram[6] & 0x40)
        {
            dumpTicks();
            serial.print("#", seqNoSend, DEC, 2);
            serial.print(" ");
        }
        serial.print("directT  to   "); // spaces for better format in log
        dumpKNXAddress(connectedAddr);
        serial.print(" ");
    );

    sendTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    sendTelegram[3] = static_cast<byte>(connectedAddr >> 8);
    sendTelegram[4] = static_cast<byte>(connectedAddr);

    sendTelegram[6] &= static_cast<byte>(~T_SEQUENCE_NUMBER_Msk);
    sendTelegram[6] |= static_cast<byte>(seqNoSend << T_SEQUENCE_NUMBER_FIRST_BIT_Pos);

    setTL4State(BcuLayer4::OPEN_WAIT);

    dump2(
        serial.print(" ");
        dumpTelegram(true, &sendTelegram[0], telegramSize(sendTelegram), true);
    );
    bus.sendTelegram(sendTelegram, telegramSize(sendTelegram));
    connectedTime = systemTime;
}

void BcuLayer4::actionA08IncrementSequenceNumber()
{
    seqNoSend++;
    seqNoSend &= 0x0F;
    connectedTime = systemTime; // "restart the connection timeout timer"
}

void BcuLayer4::actionA10Disconnect(unsigned int address)
{
    dump2(serial.println("A10Disconnect"));
    sendConControlTelegram(T_DISCONNECT_PDU, address, 0);
    resetConnection();
}

void BcuLayer4::loop()
{
    if (!enabled)
        return;
    // Send a disconnect after TL4_CONNECTION_TIMEOUT_MS milliseconds inactivity
    if ((state != BcuLayer4::CLOSED) && (elapsed(connectedTime) > TL4_CONNECTION_TIMEOUT_MS))
    {
        actionA06Disconnect();
        setTL4State(BcuLayer4::CLOSED);
        //sendConControlTelegram(T_DISCONNECT_PDU, connectedAddr, 0);
        dump2(serial.println("direct connection timed out => disconnecting"));
        resetConnection();
    }

    if ((state == BcuLayer4::OPEN_IDLE) && (telegramReadyToSend))
    {
        // event E15
        actionA07SendDirectTelegram(); // this is event E15 to send our response
        telegramReadyToSend = false;
    }

    BcuBase::loop();
}

void BcuLayer4::resetConnection()
{
    dump2(serial.println("BcuLayer4::resetConnection"));
}

bool BcuLayer4::setTL4State(BcuLayer4::TL4State newState)
{
    if (newState == state)
    {
        dumpState(state);
        return (true);
    }

    dump2(
        dumpState(state);
        serial.print("-> ");
        dumpState(newState);
    );

    state = newState;

    return (true);
}

/** @}*/
