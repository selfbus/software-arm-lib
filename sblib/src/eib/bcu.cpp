/*
 *  bcu.h - EIB bus coupling unit.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#define INSIDE_BCU_CPP
#include <sblib/eib/bcu.h>
#include <sblib/eib/apci.h>
#include <sblib/internal/functions.h>
#include <sblib/eib/com_objects.h>
#include <string.h>

#ifdef DUMP_TELEGRAMS
#include <sblib/serial.h>
#endif

extern unsigned int writeUserEepromTime;
extern volatile unsigned int systemTime;

void BCU::_begin()
{
    readUserEeprom();
}

void BCU::end()
{
	BCU_Base::end();
    writeUserEeprom();
}

void BCU::loop()
{
    if (!enabled)
        return;
    BCU_Base::loop();

    if (!bus.sendingTelegram())
        sendNextGroupTelegram();

    // Send a disconnect after 6.5 seconds inactivity
    if (connectedAddr && elapsed(connectedTime) > 6500)
    {
        sendConControlTelegram(T_DISCONNECT_PDU, 0);
        connectedAddr = 0;
    }

    if (userEeprom.isModified() && bus.idle() && bus.telegramLen == 0 && connectedAddr == 0)
    {
        if (writeUserEepromTime)
        {
            if (millis() - writeUserEepromTime > 0)
                writeUserEeprom();
        }
        else writeUserEepromTime = millis() + 50;
    }
}

void BCU::sendConControlTelegram(int cmd, int senderSeqNo)
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

void BCU::processTelegram()
{
    unsigned short destAddr = (bus.telegram[3] << 8) | bus.telegram[4];
    unsigned char tpci = bus.telegram[6] & 0xc3; // Transport control field (see KNX 3/3/4 p.6 TPDU)
    unsigned short apci = ((bus.telegram[6] & 3) << 8) | bus.telegram[7];

    if (destAddr == 0) // a broadcast
    {
        if (programmingMode()) // we are in programming mode
        {
            if (apci == APCI_INDIVIDUAL_ADDRESS_WRITE_PDU)
            {
                setOwnAddress((bus.telegram[8] << 8) | bus.telegram[9]);
            }
            else if (apci == APCI_INDIVIDUAL_ADDRESS_READ_PDU)
            {
                sendTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
                // 1+2 contain the sender address, which is set by bus.sendTelegram()
                sendTelegram[3] = 0x00;  // Zero target address, it's a broadcast
                sendTelegram[4] = 0x00;
                sendTelegram[5] = 0xe1;
                sendTelegram[6] = 0x01;  // APCI_INDIVIDUAL_ADDRESS_RESPONSE_PDU (0x0140)
                sendTelegram[7] = 0x40;
                bus.sendTelegram(sendTelegram, 8);
            }
        }
    }
    else if ((bus.telegram[5] & 0x80) == 0) // a physical destination address
    {
        if (destAddr == bus.ownAddress()) // it's our physical address
        {
            if (tpci & 0x80)  // A connection control command
            {
                processConControlTelegram(bus.telegram[6]);
            }
            else
            {
                processDirectTelegram(apci);
            }
        }
    }
    else if (tpci == T_GROUP_PDU) // a group destination address and multicast
    {
        processGroupTelegram(destAddr, apci & APCI_GROUP_MASK);
    }

    // At the end: discard the received telegram
    bus.discardReceivedTelegram();
}

bool BCU::processDeviceDescriptorReadTelegram(int id)
{
    if (id == 0)
    {
        int version = maskVersion();

        sendTelegram[5] = 0x63;
        sendTelegram[6] = 0x43;
        sendTelegram[7] = 0x40;
        sendTelegram[8] = version >> 8;
        sendTelegram[9] = version;
        return true;
    }

    return false; // unknown device descriptor
}

static void cpyToUserRam(unsigned int address, unsigned char * buffer, unsigned int count)
{
	address -= USER_RAM_START;
	if ((address > 0x60) || ((address + count) < 0x60))
	{
		memcpy(userRamData + address, buffer, count);
	}
	else
	{
		while (count--)
		{
			if (address == 0x60)
				userRam.status = * buffer;
			else
				userRamData[address] = * buffer;
			buffer++;
			address++;
		}
	}
}

static void cpyFromUserRam(unsigned int address, unsigned char * buffer, unsigned int count)
{
	address -= USER_RAM_START;
	if ((address > 0x60) || ((address + count) < 0x60))
	{
		memcpy(buffer, userRamData + address, count);
	}
	else
	{
		while (count--)
		{
			if (address == 0x60)
				* buffer = userRam.status;
			else
				* buffer = userRamData[address];
			buffer++;
			address++;
		}
	}
}

void BCU::processDirectTelegram(int apci)
{
    const int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];
    const int senderSeqNo = bus.telegram[6] & 0x3c;
    int count, address, index;
#if BCU_TYPE != BCU1_TYPE
    bool found;
    int id;
#endif
    unsigned char sendAck = 0;
    bool sendTel = false;

    if (connectedAddr != senderAddr) // ensure that the sender is correct
        return;

    connectedTime = systemTime;
    sendTelegram[6] = 0;

    int apciCmd = apci & APCI_GROUP_MASK;
    switch (apciCmd)  // ADC / memory commands use the low bits for data
    {
    case APCI_ADC_READ_PDU:
        index = bus.telegram[7] & 0x3f;  // ADC channel
        count = bus.telegram[8];         // number of samples
        sendTelegram[5] = 0x64;
        sendTelegram[6] = 0x41;
        sendTelegram[7] = 0xc0 | (index & 0x3f);   // channel
        sendTelegram[8] = count;                   // read count
        sendTelegram[9] = 0;                       // FIXME dummy - ADC value high byte
        sendTelegram[10] = 0;                      // FIXME dummy - ADC value low byte
        sendTel = true;
        break;

    case APCI_MEMORY_READ_PDU:
    case APCI_MEMORY_WRITE_PDU:
        count = bus.telegram[7] & 0x0f; // number of data byes
        address = (bus.telegram[8] << 8) | bus.telegram[9]; // address of the data block

        if (apciCmd == APCI_MEMORY_WRITE_PDU)
        {
            if (address >= USER_EEPROM_START && address < USER_EEPROM_END)
            {
                memcpy(userEepromData + (address - USER_EEPROM_START), bus.telegram + 10, count);
                userEeprom.modified();
            }
            else if (address >= USER_RAM_START && address < USER_RAM_END)
            	cpyToUserRam(address - USER_RAM_START, bus.telegram + 10, count);

            sendAck = T_ACK_PDU;

#ifdef LOAD_CONTROL_ADDR
            if (address == LOAD_CONTROL_ADDR)
            {
                int objectIdx = bus.telegram[10] >> 4;
                userEeprom.loadState[objectIdx] = loadProperty(objectIdx, bus.telegram + 10, count);
                break;
            }
#endif

#if BCU_TYPE != BCU1_TYPE
            if (userRam.deviceControl & DEVCTRL_MEM_AUTO_RESPONSE)
                apciCmd = APCI_MEMORY_READ_PDU;
#endif
        }

        if (apciCmd == APCI_MEMORY_READ_PDU)
        {
            if (address >= USER_EEPROM_START && address < USER_EEPROM_END)
                memcpy(sendTelegram + 10, userEepromData + (address - USER_EEPROM_START), count);
            else if (address >= USER_RAM_START && address < USER_RAM_END)
            	cpyFromUserRam(address - USER_RAM_START, sendTelegram + 10, count);
#ifdef LOAD_STATE_ADDR
            else if (address >= LOAD_STATE_ADDR && address < LOAD_STATE_ADDR + 8)
                memcpy(sendTelegram + 10, userEeprom.loadState + (address - LOAD_STATE_ADDR), count);
#endif
            sendTelegram[5] = 0x63 + count;
            sendTelegram[6] = 0x42;
            sendTelegram[7] = 0x40 | count;
            sendTelegram[8] = address >> 8;
            sendTelegram[9] = address;
            sendTel = true;
        }
        break;

    case APCI_DEVICEDESCRIPTOR_READ_PDU:
        if (processDeviceDescriptorReadTelegram(apci & 0x3f))
            sendTel = true;
        else sendAck = T_NACK_PDU;
        break;

    default:
        switch (apci)
        {
        case APCI_RESTART_PDU:
            writeUserEeprom();   // Flush the EEPROM before resetting
            NVIC_SystemReset();  // Software Reset
            break;

        case APCI_AUTHORIZE_REQUEST_PDU:
            sendTelegram[5] = 0x62;
            sendTelegram[6] = 0x43;
            sendTelegram[7] = 0xd2;
            sendTelegram[8] = 0x00;
            sendTel = true;
            break;

#if BCU_TYPE != BCU1_TYPE
        case APCI_PROPERTY_VALUE_READ_PDU:
        case APCI_PROPERTY_VALUE_WRITE_PDU:
            sendTelegram[5] = 0x65;
            sendTelegram[6] = 0x40 | (APCI_PROPERTY_VALUE_RESPONSE_PDU >> 8);
            sendTelegram[7] = APCI_PROPERTY_VALUE_RESPONSE_PDU & 0xff;
            index = sendTelegram[8] = bus.telegram[8];
            id = sendTelegram[9] = bus.telegram[9];
            count = (sendTelegram[10] = bus.telegram[10]) >> 4;
            address = ((bus.telegram[10] & 15) << 4) | (sendTelegram[11] = bus.telegram[11]);

            if (apci == APCI_PROPERTY_VALUE_READ_PDU)
                found = propertyValueReadTelegram(index, (PropertyID) id, count, address);
            else found = propertyValueWriteTelegram(index, (PropertyID) id, count, address);
            if (!found) sendTelegram[10] = 0;
            sendTel = true;
            break;

        case APCI_PROPERTY_DESCRIPTION_READ_PDU:
            sendTelegram[5] = 0x68;
            sendTelegram[6] = 0x64 | (APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU >> 8);
            sendTelegram[7] = APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU & 0xff;
            index = sendTelegram[8] = bus.telegram[8];
            id = sendTelegram[9] = bus.telegram[9];
            address = (sendTelegram[10] = bus.telegram[10]);
            propertyDescReadTelegram(index, (PropertyID) id, address);
            sendTel = true;
            break;
#endif /*BCU_TYPE != BCU1_TYPE*/

        default:
            sendAck = T_NACK_PDU;  // Command not supported
            break;
        }
        break;
    }

    if (sendTel)
        sendAck = T_ACK_PDU;

    if (sendAck)
        sendConControlTelegram(sendAck, senderSeqNo);
    else sendCtrlTelegram[0] = 0;

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
        else incConnectedSeqNo = false;

        bus.sendTelegram(sendTelegram, telegramSize(sendTelegram));
    }
}

