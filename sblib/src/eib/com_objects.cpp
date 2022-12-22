/**
 *  com_objects.cpp - EIB Communication objects.
 *
 *  Copyright (C) 2014-2015 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  last update: Jan. 2021 Horst Rauch
 *  			 - added some explanation on the functions of this interface between
 *  			   the user application and the communication functions of the knx bcu->bus->
 *  			 - changed some function for the transmission of object from
 *  			 user appl. to knx bus and vice versa as there was an endless loop in some cases
 *
 *  This program is the main interface for the communication of the user application of the knx bus device,
 *  based on the functions of a BCUx or BIMx provided by the complete library.
 *  The basic interface as describe in knx spec 3.3.7, 3.4.1, 3.5.1 is hidden by some functions to write and read
 *  certain object types. We include some parts of these specs for a better understanding of the interface functions.
 *  The program implements functions of the Application Interface Layer (AIL) and the Application Layer of the KNX stack.
 *  Within these layers, a mapping of the GroupObject (of the user application) to the GroupAddress (of the knx bus/system)
 *  is provided via the Association table and the GroupAddress table. As there could be a n:m relation between the GroupObject
 *  and the GroupAddresses the Appl. layer will send respective msg to all matching associations (local in the device and on the bus).
 *
 *  The interface between the user application and the knx system is based on so called GroupObjects
 *  (GO, also communication objects).  The GroupObject Server act as an interface between the user application and
 *  the application layer of the knx stack. As the (communication) stack layers and the knx bus system has no information
 *  on the type and characteristics of a GO from the user application, data is provided by the
 *  user appl. and partly configured by the ETS:
 *
 *  The GO data consist of three parts:
 *		1. the Group Object description,  comprising Value Length/Type, Priority and Config Flags
 *		2. the Group Object value and
 *		3. the Group Object communication flags ( RAM Flags).
 *
 *	The Group Object description must at least include the Group Object Type and the transmission priority.
 *
 *	The Config Flags include static information about the Group Object:
 *		Bit	Field name
 *		7 	update enable, Shall be 1 in BCU1, for receive of GroupValue_Read.resp: 0=object not updated,
 *			1= object updated (update RAM Flag)
 *		6 	Transmit Enable, request from user application to trasnmit an object (write/response): 0=no transmit,
 *			1=msg generated if communication flag enabled
 *		5 	Segment Selector Type, storage segment of the object value: 0=RAM,1= Eeprom
 *		4 	Write Enable, for GroupValue_Write.ind msg: 0=no update of the obj, 1= obj.Value is updated
 *			if communication flag enabled
 *		3 	Read Enable, for GroupValue_Read.ind msg: 0=no response is generated, 1=response send
 *			if communication flag enabled
 *		2 	Communication Enable, enables/disable communication by this obj. e.g. messages are handled/not handled
 *		1-0 Transmission Priority, 00=system, 10=urgent, 01=normal or 11=low.
 *
 *
 *	The communication flags (RAM FLags) show the state of a Group Object and are encoded as follows:
 *		Bit 	Name 				Coding
 *		3 		update-flag 		0= not updated, 1= the value of the group object has been from the bus by
 *									an A_GroupValue_Write.ind or an A_GroupValue_Read.res.
 *		2 		data request flag 	0= idle/response, 1= data-request
 *		1-0		transmission status 00= idle/ok, 01= idle/error, 10= transmitting, 11= transmit request
 *
 *  The RAM Flags are used to link the asynchronous processes of the user application with the kxn stack (BCU lib).
 *
 *//*
 * Group Object value transfer functionality (see 3.4.1) of the Application Interface Layer:
 *
 *	The application process triggers Group Object value transfers by "setting" or "clearing" the relevant
 *	communication flags of a Group Object. The Group Objects, or their images, are be held in the Group
 *	Object Server (AIL/AL). The communication flags play an essential role in triggering the Application Interface
 *	Layer's Group Object service to initiate the transfers. The local access to a Group Object of the Group
 *	Object server stimulates the Group Object server to initiate a network wide update of that Group
 *	Object. Complementary, if an update has been received, the local application shall be triggered to use the
 *	new value.
 *
 *	There are four cases to be considered:
 *	-the application wants
 *		1. to read the Group Object's value
 *		2. to write the Group Object's value or
 *	-the Group Object service has received from the application layer (e.g. from knx bus or from the user application)
 *		3. a request to read the Group Object’s value
 *		4. an update on the Group Object's value.
 *
 * !note: If one device sends an A_GroupValue_Service each device that is member of this group shall receive the A_GroupValue_Service.
 *  With this requirement, all members of a Group will have to all times the same status (Object value) in the distributed system.
 *
 *  It is also necessary to sync local objects on one single device if they belong to the same group. E.g. a local object-write will
 *  trigger update of local GO if they share the same GA.
 *
 *  A local data request (mapped to read-request service on the bus which triggers a read-response from remote device) must check
 *  all local objects and in case they share the GA a local read-response as well as a bus read-response are  generated. Whereas overall
 *  multiple response are possible, one one single response should be generated on local side.
 *
 * These functions are provided and handled by this module.
 *
 *
 *
 * *********Interface to the user application***********************
 * The interface to the user application is implemented by none blocking functions to read or write the GO, the detailed
 * handling of the GO flags and configuration is hidden within the module from user application. Only ObjectRead(),
 * ObjectWrite() and ObjectUpdate() are mainly used by the user application.
 *
 * The transmission status of a read or write request can be checked by reading the GO RAM flags via the objectFlagsTable().
 * The AIL/AL layer will set the transmission status accordingly (bit 0,1 of the RAM Flag).
 * In order to avoid data loss (acc. to knx spec, a new transmission should not be initiated as
 * long as the transmit flags are not in idle state) the user application could check the transmission status
 * and trigger a new write/read if the GO transmission status is idle. The user application might check the error state
 * of a transmission and act accordingly.
 *
 * Any new request from the user application via the ObjectRead() or ObjectWrite() functions will
 * clear the previous state of transmission and data request flag.
 *
 *
 *
 * ********** Functions of the Application Interface Layer (AIL) and Application layer (AL)******
 *
 * 	---User Application triggered---
 *
 * 	1. user appl. wants to read a Group Object value by a call to respective function (received by the AIL) requestObjectRead():
 * 	   	Check if config flag communication enabled is set, if set: set RAM flag data request and transmission request.
 *		If service request was not successful, return false.
 *
 *
 * 	2. user appl. wants to write a Group Object value (received by the AIL)by a call to respective function objectwrite():
 * 		Check if config flag transmission enable (not available in BCU1, always enabled) and communication enabled are set,
 * 		if set: set transmission request flag.
 *		If service request was not successful, return false.
 *

 * 	The respective requests will be handled by the asynchronous process of the AIL/AL by a call from the
 * 	bcu->loop function to sendnextGroupTelegram() which will check the transmission request state of the RAM flag.
 *
 * 	If the transmission request and data request flag are set, then the user wants to read the object:
 *
 *	 	1. Search the Association table for the GroupObject number (ASAP) and get the GroupAddr-index (TSAP)
 *		of the associated obj-number (ASAP). Get the GroupAddress from the GroupAddressTable with the GroupAddr-Index (TSAP)
 *		and generate a GroupValue_read.req msg for the AL and send the msg to the bcu->bus->
 *
 *		2. Set the RAM flag transmission status to transmitting if positively send to local transport/network layer.
 *
 *		3. todo: Search the association table with the TSAP for further associations with TSAP. For each found, get the ASAP,
 *		check the ConfigFlags communication enable and read enable. If all enabled, stop search, get value of the found GO
 *		and update the initial GO with the new GO value and set the update RAM flag and send a read.response with
 *		the found object value and association (GroupAddress)to the bcu->bus->
 *
 *	If only the transmission request is set, then the user wants to write the object:
 *
 *	 	1. Search the Association table from top for the first GroupObject number (ASAP) and get the GroupAddr-index (TSAP)
 * 		of the associated obj-number (ASAP). Get the GroupAddress from the GroupAddressTable with the GroupAddr-Index (TSAP)
 * 		and generate a GroupValue_write.req msg for the AL and send the msg to the bcu->bus->
 *
 * 		2. Set the RAM flag transmission status to transmitting if positively send to local transport/network layer.
 *
 * 		3. Search the association table with the TSAP for further associations with TSAP. For each found, get the ASAP,
 * 		check the ConfigFlags communication enable and write enable and set the update flag and store the new
 * 		GO value (value from the triggering GO) if enabled by the config flags
 *
 *
 * 	---BUS received Telegram triggered---
 * A call to processTelegram() from the bcu->loop initiates the handling of a new received telegram on the app-layer.
 * The received telegram is based on a valid GA (found in the GA Address Table) associated to a local object.
 *
 *		If the service request (APCI) is a write.request:
 *	 	1. Search the Association table for the the GroupAddr-index (TSAP)and get GroupObject number (ASAP).
 *	 	For each found, get the ASAP, check the ConfigFlags communication enable and write enable.
 *	 	If both enabled set the RAM update flag and store the new GO value (value from the triggering Telegram).
 *
 *		If the service request (APCI) is a read.response:
 *	 	1. Search the Association table for the the GroupAddr-index (TSAP)and get GroupObject number (ASAP).
 *	 	For each found, get the ASAP, check the ConfigFlags communication enable and update enable (Update enable
 *	 	flag is not available in BCU1, always enabled).
 *	 	If both enabled set the RAM update flag and store the new GO value (value from the triggering Telegram).
 *
 *		If the service request (APCI) is a read.request:
 *		1. Search the Association table for the the GroupAddr-index (TSAP)and get GroupObject number (ASAP).
 *		For each found, get the ASAP, check the ConfigFlags communication enable, transmit enable and read enable. If all enabled,
 *		get value of the found GO send a read.response with the found object value and association (GroupAddress)to the bcu->bus->
 *
 *
 *
 * todo: reflect the BUS result of sending/receiving of a telegram in the respective RAM-Flags for an object
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/com_objects.h>

#include <sblib/eib/addr_tables.h>
#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/apci.h>
#include <sblib/eib/property_types.h>
#include <sblib/eib/bcu_base.h>
#include <sblib/eib/bus.h>

#if defined(DUMP_COM_OBJ)
#   include <sblib/serial.h>
#endif

#ifdef DUMP_COM_OBJ
#   define d(x) x
#else
#   define d(x)
#endif

/** The COMFLAG_UPDATE flag, moved to the high nibble */
#define COMFLAG_UPDATE_HIGH (COMFLAG_UPDATE << 4)

