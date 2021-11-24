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
#include <sblib/eib/systemb.h>
#include <sblib/eib/apci.h>
#include <sblib/eib/com_objects.h>
#include <string.h>
#include <sblib/internal/variables.h>
#include <sblib/mem_mapper.h>
#include <sblib/internal/iap.h>
#include <sblib/eib/propertiesSYSTEMB.h>
#include <sblib/eib/bus.h>

void SYSTEMB::processDirectTelegram(int apci)
{
    const int senderAddr = (bus->telegram[1] << 8) | bus->telegram[2];
    const int senderSeqNo = bus->telegram[6] & 0x3c;
    int count, address, index;
    bool found;
    int id;
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
                if (userRam->deviceControl() & DEVCTRL_MEM_AUTO_RESPONSE)
                {
                    // only on successful write
                    apciCommand = APCI_MEMORY_READ_PDU;
                }
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

        case APCI_PROPERTY_VALUE_READ_PDU:
        case APCI_PROPERTY_VALUE_WRITE_PDU:
            sendTelegram[5] = 0x65;
            sendTelegram[6] = 0x40 | (APCI_PROPERTY_VALUE_RESPONSE_PDU >> 8);
            sendTelegram[7] = APCI_PROPERTY_VALUE_RESPONSE_PDU & 0xff;
            index = sendTelegram[8] = bus->telegram[8];
            id = sendTelegram[9] = bus->telegram[9];
            count = (sendTelegram[10] = bus->telegram[10]) >> 4;
            address = ((bus->telegram[10] & 15) << 4) | (sendTelegram[11] = bus->telegram[11]);

            if (apci == APCI_PROPERTY_VALUE_READ_PDU)
                found = properties->propertyValueReadTelegram(index, (PropertyID) id, count, address);
            else found = properties->propertyValueWriteTelegram(index, (PropertyID) id, count, address);
            if (!found) sendTelegram[10] = 0;
            sendTel = true;
            break;

        case APCI_PROPERTY_DESCRIPTION_READ_PDU:
            sendTelegram[5] = 0x68;
            sendTelegram[6] = 0x64 | (APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU >> 8);
            sendTelegram[7] = APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU & 0xff;
            index = sendTelegram[8] = bus->telegram[8];
            id = sendTelegram[9] = bus->telegram[9];
            address = (sendTelegram[10] = bus->telegram[10]);
            properties->propertyDescReadTelegram(index, (PropertyID) id, address);
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

void SYSTEMB::begin_BCU(int manufacturer, int deviceType, int version)
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

    unsigned int partID;
    unsigned int useOldSerialStyle;
    byte uniqueID[IAP_UID_LENGTH];

    useOldSerialStyle = 1;
    if (iapReadUID(&uniqueID[0]) == IAP_SUCCESS)
    {
        // https://community.nxp.com/t5/LPC-Microcontrollers/IAP-C-code-example-query/m-p/596139/highlight/true#M22963
        // Unfortunately the details of what go into the 128-bit GUID cannot be disclosed.
        // It can be said, however, that the 128-bit GUIDs are not random, nor are they sequential.
        // Thus to ensure there are no collisions with other devices, the full 128 bits should be used.

        // create a 48bit serial/hash from the 128bit Guid
        useOldSerialStyle = !hashUID(&uniqueID[0], sizeof(uniqueID), &userEeprom->serial()[0], userEeprom->serialSize());

        /** //TODO this could be a alternative for ETS checking the serial number
         *         it seems that the first two bytes are the Manufacturer-ID
         *         and setting the last 4 bytes to 0xFF the ETS doesn't check the serial number at all
         *         memset (userEeprom.serial, 0xFF, 6);             // set serial to 0xFFFFFFFFFFFF
         *         userEeprom.serial[0] = userEeprom.manufacturerH; // set Manufacturer-ID
         *         userEeprom.serial[1] = userEeprom.manufacturerL;
         */
    }

    if (useOldSerialStyle)
    {
        iapReadPartID(&partID);
        memcpy (userEeprom->serial(), &partID, 4);
        userEeprom->serial()[4] = SBLIB_VERSION >> 8;
        userEeprom->serial()[5] = SBLIB_VERSION;
    }
    userRam->peiType() = 0;     // PEI type: 0=no adapter connected to PEI.
    userEeprom->appType() = 0;  // Set to BCU2 application. ETS reads this when programming.

    BcuBase::begin_BCU(manufacturer, deviceType,version);
}

