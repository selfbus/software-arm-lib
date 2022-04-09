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

void BCU1::setOwnAddress (uint16_t addr)
{
    if (addr != makeWord(userEeprom->addrTab()[0], userEeprom->addrTab()[1]))
    {
        userEeprom->addrTab()[0] = HIGH_BYTE(addr);
        userEeprom->addrTab()[1] = LOW_BYTE(addr);
        userEeprom->modified();
    }
    BcuBase::setOwnAddress(addr);
}

inline void BCU1::begin(int manufacturer, int deviceType, int version)
{
    setOwnAddress(makeWord(userEeprom->addrTab()[0], userEeprom->addrTab()[1]));
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

	begin_BCU(manufacturer, deviceType, version);
    BcuBase::begin();
}

inline bool BCU1::applicationRunning() const
{
    if (!enabled)
        return false;

    return ((userRam->status & (BCU_STATUS_PROG|BCU_STATUS_AL)) == BCU_STATUS_AL &&
        userRam->runState == 1 && userEeprom->runError() == 0xff); // ETS sets the run error to 0 when programming
}

BCU1::BCU1() : BCU1(new UserRamBCU1(), new UserEepromBCU1(this), new ComObjectsBCU1(this), new AddrTablesBCU1(this))
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
    BcuBase::begin_BCU(manufacturer, deviceType, version);
}
