/*
 *  properties.h - BCU 2 properties of EIB objects.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_properties_bcu2_h
#define sblib_properties_bcu2_h

#include <sblib/eib/userEepromBCU2.h>
#include <sblib/eib/userRamBCU2.h>
#include <sblib/version.h>
#include <sblib/libconfig.h>

#include <sblib/eib/property_types.h>
#ifdef DUMP_PROPERTIES
#   include <sblib/serial.h>
#endif

#define DMP_LOADSTATE_MACHINE_WRITE_RCO_IO_LENGTH  10           //!> Data length of a valid DMP_LoadStateMachineWrite_RCo_IO telegram
#define DMP_LOADSTATE_MACHINE_WRITE_RCO_MEM_LENGTH  11          //!> Data length of a valid DMP_LoadStateMachineWrite_RCo_Mem telegram (deprecated)

#define DMP_LOADSTATE_MACHINE_WRITE_RCO_IO_PAYLOAD_OFFSET 2     //!> offset for RCo_IO mode, where the real data for Additional Load Controls starts
#define DMP_LOADSTATE_MACHINE_WRITE_RCO_MEM_PAYLOAD_OFFSET 3    //!> offset for RCo_Mem mode, where the real data for Additional Load Controls starts

#define DMP_LOADSTATE_MACHINE_WRITE_RCO_PAYLOAD_LENGTH 7        //!> the length of the payload for both implementations

/**
 * Include the C++ code snippet if DUMP_PROPERTIES is defined, do not include the code
 * if DUMP_PROPERTIES is not defined.
 *
 * @param code - the C++ code to include
 *
 * @brief Example:  DUMP_PROPERTIES(fatalError())
 */
#ifdef DUMP_PROPERTIES
#  define IF_DUMP_PROPERTIES(code) code
#else
#  define IF_DUMP_PROPERTIES(code)
#endif

// Number of interface/property objects
#define NUM_PROP_OBJECTS 6

/** Define a PropertyDef pointer to variable v in the userRam */
#define PD_USER_RAM_OFFSET(v) (UserRamBCU2::v + PPT_USER_RAM)

/** Define a PropertyDef pointer to variable v in the userEeprom */
#define PD_USER_EEPROM_OFFSET(v) (UserEepromBCU2::v + PPT_USER_EEPROM)

/**
 * Interface object type ID / Identifiers for System Interface Object Types
 * <b>See KNX Spec 3/7/3 2.2 Overview System Interface Objects</b>
 *
 * <b>changes in ObjectType must also be represented in *objectType_str[] (properties_dump.cpp)</b>
 */
enum ObjectType
{
	/** Device object. */
	OT_DEVICE = 0,

	/** Address table object. */
	OT_ADDR_TABLE = 1,

	/** Association table object. */
	OT_ASSOC_TABLE = 2,

	/** Application program object. */
	OT_APPLICATION = 3,

	/** Interface program object. */
	OT_INTERFACE_PROGRAM = 4,

	/** KNX object association table object */
	OT_KNX_OBJECT_ASSOCIATATION_TABLE = 5,

	/** Router object */
	OT_ROUTER = 6,

	/** LTE Address routing table object */
	OT_LTE_ADDRESS_ROUTING_TABLE = 7,

	/** Common External Message (cEMI) server object */
	OT_CEMI_SERVER = 8,

	/** Group object table object */
	OT_GROUP_OBJECT_TABLE  = 9,

	/** Polling master object */
	OT_POLLING_MASTER = 10,

	/** KNXnet/IP parameter object */
	OT_KNXNET_IP_PARAMETER = 11,

	/** Application controller object */
	OT_APPLICATION_CONTROLLER = 12,

	/** File server object */
	OT_FILE_SERVER = 13,

	/** Security object */
	OT_SECURITY = 17,

	/** Radio frequency (RF) medium object */
	OT_RF_MEDIUM = 19
};

