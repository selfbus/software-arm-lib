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

BCU1::BCU1() : BCU1(new UserRamBCU1(), new UserEepromBCU1(this), new ComObjectsBCU1(this), new AddrTablesBCU1(this))
{}

BCU1::BCU1(UserRamBCU1* userRam, UserEepromBCU1* userEeprom, ComObjectsBCU1* comObjects, AddrTablesBCU1* addrTables) :
        BcuDefault(userRam, userEeprom, comObjects, addrTables),
        userRam(userRam),
        userEeprom(userEeprom),
        comObjects(comObjects),
        addrTables(addrTables)
{}

void BCU1::setOwnAddress (uint16_t addr)
{
    if (addr != makeWord(userEeprom->addrTab()[0], userEeprom->addrTab()[1]))
    {
        userEeprom->addrTab()[0] = HIGH_BYTE(addr);
        userEeprom->addrTab()[1] = lowByte(addr);
        userEeprom->modified();
    }
    BcuDefault::setOwnAddress(addr);
}

inline void BCU1::begin(int manufacturer, int deviceType, int version)
{
    setOwnAddress(makeWord(userEeprom->addrTab()[0], userEeprom->addrTab()[1]));
    userRam->status() = BCU_STATUS_LINK_LAYER | BCU_STATUS_TRANSPORT_LAYER | BCU_STATUS_APPLICATION_LAYER | BCU_STATUS_USER_MODE;
    userRam->deviceControl() = 0;
    userRam->runState() = 1;

    userEeprom->runError() = 0xff;
    userEeprom->portADDR() = 0;

    userEeprom->manufacturerH() = HIGH_BYTE(manufacturer);
    userEeprom->manufacturerL() = lowByte(manufacturer);

    userEeprom->deviceTypeH() = HIGH_BYTE(deviceType);
    userEeprom->deviceTypeL() = lowByte(deviceType);

    userEeprom->version() = version;

    userEeprom->writeUserEepromTime = 0;

	begin_BCU(manufacturer, deviceType, version);
	BcuDefault::_begin();
}

bool BCU1::applicationRunning() const
{
    if (!enabled)
        return false;

    uint8_t status = userRam->status();
    uint8_t runState = userRam->runState();
    uint8_t runError = userEeprom->runError();
    return (
            ((status & (BCU_STATUS_PROGRAMMING_MODE | BCU_STATUS_APPLICATION_LAYER)) == BCU_STATUS_APPLICATION_LAYER) &&
             (runState == 1) &&
             (runError == 0xff)  // ETS sets the run error to 0 when programming
           );
}

// The method begin_BCU() is renamed during compilation to indicate the BCU type.
// If you get a link error then the library's BCU_TYPE is different from your application's BCU_TYPE.
void BCU1::begin_BCU(int manufacturer, int deviceType, int version)
{
    BcuDefault::begin_BCU(manufacturer, deviceType, version);
}
