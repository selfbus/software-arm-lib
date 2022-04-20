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
#include <sblib/eib/bcu2.h>
#include <sblib/eib/apci.h>
#include <sblib/eib/com_objects.h>
#include <string.h>
#include <sblib/internal/variables.h>
#include <sblib/mem_mapper.h>
#include <sblib/internal/iap.h>
#include <sblib/eib/propertiesBCU2.h>
#include <sblib/eib/bus.h>

void BCU2::begin_BCU(int manufacturer, int deviceType, int version)
{
    BcuDefault::begin_BCU(manufacturer, deviceType,version);
}

void BCU2::setOwnAddress (uint16_t addr)
{
    ///\todo some parts are the same in BCU1
    if (addr != makeWord(userEeprom->addrTab()[0], userEeprom->addrTab()[1]))
    {
        userEeprom->addrTab()[0] = HIGH_BYTE(addr);
        userEeprom->addrTab()[1] = LOW_BYTE(addr);
        if (userEeprom->loadState()[OT_ADDR_TABLE] == LS_LOADING)
        {
            byte * addrTab = addrTables->addrTable() + 1;
            * (addrTab + 0)  = userEeprom->addrTab()[0];
            * (addrTab + 1)  = userEeprom->addrTab()[1];
        }
        userEeprom->modified();
    }
    BcuDefault::setOwnAddress(addr);
}

inline void BCU2::begin(int manufacturer, int deviceType, int version, word readOnlyCommObjectTableAddress)
{
    setOwnAddress(makeWord(userEeprom->addrTab()[0], userEeprom->addrTab()[1]));
    ///\todo same like in bcu1
    userRam->status = BCU_STATUS_LL | BCU_STATUS_TL | BCU_STATUS_AL | BCU_STATUS_USR;
    userRam->deviceControl() = 0;
    userRam->runState = 1;

    userEeprom->runError() = 0xff;
    userEeprom->portADDR() = 0;

    userEeprom->manufacturerH() = HIGH_BYTE(manufacturer);
    userEeprom->manufacturerL() = LOW_BYTE(manufacturer);

    userEeprom->deviceTypeH() = HIGH_BYTE(deviceType);
    userEeprom->deviceTypeL() = LOW_BYTE(deviceType);

    userEeprom->version() = version;

    userEeprom->writeUserEepromTime = 0;
    ///\todo end

    userRam->peiType() = 0;     // PEI type: 0=no adapter connected to PEI.
    userEeprom->appType() = 0;  // Set to BCU2 application. ETS reads this when programming.

    if (userEeprom->loadState()[OT_ADDR_TABLE] == LS_LOADING)
    {
        byte * addrTab = addrTables->addrTable() + 1;
        setOwnAddress(makeWord((*(addrTab) << 8) , *(addrTab + 1)));
    }

    commObjectTableAddressStatic = readOnlyCommObjectTableAddress;
    if ((commObjectTableAddressStatic != 0) && ( userEeprom->commsTabAddr() != commObjectTableAddressStatic))
    {
        userEeprom->commsTabAddr() = commObjectTableAddressStatic;
        userEeprom->modified();
    }

    unsigned int partID;
    bool useOldSerialStyle;
    byte uniqueID[IAP_UID_LENGTH];

    useOldSerialStyle = true;
    if (iapReadUID(&uniqueID[0]) == IAP_SUCCESS)
    {
        // https://community.nxp.com/t5/LPC-Microcontrollers/IAP-C-code-example-query/m-p/596139/highlight/true#M22963
        // Unfortunately the details of what go into the 128-bit GUID cannot be disclosed.
        // It can be said, however, that the 128-bit GUIDs are not random, nor are they sequential.
        // Thus to ensure there are no collisions with other devices, the full 128 bits should be used.

        // create a 48bit serial/hash from the 128bit Guid
        useOldSerialStyle = !hashUID(&uniqueID[0], sizeof(uniqueID), &userEeprom->serial()[0], userEeprom->serialSize());
    }

    if (useOldSerialStyle)
    {
        iapReadPartID(&partID);
        memcpy (userEeprom->serial(), &partID, 4);
        userEeprom->serial()[4] = HIGH_BYTE(SBLIB_VERSION);
        userEeprom->serial()[5] = LOW_BYTE(SBLIB_VERSION);
    }

    userEeprom->orderInfo()[userEeprom->orderInfoSize() - 2] = HIGH_BYTE(SBLIB_VERSION);
    userEeprom->orderInfo()[userEeprom->orderInfoSize() - 1] = LOW_BYTE(SBLIB_VERSION);

    begin_BCU(manufacturer, deviceType, version);
    BcuDefault::_begin();
}