/**
 * Load states
 * <b>See KNX Spec 3/5/1 4.17.2.3.1 p. 200</b>
 *
 * <b>changes in LoadState must also be represented in *loadState_str[] (properties_dump.cpp)</b>
 */
enum LoadState
{
	/** No data is loaded */
	LS_UNLOADED = 0,

	/** Valid data is loaded */
	LS_LOADED = 1,

	/** Load process is active */
	LS_LOADING = 2,

	/** Error in data detected or error during load process */
	LS_ERROR = 3,

	/** Optional state: Unload process is active */
	LS_UNLOADING = 4,

	/** Optional state: Intermediate state between Loading and Loaded */
	LS_LOADCOMPLETING = 5
};

/**
 * Load controls
 * <b>See KNX Spec 3/5/1 4.17.2.3.1 p. 201</b>
 */
enum LoadControl
{
	/** No Operation */
	LC_NO_OPERATION = 0,

	/** start the loading of the loadable part */
	LC_START_LOADING = 1,

	/** complete the Loading of the loadable part */
	LC_LOAD_COMPLETED  = 2,

	/**
	 * used to transfer additional load information
	 * like memory allocation (absolute or relative),
	 * application entry points or special run conditions.
	 * See clause 3.27 “DM_LoadStateMachineWrite”
	 */
	LC_ADDITIONAL_LOAD_CONTROLS = 3,

	/** request to unload the loadable part */
	LC_UNLOAD = 4
};

/**
 * Segment types for Load controls
 * <b>See  KNX Spec 2/3/1 p.8 and KNX 3/5/2 p.84 </b>
 *
 * <b>changes in SegmentType must also be represented in *segmentType_str[] (properties_dump.cpp)</b>
 */
enum SegmentType
{
	/** allocate absolute data or code */
	ST_ALLOC_ABS_DATA_SEG = 0,

	/** allocates a stack */
	ST_ALLOC_ABS_STACK_SEG = 1,

	/** sets the Resource start address or the AP’s main function entry address, PEI type, AP ID = Manufacturer ID, Device Type, AP Version */
	ST_ALLOC_ABS_TASK_SEG = 2,

	/** sets the initialization function entry address, the save function entry address and the Serial PEI handler function entry address */
	ST_TASK_PTR = 3,

	/** sets the user KNX object table start address and the number of user KNX objects */
	ST_TASK_CTRL_1 = 4,

	/** sets the start address of the AP’s callback procedure, CO table start address, address of CO segment #0, address of CO segment #2 */
	ST_TASK_CTRL_2 = 5,

	/** relative allocation */
	ST_RELATIVE_ALLOCATION = 0x0A,

	/** data relative allocation */
	ST_DATA_RELATIVE_ALLOCATION = 0x0B
};

/**
 * Memory types send in Additional Load Control: LoadEvent: AllocAbsDataSeg (segment type 0)
 * <b>See  KNX Spec 3/5/2 p.111</b>
 */
enum MemoryType
{
	/**
	 * Zero page RAM, mostly found in product databases using <.../LoadProcedure/LdCtrlAbsSegment> with
	 * MaskVersions 0x0020, 0x0021, 0x0025 in two variants:
	 * start 0xBD (189) length 0x18 (24) or start 0xC8 (200) length 0x23 (35)
	 */
	MT_ZERO_PAGE_RAM = 1,
	MT_RAM = 2, //!> RAM
	MT_EEPROM = 3 //!> EEPROM
	// these memory types there found in 38 product databases (all Siemens HVAC) with MaskVersion MV-0300-01000000000000000000
	// and interface object ID OT_INTERFACE_PROGRAM = 4
	// MT_SPECIAL_6 = 6,
	// MT_SPECIAL_8 = 8
};

class BCU2;

class PropertiesBCU2
{
public:
	PropertiesBCU2(BCU2* bcuInstance) : bcu(bcuInstance) {};
	~PropertiesBCU2() = default;

