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

int AddrTables::objectOfAddr(int addr)
{
    int addrIndex = indexOfAddr(addr);

    byte* tab = assocTable();
    int num = 0;
    if (tab)
        num = *tab++;

    for (int i = 0; i < num; ++i, tab += 2)
    {
        if (tab[0] == addrIndex)
            return tab[1];
    }

    return -1;
}

int AddrTables::addrForSendObject(int objno)
{
    return 0;
}