/** The COMFLAG_TRANS_MASK mask, moved to the high nibble */
#define COMFLAG_TRANS_MASK_HIGH (COMFLAG_TRANS_MASK << 4)

ComObjects::ComObjects(BcuBase* bcuInstance) :
    bcu(bcuInstance),
    le_ptr(BIG_ENDIAN),
    transmitting_object_no(INVALID_OBJECT_NUMBER),
    sendNextObjIndex(0),
    nextUpdatedObjIndex(0)
{
}

ComObjects::~ComObjects()
{
}

int ComObjects::telegramObjectSize(int objno)
{
    int type = objectType(objno);
    if (type < BIT_7) return 0;
    return getObjectTypeSizes()[type - BIT_7];
}

void ComObjects::addObjectFlags(int objno, int flags)
{
    byte* flagsTab = objectFlagsTable();
    if(flagsTab == 0)
    	return;


    if (objno & 1)
        flags <<= 4;

    d(serial.print(" addObjFlags in (obj, flags): ");)
  	d(serial.print(objno, DEC, 2);)
    d(serial.print(", ");)
  	d(serial.print(flags, HEX, 2);)
  	d(serial.print(", is: ");)
  	d(serial.print(flagsTab[objno >> 1], HEX, 2);)

    flagsTab[objno >> 1] |= flags;

    d(serial.print(", out: ");)
	d(serial.print(flagsTab[objno >> 1], HEX, 2);)
	d(serial.println();)
}

