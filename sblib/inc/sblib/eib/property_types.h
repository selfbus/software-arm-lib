/*
 *  property_types.h - BCU 2 & BIM112 property types of EIB objects.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *                2021 Darthyson <darth@maptrack.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_property_types_h
#define sblib_property_types_h

#include <sblib/types.h>
#include <sblib/utils.h>
#include <sblib/eib/bcu_type.h>

// See BCU2 help:
// System Architecture > Interface Objects > User Interface Objects > Attributes of Properties


/**
 * Data type of a property
 * <p>See KNX 3/7/3 4.0 p.18 Property Datatypes Identifiers</p>
 *
 * <b>changes in PropertyDataType must also be represented in
 * property_types.cpp const byte propertySizes[]</b>
 */
enum PropertyDataType
{
    PDT_CONTROL             = 0,    //!< length: 1 read, 10 write
    PDT_CHAR                = 1,    //!< length: 1
    PDT_UNSIGNED_CHAR       = 2,    //!< length: 1
    PDT_INT                 = 3,    //!< length: 2
    PDT_UNSIGNED_INT        = 4,    //!< length: 2
    PDT_EIB_FLOAT           = 5,    //!< length: 2
    PDT_DATE                = 6,    //!< length: 3
    PDT_TIME                = 7,    //!< length: 3
    PDT_LONG                = 8,    //!< length: 4
    PDT_UNSIGNED_LONG       = 9,    //!< length: 4
    PDT_FLOAT               = 0x0a, //!< length: 4
    PDT_DOUBLE              = 0x0b, //!< length: 8
    PDT_CHAR_BLOCK          = 0x0c, //!< length: 10
    PDT_POLL_GROUP_SETTING  = 0x0d, //!< length: 3
    PDT_SHORT_CHAR_BLOCK    = 0x0e, //!< length: 5
    PDT_DATE_TIME           = 0x0f, //!< length: 8
    PDT_VARIABLE_LENGTH     = 0x10, //!< length: variable
    PDT_GENERIC_01          = 0x11, //!< length: 1
    PDT_GENERIC_02          = 0x12, //!< length: 2
    PDT_GENERIC_03          = 0x13, //!< length: 3
    PDT_GENERIC_04          = 0x14, //!< length: 4
    PDT_GENERIC_05          = 0x15, //!< length: 5
    PDT_GENERIC_06          = 0x16, //!< length: 6
    PDT_GENERIC_07          = 0x17, //!< length: 7
    PDT_GENERIC_08          = 0x18, //!< length: 8
    PDT_GENERIC_09          = 0x19, //!< length: 9
    PDT_GENERIC_10          = 0x1a, //!< length: 10
    PDT_GENERIC_11          = 0x1b, //!< length: 11
    PDT_GENERIC_12          = 0x1c, //!< length: 12
    PDT_GENERIC_13          = 0x1d, //!< length: 13
    PDT_GENERIC_14          = 0x1e, //!< length: 14
    PDT_GENERIC_15          = 0x1f, //!< length: 15
    PDT_GENERIC_16          = 0x20, //!< length: 16
    PDT_GENERIC_17          = 0x21, //!< length: 17
    PDT_GENERIC_18          = 0x22, //!< length: 18
    PDT_GENERIC_19          = 0x23, //!< length: 19
    PDT_GENERIC_20          = 0x24, //!< length: 20
    PDT_RESERVED_0x25       = 0x25, //!< reserved
    PDT_RESERVED_0x26       = 0x26, //!< reserved
    PDT_RESERVED_0x27       = 0x27, //!< reserved
    PDT_RESERVED_0x28       = 0x28, //!< reserved
    PDT_RESERVED_0x29       = 0x29, //!< reserved
    PDT_RESERVED_0x2a       = 0x2a, //!< reserved
    PDT_RESERVED_0x2b       = 0x2b, //!< reserved
    PDT_RESERVED_0x2c       = 0x2c, //!< reserved
    PDT_RESERVED_0x2d       = 0x2d, //!< reserved
    PDT_RESERVED_0x2e       = 0x2e, //!< reserved
    PDT_UTF_8               = 0x2f, //!< length: variable
    PDT_VERSION             = 0x30, //!< length: 2
    PDT_ALARM_INFO          = 0x31, //!< length: 6
    PDT_BINARY_INFORMATION  = 0x32, //!< length: 1 bit?
    PDT_BITSET8             = 0x33, //!< length: 1
    PDT_BITSET16            = 0x34, //!< length: 2
    PDT_ENUM8               = 0x35, //!< length: 1
    PDT_SCALING             = 0x36, //!< length: 1
    PDT_RESERVED_0x37       = 0x37, //!< reserved
    PDT_RESERVED_0x38       = 0x38, //!< reserved
    PDT_RESERVED_0x39       = 0x39, //!< reserved
    PDT_RESERVED_0x3a       = 0x3a, //!< reserved
    PDT_RESERVED_0x3b       = 0x3b, //!< reserved
    PDT_NE_VL               = 0x3c, //!< length: undefined
    PDT_NE_FL               = 0x3d, //!< length: undefined
    PDT_FUNCTION            = 0x3e, //!< length: usage dependent
    PDT_ESCAPE              = 0x3f  //!< length: defined or undefined
};


