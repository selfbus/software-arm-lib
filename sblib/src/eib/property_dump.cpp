/*
 *  property_dump.cpp - dumping to serial of BCU 2 & BIM112 properties of EIB objects.
 *
 *  Copyright (c) 2021 Darthyson <darth@maptrack.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/property_dump.h>
// TODO check possibility to use vsnprintf from stdio.h (how much would it increase lib size?)

#if BCU_TYPE != BCU1_TYPE

#ifdef DUMP_PROPERTIES

const char *objectType_str[] =
{
    "OT_DEVICE", "OT_ADDR_TABLE", "OT_ASSOC_TABLE", "OT_APPLICATION", "OT_INTERFACE_PROGRAM", "OT_KNX_OBJECT_ASSOCIATATION_TABLE",
    "OT_ROUTER", "OT_LTE_ADDRESS_ROUTING_TABLE", "OT_CEMI_SERVER", "OT_GROUP_OBJECT_TABLE", "OT_POLLING_MASTER", "OT_KNXNET_IP_PARAMETER",
    "OT_APPLICATION_CONTROLLER", "OT_FILE_SERVER", "UNKNOWN_14", "UNKNOWN_15", "UNKNOWN_16", "OT_SECURITY", "UNKNOWN_18", "OT_RF_MEDIUM"
};

const char *loadState_str[] =
{
    "LS_UNLOADED", "LS_LOADED", "LS_LOADING", "LS_ERROR", "LS_UNLOADING", "LS_LOADCOMPLETING"
};

const char *segmentType_str[] =
{
    "ST_ALLOC_ABS_DATA_SEG", "ST_ALLOC_ABS_STACK_SEG", "ST_ALLOC_ABS_TASK_SEG", "ST_TASK_PTR", "ST_TASK_CTRL_1", "ST_TASK_CTRL_2",
    "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "ST_RELATIVE_ALLOCATION", "ST_DATA_RELATIVE_ALLOCATION"
};


const char *propertyID_str[] =
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

void printObjectIdx(int objectIdx)
{
    serial.print("objectIdx=0x", objectIdx, HEX, 2);
    if ((objectIdx >= OT_DEVICE) && (objectIdx <= OT_RF_MEDIUM))
    {
        serial.print(" ");
        serial.print(objectType_str[objectIdx]);
    }
    else
    {
        serial.print(" unknown");
    }
}

void printLoadState(int loadstate)
{
    serial.print("loadstate=0x", loadstate, HEX, 2);
    if ((loadstate >= LS_UNLOADED) && (loadstate <= LS_LOADCOMPLETING))
    {
        serial.print(" ");
        serial.print(loadState_str[loadstate]);
    }
    else
    {
        serial.print(" unknown");
    }
}

void printSegmentType(int segmenttype)
{
    serial.print("segmenttype=0x", segmenttype, HEX, 2);
    if ((segmenttype >= ST_ALLOC_ABS_DATA_SEG) && (segmenttype <= ST_DATA_RELATIVE_ALLOCATION))
    {
        serial.print(" ");
        serial.print(segmentType_str[segmenttype]);
    }
    else
    {
        serial.print(" unknown");
    }
}

void printPropertyID(int propertyid)
{
    serial.print("propertyid=0x", propertyid, HEX, 2);
    if ((propertyid >= PID_OBJECT_TYPE) && (propertyid <= PID_CHANNEL_32_PARAM))
    {
        serial.print(" ");
        serial.print(propertyID_str[propertyid]);
    }
    else if (propertyid == PID_ABB_CUSTOM)
    {
        serial.print(" PID_ABB_CUSTOM");
    }
    else
    {
        serial.print(" unknown");
    }
}

void printData(const byte* data, int len)
{
    serial.print("Data: ");
    for (int i = 0; i < len; i++)
    {
        serial.print(data[i], HEX, 2);
        serial.print(" ");
    }
    serial.print("len: ", len, DEC);
}

#endif /*DUMP_PROPERTIES*/

#endif /*BCU_TYPE != BCU1_TYPE*/
