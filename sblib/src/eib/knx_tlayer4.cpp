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
#include <sblib/timer.h>
#include <sblib/eib/knx_tlayer4.h>
#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/knx_npdu.h>
#include <sblib/libconfig.h>
#include <cstring>

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

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
    uint32_t lastTick = 0;  //!< last systemtick a telegram was received or sent
    uint16_t telegramCount = 0; //!< number of telegrams since system reset
)

uint16_t disconnectCount = 0; //!< number of disconnects since system reset
uint16_t repeatedT_ACKcount = 0;

void dumpTelegramBytes(bool tx, const unsigned char * telegram, const uint8_t length, const bool newLine = true)
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

        for (uint8_t i = 0; i < length; i++)
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
        uint32_t ticks = systemTime - lastTick;
        serial.print(telegramCount, DEC, 6);
        serial.print(LOG_SEP);
        serial.print("t:", (unsigned int)ticks, DEC, 5);
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

void dumpSequenceNumber(unsigned char *telegram, const uint8_t tpci)
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

void dumpTelegramInfo(unsigned char *telegram, const uint16_t address, uint8_t tpci, const bool isTX, const TLayer4::TL4State state)
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
        serial.println("Keywords to search for: HIGH, ERROR, EVENT, IGNORED");
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

TLayer4::TLayer4(uint8_t maxTelegramLength):
    sendTelegram(new byte[maxTelegramLength]()),
    sendConnectedTelegram(new byte[maxTelegramLength]()),
    sendConnectedTelegram2(new byte[maxTelegramLength]())
{

}

void TLayer4::_begin()
{
#if defined(INCLUDE_SERIAL)
    IF_DEBUG(
            if (!serial.enabled()) // open only if not the app has it already open
            {
                serial.begin(SERIAL_SPEED);
            }
            serial.println("TLayer4::_begin debugging active");
            );
#endif
    state = TLayer4::CLOSED;
    sendTelegramBufferState = TELEGRAM_FREE;
    sendConnectedTelegramBufferState = CONNECTED_TELEGRAM_FREE;
    sendConnectedTelegramBuffer2State = CONNECTED_TELEGRAM_FREE;
    connectedAddr = 0;
    seqNoSend = -1;
    seqNoRcv = -1;
    connectedTime = 0;
    enabled = true;

    dumpLogHeader();
    dump2(telegramCount = 0;);
    disconnectCount = 0;
    repeatedT_ACKcount = 0;
}

void TLayer4::processTelegram(unsigned char *telegram, uint8_t telLength)
{
    processTelegramInternal(telegram, telLength);
    discardReceivedTelegram();
}

void TLayer4::processTelegramInternal(unsigned char *telegram, uint8_t telLength)
{
    uint16_t destAddr = destinationAddress(telegram);
    ApciCommand apciCmd = apciCommand(telegram);

    if (destAddr == 0)
    {
        // a broadcast telegram
        processBroadCastTelegram(apciCmd, telegram, telLength);
        return;
    }
    else if ((telegram[5] & T_GROUP_ADDRESS_FLAG_Msk)) ///\todo function for this in knx_tpdu.h
    {
        // a group address telegram
        processGroupAddressTelegram(apciCmd, destAddr, telegram, telLength);
        return;
    }
    else if (destAddr != ownAddress())
    {
        // not for us
        return;
    }

    ///\todo function to get tpciCommand in knx_tpdu.h
    unsigned char tpci = telegram[6] & 0xc3; //0b11000011 Transport control field (see KNX 3/3/4 p.6 TPDU)
    uint16_t senderAddr = senderAddress(telegram);

    dump2(telegramCount++;);
    dumpTelegramInfo(telegram, senderAddr, telegram[6], false, state);

    if (tpci & T_CONNECTION_CTRL_COMMAND_Msk)  // A connection control command
    {
        processConControlTelegram(senderAddr, (TPDU)tpci, telegram, telLength);
    }
    else
    {
        processDirectTelegram(apciCmd, telegram, telLength);
    }
}