	/**
	 * Load / configure a property of a interface object. Called when a "load control" property-write telegram is received.
	 *
	 * e.g.
	 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
	 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3 p.115
	 *
	 * @param objectIdx - the interface object type ID.
	 * @param data - the telegram data bytes
	 * @param len - the length of the
	 *
	 * @return The new load state.
	 */
	virtual int loadProperty(int objectIdx, const byte* data, int len);

	/**
	 * Find a property definition in a properties table.
	 *
	 * @param id - the ID of the property to find.
	 * @param table - the properties table. The last table element must be PROPERTY_DEF_TABLE_END
	 *
	 * @return Pointer to the property definition, 0 if not found.
	 */
	virtual const PropertyDef* findProperty(PropertyID id, const PropertyDef* table);

	virtual const PropertyDef* propertyDef(int objectIdx, PropertyID propertyId);

	virtual LoadState handleLoadStateMachine(const int objectIdx, const byte* data, const int len);

	virtual LoadState handleAllocAbsTaskSegment(const int objectIdx, const byte* payLoad, const int len);

	virtual LoadState handleAllocAbsDataSegment(const int objectIdx, const byte* payLoad, const int len);

	virtual LoadState handleAllocAbsStackSeg(const int objectIdx, const byte* payLoad, const int len);

	virtual LoadState handleTaskPtr(const int objectIdx, const byte* payLoad, const int len);

	virtual LoadState handleTaskCtrl1(const int objectIdx, const byte* payLoad, const int len);

	virtual LoadState handleTaskCtrl2(const int objectIdx, const byte* payLoad, const int len);

	virtual LoadState handleRelativeAllocation(const int objectIdx, const byte* payLoad, const int len);

	virtual LoadState handleDataRelativeAllocation(const int objectIdx, const byte* payLoad, const int len);

	virtual bool propertyValueReadTelegram(int objectIdx, PropertyID propertyId, int count, int start);

	virtual bool propertyValueWriteTelegram(int objectIdx, PropertyID propertyId, int count, int start);

	virtual bool propertyDescReadTelegram(int objectIdx, PropertyID propertyId, int index);

	#ifdef DUMP_PROPERTIES

	/**
	 * Prints the value (in hex) of objectIdx and its clear name
	 *
	 * @param objectIdx - object index to print
	 */
	virtual void printObjectIdx(int objectIdx);

	/**
	 * Prints a loadstate (in hex) and its clear name
	 *
	 * @param loadstate - loadstate to print
	 */
	virtual void printLoadState(int loadstate);

	/**
	 * Prints a segmenttype (in hex) and its clear name
	 *
	 * @param segmenttype - segmenttype to print
	 */
	virtual void printSegmentType(int segmenttype);

	/**
	 * Prints a propertyid (in hex) and its clear name
	 *
	 * @param propertyid - property id to print
	 */
	virtual void printPropertyID(int propertyid);

	/**
	 * Prints the data
	 * @param data - pointer to data to print
	 * @param len - length of data
	 */
	virtual void printData(const byte* data, int len);

	#endif /*DUMP_PROPERTIES*/

protected:
	virtual const PropertyDef* const* propertiesTab() const;

private:
	BCU2* bcu;

	/**
	 * The properties of the device object
	 * See KNX Spec 06 Profiles/Annex A p.103 and 9/4/1 p.50
	 */
	const PropertyDef deviceObjectProps[12] =
	{
	    /** Interface object type: 2 bytes */
	    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_DEVICE },

	    /** Device control */
	    { PID_DEVICE_CONTROL, PDT_GENERIC_01|PC_WRITABLE|PC_POINTER, PD_USER_RAM_OFFSET(deviceControlOffset) },

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

	    /** Order number: 10 byte data, stored in userEeprom.serial */
	    { PID_ORDER_INFO, PDT_GENERIC_10|PC_POINTER, PD_USER_EEPROM_OFFSET(serialOffset) },

