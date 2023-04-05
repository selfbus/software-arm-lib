/*
 *  bcu.cpp - EIB bus coupling unit.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/bcu2.h>
#include <sblib/eib/apci.h>
#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/com_objects.h>
#include <string.h>
#include <sblib/internal/variables.h>
#include <sblib/mem_mapper.h>
#include <sblib/internal/iap.h>
#include <sblib/eib/propertiesBCU2.h>
#include <sblib/eib/bus.h>

void BCU2::setOwnAddress(uint16_t addr)
{
    BcuDefault::setOwnAddress(addr);
    if (userEeprom->loadState()[OT_ADDR_TABLE] == LS_LOADING)
    {
        byte * addrTab = addrTables->addrTable() + 1;
        * (addrTab + 0)  = HIGH_BYTE(ownAddress());
        * (addrTab + 1)  = lowByte(ownAddress());
        userEeprom->modified(true);
    }
}

inline void BCU2::begin(int manufacturer, int deviceType, int version, word readOnlyCommObjectTableAddress)
{
    BcuDefault::begin(manufacturer, deviceType, version);

    userRam->peiType() = 0;     // PEI type: 0=no adapter connected to PEI.
    userEeprom->appType() = 0;  // Set to BCU2 application. ETS reads this when programming.

    if (userEeprom->loadState()[OT_ADDR_TABLE] == LS_LOADING)
    {
        byte * addrTab = addrTables->addrTable() + 1;
        setOwnAddress(makeWord(*(addrTab) , *(addrTab + 1)));
    }

    commObjectTableAddressStatic = readOnlyCommObjectTableAddress;
    if ((commObjectTableAddressStatic != 0) && ( userEeprom->commsTabAddr() != commObjectTableAddressStatic))
    {
        userEeprom->commsTabAddr() = commObjectTableAddressStatic;
        userEeprom->modified(true);
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
        userEeprom->serial()[5] = lowByte(SBLIB_VERSION);
    }

    userEeprom->orderInfo()[userEeprom->orderInfoSize() - 2] = HIGH_BYTE(SBLIB_VERSION);
    userEeprom->orderInfo()[userEeprom->orderInfoSize() - 1] = lowByte(SBLIB_VERSION);

    BcuDefault::_begin();
}

void BCU2::begin(int manufacturer, int deviceType, int version)
{
    begin(manufacturer, deviceType, version, 0);
}

bool BCU2::applicationRunning() const
{
    if (!enabled)
        return false;

    return ((userRam->runState() == 1) &&
            (userEeprom->loadState()[OT_ADDR_TABLE] == LS_LOADED) &&  // address table object
			(userEeprom->loadState()[OT_ASSOC_TABLE] == LS_LOADED) && // association table object
			(userEeprom->loadState()[OT_APPLICATION] == LS_LOADED));  // application object. All three in state "Loaded"
}

BCU2::BCU2() : BCU2(new UserRamBCU2(), new UserEepromBCU2(), new ComObjectsBCU2(this), new AddrTablesBCU2(this), new PropertiesBCU2(this))
{}

BCU2::BCU2(UserRamBCU2* userRam, UserEepromBCU2* userEeprom, ComObjectsBCU2* comObjects, AddrTablesBCU2* addrTables, PropertiesBCU2* properties) :
		BcuDefault(userRam, userEeprom, comObjects, addrTables),
		userRam(userRam),
		userEeprom(userEeprom),
		properties(properties)
{}

bool BCU2::processApci(ApciCommand apciCmd, const uint16_t senderAddr, const int8_t senderSeqNo, unsigned char *telegram, uint8_t telLength)
{
    uint8_t count;
    uint16_t address;
    uint8_t index;
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
        return (true);

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
        return (true);

    default:
        return BcuDefault::processApci(apciCmd, senderAddr, senderSeqNo, telegram, telLength);
    }
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

bool BCU2::processBroadCastTelegram(ApciCommand apciCmd, unsigned char *telegram, uint8_t telLength)
{
    switch (apciCmd)
    {
        case APCI_INDIVIDUALADDRESS_SERIALNUMBER_WRITE_PDU:
            // telegram[8-13] contains serial number to check, telegram[14-15] = new physical address
            if (memcmp(&telegram[8], &userEeprom->serial()[0], userEeprom->serialSize()) != 0)
            {
                return (false);
            }
            setOwnAddress(makeWord(telegram[14], telegram[15]));
            break;

        case APCI_INDIVIDUALADDRESS_SERIALNUMBER_READ_PDU:
            // telegram[8-13] contains serial number to check
            if (memcmp(&telegram[8], &userEeprom->serial()[0], userEeprom->serialSize()) != 0)
            {
                return (false);
            }
            sendApciIndividualAddressSerialNumberReadResponse();
            break;

        default :
            // let base class handle the broadcast
            return (BcuDefault::processBroadCastTelegram(apciCmd, telegram, telLength));
    }
    return (true);
}

void BCU2::sendApciIndividualAddressSerialNumberReadResponse()
{
    acquireSendBuffer();
    initLpdu(sendTelegram, PRIORITY_SYSTEM, false, FRAME_STANDARD);
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    setDestinationAddress(sendTelegram, 0x0000); // Zero target address, it's a broadcast
    sendTelegram[5] = 0xe0 + 11; // address type & routing count in high nibble + response length (11) in low nibble
    setApciCommand(sendTelegram, APCI_INDIVIDUALADDRESS_SERIALNUMBER_RESPONSE_PDU, 0);
    // sendTelegram[8-13] contains serial number
    for (uint8_t i = 0; i < userEeprom->serialSize(); i++)
    {
        sendTelegram[8+i] = userEeprom->serial()[i];
    }

    // sendTelegram[14-15] contains domain address
    sendTelegram[14] = 0x00;
    sendTelegram[15] = 0x00;

    // sendTelegram[16-17] reserved
    sendTelegram[16] = 0x00;
    sendTelegram[17] = 0x00;

    sendPreparedTelegram();
}
