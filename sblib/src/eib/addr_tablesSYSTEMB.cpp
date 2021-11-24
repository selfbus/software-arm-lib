/*
 * addr_tablesBCU1.cpp
 *
 *  Created on: 20.11.2021
 *      Author: dridders
 */

#include <sblib/eib/addr_tablesSYSTEMB.h>
#include <sblib/eib/systemb.h>
#include <sblib/bits.h>

int AddrTablesSYSTEMB::indexOfAddr(int addr)
{
    byte* tab = addrTable();
    int num = 0;

    if (tab)
        num = (tab[0] << 8) + tab[1];
    tab += 2;

    int addrHigh = addr >> 8;
    int addrLow = addr & 255;

    for (int i = 1; i <= num; ++i, tab += 2)
    {
        if (tab[0] == addrHigh && tab[1] == addrLow)
            return i;
    }

    return -1;
}

byte* AddrTablesSYSTEMB::addrTable()
{
    byte * addr = (byte* ) & bcu->userEeprom->addrTabAddr();
    return bcu->userMemoryPtr (makeWord (*(addr + 1), * addr));
}

byte* AddrTablesSYSTEMB::assocTable()
{
    byte * addr = (byte* ) & bcu->userEeprom->assocTabAddr();
    return bcu->userMemoryPtr (makeWord (*(addr + 1), * addr));
}
