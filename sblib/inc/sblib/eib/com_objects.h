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
    ComObjects() = delete;
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
	 * @return The ID of the next updated com-object, @ref INVALID_OBJECT_NUMBER if none was found.
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

	void processGroupTelegram(int addr, int apci, byte* tel);

	/**
	 * Process a multicast group telegram received from bus or requested by the application.
	 *
	 * @brief This function is called by bcu.processTelegram() and from the local process.
	 *        It is usually not required to call this function from within a user program.
	 *
	 * @param addr      The destination group address.
	 * @param apci      Kind of telegram to be processed
     *                     - @ref APCI_GROUP_VALUE_WRITE_PDU
     *                     - @ref APCI_GROUP_VALUE_RESPONSE_PDU
     *                     - @ref APCI_GROUP_VALUE_READ_PDU
	 * @param tel       Pointer to the telegram to read from
	 *                  (only used for @ref APCI_GROUP_VALUE_WRITE_PDU or @ref APCI_GROUP_VALUE_RESPONSE_PDU)
	 *                  if called from app, we have the triggering object as additional parameter
	 * @param trg_objno Object number triggering the group telegram from the application layer
	 */
	virtual void processGroupTelegram(uint16_t addr, int apci, byte* tel, int trg_objno) = 0;

	/**
	 * Get the communication object configuration table ("COMMS" table). This is the table
	 * with the flags that are configured by ETS (not the RAM status flags).
	 *
	 * @brief The first byte of the table contains the number of entries. The second
	 * byte contains the address of the object status flags in userRam. The rest of
	 * the table consists of the ComConfig objects - 3 bytes per communication
	 * object.
	 *
	 * @return Pointer to com-objects configuration table.
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

	/**
	 *  Send next Group read/write telegram based on RAM flag status and handle bus rx/tx status
	 *  of previously transmitted telegram
	 *
	 *  @details Periodically called from BCU-loop function.
	 *           Scan RAM flags of objects if there is a read or write request from the app.
	 *           If object config flag allows communication and transmission requests from app send respective message
	 *           and reset the RAM flags and return true. If no request is found in RAM flag return false
	 *
	 *           Scan if AIL is waiting for confirmation of lower bus-layers from last transmit request (read/write)
	 *           and set the object-flag according to bus result
	 *
	 *  @return true if a telegram was sent, otherwise false
	 */
	bool sendNextGroupTelegram();

protected:
	/**
	 * Get the size of the com-object in bytes, for sending/receiving telegrams.
	 * 0 is returned if the object's size is <= 6 bit.
	 */
	int telegramObjectSize(int objno);

	/**
	 * Add one or more flags to the flags of a communication object.
	 * This does not clear any flag of the communication object.
	 *
	 * @param objno - the ID of the communication object
	 * @param flags - the flags to add
	 *
	 * @see @ref objectWritten(int)
	 * @see @ref requestObjectRead(int)
	 */
	void addObjectFlags(int objno, int flags);

	/**
	 * Set the flags of a communication object.
	 *
	 * @param objno - the ID of the communication object
	 * @param flags - the new communication object flags
	 *
	 * @see @ref objectWritten(int)
	 * @see @ref requestObjectRead(int)
	 */
	void setObjectFlags(int objno, int flags);
	void _objectWrite(int objno, unsigned int value, int flags);
	void _objectWriteBytes(int objno, byte* value, int flags);

	/**
	 * @return The number of communication objects.
	 */
	int objectCount();

	/**
	 * Find the first group address for the communication object. This is the
	 * address that is used when sending a read-value or a write-value telegram.
	 *
	 * @param objno - the ID of the communication object
	 * @return The group address, or 0 if none found.
	 */
	int firstObjectAddr(int objno);

	/**
	 * Create and send a group read request telegram.
	 *
	 * In order to avoid overwriting a telegram in the send buffer while the bus is still sending the last telegram
	 * we wait for a free buffer
	 *
	 * @param objno - the ID of the communication object
	 * @param addr - the group address to read
	 */
	void sendGroupReadTelegram(int objno, int addr);

	/**
	 * Create and send a group write or group response telegram.
	 *
	 * In order to avoid overwriting a telegram in the send buffer while the bus is still sending the last telegram
	 * we wait for a free buffer
	 *
	 * @param objno - the ID of the communication object
	 * @param addr - the destination group address
	 * @param isResponse - true if response telegram, false if write telegram
	 */
	void sendGroupWriteTelegram(int objno, int addr, bool isResponse);
	void processGroupWriteTelegram(int objno, byte* tel);

    BcuBase* bcu;
    int le_ptr;
    int transmitting_object_no; //!< Object number of last transmitted bus message - status should be in transmitting
    int sendNextObjIndex;       //!< Next object number which  will be checked in sendNextGroupTelegram() for transmission
    int nextUpdatedObjIndex;    //!< Next object number which  will be checked in nextUpdatedObject() for processing by the application

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
    _objectWrite(objno, floatToDpt9(value), COMFLAG_TRANSREQ);
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
    _objectWrite(objno, floatToDpt9(value), COMFLAG_UPDATE);
}

inline float ComObjects::objectReadFloat(int objno)
{
    return dpt9ToFloat(objectRead(objno));
}

#endif /*sblib_com_objects_h*/
