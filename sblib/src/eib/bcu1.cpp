/*
 *  bcu.cpp - EIB bus coupling unit.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/bcu1.h>
#include <sblib/eib/apci.h>
#include <sblib/eib/com_objects.h>
#include <string.h>
#include <sblib/internal/variables.h>
#include <sblib/mem_mapper.h>
#include <sblib/eib/bus.h>

BCU1::BCU1() : BCU1(new UserRamBCU1(), new UserEepromBCU1(), new ComObjectsBCU1(this), new AddrTablesBCU1(this))
{}

BCU1::BCU1(UserRamBCU1* userRam, UserEepromBCU1* userEeprom, ComObjectsBCU1* comObjects, AddrTablesBCU1* addrTables) :
        BcuDefault(userRam, userEeprom, comObjects, addrTables),
        userRam(userRam),
        userEeprom(userEeprom),
        comObjects(comObjects),
        addrTables(addrTables)
{}

inline void BCU1::begin(int manufacturer, int deviceType, int version)
{
    BcuDefault::begin(manufacturer, deviceType, version);
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
             (runError == 0xff)  // ETS sets the run error to 0 while programming
           );
}
