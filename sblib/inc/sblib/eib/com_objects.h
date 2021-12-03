/*
 *  com_objects.h - EIB Communication objects.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *
 **  last update: Jan. 2021 Horst Rauch
 *  			 - added some explanation on the functions of this interface between
 *  			   the user application and the communication functions of the knx bus.
 *  			 - changed some function for the transmission of object from
 *  			 user appl. to knx bus and vice versa as there was an endless loop in some cases
 *
 *  This program is the main interface for the communication of the user application of the knx bus device,
 *  based on the functions of a BCUx or BIMx provided by the complete library.
 *  The basic interface as describe in knx spec 3.3.7, 3.4.1, 3.5.1 is hidden by some functions to write and read
 *  certain object types. We include some parts of these specs for a better understanding of the interface functions.
 *
 *  The interface between the user application and the knx system is based on so called GroupObjects
 *  (GO, also communication objects). The GroupObject Server act as an interface between the user application and
 *  the application layer of the knx stack. As the (communication) stack layers and the knx bus system has no information
 *  on the type and characteristics of a GO from the user application, data is provided by the
 *  user appl. and partly configured by the ETS.
 *
 *  The detailed handling of the GO flags and configuration is hidden within the module from user application.
 *  Only ObjectRead, ObjectWrite and ObjectUpdate are mainly used by the user application.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_com_objects_h
#define sblib_com_objects_h

#include <sys/param.h>
#include <sblib/eib/types.h>
#include <sblib/eib/datapoint_types.h>

class BcuBase;

#define INVALID_OBJECT_NUMBER -1

class ComObjects
{
public:
	ComObjects(BcuBase* bcu);
	~ComObjects();

	void objectEndian(int val);

	/**
	 * Get the numeric value from a communication object. Can be used for
	 * communication objects of type 1 bit up to type 4 bytes.
	 *
	 * @param objno - the ID of the communication object.
	 * @return The value of the com-object.
	 */
	unsigned int objectRead(int objno);

	/**
	 * Get the float value from a communication object. Can be used for
	 * communication objects of type 2 byte float (EIS5 / DPT9). The value is in
	 * 1/100 - a DPT9 value of 21.01 is returned as 2101.
	 *
	 * @param objno - the ID of the communication object.
	 * @return The value of the com-object in 1/100. INVALID_DPT_FLOAT is returned
	 *         for the DPT9 "invalid data" value.
	 */
	float objectReadFloat(int objno);

	/**
	 * Request the read of a communication object. Calling this function triggers the
	 * sending of a read-group-value telegram, to read the value of the communication
	 * object from the bus.
	 *
	 * When the answer is received, the communication object's value will be updated.
	 * You can cycle through all updated communication objects with nextUpdatedObject().
	 *
	 * @param objno - the ID of the communication object to mark for reading.
	 *
	 * @see objectWritten(int)
	 */
	void requestObjectRead(int objno);

	/**
	 * Get a pointer to the value bytes of the communication object. Can be used for
	 * any communication object. The minimum that is used for a communication object
	 * is 1 byte. Use objectSize(objno) to get the size of the communication object's
	 * value.
	 *
	 * @param objno - the ID of the communication object.
	 * @return The value of the com-object.
	 */
	virtual byte* objectValuePtr(int objno) = 0;

	/**
	 * Get the size of the communication object's value in bytes.
	 *
	 * @param objno - the ID of the communication object.
	 * @return The size in bytes.
	 */
	virtual int objectSize(int objno) = 0;

	/**
	 * Set the value of a communication object without triggering the
	 * sending of a write-group-value telegram.
	 *
	 * @param objno - the ID of the communication object.
	 * @param value - the new value of the communication object.
	 */
	void objectSetValue(int objno, unsigned int value);

	/**
	 * Set the value of a communication object. Calling this function triggers the
	 * sending of a write-group-value telegram.
	 *
	 * @param objno - the ID of the communication object.
	 * @param value - the new value of the communication object.
	 */
	void objectWrite(int objno, unsigned int value);

	/**
	 * Set the value of a communication object. Calling this function triggers the
	 * sending of a write-group-value telegram. The number of bytes that are copied
	 * depends on the size of the target communication object's field.
	 *
	 * @param objno - the ID of the communication object.
	 * @param value - the new value of the communication object.
	 */
	void objectWrite(int objno, byte* value);

	/**
	 * Set the value of a communication object. Calling this function triggers the
	 * sending of a write-group-value telegram.
	 *
	 * The communication object is a 2 byte float (EIS5 / DPT9) object. The value is
	 * in 1/100, so a value of 2101 would set a DPT9 float value of 21.01. The valid
	 * range of the values is -671088.64 to 670760.96.
	 *
	 * @param objno - the ID of the communication object.
	 * @param value - the new value of the communication object in 1/100.
	 *                Use INVALID_DPT_FLOAT for the DPT9 "invalid data" value.
	 */
	void objectWriteFloat(int objno, int value);

	/**
	 * Mark a communication object as written. Use this function if you directly change
	 * the value of a communication object without using objectWrite(). Calling this
	 * function triggers the sending of a write-group-value telegram.
	 *
	 * @param objno - the ID of the communication object.
	 *
	 * @see requestObjectRead(int)
	 */
	void objectWritten(int objno);

	/**
	 * Set the value of a communication object and mark the communication object
	 * as updated. This does not trigger a write-group-value telegram.
	 *
	 * @param objno - the ID of the communication object.
	 * @param value - the new value of the communication object.
	 */
	void objectUpdate(int objno, unsigned int value);

	/**
	 * Set the value of a communication object and mark the communication object
	 * as updated. This does not trigger a write-group-value telegram.
	 *
	 * @param objno - the ID of the communication object.
	 * @param value - the new value of the communication object.
	 */
	void objectUpdate(int objno, byte* value);

	/**
	 * Set the value of a communication object and mark the communication object
	 * as updated. This does not trigger a write-group-value telegram.
	 *
	 * The communication object is a 2 byte float (EIS5 / DPT9) object. The value
	 * is in 1/100, so a value of 2101 would set a DPT9 float value of 21.01.
	 * The possible range of the values is -671088.64 to 670760.96.
	 *
	 * @param objno - the ID of the communication object.
	 * @param value - the new value of the communication object in 1/100.
	 *                Use INVALID_DPT_FLOAT for the DPT9 "invalid data" value.
	 */
	void objectUpdateFloat(int objno, int value);

	/**
	 * Update the value of the communication object without changing the flags
	 *
	 * @param objno - the ID of the communication object.
	 * @param value - the new value of the communication object.
	 */
	void setObjectValue(int objno, unsigned int value);

	/**
	 * Get the ID of the next communication object that was updated
	 * over the bus by a write-value-request telegram.
	 *
	 * @return The ID of the next updated com-object, -1 if none.
	 */
	int nextUpdatedObject();

	/**
	 * Get the type of a communication object.
	 *
	 * @param objno - the ID of the communication object.
	 * @return The type of the communication object.
	 */
	ComType objectType(int objno);

	/**
	 * Returns the config of the specified communication object.
	 *
	 * @param objno The ID of the communication object
	 * @return The communication object's configuration.
	 *
	 * Remark:
	 * The first byte of the com table is the number of entries, followed
	 * by the address of the RAM area used by the com objects. After that,
	 * the configuration for each com object follows.
	 * The sizes of the RAM pointer and the com object varies between different
	 * BCU types. Therefore the sizeof operator is used instead of hard-coded
	 * values.
	 */
	virtual const ComConfig& objectConfig(int objno) = 0;

	/**
	 * Process a multicast group telegram received from bus or requested by the application.
	 *
	 * This function is called by bcu.processTelegram() and from the local process. It is usually not required to call
	 * this function from within a user program.
	 *
	 * @param addr - the destination group address.
	 * @apci - Kind of telegram to be processed
	 *         APCI_GROUP_VALUE_WRITE_PDU
	 *         APCI_GROUP_VALUE_RESPONSE_PDU
	 *         APCI_GROUP_VALUE_READ_PDU
	 * @tel - pointer to the telegram to read from
	 *         (only used if APCI_GROUP_VALUE_WRITE_PDU or APCI_GROUP_VALUE_RESPONSE_PDU)
	 *
	 *         if called from app, we have the triggering object as additional parameter
	 * @trg_objno - objno triggering the group telegram from the application layer
	 *
	 */

	void processGroupTelegram(int addr, int apci, byte* tel);

	virtual void processGroupTelegram(int addr, int apci, byte* tel, int trg_objno) = 0;

	/**
	 * Get the communication object configuration table ("COMMS" table). This is the table
	 * with the flags that are configured by ETS (not the RAM status flags).
	 *
	 * @return The com-objects configuration table.
	 *
	 * @brief The first byte of the table contains the number of entries. The second
	 * byte contains the address of the object status flags in userRam. The rest of
	 * the table consists of the ComConfig objects - 3 bytes per communication
	 * object.
	 */
	virtual byte* objectConfigTable() = 0;

	/**
	 * Get the communication object status flags table. This is the table with the
	 * status flags that are stored in RAM and get changed during normal operation.
	 *
	 * @return The com-objects status flags table.
	 *
	 * @brief The whole table consists of the status flags - 4 bits per communication
	 * object.
	 */
	virtual byte* objectFlagsTable() = 0;
	bool sendNextGroupTelegram();

