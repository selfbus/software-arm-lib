/*
 *  properties.h - BCU 2 properties of EIB objects.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_properties_systemb_h
#define sblib_properties_systemb_h

#include <sblib/eib/propertiesBCU2.h>
#include <sblib/eib/userRamMASK0701.h>
#include <sblib/eib/userEepromMASK0701.h>

class MASK0701;

/** Define a PropertyDef pointer to variable v in the userRam */
#undef PD_USER_RAM_OFFSET
#define PD_USER_RAM_OFFSET(v) (UserRamMASK0701::v + PPT_USER_RAM)

/** Define a PropertyDef pointer to variable v in the userEeprom */
#undef PD_USER_EEPROM_OFFSET
#define PD_USER_EEPROM_OFFSET(v) (UserEepromMASK0701::v + PPT_USER_EEPROM)

class PropertiesMASK0701 : public PropertiesBCU2
{
public:
	PropertiesMASK0701(MASK0701* bcuInstance) : PropertiesBCU2((BCU2*)bcuInstance), bcu(bcuInstance) {};
	~PropertiesMASK0701() = default;

	virtual LoadState handleAllocAbsDataSegment(const int objectIdx, const byte* payLoad, const int len);
	virtual LoadState handleAllocAbsTaskSegment(const int objectIdx, const byte* payLoad, const int len);

protected:
	virtual const PropertyDef* const* propertiesTab() const;

private:
	MASK0701* bcu;

	/**
	 * The properties of the device object
	 * See KNX Spec 06 Profiles/Annex A p.103 and 9/4/1 p.50
	 */
	const PropertyDef deviceObjectProps[12] =
	{
	    /** Interface object type: 2 bytes */
	    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_DEVICE },

	    /** Device control */
	    { PID_DEVICE_CONTROL, PDT_GENERIC_01|PC_WRITABLE|PC_POINTER, PD_USER_RAM_OFFSET(_deviceControlOffset) },

