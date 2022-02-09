/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Main Selfbus KNX-Library
 * @defgroup SBLIB_SUB_GROUP_1 Sub KNX Transport layer 4
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   tlayer4.cpp
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
#include <tlayer4.h>
#include "dump.h"

dump2(
    unsigned int lastTick = 0;        //!< last systemtick a telegram was received or sent
);

unsigned short telegramCount = 0;   //!< number of telegrams since system reset
unsigned short disconnectCount = 0; //!< number of disconnects since system reset

///\todo remove after bugfix and on release
unsigned short repeatedTelegramCount = 0;
unsigned short repeatedIgnoredTelegramCount = 0;
///\todo end of remove after bugfix and on release

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

void dumpTelegramBytes(bool tx, const unsigned char * telegram, const unsigned int length, const bool newLine = true)
{
    dump2(
        serial.print(LOG_SEP);
        if (tx)
        {
            serial.print("TX: ");
        }
        else
        {
            serial.print("RX: ");
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

void dumpState(TLayer4::TL4State dumpState)
{
    dump2(
        switch (dumpState)
        {
            case TLayer4::CLOSED:
                serial.print("CLSD");
                break;

            case TLayer4::OPEN_IDLE:
                serial.print("IDLE");
                break;

            case TLayer4::OPEN_WAIT:
                serial.print("WAIT");
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
        serial.print(LOG_SEP);
        serial.print("t:", ticks, DEC, 5);
        serial.print(LOG_SEP);
        if (ticks > LONG_PAUSE_THRESHOLD_MS)
        {
            serial.print("HIGH");
        }
        serial.print(LOG_SEP);
        serial.print(disconnectCount);
        serial.print(LOG_SEP);
        lastTick = systemTime;
    );
}

void dumpSequenceNumber(const unsigned int tpci)
{
    dump2(
        serial.print("#");
        if ((tpci & T_IS_SEQUENCED_Msk))
        {
            serial.print(getSequenceNumber(tpci), DEC, 2);
        }
        else
        {
            serial.print("xx");
        }
        serial.print(LOG_SEP);
    );
}

void dumpTelegramInfo(const unsigned int address, const unsigned int tpci, const bool isTX, const TLayer4::TL4State state)
{
    dump2(
        dumpTicks();
        dumpKNXAddress(address);
        serial.print(LOG_SEP);
        if (isTX)
        {
            serial.print("TX->");
        }
        else
        {
            serial.print("->RX");
        }
        serial.print(LOG_SEP);
        dumpSequenceNumber(tpci);
        dumpState(state);
        serial.print(LOG_SEP);

        if (tpci & T_CONNECTION_CTRL_COMMAND_Msk)
        {
            switch ((tpci & ~T_SEQUENCE_NUMBER_Msk))
            {
                case T_ACK_PDU:
                   serial.print("T_ACK    ");
                   break;
                case T_NACK_PDU:
                   serial.print("T_NACK   ");
                   break;
                case T_CONNECT_PDU:
                   serial.print("T_CONNECT");
                   break;
                case T_DISCONNECT_PDU:
                   serial.print("T_DISCONN");
                   break;
                default:
                   serial.print("T_unknown");
            }
        }
        else
        {
            serial.print("T_DATA   ");
        }
        serial.print(LOG_SEP);
    );
}

void dumpLogHeader()
{
    dump2(
        serial.println();
        serial.println("Keywords to search for: HIGH, ERROR, EVENT, HOTFIX");
        serial.println();
        serial.print("#Telegram");
        serial.print(LOG_SEP);
        serial.print("SysTicks");
        serial.print(LOG_SEP);
        serial.print("Warn");
        serial.print(LOG_SEP);
        serial.print("DC");
        serial.print(LOG_SEP);
        serial.print("KNX-Addr");
        serial.print(LOG_SEP);
        serial.print("TX/RX");
        serial.print(LOG_SEP);
        serial.print("#Seq");
        serial.print(LOG_SEP);
        serial.print("State");
        serial.print(LOG_SEP);
        serial.print("TPCI   ");
        serial.print(LOG_SEP);
        serial.print("Function");
        serial.print(LOG_SEP);
        serial.println("Additional info / Bytes");
    );
}

TLayer4::TLayer4()
{

}

void TLayer4::_begin()
{
    state = TLayer4::CLOSED;
    dumpLogHeader();
    telegramCount = 0;
    disconnectCount = 0;

    lastTelegramLength = 0;
    repeatedTelegramCount = 0;
    repeatedIgnoredTelegramCount = 0;
}

void TLayer4::processTelegram()
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

    unsigned char tpci = bus.telegram[6] & 0b11000011; // Transport control field (see KNX 3/3/4 p.6 TPDU)
    unsigned short apci = (unsigned short)(((bus.telegram[6] & 3) << 8) | bus.telegram[7]);
    unsigned int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];
    telegramCount++;

    dumpTelegramInfo(senderAddr, bus.telegram[6], false, state);

    if (!checkValidRepeatedTelegram())
    {
        bus.discardReceivedTelegram();
        return;
    }

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

void TLayer4::processConControlTelegram(const unsigned int senderAddr, const unsigned int tpci)
{
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
            dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen, true);
            serial.println("ERROR");
            serial.println("tpci              0x", tpci, HEX, 2);
              serial.print("connectedAddr     ");
            dumpKNXAddress(connectedAddrNew);
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

bool TLayer4::processConControlConnectPDU(int senderAddr)
{
    // event E00 and E01 handling
    dump2(
          serial.print("ConnectPDU");
          serial.print(LOG_SEP);
    );

    switch (state)
    {
        case TLayer4::CLOSED:
            actionA01Connect(senderAddr);
            return (true);
            break;

        case TLayer4::OPEN_IDLE:
        case TLayer4::OPEN_WAIT:
            if (connectedAddrNew == senderAddr)
            {
                // event E00
                setTL4State(TLayer4::CLOSED);
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

bool TLayer4::processConControlDisconnectPDU(int senderAddr)
{
    dump2(
          serial.print("T_DISCONNECT");
          serial.print(LOG_SEP);
    );
    // event E02 and E03 handling
    switch (state)
    {
        case TLayer4::CLOSED:
            actionA00Nothing();
            break;

        case TLayer4::OPEN_IDLE:
        case TLayer4::OPEN_WAIT:
            if (senderAddr == connectedAddrNew)
            {
                // event E02
                setTL4State(TLayer4::CLOSED);
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

bool TLayer4::processConControlAcknowledgmentPDU(int senderAddr, int tpci)
{
    if (!(tpci & T_ACKNOWLEDGE_Msk))
    {
        // not an acknowledgment
        dump2(
              serial.print("unknown tpci 0x");
              serial.print(tpci, HEX, 4);
              );
        dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
        return (false);
    }

    // from here on, it can only be a T_ACK_PDU or T_NACK_PDU
    const bool isACK = (!(tpci & T_NACK_Msk));

    if (isACK)
    {
        // it's a T_ACK_PDU
        dump2(
              serial.print("Acknowled");
              serial.print(LOG_SEP);
        );

        // check CLOSED state for E08, E09
        if (state == TLayer4::CLOSED)
        {
            dump2(serial.print(" EVENT 8/9/10 CLOSED "););
            actionA10Disconnect(senderAddr);
            dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
            return (false);
        }

        // check for event E10
        if (connectedAddrNew != senderAddr)
        {
            dump2(serial.print(" EVENT 10 "););

            actionA10Disconnect(senderAddr);
            dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
            return (false);
        }

        const byte curSeqNo = getSequenceNumber(bus.telegram[6]);
        // check if we got correct sequence number
        if (curSeqNo != seqNoSend)
        {
            // event E09
            dump2(
                serial.print("EVENT 9 curSeqNo ", curSeqNo);
                serial.print(" != ", seqNoSend);
                serial.print(" seqNoSend");
                serial.print(LOG_SEP);
                dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
            );
            actionA06Disconnect();
            setTL4State(TLayer4::CLOSED);
            return (false);
        }

        // check correct state for event E08
        if (state == TLayer4::OPEN_WAIT)
        {
            dump2(
                  serial.print("OK");
                  serial.print(LOG_SEP);
            );
            actionA08IncrementSequenceNumber();
            setTL4State(TLayer4::OPEN_IDLE);
            dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
            return (true);
        }
        else
        {
            dump2(serial.print(" EVENT 8 state != OPEN_WAIT"););
            actionA06Disconnect();
            setTL4State(TLayer4::CLOSED);
            dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
            return (false);
        }
    }
    else
    {
        // it's a T_NACK_PDU
        dump2(
              serial.print("T_NACK ERROR");
              serial.print(LOG_SEP);
        );

        // event E11b
        if (state == TLayer4::CLOSED)
        {
            dump2(serial.print(" EVENT 11b/14 "););
            actionA10Disconnect(senderAddr);
            dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
            return (false);
        }
        else if (connectedAddrNew != senderAddr) // check for event E14
        {
            dump2(serial.print(" EVENT 14"););
            actionA10Disconnect(senderAddr);
            dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
            return (false);
        }
        else
        {
            dump2(serial.print(" EVENT 11b_2 "););
            actionA06Disconnect();
            setTL4State(TLayer4::CLOSED);
            dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
            return (false);
        }
    }
}

void TLayer4::sendConControlTelegram(int cmd, unsigned int address, int senderSeqNo)
{
    if (cmd & 0x40)  // Add the sequence number if the command shall contain it
    {
        cmd |= (senderSeqNo << T_SEQUENCE_NUMBER_FIRST_BIT_Pos) & T_SEQUENCE_NUMBER_Msk;
    }
    telegramCount++;

    dump2(
        dumpTelegramInfo(address, cmd, true, state);
        serial.print("sendControl");
    );

    sendCtrlTelegram[0] = 0xb0; // connection controls always in system priority
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    sendCtrlTelegram[3] = (byte)(address >> 8);
    sendCtrlTelegram[4] = (byte)address;
    sendCtrlTelegram[5] = (byte)0x60;
    sendCtrlTelegram[6] = (byte)cmd;

    dump2(
        if (address != (unsigned int)connectedAddrNew)
        {
            serial.print(" ERROR");
        }
        unsigned int lengthCtrlTelegram = sizeof(sendCtrlTelegram) / sizeof(sendCtrlTelegram[0]);
        dumpTelegramBytes(true, &sendCtrlTelegram[0], lengthCtrlTelegram);
    );

    bus.sendTelegram(sendCtrlTelegram, 7);
}

void TLayer4::processDirectTelegram(int apci)
{
    dump2(
          serial.print("DirectTele");
          serial.print(LOG_SEP);
    );

    const int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];
    const byte seqNo = getSequenceNumber(bus.telegram[6]);

    // check for event E07 or CLOSED state of events E04, E05, E06
    if ((connectedAddrNew != senderAddr) || (state == TLayer4::CLOSED))
    {
        // event E07 or state CLOSED -> always action A10
        dump2(
                if (connectedAddrNew != senderAddr)
                {
                    serial.print("EVENT 7 ");
                }
                else
                {
                    serial.print("EVENT 4/5/6 ");
                }

                dumpState(state);
                serial.print(" connectedAddr ");
                dumpKNXAddress(connectedAddrNew);
                if (connectedAddrNew != senderAddr)
                {
                    serial.print(" != ");
                }
                else
                {
                    serial.print(" == ");
                }
                dumpKNXAddress(senderAddr);
                serial.print(" senderAddr ");
             );
        actionA10Disconnect(senderAddr);
        dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
        return;
    }

    if (seqNo == seqNoRcv)
    {
        ///\todo BUG event E04 needs more checks
        // event E04
        telegramReadyToSend = actionA02sendAckPduAndProcessApci(apci, seqNo, &bus.telegram[7]);
        /*
        // this either does not work, it's even worse
        if (state == TLayer4::OPEN_IDLE)
        {
            telegramReadyToSend = actionA02sendAckPduAndProcessApci(apci, seqNo, &bus.telegram[7]);
        }
        else
        {
            // according to KNX Spec 2.1, we should execute action A02
            // but that can lead to a infinite loop, instead we execute A06 to disconnect the connection
            dump2(serial.print("EVENT 4 DirectTelegram while waiting for T_ACK ");); ///\todo CHECK THIS
            setTL4State(TLayer4::CLOSED);
            actionA06Disconnect();
            dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
        }
        */
        return;
    }

    if (seqNo == ((seqNoRcv-1) & 0x0F))
    {
        // event E05
        dump2(
            serial.print("EVENT 5 seqNo ", seqNo);
            serial.print(" != ", (seqNoRcv-1) & 0x0F);
            serial.print(" seqNoRcv-1");
            serial.print(LOG_SEP);
            dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
        );
        actionA03sendAckPduAgain(seqNo);
        return;
    }

    // can only be event E06 ((SeqNo_of_PDU != SeqNoRcv ) and (SeqNo_of_PDU !=((SeqNoRcv-1)&Fh))
    dump2(
          serial.print("EVENT 6 out of sequence ");
    );
    actionA06Disconnect();
    setTL4State(TLayer4::CLOSED);
    dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
}

unsigned char TLayer4::processApci(int apci, const int senderAddr, const int senderSeqNo, bool * sendTel, unsigned char * data)
{
    dump2(
          serial.print("processApci");
          serial.print(LOG_SEP);
    );
    *sendTel = false;
    return (0); // we return nothing
}

void TLayer4::actionA00Nothing()
{
    dump2(serial.println("A00Nothing "));
}

void TLayer4::actionA01Connect(unsigned int address)
{
    dump2(serial.print("A01Connect with ");
          dumpKNXAddress(address);
          serial.print(LOG_SEP);
    );
    resetConnection();
    telegramReadyToSend = false;
    connectedAddrNew = address;
    seqNoSend = 0;
    seqNoRcv = 0;
    connectedTime = systemTime; // "start connection timeout timer"
    setTL4State(TLayer4::OPEN_IDLE);
    dump2(lastTick = connectedTime;); // for debug logging
}

bool TLayer4::actionA02sendAckPduAndProcessApci(int apci, const int seqNo, unsigned char * data)
{
    dump2(serial.print("actionA02 "));
    bool sendTel = false;
    processApci(apci, connectedAddrNew, seqNo, &sendTel, data);
    dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen);
    sendConControlTelegram(T_ACK_PDU, connectedAddrNew, seqNo);
    seqNoRcv++;
    seqNoRcv &= 0x0F;
    connectedTime = systemTime; // "restart the connection timeout timer"
    if (sendTel)
    {
        sendTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte, set same priority as received
    }
    return sendTel;
}

void TLayer4::actionA03sendAckPduAgain(const int seqNo)
{
    dump2(serial.println("ERROR A03sendAckPduAgain "));
    sendConControlTelegram(T_ACK_PDU, connectedAddrNew, seqNo);
    connectedTime = systemTime; // "restart the connection timeout timer"
}

void TLayer4::actionA05DisconnectUser()
{
    disconnectCount++;
    dump2(
        serial.print("A05DisconnectUser ");
    );
    resetConnection();
}

void TLayer4::actionA06Disconnect()
{
    disconnectCount++;
    setTL4State(TLayer4::CLOSED);
    dump2(
        serial.println("A06Disconnect");
    );
    sendConControlTelegram(T_DISCONNECT_PDU, connectedAddrNew, 0);
    resetConnection();
}

void TLayer4::actionA07SendDirectTelegram()
{
    if (state == CLOSED)
    {
        dump2(
              serial.print("ERROR A07SendTelegram Connection closed");
              serial.print(LOG_SEP);
        );
        return;
    }

    if (!telegramReadyToSend)
    {
        dump2(
              serial.print("ERROR A07SendTelegram Nothing to send");
              serial.print(LOG_SEP);
        );
        return;
    }

    // set sequence number
    sendTelegram[6] &= static_cast<byte>(~T_SEQUENCE_NUMBER_Msk);
    sendTelegram[6] |= static_cast<byte>(seqNoSend << T_SEQUENCE_NUMBER_FIRST_BIT_Pos);
    telegramCount++;

    dump2(
        dumpTelegramInfo(connectedAddrNew, sendTelegram[6], true, state);
        serial.print("sendDirectT");
        serial.print(LOG_SEP);
        serial.print("A07SendTelegram");
        serial.print(LOG_SEP);
    );

    // 0 priority is was already as set in actionA02sendAckPduAndProcessApci
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    sendTelegram[3] = static_cast<byte>(connectedAddrNew >> 8);
    sendTelegram[4] = static_cast<byte>(connectedAddrNew);

    setTL4State(TLayer4::OPEN_WAIT);

    dump2(
        dumpTelegramBytes(true, &sendTelegram[0], telegramSize(sendTelegram), true);
    );
    telegramReadyToSend = false;
    bus.sendTelegram(sendTelegram, telegramSize(sendTelegram));
    connectedTime = systemTime;
}

void TLayer4::actionA08IncrementSequenceNumber()
{
    dump2(serial.print("A08IncrementSequenceNumber "));
    seqNoSend++;
    seqNoSend &= 0x0F;
    connectedTime = systemTime; // "restart the connection timeout timer"
}

void TLayer4::actionA10Disconnect(unsigned int address)
{
    disconnectCount++;
    dump2(
        serial.println("ERROR A10Disconnect ");
    );
    sendConControlTelegram(T_DISCONNECT_PDU, address, 0);
}

void TLayer4::loop()
{
    if (!enabled)
        return;
    // Send a disconnect after TL4_CONNECTION_TIMEOUT_MS milliseconds inactivity
    if ((state != TLayer4::CLOSED) && (elapsed(connectedTime) >= TL4_CONNECTION_TIMEOUT_MS))
    {
        actionA06Disconnect();
        dump2(serial.println("direct connection timed out => disconnecting"));
    }

    // if ((state == TLayer4::OPEN_IDLE) && (telegramReadyToSend) && (bus.idle()))
    if (telegramReadyToSend && bus.idle())
    {
        // event E15
        actionA07SendDirectTelegram(); // this is event E15 to send our response
    }

    BcuBase::loop();
}

void TLayer4::resetConnection()
{
    dump2(serial.println("TLayer4::resetConnection"));
}

bool TLayer4::setTL4State(TLayer4::TL4State newState)
{
    dump2(
        dumpState(state);
        serial.print("->");
        dumpState(newState);
        serial.print(" ");
    );

    if (newState == state)
    {
        return (true);
    }
    state = newState;
    return (true);
}

void TLayer4::copyTelegram()
{
    for (unsigned short i = 0; i < bus.telegramLen; i++)
    {
        lastTelegram[i] = bus.telegram[i];
    }
    lastTelegramLength = bus.telegramLen;
}

bool TLayer4::checkValidRepeatedTelegram()
{
    if (!is_repeated(bus.telegram[0]))
    {
        // telegram is not repeated, buffer it for later checks
        copyTelegram();
        return true;
    }

    dump2(
        repeatedTelegramCount++;
        serial.println("REPEATED");
    );

    if ((lastTelegramLength < 1) || (lastTelegramLength != bus.telegramLen))
    {
        copyTelegram();
        return true;
    }

    // ignore repeated flag (5.bit) of control byte
    if (bus.telegram[0] != (lastTelegram[0] & 0xDF))
    {
        copyTelegram();
        return true;
    }

    // check remaining bytes, exclude control byte (1.byte) and checksum (last byte)
    for (int i = 1; i < bus.telegramLen - 1; i++)
    {
        if (lastTelegram[i] != bus.telegram[i])
        {
            // telegrams don't match, everything ok
            copyTelegram();
            return true;
        }
    }

    // we received a already processed telegram, ignore it
    dump2(
        repeatedIgnoredTelegramCount++;
        serial.print("IGNORED");
        serial.print(LOG_SEP);
        dumpTelegramBytes(false, &bus.telegram[0], bus.telegramLen, true);
    );
    return false;
}

/** @}*/
