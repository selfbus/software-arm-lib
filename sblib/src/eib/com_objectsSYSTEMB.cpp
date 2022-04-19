/*
 * com_objectsSYSTEMB.cpp
 *
 *  Created on: 19.11.2021
 *      Author: dridders
 */

#include <sblib/eib/com_objectsSYSTEMB.h>
#include <sblib/eib/apci.h>
#include <sblib/eib/systemb.h>
#include <sblib/bits.h>
#include <sblib/eib/typesSYSTEMB.h>

int ComObjectsSYSTEMB::objectSize(int objno)
{
    int type = objectType(objno);
    if (type < BIT_7) return 1;
    if (type < 21)
        return objectTypeSizes[type - BIT_7];
    if (type < 255)
        return (type -6);
    return 252;
}

byte* ComObjectsSYSTEMB::objectValuePtr(int objno)
{
    int ramAddr = bcu->userRam->userRamStart + 2;
    for (int i = 1; i < objno; i++)
        ramAddr += objectSize(i);
    return ((BcuDefault*)bcu)->userMemoryPtr(ramAddr);
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
void ComObjectsSYSTEMB::processGroupTelegram(int addr, int apci, byte* tel, int trg_objno)
{
    //
    // Spec: Resources 4.11.4 Group Object Association Table - Realization Type 6
    //
    const ComConfig* configTab = &objectConfig(0);
    const byte* assocTab = bcu->addrTables->assocTable();
    const int endAssoc = 2 +  makeWord(assocTab[0], assocTab[1]) * 4;   // length field has 2 octets and each entry has 4 octets on SYSTEM B
    int objno, objConf;

    // Convert the group address into the index into the group address table
    const int gapos = bcu->addrTables->indexOfAddr(addr);
    if (gapos < 0) return;

    // Loop over all entries in the association table, as one group address
    // could be assigned to multiple com-objects.
    for (int idx = 2; idx < endAssoc; idx += 4)
    {
        // Check if grp-address index in assoc table matches the dest grp address index
        int gadest = makeWord(assocTab[idx], assocTab[idx +1]); // get destination group address index
        if (gapos == gadest) // We found an association for our addr
        {
            objno = makeWord(assocTab[idx +2], assocTab[idx +3]);  // Get the com-object number from the assoc table
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
                    sendGroupWriteTelegram(objno, addr, true);
            }
        }
    }
}

byte* ComObjectsSYSTEMB::objectConfigTable()
{
    byte * addr = (byte* ) & ((SYSTEMB*)bcu)->userEeprom->commsTabAddr();
    return ((BcuDefault*)bcu)->userMemoryPtr (makeWord (*(addr + 1), * addr));
}

byte* ComObjectsSYSTEMB::objectFlagsTable()
{
    const byte* configTable = objectConfigTable();
    if(le_ptr == LITTLE_ENDIAN)
    	return ((BcuDefault*)bcu)->userMemoryPtr(makeWord(configTable[2], configTable[1]));

    return ((BcuDefault*)bcu)->userMemoryPtr(makeWord(configTable[1], configTable[2]));
}

inline const byte* ComObjectsSYSTEMB::getObjectTypeSizes()
{
	return objectTypeSizes;
}

inline const ComConfig& ComObjectsSYSTEMB::objectConfig(int objno)
{
    return (*(const ComConfigSYSTEMB*) (objectConfigTable() + 2 + (objno -1) * sizeof(ComConfigSYSTEMB) )).baseConfig;
}
