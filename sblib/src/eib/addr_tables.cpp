/*
 *  addr_tables.cpp - BCU communication address tables.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/addr_tables.h>

#include <sblib/eib/property_types.h>
#include <sblib/bits.h>
#include <sblib/eib/userEeprom.h>

uint16_t AddrTables::addrCount()
{
    byte* ptrAddrTable = addrTable();
    return (*ptrAddrTable);
}
