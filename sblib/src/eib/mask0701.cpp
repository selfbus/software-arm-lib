/*
 * mask0701.cpp
 *
 *  Created on: 20.11.2021
 *      Author: dridders
 */

#include <sblib/eib/mask0701.h>
#include <sblib/eib/propertiesMASK0701.h>
#include <cstring>
#include <sblib/utils.h>

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

MASK0701::MASK0701() : MASK0701(new UserRamMASK0701(), new UserEepromMASK0701(this), new ComObjectsBCU2(this), new AddrTablesMASK0701(this), new PropertiesMASK0701(this))
{}

MASK0701::MASK0701(UserRamMASK0701* userRam, UserEepromMASK0701* userEeprom, ComObjectsBCU2* comObjects, AddrTablesMASK0701* addrTables, PropertiesMASK0701* properties) :
		BCU2(userRam, userEeprom, comObjects, addrTables, properties)/*,
		userRam(userRam),
		userEeprom(userEeprom),
		comObjects(comObjects),
		addrTables(addrTables)*/
{}

bool MASK0701::processApciMemoryReadPDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    DB_MEM_OPS(
       serial.print("ApciMemoryReadPDU : 0x", addressStart, HEX, 4);
       serial.print(" count: ", lengthPayLoad, DEC);
    );
    // special handling of DMP_LoadStateMachineRead_RCo_Mem (APCI_MEMORY_READ_PDU)
    // See KNX Spec. 3/5/2 3.30.2 p.121  (deprecated)
    if (addressStart >= LOAD_STATE_ADDR && addressStart < LOAD_STATE_ADDR + INTERFACE_OBJECT_COUNT)
    {
    	unsigned int objectIdx = addressStart - LOAD_STATE_ADDR;
    	// memcpy(payLoad, &userEeprom->loadState() + (addressStart - LOAD_STATE_ADDR), lengthPayLoad);
    	memcpy(payLoad, &userEeprom->loadState()[objectIdx], lengthPayLoad);
    	DB_MEM_OPS(serial.print(" LOAD_STATE_ADDR: 0x", addressStart, HEX); serial.println(", objIdx: ", objectIdx, DEC));
        DB_PROPERTIES(serial.print(" LOAD_STATE_ADDR: 0x", addressStart, HEX); serial.println(", objIdx: ", objectIdx, DEC));
        return true;
    }
    bool result = BcuDefault::processApciMemoryReadPDU(addressStart, payLoad, lengthPayLoad);
    DB_MEM_OPS(
       if (result)
       {
           serial.print("           result :", addressStart, HEX, 4);
           serial.print(" Data:");
           for(int i=0; i<lengthPayLoad ; i++)
           {
               serial.print(" ", payLoad[i], HEX, 2);
           }
           serial.println(" count: ", lengthPayLoad, DEC);
       }
    );
    return result;
}

bool MASK0701::processApciMemoryWritePDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    DB_MEM_OPS(
       serial.print("ApciMemoryWritePDU:", addressStart, HEX, 4);
       serial.print(" Data:");
       for(int i=0; i<lengthPayLoad ; i++)
       {
           serial.print(" ", payLoad[i], HEX, 2);
       }
       serial.print(" count: ", lengthPayLoad, DEC);
    );
    // special handling of DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU)
    // See KNX Spec. 3/5/2 3.28.2 p.109 (deprecated)
    if (addressStart == LOAD_CONTROL_ADDR)
    {
        unsigned int objectIdx = payLoad[0] >> 4;
        DB_PROPERTIES(serial.println(" LOAD_CONTROL_ADDR: objectIdx:", objectIdx, HEX));
        if (objectIdx < INTERFACE_OBJECT_COUNT)
        {
            // userEeprom.loadState[objectIdx] = (uint8_t)loadProperty(objectIdx, &payLoad[0], lengthPayLoad);
            userEeprom->loadState()[objectIdx] = properties->loadProperty(objectIdx, &payLoad[0], lengthPayLoad);
            userEeprom->modified();
            DB_PROPERTIES(serial.println());
            return true;
        }
        else
        {
            DB_PROPERTIES(serial.println(" not found"));
            return false;
        }
    }

    return BcuDefault::processApciMemoryWritePDU(addressStart, payLoad, lengthPayLoad);
}