	    /** Load state control */
	    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadStateOffset + OT_DEVICE) },

	    /** Service control: 2 bytes stored in userEeprom.serviceControl */
	    { PID_SERVICE_CONTROL, PDT_UNSIGNED_INT|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(serviceControlOffset) },

	    /** Firmware revision: 1 byte */
	    { PID_FIRMWARE_REVISION, PDT_UNSIGNED_CHAR, SBLIB_VERSION },

	    /** Serial number: 6 byte data, stored in userEeprom.serial */
	    { PID_SERIAL_NUMBER, PDT_GENERIC_06|PC_POINTER, PD_USER_EEPROM_OFFSET(serialOffset) },

	    /** Manufacturer ID: unsigned int, stored in userEeprom.manufacturerH (and manufacturerL)
	     *  Configured as PDT_GENERIC_02 and not as PDT_UNSIGNED_INT to avoid swapping the byte order
	     */
	    { PID_MANUFACTURER_ID, PDT_GENERIC_02|PC_POINTER, PD_USER_EEPROM_OFFSET(manufacturerHOffset) },

	    /** Order number: 10 byte data, stored in userEeprom.order, last two bytes represent sblib version in hex */
	    { PID_ORDER_INFO, PDT_GENERIC_10|PC_POINTER, PD_USER_EEPROM_OFFSET(orderInfoOffset) },

	    /** PEI type: 1 byte */
	    { PID_PEI_TYPE, PDT_UNSIGNED_CHAR|PC_POINTER, PD_USER_RAM_OFFSET(_peiTypeOffset) },

	    /** Port A configuration: 1 byte, stored in userEeprom.portADDR */
	    { PID_PORT_CONFIGURATION, PDT_UNSIGNED_CHAR|PC_POINTER, PD_USER_EEPROM_OFFSET(portADDROffset) },

	    /** Hardware type: 6 byte data */
	    { PID_HARDWARE_TYPE, PDT_GENERIC_06|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(orderOffset) },

	    /** End of table */
	    PROPERTY_DEF_TABLE_END
	};

	/**
	 * The properties of the address table object
	 * See KNX Spec 06 Profiles/Annex A p.103 and 9/4/1 p.51
	 */
	const PropertyDef addrTabObjectProps[5] =
	{
	    /** Interface object type: 2 bytes */
	    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_ADDR_TABLE },

	    /** Load state control */
	    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadStateOffset + OT_ADDR_TABLE) },

	    /** Pointer to the address table */
	    { PID_TABLE_REFERENCE, PDT_UNSIGNED_INT|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(addrTabAddrOffset) },

	    /** Pointer to the memory control block */
	    { PID_MCB_TABLE, PDT_GENERIC_08|PC_POINTER|PC_WRITABLE, PD_USER_EEPROM_OFFSET(addrTabMcbOffset) },

	    /** End of table */
	    PROPERTY_DEF_TABLE_END
	};

	/**
	 * The properties of the association table object,
	 * See KNX Spec 06 Profiles/Annex A p.103 and 9/4/1 p.51
	 */
	const PropertyDef assocTabObjectProps[6] =
	{
	    /** Interface object type: 2 bytes */
	    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_ASSOC_TABLE },

	    /** Load state control */
	    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadStateOffset + OT_ASSOC_TABLE) },

	    /** Pointer to the association table */
	    { PID_TABLE_REFERENCE, PDT_UNSIGNED_INT|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(assocTabAddrOffset) },

	    /** */
	    { PID_TABLE, PDT_GENERIC_04|PC_WRITABLE, 0x00FF },

	    /** Pointer to the memory control block */
	    { PID_MCB_TABLE, PDT_GENERIC_08|PC_POINTER|PC_WRITABLE, PD_USER_EEPROM_OFFSET(assocTabMcbOffset) },

	    /** End of table */
	    PROPERTY_DEF_TABLE_END
	};

	/**
	 * The properties of the application program object,
	 * See KNX Spec 9/4/1 p.52
	 */
	const PropertyDef appObjectProps[8] =
	{
	    /** Interface object type: 2 bytes */
	    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_APPLICATION },

	    /** Load state control */
	    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadStateOffset + OT_APPLICATION) },

	    /** Run state control */
	    { PID_RUN_STATE_CONTROL, PDT_UNSIGNED_CHAR|PC_POINTER, PD_USER_RAM_OFFSET(_runStateOffset) },

	    /** Program version */
	    { PID_PROG_VERSION, PDT_GENERIC_05|PC_POINTER, PD_USER_EEPROM_OFFSET(manufacturerHOffset) },

	    /** Pointer to the communication objects table */
	    { PID_TABLE_REFERENCE, PDT_UNSIGNED_INT|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(commsTabAddrOffset) },

	    /** Pointer to the memory control block */
	    { PID_MCB_TABLE, PDT_GENERIC_08|PC_POINTER|PC_WRITABLE, PD_USER_EEPROM_OFFSET(commsTabMcbOffset) },

	    /** ABB_CUSTOM */
	    { PID_ABB_CUSTOM, PDT_GENERIC_10|PC_POINTER|PC_WRITABLE, PD_USER_RAM_OFFSET(_user2Offset) },

	    /** End of table */
	    PROPERTY_DEF_TABLE_END
	};

	const PropertyDef interfaceObjectProps[6] =
	{
	    /** Interface object type: 2 bytes */
	    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_INTERFACE_PROGRAM },

	    /** Load state control */
	    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadStateOffset + OT_INTERFACE_PROGRAM) },

	    /** Pointer to the interface objects table */
	    { PID_TABLE_REFERENCE, PDT_UNSIGNED_INT|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(eibObjAddrOffset) },

	    /** */
	    { PID_PROG_VERSION, PDT_GENERIC_05|PC_WRITABLE|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(eibObjVerOffset) },

	    /** Pointer to the memory control block */
	    { PID_MCB_TABLE, PDT_GENERIC_08|PC_POINTER|PC_WRITABLE, PD_USER_EEPROM_OFFSET(eibObjMcbOffset) },

	    /** Error code */
	    PROPERTY_DEF_TABLE_END
	};

	/**
	 * The properties of the KNX object association table object, could not find the specification in KNX Spec 2.1
	 */
	const PropertyDef knxAssocTabObjectProps[6] =
	{
	    // XXX check correct properties for object OT_KNX_OBJECT_ASSOCIATATION_TABLE
	    /** Interface object type: 2 bytes */
	    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_KNX_OBJECT_ASSOCIATATION_TABLE },

	    /** Load state control */
	    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadStateOffset + OT_KNX_OBJECT_ASSOCIATATION_TABLE) },

	    /** Pointer to the communication objects table */
	    { PID_TABLE_REFERENCE, PDT_UNSIGNED_INT|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(commsSeg0AddrOffset) },

	    /** */
	    { PID_PROG_VERSION, PDT_GENERIC_05|PC_WRITABLE|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(commsSeg0VerOffset) },

	    /** Error code */
	    { PID_ERROR_CODE, PDT_GENERIC_01, 0 }, // XXX implement PID_ERROR_CODE handling

	    // XXX implement properties PID_TABLE, PID_MCB_TABLE and maybe more

	    /** End of table */
	    PROPERTY_DEF_TABLE_END
	};

	/**
	 * The interface objects for
	 * BCU2     (MASK_VERSIONs 0x0020, 0x0021, 0x0025)
	 * BIM112   (MASK_VERSIONs 0x0701, 0x0705)
	 * SYSTEM_B (MASK_VERSIONs 0x07B0)
	 */
	const PropertyDef* const propertiesTabInstance[NUM_PROP_OBJECTS] =
	{
	    deviceObjectProps,     //!> Interface Object 0, mandatory
	    addrTabObjectProps,    //!> Interface Object 1, mandatory
	    assocTabObjectProps,   //!> Interface Object 2, mandatory
	    appObjectProps,        //!> Interface Object 3, mandatory
		interfaceObjectProps,  //!> Interface Object 4, required for SYSTEM_B
		knxAssocTabObjectProps //!> Interface Object 5, some newer MASK_VERSIONs (>= 0x0701) use this to set the address of the communication object table
	};
};


inline const PropertyDef* const* PropertiesMASK0701::propertiesTab() const
{
	return propertiesTabInstance;
}

#endif /*sblib_properties_systemb_h*/