void TLayer4::processConControlTelegram(const uint16_t& senderAddr, const TPDU& tpci, unsigned char *telegram, const uint8_t& telLength)
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
            break;
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

bool TLayer4::processConControlConnectPDU(uint16_t senderAddr)
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

        case TLayer4::OPEN_IDLE:
        case TLayer4::OPEN_WAIT:
            if (connectedAddr == senderAddr)
            {
                // event E00
                actionA00Nothing();
            }
            else
            {
                // event E01
                actionA10Disconnect(senderAddr);
            }
            break;

        default:
            dump2(serial.print("ERROR processConControlConnectPDU should never land here"));
            break;
    }
    return (false);
}

bool TLayer4::processConControlDisconnectPDU(uint16_t senderAddr)
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

bool TLayer4::processConControlAcknowledgmentPDU(uint16_t senderAddr, const TPDU& tpci, unsigned char *telegram, uint8_t telLength)
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

        // check CLOSED and OPEN_IDLE states for E08, E09, E10
        if ((state == TLayer4::CLOSED) || (state == TLayer4::OPEN_IDLE))
        {
            dump2(
                serial.print(" EVENT 8/9/10 ");
                dumpTelegramBytes(false, telegram, telLength);
            );
            actionA00Nothing();
            return (false);
        }

        // check for event E10
        if (connectedAddr != senderAddr)
        {
            dump2(
               serial.print(" EVENT 10 ");
               dumpTelegramBytes(false, telegram, telLength);
            );
            actionA00Nothing();
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
            actionA06DisconnectAndClose();
            return (false);
        }

        // event E08
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
        // it's a T_NACK_PDU
        dump2(
              serial.print("T_NACK ERROR");
              serial.print(LOG_SEP);
        );

        // check CLOSED state for events E11, E12, E13, E14
        const byte curSeqNo = sequenceNumber(telegram);
        if ((state == TLayer4::CLOSED) || (curSeqNo != seqNoSend) || (connectedAddr != senderAddr))
        {
            dump2(
                if (connectedAddr != senderAddr)
                {
                    serial.print("EVENT 14 ");
                }
                else if (curSeqNo != seqNoSend)
                {
                    serial.print("EVENT 11 ");
                }
                else
                {
                    serial.print("EVENT 12/13 ");
                }
                dumpTelegramBytes(false, telegram, telLength);
            );
            actionA00Nothing();
            return (false);
        }

        if (repCount < TL4_MAX_REPETITION_COUNT && state == TLayer4::OPEN_WAIT)
        {
            // event E12
            dump2(
                serial.print(" EVENT 12 ");
                serial.print("repCount ", repCount);
                serial.print("< ", TL4_MAX_REPETITION_COUNT);
                serial.print("maxRepCount ");
            );
            dumpTelegramBytes(false, telegram, telLength);
            actionA09RepeatMessage();
            return (false);
        }
        // event E12/E13
        dump2(
            serial.print(" EVENT 12/13 ");
            serial.print("repCount ", repCount);
            serial.print(", maxRepCount ", TL4_MAX_REPETITION_COUNT);
            dumpTelegramBytes(false, telegram, telLength);
        );
        actionA06DisconnectAndClose();
        return (false);
    }
}

