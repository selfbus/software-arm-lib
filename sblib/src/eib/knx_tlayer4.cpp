/**************************************************************************//**
 * @defgroup SBLIB_SUB_GROUP_KNX KNX Transport layer 4
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   knx_tlayer4.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/


#include <sblib/timeout.h>
#include <sblib/eib/knx_tlayer4.h>
#include <sblib/eib/apci.h>
#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/knx_tpdu.h>

///\todo implement better debugging
#if defined(DUMP_TL4)
#   define d1(x) {serial.print(x);}
#   define dline(x) {serial.println(x);}
#   define d2(u,v,w) {serial.print(u,v,w);}
#   define d3(x) {x;}
#   define dump2(code) code
#   define dumpKNXAddress(addr) \
            {\
                serial.print(knx_area(addr)); \
                serial.print(".", knx_line(addr)); \
                serial.print(".", knx_device(addr)); \
            }
#define LOG_SEP '\t'
#define knx_area(addr) ((addr >> 12) & 0x0F)
#define knx_line(addr) ((addr >> 8) & 0x0F)
#define knx_device(addr) (addr & 0xFF)
#else
#   define d1(x)
#   define d2(u,v,w)
#   define d3(x)
#   define dline(x)
#   define dump2(x)
#   define dumpKNXAddress(addr)
#endif

dump2(
    unsigned int lastTick = 0;        //!< last systemtick a telegram was received or sent
    unsigned short telegramCount = 0;   //!< number of telegrams since system reset
);

unsigned short disconnectCount = 0; //!< number of disconnects since system reset

///\todo remove after bugfix and on release
unsigned short repeatedTelegramCount = 0;
unsigned short repeatedIgnoredTelegramCount = 0;
unsigned short invalidCheckSum = 0;
///\todo end of remove after bugfix and on release

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

void dumpSequenceNumber(unsigned char *telegram, const unsigned int tpci)
{
    dump2(
        serial.print("#");
        if ((tpci & T_IS_SEQUENCED_Msk))
        {
            serial.print(sequenceNumber(telegram), DEC, 2);
        }
        else
        {
            serial.print("xx");
        }
        serial.print(LOG_SEP);
    );
}

void dumpTelegramInfo(unsigned char *telegram, const unsigned int address, const unsigned int tpci, const bool isTX, const TLayer4::TL4State state)
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
        dumpSequenceNumber(telegram, tpci);
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
    // set sending buffer to free
    sendTelegram[0] = 0;
    sendCtrlTelegram[0] = 0;

    dumpLogHeader();
    dump2(telegramCount = 0;);
    disconnectCount = 0;

    lastTelegramLength = 0;
    repeatedTelegramCount = 0;
    repeatedIgnoredTelegramCount = 0;
    invalidCheckSum = 0;
}

bool TLayer4::processTelegram(unsigned char *telegram, unsigned short telLength)
{
    bool telegramProcessed = processTelegramInternal(telegram, telLength);
    if (telegramProcessed)
    {
        bus.discardReceivedTelegram();
    }
    return (telegramProcessed);
}

bool TLayer4::processTelegramInternal(unsigned char *telegram, unsigned short telLength)
{
    if (!checksumValid(telegram, telLength))
    {

        return (true);
    }

    unsigned short destAddr = destinationAddress(telegram);
    ApciCommand apciCmd = apciCommand(telegram);

    if (destAddr == 0)
    {
        // a broadcast telegram
        return (processBroadCastTelegram(apciCmd, telegram, telLength));
    }
    else if ((telegram[5] & T_GROUP_ADDRESS_FLAG_Msk))
    {
        // a group address telegram
        return (processGroupAddressTelegram(apciCmd, destAddr, telegram, telLength));
    }
    else if (destAddr != ownAddress())
    {
        // not for us
        return (true);
    }

    unsigned char tpci = telegram[6] & 0b11000011; // Transport control field (see KNX 3/3/4 p.6 TPDU)
    unsigned int senderAddr = senderAddress(telegram);

    dump2(telegramCount++;);
    dumpTelegramInfo(telegram, senderAddr, telegram[6], false, state);

    if (!checkValidRepeatedTelegram(telegram, telLength))
    {
        // already processed
        return (true);
    }
    else
    {
        // telegram is not repeated, buffer it for later checks
        copyTelegram(telegram, telLength);
    }

    if (tpci & T_CONNECTION_CTRL_COMMAND_Msk)  // A connection control command
    {
        processConControlTelegram(senderAddr, telegram[6], telegram, telLength);
    }
    else
    {
        processDirectTelegram(apciCmd, telegram, telLength);
    }
    return (true);
}

void TLayer4::processConControlTelegram(const unsigned int senderAddr, const unsigned int tpci, unsigned char *telegram, unsigned short telLength)
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
            result = processConControlAcknowledgmentPDU(senderAddr, tpci, telegram, telLength);
    }

    if (!result)
    {
        dump2(
            dumpTelegramBytes(false, telegram, telLength, true);
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
                serial.println("curSeqNo         #", sequenceNumber(telegram), DEC, 2);
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
            if (connectedAddr == senderAddr)
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
            if (senderAddr == connectedAddr)
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

bool TLayer4::processConControlAcknowledgmentPDU(int senderAddr, int tpci, unsigned char *telegram, unsigned short telLength)
{
    if (!(tpci & T_ACKNOWLEDGE_Msk))
    {
        // not an acknowledgment
        dump2(
              serial.print("unknown tpci 0x");
              serial.print(tpci, HEX, 4);
              );
        dumpTelegramBytes(false, telegram, telLength);
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
            dumpTelegramBytes(false, telegram, telLength);
            return (false);
        }

        // check for event E10
        if (connectedAddr != senderAddr)
        {
            dump2(serial.print(" EVENT 10 "););

            actionA10Disconnect(senderAddr);
            dumpTelegramBytes(false, telegram, telLength);
            return (false);
        }

        const byte curSeqNo = sequenceNumber(telegram);
        // check if we got correct sequence number
        if (curSeqNo != seqNoSend)
        {
            // event E09
            dump2(
                serial.print("EVENT 9 curSeqNo ", curSeqNo);
                serial.print(" != ", seqNoSend);
                serial.print(" seqNoSend");
                serial.print(LOG_SEP);
                dumpTelegramBytes(false, telegram, telLength);
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
            dumpTelegramBytes(false, telegram, telLength);
            return (true);
        }
        else
        {
            dump2(serial.print(" EVENT 8 state != OPEN_WAIT"););
            actionA06Disconnect();
            setTL4State(TLayer4::CLOSED);
            dumpTelegramBytes(false, telegram, telLength);
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
            dumpTelegramBytes(false, telegram, telLength);
            return (false);
        }
        else if (connectedAddr != senderAddr) // check for event E14
        {
            dump2(serial.print(" EVENT 14"););
            actionA10Disconnect(senderAddr);
            dumpTelegramBytes(false, telegram, telLength);
            return (false);
        }
        else
        {
            dump2(serial.print(" EVENT 11b_2 "););
            actionA06Disconnect();
            setTL4State(TLayer4::CLOSED);
            dumpTelegramBytes(false, telegram, telLength);
            return (false);
        }
    }
}

void TLayer4::sendConControlTelegram(int cmd, unsigned int address, int senderSeqNo)
{
    if (cmd & T_SEQUENCED_COMMAND)  // Add the sequence number if the command shall contain it
    {
        cmd |= (senderSeqNo << T_SEQUENCE_NUMBER_FIRST_BIT_Pos) & T_SEQUENCE_NUMBER_Msk;
    }

    dump2(
        telegramCount++;
        dumpTelegramInfo(bus.telegram, address, cmd, true, state); ///\todo rewrite dumpTelegramInfo without using bus.telegram
        serial.print("sendControl");
    );

    initLpdu(sendCtrlTelegram, PRIORITY_SYSTEM, false, FRAME_STANDARD); // connection control commands always in system priority
    // sender address will be set by bus.sendTelegram()
    setDestinationAddress(sendCtrlTelegram, address);
    sendCtrlTelegram[5] = (byte)0x60; ///\todo set correct routing counter
    sendCtrlTelegram[6] = (byte)cmd;

    dump2(
        if (address != (unsigned int)connectedAddr)
        {
            serial.print(" ERROR");
        }
        unsigned int lengthCtrlTelegram = sizeof(sendCtrlTelegram) / sizeof(sendCtrlTelegram[0]);
        dumpTelegramBytes(true, &sendCtrlTelegram[0], lengthCtrlTelegram);
    );

    bus.sendTelegram(sendCtrlTelegram, 7);
}

void TLayer4::processDirectTelegram(ApciCommand apciCmd, unsigned char *telegram, unsigned short telLength)
{
    dump2(
          serial.print("DirectTele");
          serial.print(LOG_SEP);
    );

    const int senderAddr = senderAddress(telegram);
    const byte seqNo = sequenceNumber(telegram);

    // check for event E07 or CLOSED state of events E04, E05, E06
    if ((connectedAddr != senderAddr) || (state == TLayer4::CLOSED))
    {
        // event E07 or state CLOSED -> always action A10
        dump2(
                if (connectedAddr != senderAddr)
                {
                    serial.print("EVENT 7 ");
                }
                else
                {
                    serial.print("EVENT 4/5/6 ");
                }

                dumpState(state);
                serial.print(" connectedAddr ");
                dumpKNXAddress(connectedAddr);
                if (connectedAddr != senderAddr)
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
        dumpTelegramBytes(false, telegram, telLength);
        return;
    }

    if (seqNo == seqNoRcv)
    {
        ///\todo BUG event E04 needs more checks
        // event E04
        telegramReadyToSend = actionA02sendAckPduAndProcessApci(apciCmd, seqNo, telegram, telLength);
        /*
        // this either does not work, it's even worse
        if (state == TLayer4::OPEN_IDLE)
        {
            telegramReadyToSend = actionA02sendAckPduAndProcessApci(apci, seqNo, telegram, telLength);
        }
        else
        {
            // according to KNX Spec 2.1, we should execute action A02
            // but that can lead to a infinite loop, instead we execute A06 to disconnect the connection
            dump2(serial.print("EVENT 4 DirectTelegram while waiting for T_ACK ");); ///\todo CHECK THIS
            setTL4State(TLayer4::CLOSED);
            actionA06Disconnect();
            dumpTelegramBytes(false, telegram, telLength);
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
            dumpTelegramBytes(false, telegram, telLength);
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
    dumpTelegramBytes(false, telegram, telLength);
}

unsigned char TLayer4::processApci(ApciCommand apciCmd, const int senderAddr, const int senderSeqNo, bool * sendResponse, unsigned char * telegram, unsigned short telLength)
{
    dump2(
          serial.print("TLayer4::processApci");
          serial.print(LOG_SEP);
    );
    *sendResponse = false;
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
    connectedAddr = address;
    seqNoSend = 0;
    seqNoRcv = 0;
    connectedTime = systemTime; // "start connection timeout timer"
    setTL4State(TLayer4::OPEN_IDLE);
    dump2(lastTick = connectedTime;); // for debug logging
}

bool TLayer4::actionA02sendAckPduAndProcessApci(ApciCommand apciCmd, const int seqNo, unsigned char *telegram, unsigned short telLength)
{
    dump2(serial.print("actionA02 "));
    bool sendResponse = false;
    processApci(apciCmd, connectedAddr, seqNo, &sendResponse, telegram, telLength);
    dumpTelegramBytes(false,telegram, telLength);
    sendConControlTelegram(T_ACK_PDU, connectedAddr, seqNo); // KNX Spec. 3/3/4 5.5.4 p.26 "TL4 Style 1 Rationalised" No Sending of T_NAK frames
    seqNoRcv++;                 // increment sequence counter
    seqNoRcv &= 0x0F;           // handle overflow
    connectedTime = systemTime; // "restart the connection timeout timer"
    if (sendResponse)
    {
        ///\todo normally this has to be done in Layer 2
        initLpdu(sendTelegram, priority(telegram), false, FRAME_STANDARD); // same priority as received
        bus.discardReceivedTelegram();
        sendResponse = false;
        /* this will already be checked in bus.sendTelegram()
        while ((bus.sendCurTelegram != 0) || (bus.sendNextTel != 0))
               ;
        */
        telegramReadyToSend = true;
        actionA07SendDirectTelegram();
        //while (true)
        //    waitForInterrupt();
    }
    return sendResponse;
}