void BCU::processConControlTelegram(int tpci)
{
    int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];

    if (tpci & 0x40)  // An acknowledgement
    {
        tpci &= 0xc3;
        if (tpci == T_ACK_PDU) // A positive acknowledgement
        {
            if (incConnectedSeqNo && connectedAddr == senderAddr)
            {
                connectedSeqNo += 4;
                connectedSeqNo &= 0x3c;
                incConnectedSeqNo = false;
            }
        }
        else if (tpci == T_NACK_PDU)  // A negative acknowledgement
        {
            if (connectedAddr == senderAddr)
            {
                sendConControlTelegram(T_DISCONNECT_PDU, 0);
                connectedAddr = 0;
            }
        }

        incConnectedSeqNo = true;
    }
    else  // A connect/disconnect command
    {
        if (tpci == T_CONNECT_PDU)  // Open a direct data connection
        {
            if (connectedAddr == 0)
            {
                connectedTime = systemTime;
                connectedAddr = senderAddr;
                connectedSeqNo = 0;
                incConnectedSeqNo = false;
                bus.setSendAck (0);
            }
        }
        else if (tpci == T_DISCONNECT_PDU)  // Close the direct data connection
        {
            if (connectedAddr == senderAddr)
            {
                connectedAddr = 0;
                bus.setSendAck (0);
            }
        }
    }
}