protected:
	BcuBase* bcu;
	int le_ptr = BIG_ENDIAN;
	int transmitting_object_no= INVALID_OBJECT_NUMBER; // object number of last transmitted bus msg - status should be in transmitting
	int sndStartIdx = 0;

	int telegramObjectSize(int objno);
	void addObjectFlags(int objno, int flags);
	void setObjectFlags(int objno, int flags);
	void _objectWrite(int objno, unsigned int value, int flags);
	void _objectWriteBytes(int objno, byte* value, int flags);
	int objectCount();
	int firstObjectAddr(int objno);
	void sendGroupReadTelegram(int objno, int addr);
	void sendGroupWriteTelegram(int objno, int addr, bool isResponse);
	void processGroupWriteTelegram(int objno, byte* tel);
	virtual const byte* getObjectTypeSizes() = 0;
};


//
//  Inline functions
//

inline void ComObjects::objectEndian(int val)
{
	le_ptr=val;
}

inline void ComObjects::processGroupTelegram(int addr, int apci, byte* tel)
{ // call with neg/invalid object

	processGroupTelegram(addr, apci, tel, INVALID_OBJECT_NUMBER);
}

inline ComType ComObjects::objectType(int objno)
{
    return (ComType) objectConfig(objno).type;
}

inline void ComObjects::requestObjectRead(int objno)
{
    setObjectFlags(objno, COMFLAG_TRANSREQ | COMFLAG_DATAREQ);
}