	    /** PEI type: 1 byte */
	    { PID_PEI_TYPE, PDT_UNSIGNED_CHAR|PC_POINTER, PD_USER_RAM_OFFSET(peiTypeOffset) },

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
	    { PID_RUN_STATE_CONTROL, PDT_UNSIGNED_CHAR|PC_POINTER, PD_USER_RAM_OFFSET(runStateOffset) },

	    /** Program version */
	    { PID_PROG_VERSION, PDT_GENERIC_05|PC_POINTER, PD_USER_EEPROM_OFFSET(manufacturerHOffset) },

	    /** Pointer to the communication objects table */
	    { PID_TABLE_REFERENCE, PDT_UNSIGNED_INT|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(commsTabAddrOffset) },

	    /** Pointer to the memory control block */
	    { PID_MCB_TABLE, PDT_GENERIC_08|PC_POINTER|PC_WRITABLE, PD_USER_EEPROM_OFFSET(commsTabMcbOffset) },

	    /** ABB_CUSTOM */
	    { PID_ABB_CUSTOM, PDT_GENERIC_10|PC_POINTER|PC_WRITABLE, PD_USER_RAM_OFFSET(user2Offset) },

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

#ifdef DUMP_PROPERTIES
	const char *objectType_str[20] =
	{
		"OT_DEVICE", "OT_ADDR_TABLE", "OT_ASSOC_TABLE", "OT_APPLICATION", "OT_INTERFACE_PROGRAM", "OT_KNX_OBJECT_ASSOCIATATION_TABLE",
		"OT_ROUTER", "OT_LTE_ADDRESS_ROUTING_TABLE", "OT_CEMI_SERVER", "OT_GROUP_OBJECT_TABLE", "OT_POLLING_MASTER", "OT_KNXNET_IP_PARAMETER",
		"OT_APPLICATION_CONTROLLER", "OT_FILE_SERVER", "UNKNOWN_14", "UNKNOWN_15", "UNKNOWN_16", "OT_SECURITY", "UNKNOWN_18", "OT_RF_MEDIUM"
	};

	const char *loadState_str[6] =
	{
		"LS_UNLOADED", "LS_LOADED", "LS_LOADING", "LS_ERROR", "LS_UNLOADING", "LS_LOADCOMPLETING"
	};

	const char *segmentType_str[12] =
	{
		"ST_ALLOC_ABS_DATA_SEG", "ST_ALLOC_ABS_STACK_SEG", "ST_ALLOC_ABS_TASK_SEG", "ST_TASK_PTR", "ST_TASK_CTRL_1", "ST_TASK_CTRL_2",
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "ST_RELATIVE_ALLOCATION", "ST_DATA_RELATIVE_ALLOCATION"
	};