void SYSTEMB::setOwnAddress (int addr)
{
    userEeprom->addrTab()[0] = addr >> 8;
    userEeprom->addrTab()[1] = addr;
    if (userEeprom->loadState()[OT_ADDR_TABLE] == LS_LOADING)
    {
        byte * addrTab =  addrTables->addrTable() + 1;

        * (addrTab + 0)  = addr >> 8;
        * (addrTab + 1)  = addr;
    }
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
 * @param readOnlyCommObjectTableAddress - optional (16bit), to set a read-only CommObjectTable address which can't be changed by KNX telegrams
 *                                         This is a workaround for ETS product databases, which send the "wrong" ComObjectTable address.
 *                                         Compare inside the extracted *.knxprod product database M-xxxx_x_xxxx-xx-xxxx.xml
 *                                         node <KNX/ManufacturerData/Manufacturer/ApplicationPrograms/ApplicationProgram/Static/ComObjectTable CodeSegment="M-xxxx_x-xxxx-xx-xxxx_xx-HHHH"
 *                                         HHHH = communication object table address in hexadecimal
 *                                         and
 *                                         node <KNX/ManufacturerData/Manufacturer/ApplicationPrograms/ApplicationProgram/Static/LoadProcedures/LdCtrlTaskSegment LsmIdx="3" Address="dddddd" />
 *                                         dddddd = communication object table address in decimal ETS wants us to use
 *                                         convert dddddd to hexadecimal => WWWW
 *                                         in case HHHH != WWW    z = bcu.getOwnAddress();
 *                                         W ,
 *                                         use bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION, 0xHHHH) to set the correct read-only ComObjectTable address (HHHH)
 */
inline void SYSTEMB::begin(int manufacturer, int deviceType, int version, word readOnlyCommObjectTableAddress)
{
	ownAddr = (userEeprom->addrTab()[0] << 8) | userEeprom->addrTab()[1];
	if (userEeprom->loadState()[OT_ADDR_TABLE] == LS_LOADING)
	{
		byte * addrTab = addrTables->addrTable() + 1;
		ownAddr = (*(addrTab) << 8) | *(addrTab + 1);
	}

    begin_BCU(manufacturer, deviceType, version);
    commObjectTableAddressStatic = readOnlyCommObjectTableAddress;
    if ((commObjectTableAddressStatic != 0) && ( userEeprom->commsTabAddr() != commObjectTableAddressStatic))
    {
        userEeprom->commsTabAddr() = commObjectTableAddressStatic;
        userEeprom->modified();
    }
}

inline bool SYSTEMB::applicationRunning() const
{
    if (!enabled)
        return false;

    return userRam->runState == 1 &&
        userEeprom->loadState()[OT_ADDR_TABLE] == LS_LOADED &&  // Address table object
        userEeprom->loadState()[OT_ASSOC_TABLE] == LS_LOADED && // Association table object &
        userEeprom->loadState()[OT_APPLICATION] == LS_LOADED;   // Application object. All three in State "Loaded"
}

SYSTEMB::SYSTEMB() : SYSTEMB(new UserRamSYSTEMB(), new UserEepromSYSTEMB(bus), new ComObjectsSYSTEMB(this), new AddrTablesSYSTEMB(this), new PropertiesSYSTEMB(this))
{}

SYSTEMB::SYSTEMB(UserRamSYSTEMB* userRam, UserEepromSYSTEMB* userEeprom, ComObjectsSYSTEMB* comObjects, AddrTablesSYSTEMB* addrTables, PropertiesSYSTEMB* properties) :
		BCU2(userRam, userEeprom, comObjects, addrTables, properties)
		//userRam(userRam),
		//userEeprom(userEeprom),
		//comObjects(comObjects),
		//addrTables(addrTables)
{}
