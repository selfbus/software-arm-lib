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

#endif /*DUMP_PROPERTIES*/

#endif /*BCU_TYPE != BCU1_TYPE*/
