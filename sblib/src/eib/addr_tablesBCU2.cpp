/*
 * addr_tablesBCU2.cpp
 *
 *  Created on: 20.11.2021
 *      Author: dridders
 */

#include <sblib/eib/addr_tablesBCU2.h>
#include <sblib/eib/bcu2.h>
#include <sblib/bits.h>

int AddrTablesBCU2::indexOfAddr(int addr)
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

byte* AddrTablesBCU2::addrTable()
{
    byte * addr = (byte* ) & bcu->userEeprom->addrTabAddr();
    unsigned short memAddr = makeWord (*(addr + 1), * addr);

    return bcu->userMemoryPtr (memAddr);
}

byte* AddrTablesBCU2::assocTable()
{
    byte * addr = (byte* ) & bcu->userEeprom->assocTabAddr();
    return bcu->userMemoryPtr (makeWord (*(addr + 1), * addr));
}

uint16_t AddrTablesBCU2::addrCount()
{
    byte* ptrAddrTable = addrTable();
    uint16_t count = makeWord (*(ptrAddrTable + 1), * ptrAddrTable);
    return (count);
}