/**
 * Definition of a property.
 */
struct PropertyDef
{
    /**
     * The ID of the property.
     */
    byte id;

    /**
     * The control byte contains bits that define the characteristics of a property.
     *
     * Bit 0..4: type, see PropertyDataType
     * Bit 5: the property value is a pointer
     * Bit 6: the property is an array
     * Bit 7: the property is writable
     */
    byte control;

    /**
     * Address of the property value, or the value itself if bit 5 of control is unset.
     * If it is a pointer, it points into BCU2 address space: userEeprom or userRam.
     * Bit 15 is special if valPtr contains a pointer: then the pointer points to a function.
     *
     * See valuePointer() below.
     */
    word valAddr;

    /**
     * Get the value pointer.
     *
     * @return The pointer to the property value.
     */
    byte* valuePointer() const;

    /**
     * Test if the valuePointer() points to the userEeprom.
     *
     * @return True if it is an EEPROM pointer, false if not.
     */
    bool isEepromPointer() const;

    /**
     * Get the data type of the property.
     *
     * @return The property type, see PropertyDataType
     */
    PropertyDataType type() const;

    /**
     * Get the size of the property in bytes.
     *
     * @return The size of the property.
     */
    int size() const;
};


/**
 * Property ID.
 * <p>See KNX 3/7/3 3.3 p.12 Interface Object Type specific Standardized Property Identifiers</p>
 *
 * <b>changes in PropertyID must also be represented in *propertyID_str[] (properties_dump.cpp)</b>
 */
enum PropertyID
{
    /** Generic object property: Interface Object Type, PDT_UNSIGNED_INT */
    PID_OBJECT_TYPE = 1,

    /** Interface Object Name, PDT_UNSIGNED_CHAR[] */
    PID_OBJECT_NAME = 2,

    /** - */
    PID_SEMAPHOR = 3,

    /** - */
    PID_GROUP_OBJECT_REFERENCE = 4,

    /** Application object property: load state control, PDT_CONTROL */
    PID_LOAD_STATE_CONTROL = 5,

    /** Application object property: run state control, PDT_CONTROL */
    PID_RUN_STATE_CONTROL = 6,

    /** Application object property: table address, PDT_UNSIGNED_LONG */
    PID_TABLE_REFERENCE = 7,

    /** Device object property: service control, PDT_UNSIGNED_INT */
    PID_SERVICE_CONTROL = 8,

    /** Device object property: firmware revision, PDT_UNSIGNED_CHAR */
    PID_FIRMWARE_REVISION = 9,

    /** Supported Services */
    PID_SERVICES_SUPPORTED = 10,

    /** Device object property: KNX serial number, PDT_GENERIC_06 */
    PID_SERIAL_NUMBER = 11,

