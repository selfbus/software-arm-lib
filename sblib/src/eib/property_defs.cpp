/*
 *  property_defs.cpp - Definitions of BCU2, BIM112 (or newer) EIB interface objects with their properties.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/version.h>
#include <sblib/eib/user_memory.h>
#include <sblib/internal/variables.h>

#if BCU_TYPE != BCU1_TYPE

/**
 * The properties of the device object
 * See KNX Spec 06 Profiles/Annex A p.103 and 9/4/1 p.50
 */
static const PropertyDef deviceObjectProps[] =
{
    /** Interface object type: 2 bytes */
    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_DEVICE },

    /** Device control */
    { PID_DEVICE_CONTROL, PDT_GENERIC_01|PC_WRITABLE|PC_POINTER, PD_USER_RAM_OFFSET(deviceControl) },

    /** Load state control */
    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadState[OT_DEVICE]) },

    /** Service control: 2 bytes stored in userEeprom.serviceControl */
    { PID_SERVICE_CONTROL, PDT_UNSIGNED_INT|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(serviceControl) },

    /** Firmware revision: 1 byte */
    { PID_FIRMWARE_REVISION, PDT_UNSIGNED_CHAR, SBLIB_VERSION },

    /** Serial number: 6 byte data, stored in userEeprom.serial */
    { PID_SERIAL_NUMBER, PDT_GENERIC_06|PC_POINTER, PD_USER_EEPROM_OFFSET(serial) },

    /** Manufacturer ID: unsigned int, stored in userEeprom.manufacturerH (and manufacturerL)
     *  Configured as PDT_GENERIC_02 and not as PDT_UNSIGNED_INT to avoid swapping the byte order
     */
    { PID_MANUFACTURER_ID, PDT_GENERIC_02|PC_POINTER, PD_USER_EEPROM_OFFSET(manufacturerH) },

    /** Order number: 10 byte data, stored in userEeprom.serial */
    { PID_ORDER_INFO, PDT_GENERIC_10|PC_POINTER, PD_USER_EEPROM_OFFSET(serial) },

    /** PEI type: 1 byte */
    { PID_PEI_TYPE, PDT_UNSIGNED_CHAR|PC_POINTER, PD_USER_RAM_OFFSET(peiType) },

    /** Port A configuration: 1 byte, stored in userEeprom.portADDR */
    { PID_PORT_CONFIGURATION, PDT_UNSIGNED_CHAR|PC_POINTER, PD_USER_EEPROM_OFFSET(portADDR) },

    /** Hardware type: 6 byte data */
    { PID_HARDWARE_TYPE, PDT_GENERIC_06|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(order) },

    /** End of table */
    PROPERTY_DEF_TABLE_END
};

/**
 * The properties of the address table object
 * See KNX Spec 06 Profiles/Annex A p.103 and 9/4/1 p.51
 */
static const PropertyDef addrTabObjectProps[] =
{
    /** Interface object type: 2 bytes */
    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_ADDR_TABLE },

    /** Load state control */
    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadState[OT_ADDR_TABLE]) },

    /** Pointer to the address table */
    { PID_TABLE_REFERENCE, PDT_UNSIGNED_INT|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(addrTabAddr) },

    /** End of table */
    PROPERTY_DEF_TABLE_END
};

/**
 * The properties of the association table object, See KNX Spec 06 Profiles/Annex A p.103 and 9/4/1 p.51
 */
static const PropertyDef assocTabObjectProps[] =
{
    /** Interface object type: 2 bytes */
    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_ASSOC_TABLE },

    /** Load state control */
    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadState[OT_ASSOC_TABLE]) },

    /** Pointer to the association table */
    { PID_TABLE_REFERENCE, PDT_UNSIGNED_INT|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(assocTabAddr) },

    /** End of table */
    PROPERTY_DEF_TABLE_END
};

/**
 * The properties of the application program object, See KNX Spec 9/4/1 p.52
 */
static const PropertyDef appObjectProps[] =
{
    /** Interface object type: 2 bytes */
    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_APPLICATION },

    /** Load state control */
    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadState[OT_APPLICATION]) },

    /** Run state control */
    { PID_RUN_STATE_CONTROL, PDT_UNSIGNED_CHAR|PC_POINTER, PD_USER_RAM_OFFSET(runState) },

    /** Program version */
    { PID_PROG_VERSION, PDT_GENERIC_05|PC_POINTER, PD_USER_EEPROM_OFFSET(manufacturerH) },

    /** Pointer to the communication objects table */
    { PID_TABLE_REFERENCE, PDT_UNSIGNED_INT|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(commsTabAddr) },

    /** ABB_CUSTOM */
    { PID_ABB_CUSTOM, PDT_GENERIC_10|PC_POINTER|PC_WRITABLE, PD_USER_RAM_OFFSET(user2) },

    /** End of table */
    PROPERTY_DEF_TABLE_END
};

/**
 * The properties of the KNX object association table object, could not find the specification in KNX Spec 2.1
 */
static const PropertyDef knxAssocTabObjectProps[] =
{
    // TODO check correct properties for object OT_KNX_OBJECT_ASSOCIATATION_TABLE
    /** Interface object type: 2 bytes */
    { PID_OBJECT_TYPE, PDT_UNSIGNED_INT, OT_KNX_OBJECT_ASSOCIATATION_TABLE },

    /** Load state control */
    { PID_LOAD_STATE_CONTROL, PDT_CONTROL|PC_WRITABLE|PC_POINTER, PD_USER_EEPROM_OFFSET(loadState[OT_KNX_OBJECT_ASSOCIATATION_TABLE]) },

    /** Pointer to the communication objects table */
    { PID_TABLE_REFERENCE, PDT_UNSIGNED_INT|PC_ARRAY_POINTER, PD_USER_EEPROM_OFFSET(commsTabAddr) },

    /** Error code */
    { PID_ERROR_CODE, PDT_GENERIC_01, 0 }, // TODO implement PID_ERROR_CODE handling

    // TODO implement properties PID_TABLE, PID_MCB_TABLE and maybe more

    /** End of table */
    PROPERTY_DEF_TABLE_END
};

/**
 * The interface objects for
 * BCU2   (MASK_VERSIONs 0x0020, 0x0021, 0x0025)
 * BIM112 (MASK_VERSIONs 0x0701, 0x0705)
 */
const PropertyDef* const propertiesTab[NUM_PROP_OBJECTS] =
{
    deviceObjectProps,     //!> Interface Object 0, mandatory
    addrTabObjectProps,    //!> Interface Object 1, mandatory
    assocTabObjectProps,   //!> Interface Object 2, mandatory
    appObjectProps,        //!> Interface Object 3, mandatory
    knxAssocTabObjectProps //!> Interface Object 5, some newer MASK_VERSIONs (>= 0x0701) use this to set the address of the communication object table
};

#endif /*BCU_TYPE != BCU1_TYPE*/