void ComObjects::setObjectFlags(int objno, int flags)
{
    byte* flagsPtr = objectFlagsTable();
    if (flagsPtr == nullptr)
    {
        return;
    }
    flagsPtr += objno >> 1; // "select" high or low nibble according to objno odd or even

    d(
      serial.print(" setObjFlags obj: ", objno, DEC, 2);
      serial.print(" is: ", *flagsPtr, HEX, 2);
	  serial.print(" to: ", flags, HEX, 2);
    )

    if (objno & 1)
    {
        *flagsPtr &= 0x0f;
        *flagsPtr |= flags << 4;
    }
    else
    {
        *flagsPtr &= 0xf0;
        *flagsPtr |= flags;
    }
	d(serial.println(" out: ", *flagsPtr, HEX, 2);)
}

unsigned int ComObjects::objectRead(int objno)
{
	int sz = objectSize(objno);
	byte* ptr = objectValuePtr(objno) + sz;
	unsigned int value = *--ptr;

	while (--sz > 0)
	{
		value <<= 8;
		value |= *--ptr;
	}
    return value;
}

void ComObjects::_objectWrite(int objno, unsigned int value, int flags)
{
    byte* ptr = objectValuePtr(objno);
    if (ptr == nullptr)
    {
        return;
    }
    int sz = objectSize(objno);

    if ((ptr == 0) || (sz == 0xff)) ///\todo check maximum possible objectSize sz = objectTypeSizes[10] = 15? 0xff can indicate a corrupted Ram/EEPROM
        return;

    for (; sz > 0; --sz)
    {
        *ptr++ = value;
        value >>= 8;
    }

    //addObjectFlags(objno, flags);
    setObjectFlags(objno, flags); //clear any pending ram com object flags and set new flags
}