    /** Device object property: manufacturer ID, PDT_UNSIGNED_INT */
    PID_MANUFACTURER_ID = 12,

    /** Application object property: program version, PDT_GENERIC_05 */
    PID_PROG_VERSION = 13,

    /** Device object property: device control, PDT_BISET8 alt. PDT_GENERIC_01 */
    PID_DEVICE_CONTROL = 14,

    /** Device object property: order info, PDT_GENERIC_10 */
    PID_ORDER_INFO = 15,

    /** Device object property: PEI type, PDT_UNSIGNED_CHAR */
    PID_PEI_TYPE = 16,

    /** Device object property: PortADDR configuration, PDT_UNSIGNED_CHAR */
    PID_PORT_CONFIGURATION = 17,

    /** Pollgroup Settings, PDT_POLL_GROUP_SETTINGS */
    PID_POLL_GROUP_SETTINGS = 18,

    /** Manufacturer Data, PDT_GENERIC_04 (PDT_GENERIC_XX) */
    PID_MANUFACTURER_DATA = 19,

    /** - */
    PID_ENABLE = 20,

    /** Description PDT_UNSIGNED_CHAR[ ] */
    PID_DESCRIPTION = 21,

    /** - */
    PID_FILE = 22,

    /** Table, PDT_UNSIGNED_INT[], PDT_GENERIC_02[] */
    PID_TABLE = 23,

    /** Interface Object Link, PDT_FUNCTION */
    PID_ENROL = 24,

    /** Version, PDT_VERSION (U6U6U6), (PDT_GENERIC_02) */
    PID_VERSION = 25,

    /** Group Address Assignment, PDT_FUNCTION */
    PID_GROUP_OBJECT_LINK = 26,

    /** Memory Control Table, PDT_GENERIC_07[] */
    PID_MCB_TABLE = 27,

    /** Error code, PDT_GENERIC_01 */
    PID_ERROR_CODE = 28,

    /** Object Index, PDT_UNSIGNED_CHAR */
    PID_OBJECT_INDEX = 29,

    // .....

    /** Routing Count, Default Value of Routing count, PDT_UNSIGNED_CHAR */
    PID_ROUTING_COUNT = 51,

    /** MaxRetryCount, Value for NAK/BUSY Retries */
    PID_MAX_RETRY_COUNT = 52,

    /** Errorflags, PDT_UNSIGNED_CHAR */
    PID_ERROR_FLAGS = 53,

    /** ProgrammingMode, Bit 0 = Programming Mode, PDT_BITSET8 */
    PID_PROGMODE = 54,

    /** Product Identification, Manufacturer Specific Device type, PDT_GENERIC_10 */
    PID_PRODUCT_ID = 55,

    /** Max. APDU-Length, Maximum APDU-Length, PDT_UNSIGNED_INT */
    PID_MAX_APDULENGTH = 56,

    /** SubNetAddress, High octet of Individual Address, PDT_UNSIGNED_CHAR */
    PID_SUBNET_ADDR = 57,

    /** DeviceAddress, Low octet of Individual Address, PDT_UNSIGNED_CHAR */
    PID_DEVICE_ADDR = 58,

    /** PushbuttonConf., Pushbutton Configuration, PDT_GENERIC_04 */
    PID_PB_CONFIG = 59,

    /** AddressReport, PDT_GENERIC_06 */
    PID_ADDR_REPORT = 60,

    /** AddressCheck, PDT_GENERIC_01 */
    PID_ADDR_CHECK = 61,

    /** Object Value, access to a Group Object value, PDT_FUNCTION */
    PID_OBJECT_VALUE = 62,

    /** Object Link, function call set and delete links, PDT_FUNCTION */
    PID_OBJECTLINK = 63,

    /** Application, change active application, PDT_FUNCTION */
    PID_APPLICATION = 64,

    /** Parameter, access to parameters, PDT_FUNCTION */
    PID_PARAMETER = 65,

