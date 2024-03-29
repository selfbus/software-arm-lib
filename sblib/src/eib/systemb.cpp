/*
 *  systemb.cpp - EIB bus coupling unit.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/systemb.h>

SYSTEMB::SYSTEMB() : SYSTEMB(new UserRamSYSTEMB(), new UserEepromSYSTEMB(), new ComObjectsSYSTEMB(this), new AddrTablesSYSTEMB(this), new PropertiesSYSTEMB(this))
{}

SYSTEMB::SYSTEMB(UserRamSYSTEMB* userRam, UserEepromSYSTEMB* userEeprom, ComObjectsSYSTEMB* comObjects, AddrTablesSYSTEMB* addrTables, PropertiesSYSTEMB* properties) :
         MASK0701(userRam, userEeprom, comObjects, addrTables, properties)
{}