void ComObjects::_objectWriteBytes(int objno, byte* value, int flags)
{
    byte* ptr = objectValuePtr(objno);
    int sz = objectSize(objno);

    for (; sz > 0; --sz)
        *ptr++ = *value++;

    addObjectFlags(objno, flags);
}

inline int ComObjects::objectCount()
{
    // The first byte of the config table contains the number of com-objects
    return *objectConfigTable();
}

int ComObjects::firstObjectAddr(int objno)
{
    byte* assocTab = bcu->addrTables->assocTable();
    byte* assocTabEnd = assocTab + (*assocTab << 1);

    for (++assocTab; assocTab < assocTabEnd; assocTab += 2)
    {
        if (assocTab[1] != objno)
        {
            continue;
        }
        uint16_t addressTablePos = (assocTab[0]) + 1; // +1 because first address in addressTable is our own address

        if (addressTablePos > bcu->addrTables->addrCount())
        {
            continue;
        }

        byte* addr = bcu->addrTables->addrTable() + 1 + (assocTab[0] << 1);
        return ((addr[0] << 8) | addr[1]);
    }
    return (0);
}

void ComObjects::sendGroupReadTelegram(int objno, int addr)
{
	while (bcu->sendTelegram[0]);  // wait for a free buffer
    ///\todo Set routing count and priority according to the parameters set from ETS in the EEPROM, add ID/objno for result association from bus-layer
    // check of spec 3.7.4. : no additional search for associations to Grp Addr for local read and possible response
    initLpdu(bcu->sendTelegram, PRIORITY_LOW, false, FRAME_STANDARD);
    setDestinationAddress(bcu->sendTelegram, addr);
    bcu->sendTelegram[5] = 0xe1; // routing count + length
    setApciCommand(bcu->sendTelegram, APCI_GROUP_VALUE_READ_PDU, 0);
    bcu->bus->sendTelegram(bcu->sendTelegram, 8);
    transmitting_object_no = objno; //save transmitting object for status check
    bcu->bus->setBusRXStateValid(false);
}

