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
uint16_t ignoredNdataIndividual = 0;

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
    sendTelegram(new byte[maxTelegramLength]())
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
    // set sending buffer to free
    sendTelegram[0] = 0;
    sendCtrlTelegram[0] = 0;
    telegramReadyToSend = false;
    connectedAddr = 0;
    seqNoSend = -1;
    seqNoRcv = -1;
    connectedTime = 0;
    enabled = true;

    dumpLogHeader();
    dump2(telegramCount = 0;);
    disconnectCount = 0;
    ignoredNdataIndividual = 0;
}

void TLayer4::processTelegram(unsigned char *telegram, uint8_t telLength)
{
    processTelegramInternal(telegram, telLength);
    discardReceivedTelegram();
}

bool TLayer4::processTelegramInternal(unsigned char *telegram, uint8_t telLength)
{
    uint16_t destAddr = destinationAddress(telegram);
    ApciCommand apciCmd = apciCommand(telegram);

    if (destAddr == 0)
    {
        // a broadcast telegram
        return (processBroadCastTelegram(apciCmd, telegram, telLength));
    }
    else if ((telegram[5] & T_GROUP_ADDRESS_FLAG_Msk)) ///\todo function for this in knx_tpdu.h
    {
        // a group address telegram
        return (processGroupAddressTelegram(apciCmd, destAddr, telegram, telLength));
    }
    else if (destAddr != ownAddress())
    {
        // not for us
        return (true);
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
    return (true);
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
            break;

        case TLayer4::OPEN_IDLE:
        case TLayer4::OPEN_WAIT:
            if (connectedAddr == senderAddr)
            {
                // event E00
                actionA06DisconnectAndClose();
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
            actionA06DisconnectAndClose();
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
            actionA06DisconnectAndClose();
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
            actionA06DisconnectAndClose();
            dumpTelegramBytes(false, telegram, telLength);
            return (false);
        }
    }
}

void TLayer4::sendConControlTelegram(TPDU cmd, uint16_t address, int8_t senderSeqNo)
{
    sendCtrlTelegram[6] = (uint8_t)cmd;
    if (cmd & T_SEQUENCED_COMMAND)  // Add the sequence number if the command shall contain it
    {
        setSequenceNumber(sendCtrlTelegram, senderSeqNo);
    }

    dump2(
        telegramCount++;
        ///\todo create better debug-file/class and include below dump, also rewrite dumpTelegramInfo without using bus.telegram & sendCtrlTelegram[6]
        // dumpTelegramInfo(bus.telegram, address, sendCtrlTelegram[6], true, state);
        serial.print("sendControl");
    );

    initLpdu(sendCtrlTelegram, PRIORITY_SYSTEM, false, FRAME_STANDARD); // connection control commands always in system priority
    // sender address will be set by bus.sendTelegram()

    setDestinationAddress(sendCtrlTelegram, address);
    sendCtrlTelegram[5] = (uint8_t)0x60; ///\todo set correct routing counter

    dump2(
        if (address != connectedAddr)
        {
            serial.print(" ERROR");
        }
        uint8_t lengthCtrlTelegram = sizeof(sendCtrlTelegram) / sizeof(sendCtrlTelegram[0]);
        dumpTelegramBytes(true, &sendCtrlTelegram[0], lengthCtrlTelegram);
    );

    send(sendCtrlTelegram, 7);
}

void TLayer4::processDirectTelegram(ApciCommand apciCmd, unsigned char *telegram, uint8_t telLength)
{
    dump2(
          serial.print("DirectTele");
          serial.print(LOG_SEP);
    );

    const uint16_t senderAddr = senderAddress(telegram);
    const uint8_t seqNo = sequenceNumber(telegram);

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
        // event E04 and state != TLayer4::CLOSED
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
            actionA06DisconnectAndClose();
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
    actionA06DisconnectAndClose();
    dumpTelegramBytes(false, telegram, telLength);
}

unsigned char TLayer4::processApci(ApciCommand apciCmd, const uint16_t senderAddr, const int8_t senderSeqNo, bool * sendResponse, unsigned char * telegram, uint8_t telLength)
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

void TLayer4::actionA01Connect(uint16_t address)
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

bool TLayer4::actionA02sendAckPduAndProcessApci(ApciCommand apciCmd, const int8_t seqNo, unsigned char *telegram, uint8_t telLength)
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

        ///todo get callback from L2 of successfully sent T_ACK and then execute below steps
        discardReceivedTelegram();
        sendResponse = false;
        telegramReadyToSend = true;
        actionA07SendDirectTelegram();
    }
    return sendResponse;
}

void TLayer4::actionA03sendAckPduAgain(const int8_t seqNo)
{
    ///\todo clarify after KNX Spec 3.0 public release
    // The Data Link Layer filters out repeated telegrams, provided that the repetition follows
    // directly after the original telegram. If another telegram sneaks in (e.g. due to higher
    // priority), we encounter such repeated telegrams in the Transport Layer.
    //
    // Per KNX Spec 2.1, Chapter 3/3/4 Section 5.4.4.3 p.27, this is Event E05 and its corresponding Action A3.
    // The spec says to send another T_ACK for such a repeated telegram, but here's the catch:
    // If the client does not implement Style 3, such as calimero-core 2.5.1 at the time of this writing (2023/04/02),
    // it will see a duplicate T_ACK in state OPEN_IDLE (events E08/E09) and consequently close the
    // connection.
    //
    // Prevent this by staying silent for TL4_T_ACK_SUPPRESS_WINDOW_MS time and intentionally disobeying the spec.

    dump2(serial.print("ERROR A03sendAckPduAgain "));
    if ((connectedTime + TL4_T_ACK_SUPPRESS_WINDOW_MS) <= systemTime)
    {
        dump2(
            serial.print("sending T_ACK seqNo# ", seqNo, DEC, 2);
            serial.print(" again");
        );
        // KNX Spec 2.1 conform handling of action A03 -> sending a T_ACK_PDU:
        sendConControlTelegram(T_ACK_PDU, connectedAddr, seqNo);
    }
    else
    {
        // Prevention by staying silent for TL4_T_ACK_SUPPRESS_WINDOW_MS time and intentionally disobeying the spec.
        dump2(serial.print("IGNORED N_DATA_INDIVIDUAL"));
        ignoredNdataIndividual++; // we received a already processed N_DATA_INDIVIDUAL, ignore it
    }
    dump2(serial.println());
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
    send(sendTelegram, telegramSize(sendTelegram));
    connectedTime = systemTime;
}

void TLayer4::actionA08IncrementSequenceNumber()
{
    dump2(serial.print("A08IncrementSequenceNumber "));
    seqNoSend++;
    seqNoSend &= 0x0F;
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
        actionA06DisconnectAndClose();
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

/** @}*/