void TLayer4::sendConControlTelegram(TPDU cmd, uint16_t address, int8_t senderSeqNo)
{
    auto sendBuffer = acquireSendBuffer();

    initLpdu(sendBuffer, PRIORITY_SYSTEM, false, FRAME_STANDARD); // connection control commands always in system priority
    // sender address will be set by bus.sendTelegram()
    setDestinationAddress(sendBuffer, address);
    sendBuffer[5] = (uint8_t)0x60; ///\todo set correct routing counter
    sendBuffer[6] = (uint8_t)cmd;
    if (cmd & T_SEQUENCED_COMMAND)  // Add the sequence number if the command shall contain it
    {
        setSequenceNumber(sendBuffer, senderSeqNo);
    }

    dump2(
        telegramCount++;
        ///\todo create better debug-file/class and include below dump, also rewrite dumpTelegramInfo without using bus.telegram & sendBuffer[6]
        // dumpTelegramInfo(bus.telegram, address, sendBuffer[6], true, state);
        serial.print("sendControl");
        if (address != connectedAddr)
        {
            serial.print(" ERROR");
        }
        dumpTelegramBytes(true, sendBuffer, 7);
    );

    sendPreparedTelegram();
}

void TLayer4::sendPreparedTelegram()
{
    sendTelegramBufferState = TELEGRAM_SENDING;
    send(sendTelegram, telegramSize(sendTelegram));
}

void TLayer4::sendPreparedConnectedTelegram()
{
    auto sendBuffer = acquireSendBuffer();
    memcpy(sendBuffer, sendConnectedTelegram, telegramSize(sendConnectedTelegram));
    sendPreparedTelegram();
}

uint8_t * TLayer4::acquireSendBuffer()
{
    // Someone wants to write into the shared @ref sendTelegram buffer. Wait until the current
    // telegram in it is sent.
    while (sendTelegramBufferState != TELEGRAM_FREE);

    // Then allocate it for the caller.
    sendTelegramBufferState = TELEGRAM_ACQUIRED;

    return sendTelegram;
}

void TLayer4::finishedSendingTelegram(bool successful)
{
    sendTelegramBufferState = TELEGRAM_FREE;

    if (sendConnectedTelegramBufferState == CONNECTED_TELEGRAM_WAIT_T_ACK_SENT)
    {
        // For successfully sent connection control telegrams, continue with the next direct telegram.
        // If there was an error, one of the connected parties will encounter a timeout and either close
        // the connection or send the last telegram again. In both cases it does not make sense to respond
        // with an outdated telegram, so throw it away instead.
        sendConnectedTelegramBufferState = successful ? CONNECTED_TELEGRAM_WAIT_LOOP : CONNECTED_TELEGRAM_FREE;
    }
    else if (sendConnectedTelegramBuffer2State == CONNECTED_TELEGRAM_WAIT_T_ACK_SENT)
    {
        sendConnectedTelegramBuffer2State = successful ? CONNECTED_TELEGRAM_WAIT_LOOP : CONNECTED_TELEGRAM_FREE;
    }
}