void ComObjects::sendGroupWriteTelegram(int objno, int addr, bool isResponse)
{
    byte* valuePtr = objectValuePtr(objno);
    int objSize = telegramObjectSize(objno);
    byte addData = 0;
    ApciCommand cmd;

	while (bcu->sendTelegram[0]);  // wait for a free buffer
	///\todo Set routing count and priority according to the parameters set from ETS in the EEPROM, add ID/objno for result association from bus-layer
	initLpdu(bcu->sendTelegram, PRIORITY_LOW, false, FRAME_STANDARD);
	setDestinationAddress(bcu->sendTelegram, addr);
    bcu->sendTelegram[5] = 0xe0 | ((objSize + 1) & 0xff);

    isResponse ? cmd = APCI_GROUP_VALUE_RESPONSE_PDU : cmd = APCI_GROUP_VALUE_WRITE_PDU;

    if (objSize)
        reverseCopy(bcu->sendTelegram + 8, valuePtr, objSize);
    else
        addData = *valuePtr;

    setApciCommand(bcu->sendTelegram, cmd, addData);

    // Process this telegram in the receive queue (if there is a local receiver of this group address)
    processGroupTelegram(addr, APCI_GROUP_VALUE_WRITE_PDU, bcu->sendTelegram, objno);

    bcu->bus->sendTelegram(bcu->sendTelegram, 8 + objSize);
    transmitting_object_no = objno; //save transmitting object for status check
    bcu->bus->setBusTXStateValid(false);
}

bool ComObjects::sendNextGroupTelegram()
{
    byte* flagsTab = objectFlagsTable();
    if(flagsTab == nullptr)
    {
        return (false);
    }

    uint16_t addr;
    uint8_t flags;
    uint16_t config;
    uint16_t numObjs = objectCount();
    if (numObjs == 0)
    {
        return (false);
    }
    sendNextObjIndex %= numObjs;

	//const ComConfig* configTab = &objectConfig(0);
///\todo BUG This commented out section can lead to LL_BUSY responses of the Bus and it wont recover from that state
/*
    // pending transmission status check, switch off interrupts to avoid reading/storing changing data
    noInterrupts();
	if ( !(transmitting_object_no == INVALID_OBJECT_NUMBER) && bcu->bus->getBusTXStateValid() )
	{
		//check if state ok and update RAM Flag of object
		if ( !bcu->bus->sendTelegramState())
		{
			// set RAM Status flag to ok : clear COMFLAG_TRANS_MASK	and possible DATAREQ
         	unsigned int mask = (COMFLAG_TRANS_MASK |COMFLAG_DATAREQ )  << (transmitting_object_no & 1 ? 4 :  0);
            flagsTab[transmitting_object_no >> 1] &= ~mask;
		}
		else
		{
            //set status to error, clear COMFLAG_TRANS_MASK and set COMFLAG_ERROR
            unsigned int mask = (COMFLAG_TRANS_MASK |COMFLAG_DATAREQ)  << (transmitting_object_no & 1 ? 4 :  0);
            flagsTab[transmitting_object_no >> 1] &= ~mask;
            mask = (COMFLAG_ERROR) << (transmitting_object_no & 1 ? 4 :  0);
            flagsTab[transmitting_object_no >> 1] |= mask;
		}

		// clear pending status check
		transmitting_object_no = INVALID_OBJECT_NUMBER;
		bcu->bus->setBusTXStateValid(false);
	}
	interrupts();
*/
///\todo BUG END
    // scan all objects, read config and group address of object
    for (uint16_t objno = sendNextObjIndex; objno < numObjs; ++objno)
    {
        uint8_t* h = (objectConfigTable() + 3); // 1 tablesize 2 RAM-Flags-Table Pointer
/*        h = h + objno * 4;
        serial.print("obj ", objno, DEC); serial.print(" ");serial.print(((ComConfigBCU2*)h)->DataPtrType[0]));
        d(

        );
        return (const ComConfigBCU2*) (objectConfigTable() + 1 + sizeof(ComConfigBCU2::DataPtrType) + objno * sizeof(ComConfigBCU2) );

        Data Pointer // low mem
        Config Octet
        Type Octet  // high mem
*/
        const ComConfig& configTab = objectConfig(objno);
        config = configTab.config;
        addr = firstObjectAddr(objno);

        // check if <transmit enable> and <communication enable> is set in the config for the resp. object.
        if ((addr == 0) || !(config & COMCONF_COMM)|| !(config & COMCONF_TRANS))
        {
             continue;  // no communication allowed or no grp-adr associated, next obj.
        }

        // check ram-flags for read or write request
    	flags = flagsTab[objno >> 1];
        if (objno & 1)
        {
            flags >>= 4;
        }

        if ((flags & COMFLAG_TRANSREQ) == COMFLAG_TRANSREQ)
        {
            //app is triggering a object read or write request on the bus
            if (flags & COMFLAG_DATAREQ)
            	// app triggered a read request on the bus - no further search for local objects belonging to the same group,
            	// they will be updated by the response to the read request
                sendGroupReadTelegram(objno, addr);
            else
            	// app triggered a write request on the bus  and check for additional associations to Grp Addr for local writes
            	sendGroupWriteTelegram(objno, addr, false);

            // we set the status to TRANSMITING (0x02), clear DATAREQ flag
          	unsigned int mask = (COMFLAG_TRANS_MASK | COMFLAG_DATAREQ)  << (objno & 1 ? 4 :  0);
            flagsTab[objno >> 1] &= ~mask;
            mask = (COMFLAG_ERROR) << (objno & 1 ? 4 :  0);
            flagsTab[objno >> 1] |= mask;


            sendNextObjIndex = objno + 1;
            return true;
        }
    }

    sendNextObjIndex++; // nothing found to send, lets prepare for next round
    return false;
}