    /** Object Address, read group object addresses, PDT_FUNCTION */
    PID_OBJECTADDRESS = 66,

    /** PSU Type, nominal current of device providing distributed power supply device information; real data, PDT_UNSIGNED_INT, DPT_UElCurrentmA (7.012) */
    PID_PSU_TYPE = 67,

    /** PSU Status, status of DPSU in device device information; real data, PDT_BINARY_INFORMATION (alt.: PDT_UNSIGNED_CHAR) DPT_Switch (1.001) */
    PID_PSU_STATUS = 68,

    /** PSU Enable, control over DPSU functionality in the device device information: real data, PDT_ENUM8 DPT_PSUMode (20.008) */
    PID_PSU_ENABLE = 69,

    /** Domain Address, To read out the Domain Address of a device., PDT_UNSIGNED_INT */
    PID_DOMAIN_ADDRESS = 70,

    /** Interface Object List, cEMI, PDT_UNSIGNED_INT */
    PID_IO_LIST = 71,

    /** Management Descriptor 1, Descriptor for exteded device descriptor for mask version 0100 (so far only allowed for this mask). This property shall not be used for other masks., PDT_GENERIC_10 */
    PID_MGT_DESCRIPTOR_01 = 72,

    /** PL110 Parameters, Frequency setting. Indication of whether a repeater is present Indication of whether the device is a repeater or not., PDT_GENERIC_01 */
    PID_PL110_PARAM = 73,

    /** RF Repeat Counter, To be completed., PDT_UNSIGNED_CHAR */
    PID_RF_REPEAT_COUNTER = 74,

    /** BiBat Receive Block Table, Settings for a RF BiBat Slave in which synchronous blocks it shall receive., PDT_UNSIGNED_CHAR[16] */
    PID_RECEIVE_BLOCK_TABLE = 75,

    /** BiBat Random Pause Table, Random pause table in a RF BiBat domain., PDT_UNSIGNED_CHAR[12] */
    PID_RANDOM_PAUSE_TABLE = 76,

    /** BiBat Receive Block Number, Number or receive blocks supported by the RF BiBat slave., PDT_UNSIGNED_CHAR */
    PID_RECEIVE_BLOCK_NR = 77,

    /** Hardware Type, Property identifying the hardware, allowing differentiation between hardware types of the same mask, introduced for a check possibility for ETS., PDT_GENERIC_06 */
    PID_HARDWARE_TYPE = 78,

    /** BiBat Retransmitter Number, Defines the time delay for the retransmitter., PDT_UNSIGNED_CHAR */
    PID_RETRANSMITTER_NUMBER = 79,

    /** Serial Number Table, Table containing the KNX Serial Number of the BiBat Master and optionally other devices., PDT_GENERIC_06[] */
    PID_SERIAL_NR_TABLE = 80,

    /** BiBat Master Individual Address, Individual Address of the BiBat Master, used by BiBat Slave in help call., PDT_UNSIGNED_INT */
    PID_BIBATMASTER_ADDRESS = 81,

    /** RF Domain Address, RF Domain Address for cEMI server filtering and sending, PDT_GENERIC_06 */
    PID_RF_DOMAIN_ADDRESS = 82,

    /** Device Descriptor, Device Descriptor for KNXnet/IP devices., PDT_GENERIC_02 */
    PID_DEVICE_DESCRIPTOR = 83,

    /** Metering Filter Table, Metering Filter Table for RF M-Bus Repeater and for the M-Bus Data Collector., PDT_GENERIC_08[] */
    PID_METERING_FILTER_TABLE = 84,

    /** group telegram rate limitation time base, Telegram rate limitation time base, PDT_UNSIGNED_INT */
    PID_GROUP_TELEGR_RATE_LIMIT_TIME_BASE = 85,

    /** group telegram rate limitation number of telegrams, Telegram rate limitation number, PDT_UNSIGNED_INT */
    PID_GROUP_TELEGR_RATE_LIMIT_NO_OF_TELEGR = 86,

    // .....