inline void ComObjects::objectWritten(int objno)
{
    addObjectFlags(objno, COMFLAG_TRANSREQ);
}

inline void ComObjects::objectSetValue(int objno, unsigned int value)
{
    _objectWrite(objno, value, 0);
}

inline void ComObjects::objectWrite(int objno, unsigned int value)
{
    _objectWrite(objno, value, COMFLAG_TRANSREQ);
}

inline void ComObjects::objectWrite(int objno, byte* value)
{
    _objectWriteBytes(objno, value, COMFLAG_TRANSREQ);
}

inline void ComObjects::objectWriteFloat(int objno, int value)
{
    _objectWrite(objno, dptToFloat(value), COMFLAG_TRANSREQ);
}

inline void ComObjects::objectUpdate(int objno, unsigned int value)
{
    _objectWrite(objno, value, COMFLAG_UPDATE);
}

inline void ComObjects::objectUpdate(int objno, byte* value)
{
    _objectWriteBytes(objno, value, COMFLAG_UPDATE);
}

inline void ComObjects::objectUpdateFloat(int objno, int value)
{
    _objectWrite(objno, dptToFloat(value), COMFLAG_UPDATE);
}

inline float ComObjects::objectReadFloat(int objno)
{
    return dptFromFloat(objectRead(objno));
}

#endif /*sblib_com_objects_h*/
