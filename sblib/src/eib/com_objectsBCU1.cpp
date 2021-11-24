/*
 * com_objectsBCU1.cpp
 *
 *  Created on: 19.11.2021
 *      Author: dridders
 */

#include <sblib/eib/com_objectsBCU1.h>
#include <sblib/eib/bcu1.h>
#include <sblib/eib/apci.h>

int ComObjectsBCU1::objectSize(int objno)
{
    int type = objectType(objno);
    if (type < BIT_7) return 1;
    return objectTypeSizes[type - BIT_7];
}

byte* ComObjectsBCU1::objectValuePtr(int objno)
{
    // The object configuration
    const ComConfigBCU1& cfg = (ComConfigBCU1&)objectConfig(objno);

    if (cfg.config & COMCONF_VALUE_TYPE) // 0 if user RAM, >0 if user EEPROM
        return bcu->userEeprom->userEepromData + cfg.dataPtr;
    return bcu->userRam->userRamData + cfg.dataPtr;
}

/*
 *  A Group read/write Telegram on the bus or a
 *  Telegram transmitt-request (object read/write)from the app was received
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
void ComObjectsBCU1::processGroupTelegram(int addr, int apci, byte* tel, int trg_objno)
{
/**
 * Spec: Resources 4.11.2 Group Object Association Table - Realisation Type 1
 */
    const ComConfig* configTab = &objectConfig(0);
    const byte* assocTab = bcu->addrTables->assocTable();
    const int endAssoc = 1 + (*assocTab) * 2;
    int objno, objConf;

    // Convert the group address into the index into the group address table
    const int gapos = bcu->addrTables->indexOfAddr(addr);
    if (gapos < 0) return;

    // Loop over all entries in the association table, as one group address
    // could be assigned to multiple com-objects.
    for (int idx = 1; idx < endAssoc; idx += 2)
    {
        // Check if grp-address index in assoc table matches the dest grp address index
        if (gapos == assocTab[idx]) // We found an association for our addr
        {
            objno = assocTab[idx + 1];  // Get the com-object number from the assoc table

            if (objno == trg_objno)
            	continue; // no update of the object triggered by the app

            objConf = configTab[objno].config;

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
}

byte* ComObjectsBCU1::objectConfigTable()
{
    return bcu->userEeprom->userEepromData + bcu->userEeprom->commsTabPtr();
}

byte* ComObjectsBCU1::objectFlagsTable()
{
    return (byte*)(bcu->userRam->userRamData + bcu->userEeprom->userEepromData[bcu->userEeprom->commsTabPtr() + 1]);
}

inline const byte* ComObjectsBCU1::getObjectTypeSizes()
{
	return objectTypeSizes;
}

inline const ComConfig& ComObjectsBCU1::objectConfig(int objno)
{
    return *(const ComConfigBCU1*) (objectConfigTable() + 2 + (objno -1) * sizeof(ComConfigBCU1) );
}