void TLayer4::actionA03sendAckPduAgain(const int seqNo)
{
    dump2(serial.println("ERROR A03sendAckPduAgain "));
    sendConControlTelegram(T_ACK_PDU, connectedAddr, seqNo);
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
    sendConControlTelegram(T_DISCONNECT_PDU, connectedAddr, 0);
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

    setSequenceNumber(sendTelegram, seqNoSend);

    dump2(
        telegramCount++;
        dumpTelegramInfo(sendTelegram, connectedAddr, sendTelegram[6], true, state);
        serial.print("sendDirectT");
        serial.print(LOG_SEP);
        serial.print("A07SendTelegram");
        serial.print(LOG_SEP);
    );

    // priority already set in actionA02sendAckPduAndProcessApci
    // sender address will be set by bus.sendTelegram()
    setDestinationAddress(sendTelegram, connectedAddr);
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

/* This is now done in actionA02sendAckPduAndProcessApci
    if ((state == TLayer4::OPEN_IDLE) && (telegramReadyToSend) && (bus.idle())) ///\todo this is according to spec
    // if (telegramReadyToSend && bus.idle()) // but this worked better
    {
        // event E15
        actionA07SendDirectTelegram(); // this is event E15 to send our response
    }
*/
}

void TLayer4::resetConnection()
{
    dump2(
          serial.print("TLayer4::resetConnection");
          serial.print(LOG_SEP)
    );
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

void TLayer4::copyTelegram(unsigned char *telegram, unsigned short telLength)
{
    for (unsigned short i = 0; i < telLength; i++)
    {
        lastTelegram[i] = telegram[i];
    }
    lastTelegramLength = telLength;
}

bool TLayer4::checkValidRepeatedTelegram(unsigned char *telegram, unsigned short telLength)
{
    if (!isRepeated(telegram))
    {
        return true;
    }

    dump2(
        repeatedTelegramCount++;
        serial.println("REPEATED");
    );

    if ((lastTelegramLength < 1) || (lastTelegramLength != telLength))
    {
        return true;
    }

    // check controlByte of last and new telegram, ignoring the repeated flag
    setRepeated(lastTelegram, true);
    if (controlByte(telegram) != controlByte(lastTelegram))
    {
        return true;
    }

    // check remaining bytes, exclude control byte (1.byte) and checksum (last byte)
    for (int i = 1; i < telLength - 1; i++)
    {
        if (lastTelegram[i] != telegram[i])
        {
            // telegrams don't match, everything ok
            return true;
        }
    }

    // we received a already processed telegram, ignore it
    dump2(
        repeatedIgnoredTelegramCount++;
        serial.print("IGNORED");
        serial.print(LOG_SEP);
        dumpTelegramBytes(false, telegram, telLength, true);
    );
    return false;
}

///\todo remove on release
bool TLayer4::checksumValid(unsigned char *telegram, unsigned short telLength)
{
    unsigned char checksum = 0xff;
    // Calculate the checksum
    for (unsigned short i = 0; i < (telLength-1); i++)
    {
        checksum ^= telegram[i];
    }

    if (telegram[telLength-1] == checksum)
    {
        return (true);
    }

    dump2(
        invalidCheckSum++;
        serial.print("ERROR CHECKSUM");
        serial.print(LOG_SEP);
        dumpTelegramBytes(false, telegram, telLength, true);
    );
    return (false);
}

/** @}*/
