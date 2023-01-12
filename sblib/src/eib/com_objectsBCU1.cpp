/*
 * com_objectsBCU1.cpp
 *
 *  Created on: 19.11.2021
 *      Author: dridders
 */

#include <sblib/eib/com_objectsBCU1.h>
#include <sblib/eib/bcu1.h>
#include <sblib/eib/apci.h>
#include <sblib/utils.h>

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

int ComObjectsBCU1::objectSize(int objno)
{
    // The size of the object types BIT_7...VARDATA in bytes
    const byte objectTypeSizes[10] = { 1, 1, 2, 3, 4, 6, 8, 10, 14, 14 };

    int type = objectType(objno);
    if (type < BIT_7)
        return 1;
    if (type <= VARDATA)
        return objectTypeSizes[type - BIT_7];
    return -1;
}

byte* ComObjectsBCU1::objectValuePtr(int objno)
{
    // The object configuration
    const ComConfigBCU1* cfg = objectConfigBCU1(objno);
    if (cfg == nullptr)
    {
        return (nullptr);
    }
    uint16_t addrObjValue = cfg->dataPtr;
    uint8_t* objValuePtr;
    if (cfg->baseConfig.config & COMCONF_VALUE_TYPE) // 0 in user RAM, >0 in user EEPROM
    {
        addrObjValue += ((BcuDefault*)bcu)->userEeprom->startAddr();
        objValuePtr = ((BcuDefault*)bcu)->userMemoryPtr(addrObjValue);
        // return ((BcuDefault*)bcu)->userEeprom->userEepromData + cfg->dataPtr;
    }
    else
    {
        objValuePtr = ((BcuDefault*)bcu)->userMemoryPtr(addrObjValue);
        // return bcu->userRam->userRamData + cfg->dataPtr;
    }
    return (objValuePtr);


    // if (cfg->baseConfig.config & COMCONF_VALUE_TYPE) // 0 if user RAM, >0 if user EEPROM
    //    return ((BcuDefault*)bcu)->userEeprom->userEepromData + cfg->dataPtr;
    //return bcu->userRam->userRamData + cfg->dataPtr;
}

/*
 *  A Group read/write Telegram on the bus or a
 *  Telegram transmit-request (object read/write) from the app was received
 *
 *  From bus: Called from bus::ProcessTelegram function triggered by the BCUbase-loop function.
 *  From app: Called from sendGroupWriteTelegram or sendGroupReadTelegram
 *
 *  Scan the AssociationTable for the received GrpAdr for a valid association to a local object.
 *
 *  In order to avoid an endless loop by updating we need to skip the triggering object from the app.
 *  In case of a received bus request the triggering objectnumber was set to an invalid value (-1).
 *
 *
 *  @return void
 *
 */
void ComObjectsBCU1::processGroupTelegram(uint16_t addr, int apci, byte* tel, int trg_objno)
{
/**
 * Spec: Resources 4.11.2 Group Object Association Table - Realization Type 1
 */
    const byte* assocTab = bcu->addrTables->assocTable();
    const int endAssoc = 1 + (*assocTab) * 2;
    int objno, objConf;

    DB_COM_OBJ(
            serial.print("grpAddr ", mainGroup(addr));
            serial.print("/", middleGroup(addr));
            serial.print("/", lowGroup(addr));
            serial.print(": gapos ");
            );
    // Convert the group address into the index of the group address table
    const int gapos = bcu->addrTables->indexOfAddr(addr);
    if (gapos < 0)
    {
        DB_COM_OBJ(serial.println("not found"););
        return;
    }
    else
    {
        DB_COM_OBJ(serial.println(gapos););
    }

    // Loop over all entries in the association table, as one group address
    // could be assigned to multiple com-objects.
    for (int idx = 1; idx < endAssoc; idx += 2)
    {
        // Check if grp-address index in assoc table matches the dest grp address index
        if (gapos != assocTab[idx])
        {
            continue;
        }
        // We found an association for our addr
        objno = assocTab[idx + 1];  // Get the com-object number from the assoc table
        DB_COM_OBJ(serial.println("objno  : ", objno););

        if (objno == trg_objno)
        {
            DB_COM_OBJ(serial.println("triggered by app ", objno););
            continue; // no update of the object triggered by the app
        }

        //DB_COM_OBJ(serial.println("commsTabAddr: 0x", ((UserEepromBCU1*)((BcuDefault*)bcu)->userEeprom)->commsTabAddr(), HEX););
        objConf = objectConfig(objno).config;
        DB_COM_OBJ(serial.println("objConf: 0x", objno, HEX, 2););



        if (apci == APCI_GROUP_VALUE_WRITE_PDU || apci == APCI_GROUP_VALUE_RESPONSE_PDU)
        {
            // Check if communication and write are enabled
            if ((objConf & COMCONF_WRITE_COMM) == COMCONF_WRITE_COMM)
                processGroupWriteTelegram(objno, tel); // set update flag and update value of object
        }
        else if (apci == APCI_GROUP_VALUE_READ_PDU)
        {
            // Check if communication and read are enabled
            if ((objConf & COMCONF_READ_COMM) == COMCONF_READ_COMM)
                // we received read-request from bus - so send response back and search for more associations
                sendGroupWriteTelegram(objno, addr, true); // send write to the bus and update all associated local objects
        }
    }
}

byte* ComObjectsBCU1::objectConfigTable() // stored in eeprom
{
    uint16_t commsTabPtr = ((BcuDefault*)bcu)->userEeprom->commsTabPtr();
    if (commsTabPtr == 0)
    {
        return (nullptr);
    }
    commsTabPtr += ((BcuDefault*)bcu)->userEeprom->startAddr(); // for BCU1 add 0x100

    return ((BcuDefault*)bcu)->userMemoryPtr(commsTabPtr);
    // return ((BcuDefault*)bcu)->userEeprom->userEepromData + commsTabPtr; // this did not add 0x100
}

byte* ComObjectsBCU1::objectFlagsTable() // stored in RAM
{
    uint8_t* objCfgTablePtr = objectConfigTable();
    if (objCfgTablePtr == 0)
    {
        return (nullptr);
    }
    objCfgTablePtr++; // add 1 to get the RAM-Flags-Table Pointer
    return ((BcuDefault*)bcu)->userMemoryPtr(*objCfgTablePtr);
    //return (byte*)(((BcuDefault*)bcu)->userRam->userRamData + ((BcuDefault*)bcu)->userEeprom->userEepromData[((BcuDefault*)bcu)->userEeprom->commsTabPtr() + 1]);
}

inline const ComConfig& ComObjectsBCU1::objectConfig(int objno) { return objectConfigBCU1(objno)->baseConfig; }

inline const ComConfigBCU1* ComObjectsBCU1::objectConfigBCU1(int objno)
{
	byte *objConfigTable = objectConfigTable();
	if (objConfigTable == nullptr)
	{
	    return (nullptr);
	}
    return (const ComConfigBCU1*) (objConfigTable + 1 + sizeof(ComConfigBCU1::DataPtrType) + objno * sizeof(ComConfigBCU1) );
}
