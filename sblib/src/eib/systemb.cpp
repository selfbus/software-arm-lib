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
#include <sblib/eib/apci.h>
#include <sblib/eib/com_objects.h>
#include <string.h>
#include <sblib/internal/variables.h>
#include <sblib/mem_mapper.h>
#include <sblib/internal/iap.h>
#include <sblib/eib/propertiesSYSTEMB.h>
#include <sblib/eib/bus.h>

SYSTEMB::SYSTEMB() : SYSTEMB(new UserRamSYSTEMB(), new UserEepromSYSTEMB(this), new ComObjectsSYSTEMB(this), new AddrTablesSYSTEMB(this), new PropertiesSYSTEMB(this))
{}

SYSTEMB::SYSTEMB(UserRamSYSTEMB* userRam, UserEepromSYSTEMB* userEeprom, ComObjectsSYSTEMB* comObjects, AddrTablesSYSTEMB* addrTables, PropertiesSYSTEMB* properties) :
         MASK0701(userRam, userEeprom, comObjects, addrTables, properties)
{}
