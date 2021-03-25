/*
 *  properties.h - BCU 2 properties of EIB objects.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_properties_h
#define sblib_properties_h

#include <sblib/eib/property_types.h>

#if BCU_TYPE != BCU1_TYPE

/**
 * Load / configure a property of a interface object. Called when a "load control" property-write telegram is received.
 *
 * e.g.
 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3 p.115
 *
 * @param objectIdx - the interface object type ID.
 * @param data - the telegram data bytes
 * @param len - the length of the data.
 *
 * @return The new load state.
 */
int loadProperty(int objectIdx, const byte* data, int len);

/**
 * Find a property definition in a properties table.
 *
 * @param id - the ID of the property to find.
 * @param table - the properties table. The last table element must be PROPERTY_DEF_TABLE_END
 *
 * @return Pointer to the property definition, 0 if not found.
 */
const PropertyDef* findProperty(PropertyID id, const PropertyDef* table);

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

#endif /*BCU_TYPE != BCU1_TYPE*/

#endif /*sblib_properties_h*/
