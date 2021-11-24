/*
 *  bcu.cpp - EIB bus coupling unit.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  last change: 10. April 2021 HoRa:
 *  	call to bus:setSendAck() in "connect/disconnect command at Layer4" removed- not needed, will lead to undefined effects when the
 *  	bus state machine is sending an ack in parallel due to asynchronous interrupt process
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#define INSIDE_BCU_CPP
#include <sblib/eib/bcu1.h>
#include <sblib/eib/apci.h>
#include <sblib/eib/com_objects.h>
#include <string.h>
#include <sblib/internal/variables.h>
#include <sblib/mem_mapper.h>
#include <sblib/eib/bus.h>

void BCU1::processDirectTelegram(int apci)
{
    const int senderAddr = (bus->telegram[1] << 8) | bus->telegram[2];
    const int senderSeqNo = bus->telegram[6] & 0x3c;
    int count, address, index;
    unsigned char sendAck = 0;
    bool sendTel = false;

    if (connectedAddr != senderAddr) // ensure that the sender is correct
        return;

    connectedTime = systemTime;
    sendTelegram[6] = 0;

    int apciCommand = apci & APCI_GROUP_MASK;
    switch (apciCommand)  // ADC / memory commands use the low bits for data
    {
    case APCI_ADC_READ_PDU: // todo adc service  to be implemented for bus voltage and PEI
        index = bus->telegram[7] & 0x3f;  // ADC channel
        count = bus->telegram[8];         // number of samples
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
        count = bus->telegram[7] & 0x0f; // number of data bytes
        address = (bus->telegram[8] << 8) | bus->telegram[9]; // address of the data block

        if (apciCommand == APCI_MEMORY_WRITE_PDU)
        {
            if (processApciMemoryWritePDU(address, &bus->telegram[10], count))
            {
            }
            sendAck = T_ACK_PDU;
        }

        if (apciCommand == APCI_MEMORY_READ_PDU)
        {
            if (!processApciMemoryReadPDU(address, &sendTelegram[10], count))
            {
                // address space unreachable, need to respond with count 0
                count = 0;
            }

            // send a APCI_MEMORY_RESPONSE_PDU response
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
        case APCI_RESTART_RESPONSE_PDU:
            if (checkApciForMagicWord(apci, bus->telegram[8], bus->telegram[9]))
            {
                // special version of APCI_RESTART_TYPE1_PDU  used by Selfbus bootloader
                // restart with parameters, we need to start in flashmode
                // this is only allowed with programming mode on, otherwise it will result in a simple reset
                unsigned int * magicWord = BOOTLOADER_MAGIC_ADDRESS;
                *magicWord = BOOTLOADER_MAGIC_WORD;
                /* if (programmingMode())
                {
                    *magicWord = BOOTLOADER_MAGIC_WORD;
                }
                else
                {
                    *magicWord = 0;
                }
                */
            }

            if (usrCallback)
            {
                usrCallback->Notify(USR_CALLBACK_RESET);
            }

            userEeprom->writeUserEeprom();   // Flush the EEPROM before resetting

            if (memMapper)
            {
                memMapper->doFlash();
            }
            //FIXME 3/5/3 KNX spec 3.7 (page 63) says send an appropriate LM_Reset.ind message through the EMI interface

            NVIC_SystemReset();  // Software Reset
            break;

        case APCI_AUTHORIZE_REQUEST_PDU:
            sendTelegram[5] = 0x62;
            sendTelegram[6] = 0x43;
            sendTelegram[7] = 0xd2;
            sendTelegram[8] = 0x00;
            sendTel = true;
            break;

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
        sendTelegram[0] = 0xb0 | (bus->telegram[0] & 0x0c); // Control byte
        // 1+2 contain the sender address, which is set by bus->sendTelegram()
        sendTelegram[3] = connectedAddr >> 8;
        sendTelegram[4] = connectedAddr;

        if (sendTelegram[6] & 0x40) // Add the sequence number if applicable
        {
            sendTelegram[6] &= ~0x3c;
            sendTelegram[6] |= connectedSeqNo;
            incConnectedSeqNo = true;
        }
        else incConnectedSeqNo = false;

        bus->sendTelegram(sendTelegram, telegramSize(sendTelegram));
    }
}

void BCU1::setOwnAddress (int addr)
{
    userEeprom->addrTab()[0] = addr >> 8;
    userEeprom->addrTab()[1] = addr;
    userEeprom->modified();

    ownAddr = addr;
}

/**
 * Begin using the EIB bus coupling unit, and set the manufacturer-ID, device type,
 * program version and a optional read-only CommObjectTable address which
 * can't be changed by ETS/KNX telegrams
 *
 * @param manufacturer - the manufacturer ID (16 bit)
 * @param deviceType - the device type (16 bit)
 * @param version - the version of the application program (8 bit)
 */
inline void BCU1::begin(int manufacturer, int deviceType, int version)
{
	ownAddr = (userEeprom->addrTab()[0] << 8) | userEeprom->addrTab()[1];

	begin_BCU(manufacturer, deviceType, version);
	commObjectTableAddressStatic = 0;
}

inline bool BCU1::applicationRunning() const
{
    if (!enabled)
        return false;

    return ((userRam->status & (BCU_STATUS_PROG|BCU_STATUS_AL)) == BCU_STATUS_AL &&
        userRam->runState == 1 && userEeprom->runError() == 0xff); // ETS sets the run error to 0 when programming
}

BCU1::BCU1() : BCU1(new UserRamBCU1(), new UserEepromBCU1(bus), new ComObjectsBCU1(this), new AddrTablesBCU1(this))
{}

BCU1::BCU1(UserRamBCU1* userRam, UserEepromBCU1* userEeprom, ComObjectsBCU1* comObjects, AddrTablesBCU1* addrTables) :
		BcuBase(userRam, userEeprom, comObjects, addrTables),
		userRam(userRam),
		userEeprom(userEeprom),
		comObjects(comObjects),
		addrTables(addrTables)
{}

// The method begin_BCU() is renamed during compilation to indicate the BCU type.
// If you get a link error then the library's BCU_TYPE is different from your application's BCU_TYPE.
void BCU1::begin_BCU(int manufacturer, int deviceType, int version)
{
    userRam->status = BCU_STATUS_LL | BCU_STATUS_TL | BCU_STATUS_AL | BCU_STATUS_USR;
    userRam->deviceControl() = 0;
    userRam->runState = 1;

    userEeprom->runError() = 0xff;
    userEeprom->portADDR() = 0;

    userEeprom->manufacturerH() = manufacturer >> 8;
    userEeprom->manufacturerL() = manufacturer;

    userEeprom->deviceTypeH() = deviceType >> 8;
    userEeprom->deviceTypeL() = deviceType;

    userEeprom->version() = version;

    userEeprom->writeUserEepromTime = 0;

    BcuBase::begin_BCU(manufacturer, deviceType, version);
}