void BCU2::begin(int manufacturer, int deviceType, int version)
{
    begin(manufacturer, deviceType, version, 0);
}

inline bool BCU2::applicationRunning() const
{
    if (!enabled)
        return false;

    return userRam->runState == 1 &&
    		userEeprom->loadState()[OT_ADDR_TABLE] == LS_LOADED &&  // Address table object
			userEeprom->loadState()[OT_ASSOC_TABLE] == LS_LOADED && // Association table object &
			userEeprom->loadState()[OT_APPLICATION] == LS_LOADED;   // Application object. All three in State "Loaded"
}

BCU2::BCU2() : BCU2(new UserRamBCU2(), new UserEepromBCU2(this), new ComObjectsBCU2(this), new AddrTablesBCU2(this), new PropertiesBCU2(this))
{}

BCU2::BCU2(UserRamBCU2* userRam, UserEepromBCU2* userEeprom, ComObjectsBCU2* comObjects, AddrTablesBCU2* addrTables, PropertiesBCU2* properties) :
		BcuDefault(userRam, userEeprom, comObjects, addrTables),
		userRam(userRam),
		userEeprom(userEeprom),
//		comObjects(comObjects),
//		addrTables(addrTables),
		properties(properties)
{}

unsigned char BCU2::processApci(ApciCommand apciCmd, const uint16_t senderAddr, const int8_t senderSeqNo, bool *sendResponse, unsigned char *telegram, uint8_t telLength)
{
    uint8_t count;
    uint16_t address;
    uint8_t index;
    unsigned char sendAckTpu = 0;
    *sendResponse = false;
    bool found;
    uint8_t id;

    switch (apciCmd)
    {
    case APCI_PROPERTY_VALUE_READ_PDU:
    case APCI_PROPERTY_VALUE_WRITE_PDU:
        index = telegram[8];
        id = telegram[9];
        count = telegram[10] >> 4;
        address = ((telegram[10] & 0x0f) << 4);
        address = (uint16_t)(address | (telegram[11]));

        sendTelegram[5] = 0x60 + 5; // routing count in high nibble + response length in low nibble
        setApciCommand(sendTelegram, APCI_PROPERTY_VALUE_RESPONSE_PDU, 0);
        sendTelegram[8] = index;
        sendTelegram[9] = id;
        sendTelegram[10] = telegram[10];
        sendTelegram[11] = telegram[11];

        if (apciCmd == APCI_PROPERTY_VALUE_READ_PDU)
            found = properties->propertyValueReadTelegram(index, (PropertyID) id, count, address);
        else
            found = properties->propertyValueWriteTelegram(index, (PropertyID) id, count, address);
        if (!found) sendTelegram[10] = 0;
        *sendResponse = true;
        break;

    case APCI_PROPERTY_DESCRIPTION_READ_PDU:
        index = telegram[8];
        id = telegram[9];
        address = telegram[10];
        sendTelegram[5] = 0x60 + 8; // routing count in high nibble + response length in low nibble
        setApciCommand(sendTelegram, APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU, 0);
        sendTelegram[8] = index;
        sendTelegram[9] = id;
        sendTelegram[10] = (uint8_t)address;
        properties->propertyDescReadTelegram(index, (PropertyID) id, address);
        *sendResponse = true;
        break;

    default:
        sendAckTpu = BcuDefault::processApci(apciCmd, senderAddr, senderSeqNo, sendResponse, telegram, telLength);
        break;
    }
    return (sendAckTpu);
}

word BCU2::getCommObjectTableAddressStatic() const
{
    return commObjectTableAddressStatic;
}

void BCU2::setHardwareType(const byte* hardwareType, uint8_t size)
{
    if (size > userEeprom->orderSize())
    {
        size = userEeprom->orderSize();
    }
    memcpy(userEeprom->order(), hardwareType, size);
}