int ComObjects::nextUpdatedObject()
{
    byte* flagsTab = objectFlagsTable();
    if(flagsTab == nullptr)
    {
    	return (INVALID_OBJECT_NUMBER);
    }

    uint8_t flags;
    uint16_t numObjs = objectCount();

    if (numObjs == 0)
    {
        return (INVALID_OBJECT_NUMBER);
    }
    nextUpdatedObjIndex = 0; ///\todo this is old behavior
    //nextUpdatedObjIndex %= numObjs; ///\todo this should be a less resource intense alternative, even more could be here optimized

    for (uint16_t objno = nextUpdatedObjIndex; objno < numObjs; ++objno)
    {
        flags = flagsTab[objno >> 1]; // gets the same byte twice

        if (objno & 1) flags &= COMFLAG_UPDATE_HIGH; // check high or low nibble
        else flags &= COMFLAG_UPDATE;

        // d(serial.print(" obj: ", objno, DEC); serial.println(", fi: ", flags, HEX, 2);)

        if (flags)
        {
            // fi = flags in, fo = flags out
            d(serial.print(" flags set obj: ", objno, DEC); serial.print(", fi: ", flags, HEX, 2); serial.print("; ");)

        	flagsTab[objno >> 1] &= ~flags;
            nextUpdatedObjIndex = objno + 1;

            d(serial.println(" fo: ", flagsTab[objno >> 1], HEX, 2);)
			return objno;
        }
    }
    nextUpdatedObjIndex++; // nothing found to send, lets prepare for next round
    return INVALID_OBJECT_NUMBER;
}

void ComObjects::processGroupWriteTelegram(int objno, byte* tel)
{
    byte* valuePtr = objectValuePtr(objno);

    if (valuePtr == nullptr)
    {
        IF_DEBUG(fatalError(););
        return;
    }

    int count = telegramObjectSize(objno);

    if (count > 0) reverseCopy(valuePtr, tel + 8, count);
    else *valuePtr = tel[7] & 0x3f;

    addObjectFlags(objno, COMFLAG_UPDATE);
}