	const char *propertyID_str[133] =
	{
		"PID_0",
		"PID_OBJECT_TYPE", "PID_OBJECT_NAME", "PID_SEMAPHOR", "PID_GROUP_OBJECT_REFERENCE", "PID_LOAD_STATE_CONTROL", "PID_RUN_STATE_CONTROL",
		"PID_TABLE_REFERENCE", "PID_SERVICE_CONTROL", "PID_FIRMWARE_REVISION", "PID_SERVICES_SUPPORTED", "PID_SERIAL_NUMBER", "PID_MANUFACTURER_ID",
		"PID_PROG_VERSION", "PID_DEVICE_CONTROL", "PID_ORDER_INFO", "PID_PEI_TYPE", "PID_PORT_CONFIGURATION", "PID_POLL_GROUP_SETTINGS", "PID_MANUFACTURER_DATA",
		"PID_ENABLE", "PID_DESCRIPTION", "PID_FILE", "PID_TABLE", "PID_ENROL", "PID_VERSION", "PID_GROUP_OBJECT_LINK", "PID_MCB_TABLE", "PID_ERROR_CODE",
		"PID_OBJECT_INDEX",
		"PID_30", "PID_31", "PID_32", "PID_33", "PID_34", "PID_35", "PID_36", "PID_37", "PID_38", "PID_39", "PID_40",
		"PID_41", "PID_42", "PID_43", "PID_44", "PID_45", "PID_46", "PID_47", "PID_48", "PID_49", "PID_50",
		"PID_ROUTING_COUNT", "PID_MAX_RETRY_COUNT", "PID_ERROR_FLAGS",
		"PID_PROGMODE", "PID_PRODUCT_ID", "PID_MAX_APDULENGTH", "PID_SUBNET_ADDR", "PID_DEVICE_ADDR", "PID_PB_CONFIG", "PID_ADDR_REPORT",
		"PID_ADDR_CHECK", "PID_OBJECT_VALUE", "PID_OBJECTLINK", "PID_APPLICATION", "PID_PARAMETER", "PID_OBJECTADDRESS", "PID_PSU_TYPE", "PID_PSU_STATUS",
		"PID_PSU_ENABLE", "PID_DOMAIN_ADDRESS", "PID_IO_LIST", "PID_MGT_DESCRIPTOR_01", "PID_PL110_PARAM", "PID_RF_REPEAT_COUNTER", "PID_RECEIVE_BLOCK_TABLE",
		"PID_RANDOM_PAUSE_TABLE", "PID_RECEIVE_BLOCK_NR", "PID_HARDWARE_TYPE", "PID_RETRANSMITTER_NUMBER", "PID_SERIAL_NR_TABLE", "PID_BIBATMASTER_ADDRESS",
		"PID_RF_DOMAIN_ADDRESS", "PID_DEVICE_DESCRIPTOR", "PID_METERING_FILTER_TABLE", "PID_GROUP_TELEGR_RATE_LIMIT_TIME_BASE",
		"PID_GROUP_TELEGR_RATE_LIMIT_NO_OF_TELEGR",
		"PID_87", "PID_88", "PID_89", "PID_90", "PID_91", "PID_92", "PID_93", "PID_94", "PID_95", "PID_96", "PID_97", "PID_98", "PID_99", "PID_100",
		"PID_CHANNEL_01_PARAM", "PID_CHANNEL_02_PARAM", "PID_CHANNEL_03_PARAM", "PID_CHANNEL_04_PARAM", "PID_CHANNEL_05_PARAM",
		"PID_CHANNEL_06_PARAM", "PID_CHANNEL_07_PARAM", "PID_CHANNEL_08_PARAM", "PID_CHANNEL_09_PARAM", "PID_CHANNEL_10_PARAM",
		"PID_CHANNEL_11_PARAM", "PID_CHANNEL_12_PARAM", "PID_CHANNEL_13_PARAM", "PID_CHANNEL_14_PARAM", "PID_CHANNEL_15_PARAM",
		"PID_CHANNEL_16_PARAM", "PID_CHANNEL_17_PARAM", "PID_CHANNEL_18_PARAM", "PID_CHANNEL_19_PARAM", "PID_CHANNEL_20_PARAM",
		"PID_CHANNEL_21_PARAM", "PID_CHANNEL_22_PARAM", "PID_CHANNEL_23_PARAM", "PID_CHANNEL_24_PARAM", "PID_CHANNEL_25_PARAM",
		"PID_CHANNEL_26_PARAM", "PID_CHANNEL_27_PARAM", "PID_CHANNEL_28_PARAM", "PID_CHANNEL_29_PARAM", "PID_CHANNEL_30_PARAM",
		"PID_CHANNEL_31_PARAM", "PID_CHANNEL_32_PARAM",

		// PID_ABB_CUSTOM is 0xcc so there is a big gap between PID_CHANNEL_32_PARAM (132) and PID_ABB_CUSTOM
		// which is handled separately in printPropertyID(...)
	};
#endif
};

inline const PropertyDef* const* PropertiesBCU2::propertiesTab() const
{
	return propertiesTabInstance;
}

#endif /*sblib_properties_h*/
