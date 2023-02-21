/*
 * addr_tablesBCU1.cpp
 *
 *  Created on: 20.11.2021
 *      Author: dridders
 */

#include <sblib/eib/addr_tablesBCU1.h>
#include <sblib/eib/bcu1.h>

int AddrTablesBCU1::indexOfAddr(int addr)
{
    byte* tab = addrTable();
    int num = 0;

    if (tab)
        num = *tab;
    tab += 3;

    int addrHigh = addr >> 8;
    int addrLow = addr & 255;

    for (int i = 1; i <= num; ++i, tab += 2)
    {
        if (tab[0] == addrHigh && tab[1] == addrLow)
            return i;
    }

    return -1;
}

byte* AddrTablesBCU1::addrTable()
{
    return (byte*) &bcu->userEeprom->addrTabSize();
}

byte* AddrTablesBCU1::assocTable()
{
    return bcu->userEeprom->userEepromData + bcu->userEeprom->assocTabPtr();
}
