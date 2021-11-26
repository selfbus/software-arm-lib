/*
 * mask0701.cpp
 *
 *  Created on: 20.11.2021
 *      Author: dridders
 */

#include <sblib/eib/mask0701.h>
#include <sblib/eib/propertiesMASK0701.h>
#include <cstring>

// Enable informational debug statements
#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#   define DB(x) x
#else
#   define DB(x)
#endif

MASK0701::MASK0701() : MASK0701(new UserRamMASK0701(), new UserEepromMASK0701(this), new ComObjectsMASK0701(this), new AddrTablesMASK0701(this), new PropertiesMASK0701(this))
{}

MASK0701::MASK0701(UserRamMASK0701* userRam, UserEepromMASK0701* userEeprom, ComObjectsMASK0701* comObjects, AddrTablesMASK0701* addrTables, PropertiesMASK0701* properties) :
		BCU2(userRam, userEeprom, comObjects, addrTables, properties)/*,
		userRam(userRam),
		userEeprom(userEeprom),
		comObjects(comObjects),
		addrTables(addrTables)*/
{}

bool MASK0701::processApciMemoryReadPDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    // special handling of DMP_LoadStateMachineRead_RCo_Mem (APCI_MEMORY_READ_PDU)
    // See KNX Spec. 3/5/2 3.30.2 p.121  (deprecated)
    if (addressStart >= 0xb6e9 && addressStart < 0xb6e9 + INTERFACE_OBJECT_COUNT)
    {
    	unsigned int objectIdx = addressStart - 0xb6e9;
        memcpy(payLoad, &userEeprom->loadState()[objectIdx], lengthPayLoad);
        DB(serial.print(" LOAD_STATE_ADDR: ", addressStart, HEX); serial.println(", objIdx: ", objectIdx, HEX));
        return true;
    }

	return BcuBase::processApciMemoryReadPDU(addressStart, payLoad, lengthPayLoad);
}

bool MASK0701::processApciMemoryWritePDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    // special handling of DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU)
    // See KNX Spec. 3/5/2 3.28.2 p.109 (deprecated)
    if (addressStart == 0x104)
    {
        unsigned int objectIdx = payLoad[0] >> 4;
        DB(serial.println(" LOAD_CONTROL_ADDR: objectIdx:", objectIdx, HEX));
        if (objectIdx < INTERFACE_OBJECT_COUNT)
        {
            userEeprom->loadState()[objectIdx] = properties->loadProperty(objectIdx, &payLoad[0], lengthPayLoad);
            userEeprom->modified();
            DB(serial.println());
            return true;
        }
        else
        {
            DB(serial.println(" not found"));
            return false;
        }
    }

    return BcuBase::processApciMemoryWritePDU(addressStart, payLoad, lengthPayLoad);
}
