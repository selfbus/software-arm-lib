/*
 * bcu_updater.cpp
 *
 *  Created on: 15.07.2015
 *      Author: glueck
 */

#include "bcu_updater.h"
#include <sblib/serial.h>


// The EIB bus access object
Bus bus(timer16_1, PIN_EIB_RX, PIN_EIB_TX, CAP0, MAT0);

#if defined(DUMP_TELEGRAMS_LVL2) && defined(DEBUG)
    #include <sblib/serial.h>
#endif

#if defined(DUMP_TELEGRAMS_LVL2) && defined(DEBUG)
#   define dump2(code) code
#else
#   define dump2(x)
#endif

void BcuUpdate::processTelegram()
{
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

    unsigned short destAddr = (bus.telegram[3] << 8) | bus.telegram[4];
    unsigned char tpci = bus.telegram[6] & 0xc3; // Transport control field (see KNX 3/3/4 p.6 TPDU)
    unsigned short apci = ((bus.telegram[6] & 3) << 8) | bus.telegram[7];

    if ((bus.telegram[5] & 0x80) == 0) // a physical destination address
    {
        if (destAddr == bus.ownAddress()) // it's our physical address
        {
            if (tpci & 0x80)  // A connection control command
            {
                dump2(serial.println("procControlT"));
                processConControlTelegram(bus.telegram[6]);
            }
            else
            {
                dump2(serial.println("procDirectT"));
                processDirectTelegram(apci);
            }
        }
    }
    // At the end: discard the received telegram
    bus.discardReceivedTelegram();
}

void BcuUpdate::processDirectTelegram(int apci)
{
    const int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];
    const int senderSeqNo = bus.telegram[6] & 0x3c;
    unsigned char sendAck = 0;
    bool sendTel = false;

    if (connectedAddr != senderAddr) // ensure that the sender is correct
        return;

    connectedTime = systemTime;
    sendTelegram[6] = 0;

    int apciCommand = apci & APCI_GROUP_MASK;
    if ((apciCommand == APCI_MEMORY_READ_PDU) | (apciCommand == APCI_MEMORY_WRITE_PDU))
    {
        dump2(serial.println("R/W DATA"));
        sendAck = handleMemoryRequests(apciCommand, &sendTel, &bus.telegram[7]);
    }
    else if (apciCommand == APCI_RESTART_PDU)
    {
        dump2(serial.println("APCI_RESTART_PDU"));
        sendAck = T_ACK_PDU;
        restartRequest(RESET_DELAY_MS); // Software Reset
    }
    else
    {
        dump2(serial.println("APCI_UNKNOWN 0x", apciCommand, HEX, 4));
        sendAck = T_NACK_PDU;  // Command not supported
        //TODO in case we run into this, maybe Reset?
    }

    if (sendTel)
    {
        dump2(serial.println("TX-sendTel"));
        sendAck = T_ACK_PDU;
    }

    if (sendAck)
    {
        dump2(serial.println("TX-ACK"));
        sendConControlTelegram(sendAck, senderSeqNo);
    }
    else
    {
        sendCtrlTelegram[0] = 0;
    }

    if (sendTel)
    {
        sendTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
        // 1+2 contain the sender address, which is set by bus.sendTelegram()
        sendTelegram[3] = connectedAddr >> 8;
        sendTelegram[4] = connectedAddr;

        if (sendTelegram[6] & 0x40) // Add the sequence number if applicable
        {
            sendTelegram[6] &= ~0x3c;
            sendTelegram[6] |= connectedSeqNo;
            incConnectedSeqNo = true;
        }
        else
            incConnectedSeqNo = false;
        dump2(serial.println("TX-DATA"));
        bus.sendTelegram(sendTelegram, telegramSize(sendTelegram));
    }
}

void BcuUpdate::processConControlTelegram(int tpci)
{
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
            dump2(serial.println("T_CONNECT_PDU"));
            dump2(serial.println("T_CONNECT_PDU"));
            if (connectedAddr == 0 || connectedAddr == senderAddr)
            {
                connectedTime = systemTime;
                connectedAddr = senderAddr;
                connectedSeqNo = 0;
                incConnectedSeqNo = false;
                dump2(serial.println("RX-CON"));
                bus.setSendAck(SB_BUS_ACK);
            }
        }
        else if (tpci == T_DISCONNECT_PDU)  // Close the direct data connection
        {
            dump2(serial.println("T_DISCONNECT_PDU"));
            if (connectedAddr == senderAddr)
            {
                connectedAddr = 0;
                bus.setSendAck(SB_BUS_ACK);
                dump2(serial.println("RX-DIS"));
            }
        }
    }
}

void BcuUpdate::sendConControlTelegram(int cmd, int senderSeqNo)
{
    if (cmd & 0x40)  // Add the sequence number if the command shall contain it
        cmd |= senderSeqNo & 0x3c;

    sendCtrlTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    sendCtrlTelegram[3] = connectedAddr >> 8;
    sendCtrlTelegram[4] = connectedAddr;
    sendCtrlTelegram[5] = 0x60;
    sendCtrlTelegram[6] = cmd;

    bus.sendTelegram(sendCtrlTelegram, 7);
}