    /** Channel 1 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_01_PARAM = 101,

    /** Channel 2 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_02_PARAM = 102,

    /** Channel 3 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_03_PARAM = 103,

    /** Channel 4 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_04_PARAM = 104,

    /** Channel 5 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_05_PARAM = 105,

    /** Channel 6 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_06_PARAM = 106,

    /** Channel 7 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_07_PARAM = 107,

    /** Channel 8 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_08_PARAM = 108,

    /** Channel 9 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_09_PARAM = 109,

    /** Channel 10 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_10_PARAM = 110,

    /** Channel 11 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_11_PARAM = 111,

    /** Channel 12 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_12_PARAM = 112,

    /** Channel 13 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_13_PARAM = 113,

    /** Channel 14 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_14_PARAM = 114,

    /** Channel 15 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_15_PARAM = 115,

    /** Channel 16 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_16_PARAM = 116,

    /**  Channel 17 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_17_PARAM = 117,

    /**  Channel 18 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_18_PARAM = 118,

    /**  Channel 19 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_19_PARAM = 119,

    /**  Channel 20 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_20_PARAM = 120,

    /**  Channel 21 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_21_PARAM = 121,

    /**  Channel 22 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_22_PARAM = 122,

    /**  Channel 23 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_23_PARAM = 123,

    /**  Channel 24 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_24_PARAM = 124,

    /**  Channel 25 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_25_PARAM = 125,

    /**  Channel 26 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_26_PARAM = 126,

    /**  Channel 27 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_27_PARAM = 127,

    /**  Channel 28 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_28_PARAM = 128,

    /**  Channel 29 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_29_PARAM = 129,

    /**  Channel 30 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_30_PARAM = 130,

    /**  Channel 31 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_31_PARAM = 131,

    /**  Channel 32 Param, Easy configuration channel parameter, PDT_GENERIC_01[] */
    PID_CHANNEL_32_PARAM = 132,

    // .....

    /** ABB specific property, PDT_GENERIC_10 */
    PID_ABB_CUSTOM = 0xcc
};


/**
 * Property control constants.
 */
enum PropertyControl
{
    PC_WRITABLE = 0x80,       //!< The property can be modified
    PC_ARRAY = 0x40,          //!< The property is an array (max. 255 bytes)
    PC_POINTER = 0x20,        //!< valPtr of the property definition is a pointer
    PC_ARRAY_POINTER = 0x60,  //!< Combination of PC_ARRAY|PC_POINTER
    PC_TYPE_MASK = 0x1f       //!< Bit mask for the property type
};


/**
 * Property pointer type.
 */
enum PropertyPointerType
{
    PPT_USER_RAM = 0,         //!< Pointer to user RAM
    PPT_USER_EEPROM = 0x4000, //!< Pointer to user EEPROM
    PPT_MASK = 0x7000,        //!< Bitmask for property pointer types
    PPT_OFFSET_MASK = 0x0fff  //!< Bitmask for property pointer offsets
};


/** Define a PropertyDef pointer to variable v in the userRam */
#define PD_USER_RAM_OFFSET(v) (OFFSET_OF(UserRam, v) + PPT_USER_RAM)

/** Define a PropertyDef pointer to variable v in the userEeprom */
#define PD_USER_EEPROM_OFFSET(v) (OFFSET_OF(UserEeprom, v) + PPT_USER_EEPROM)

/** Define a PropertyDef pointer to variable v in the internal constants table */
#define PD_CONSTANTS_OFFSET(v) (OFFSET_OF(ConstPropValues, v) + PPT_CONSTANTS)

/** Mark the end of a property definition table */
#define PROPERTY_DEF_TABLE_END  { 0, 0, 0 }


//
//  Inline functions
//

inline PropertyDataType PropertyDef::type() const
{
    return (PropertyDataType) (control & 31);
}

inline bool PropertyDef::isEepromPointer() const
{
    return (valAddr & PPT_USER_EEPROM) != 0;
}

#endif /*sblib_property_types_h*/