void TLayer4::processDirectTelegram(ApciCommand apciCmd, unsigned char *telegram, uint8_t telLength)
{
    dump2(
          serial.print("DirectTele");
          serial.print(LOG_SEP);
    );

    const uint16_t senderAddr = senderAddress(telegram);

    ///\todo function to check for individual_pdu in knx_tpdu.h
    unsigned char tpci = telegram[6] & 0xfc; //0b11111100 Transport control field (see KNX 3/3/4 p.6 TPDU)
    if (tpci == 0)
    {
        // T_Data_Individual-PDU: Allow implementations to overwrite fields (priority, destination)
        auto sendBuffer = acquireSendBuffer();
        initLpdu(sendBuffer, priority(telegram), false, FRAME_STANDARD); // same priority as received
        setDestinationAddress(sendBuffer, senderAddr);
        if (processApci(apciCmd, telegram, telLength, sendBuffer))
        {
            sendPreparedTelegram();
        }
        else
        {
            sendTelegramBufferState = TELEGRAM_FREE;
        }
        return;
    }

    const uint8_t seqNo = sequenceNumber(telegram);

    // check for event E07 or CLOSED state of events E04, E05, E06
    if ((connectedAddr != senderAddr) || (state == TLayer4::CLOSED))
    {
        // event E07 or state CLOSED -> always action A00
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
        dumpTelegramBytes(false, telegram, telLength);
        actionA00Nothing();
        return;
    }

    if (seqNo == seqNoRcv)
    {
        // event E04 and state != TLayer4::CLOSED
        actionA02sendAckPduAndProcessApci(apciCmd, seqNo, telegram, telLength);
        return;
    }

    if (seqNo == ((seqNoRcv-1) & 0x0F))
    {
        // event E05
        dump2(
            serial.print("EVENT 5 seqNo ", seqNo);
            serial.print(" == ", (seqNoRcv-1) & 0x0F);
            serial.print(" (seqNoRcv-1)");
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
    actionA04SendNAck(seqNo);
    dumpTelegramBytes(false, telegram, telLength);
}

bool TLayer4::processApci(ApciCommand apciCmd, unsigned char * telegram, uint8_t telLength, uint8_t * sendBuffer)
{
    dump2(
          serial.print("TLayer4::processApci");
          serial.print(LOG_SEP);
    );
    return (false); // we return nothing
}

void TLayer4::actionA00Nothing()
{
    dump2(serial.println("A00Nothing"));
}

void TLayer4::actionA01Connect(uint16_t address)
{
    dump2(serial.print("A01Connect with ");
          dumpKNXAddress(address);
          serial.print(LOG_SEP);
    );
    resetConnection();
    connectedAddr = address;
    seqNoSend = 0;
    seqNoRcv = 0;
    connectedTime = systemTime; // "start connection timeout timer"
    setTL4State(TLayer4::OPEN_IDLE);
    dump2(lastTick = connectedTime;); // for debug logging
}

void TLayer4::actionA02sendAckPduAndProcessApci(ApciCommand apciCmd, const int8_t seqNo, unsigned char *telegram, uint8_t telLength)
{
    dump2(serial.print("actionA02 "));
    dumpTelegramBytes(false,telegram, telLength);
    sendConControlTelegram(T_ACK_PDU, connectedAddr, seqNo);
    seqNoRcv++;                 // increment sequence counter
    seqNoRcv &= 0x0F;           // handle overflow
    connectedTime = systemTime; // "restart the connection timeout timer"

    auto sendBuffer = (sendConnectedTelegramBufferState == CONNECTED_TELEGRAM_FREE) ? sendConnectedTelegram : sendConnectedTelegram2;
    auto sendResponse = processApci(apciCmd, telegram, telLength, sendBuffer);
    if (sendResponse)
    {
        ///\todo normally this has to be done in Layer 2
        initLpdu(sendBuffer, priority(telegram), false, FRAME_STANDARD); // same priority as received
        setDestinationAddress(sendBuffer, connectedAddr);
        if (sendBuffer == sendConnectedTelegram)
        {
            setSequenceNumber(sendBuffer, seqNoSend);
            sendConnectedTelegramBufferState = CONNECTED_TELEGRAM_WAIT_T_ACK_SENT;
        }
        else
        {
            setSequenceNumber(sendBuffer, ((seqNoSend + 1) & 0x0F));
            sendConnectedTelegramBuffer2State = CONNECTED_TELEGRAM_WAIT_T_ACK_SENT;
        }
    }
}

void TLayer4::actionA03sendAckPduAgain(const int8_t seqNo)
{
    dump2(serial.println("ERROR A03sendAckPduAgain "));
    sendConControlTelegram(T_ACK_PDU, connectedAddr, seqNo);
    repeatedT_ACKcount++; // counting for statistics
    connectedTime = systemTime; // "restart the connection timeout timer"
}

void TLayer4::actionA04SendNAck(const uint8_t seqNo)
{
    dump2(serial.println("ERROR actionA04SendNAck"));
    sendConControlTelegram(T_NACK_PDU, connectedAddr, seqNo);
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

void TLayer4::actionA06DisconnectAndClose()
{
    disconnectCount++;
    setTL4State(TLayer4::CLOSED);
    dump2(
        serial.println("actionA06DisconnectAndClose");
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

    if (sendConnectedTelegramBufferState != CONNECTED_TELEGRAM_WAIT_LOOP)
    {
        dump2(
              serial.print("ERROR A07SendTelegram Nothing to send");
              serial.print(LOG_SEP);
        );
        return;
    }

    dump2(
        telegramCount++;
        dumpTelegramInfo(sendConnectedTelegram, connectedAddr, sendConnectedTelegram[6], true, state);
        serial.print("sendDirectT");
        serial.print(LOG_SEP);
        serial.print("A07SendTelegram");
        serial.print(LOG_SEP);
    );

    setTL4State(TLayer4::OPEN_WAIT);
    sendConnectedTelegramBufferState = CONNECTED_TELEGRAM_SENDING;

    dump2(
        dumpTelegramBytes(true, sendConnectedTelegram, telegramSize(sendConnectedTelegram), true);
    );
    sendPreparedConnectedTelegram();
    repCount = 0;
    sentTelegramTime = systemTime; // "start the acknowledge timeout timer"
    connectedTime = systemTime; // "restart the connection timeout timer"
}

void TLayer4::actionA08IncrementSequenceNumber()
{
    dump2(serial.print("A08IncrementSequenceNumber "));
    seqNoSend++;
    seqNoSend &= 0x0F;
    connectedTime = systemTime; // "restart the connection timeout timer"
    sendConnectedTelegramBufferState = CONNECTED_TELEGRAM_FREE;

    if (sendConnectedTelegramBuffer2State != CONNECTED_TELEGRAM_FREE)
    {
        memcpy(sendConnectedTelegram, sendConnectedTelegram2, telegramSize(sendConnectedTelegram2));
        sendConnectedTelegramBufferState = sendConnectedTelegramBuffer2State;
        sendConnectedTelegramBuffer2State = CONNECTED_TELEGRAM_FREE;
    }
}

void TLayer4::actionA09RepeatMessage()
{
    dump2(
        serial.print("ERROR actionA09RepeatMessage ");
        dumpTelegramBytes(true, sendConnectedTelegram, telegramSize(sendConnectedTelegram));
    );

    sendPreparedConnectedTelegram();
    repCount++;
    sentTelegramTime = systemTime; // "start the acknowledge timeout timer"
    connectedTime = systemTime; // "restart the connection timeout timer"
}

void TLayer4::actionA10Disconnect(uint16_t address)
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
        // event E16
        actionA06DisconnectAndClose();
        dump2(serial.println("direct connection timed out => disconnecting"));
    }

    // Send a potential response message
    if ((state == TLayer4::OPEN_IDLE) && (sendConnectedTelegramBufferState == CONNECTED_TELEGRAM_WAIT_LOOP))
    {
        // event E15
        actionA07SendDirectTelegram();
    }

    // Repeat the message after TL4_T_ACK_TIMEOUT_MS milliseconds
    if ((state == TLayer4::OPEN_WAIT) && (elapsed(sentTelegramTime) >= TL4_T_ACK_TIMEOUT_MS))
    {
        if (repCount < TL4_MAX_REPETITION_COUNT)
        {
            // event E17
            dump2(
                dumpTicks();
                serial.println("T_ACK timed out => repeating");
            );

            actionA09RepeatMessage();
        }
        else
        {
            // event E18
            dump2(
                dumpTicks();
                serial.println("T_ACK timed out and too many repetitions => disconnecting");
            );
            actionA06DisconnectAndClose();
        }
    }
}

void TLayer4::resetConnection()
{
    dump2(
          serial.print("TLayer4::resetConnection");
          serial.print(LOG_SEP)
    );

    sendConnectedTelegramBufferState = CONNECTED_TELEGRAM_FREE;
    sendConnectedTelegramBuffer2State = CONNECTED_TELEGRAM_FREE;
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

/** @}*/
