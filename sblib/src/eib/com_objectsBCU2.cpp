/*
 * com_objectsBCU2.cpp
 *
 *  Created on: 19.11.2021
 *      Author: dridders
 */

#include <sblib/eib/com_objectsBCU2.h>
#include <sblib/eib/bcu2.h>
#include <sblib/bits.h>

byte* ComObjectsBCU2::objectValuePtr(int objno)
{
    // The object configuration
    const ComConfigBCU2* cfg = objectConfigBCU2(objno);

    // TODO Should handle userRam.segment0addr and userRam.segment1addr here
    // if (cfg.config & COMCONF_VALUE_TYPE) // 0 if segment 0, !=0 if segment 1
    const byte * addr = (const byte *) &cfg->dataPtr;
    if (le_ptr == LITTLE_ENDIAN)
        return ((BcuDefault*)bcu)->userMemoryPtr(makeWord(addr[1], addr[0]));
    else
        return ((BcuDefault*)bcu)->userMemoryPtr(makeWord(addr[0], addr[1]));
}

byte* ComObjectsBCU2::objectConfigTable()
{
    byte * addr = (byte* ) & ((BCU2*)bcu)->userEeprom->commsTabAddr();
    uint16_t comObjTableAddr = makeWord (*(addr + 1), * addr);
    return ((BcuDefault*)bcu)->userMemoryPtr(comObjTableAddr);
}

byte* ComObjectsBCU2::objectFlagsTable()
{
    const byte* configTable = objectConfigTable();
    uint16_t flagsTableAddress;

    if(le_ptr == LITTLE_ENDIAN)
    {
        flagsTableAddress = makeWord(configTable[2], configTable[1]);
    }
    else
    {
        flagsTableAddress = makeWord(configTable[1], configTable[2]);
    }
    return ((BcuDefault*)bcu)->userMemoryPtr(flagsTableAddress);
}

const ComConfigBCU2* ComObjectsBCU2::objectConfigBCU2(int objno)
{
    const byte* configTable = objectConfigTable();
    if (configTable == nullptr)
    {
        return (nullptr);
    }
    uint16_t offSet = 1 + sizeof(ComConfigBCU2::DataPtrType) + objno * sizeof(ComConfigBCU2);
    return (const ComConfigBCU2*) (configTable + offSet);
}

const ComConfig& ComObjectsBCU2::objectConfig(int objno)
{
    return objectConfigBCU2(objno)->baseConfig;
}
