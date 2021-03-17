/*
 *  properties.cpp - properties of EIB objects for all BCUs except BCU 1
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/properties.h>
#include <sblib/eib/bcu_type.h>

#if BCU_TYPE != BCU1_TYPE

#include <sblib/core.h>
#include <sblib/eib/user_memory.h>
#include <sblib/eib/bcu.h>
#include <sblib/internal/functions.h>
#include <sblib/internal/variables.h>

#include <string.h> // for memcpy in propertyValueReadTelegram()

#include <sblib/eib/property_dump.h>

// Documentation:
// see KNX 6/6 Profiles, p. 94+
// see KNX 3/7/3 Standardized Identifier Tables, p. 11+

const PropertyDef* findProperty(PropertyID propertyId, const PropertyDef* table)
{
    const PropertyDef* defFound = nullptr;

    for (const PropertyDef* def = table; def->id; ++def)
    {
        if (def->id == propertyId)
        {
            defFound = def;
            break;
        }
    }

#ifdef DUMP_PROPERTIES
    if (defFound == nullptr)
    {
        printObjectIdx(table->id);
        serial.print(" propertyId=0x", propertyId, HEX, 2);
        if (propertyId == PID_MAX_APDULENGTH) // this is a quite common property, so lets print their name
        {
            serial.print(" PID_MAX_APDULENGTH");
        }
        serial.println(" not implemented");
    }
#endif

    return defFound;
}

/**
 * Get a property definition of an interface object.
 *
 * @param objectIdx - the index of the interface object.
 * @param propertyId - the property ID.
 *
 * @return The property definition, or 0 if not found.
 */
const PropertyDef* propertyDef(int objectIdx, PropertyID propertyId)
{
    if (objectIdx >= NUM_PROP_OBJECTS)
    {
        IF_DUMP_PROPERTIES(printObjectIdx(objectIdx); serial.println(" not implemented!"););
        return 0;
    }
    return findProperty(propertyId, propertiesTab[objectIdx]);
}

/**
 * handles load controls for a interface object (not really implemented)
 *
 * DM_LoadStateMachineWrite KNX Spec. 3/5/2 3.28 p.108
 * also See KNX 6/6 Profiles, p. 101 for load states
 * See BCU2 help System Architecture > Load Procedure
 *
 * @param objectIdx - interface object index (ObjectType)
 * @param data - the telegram data bytes
 * @param len - the length of the data
 * @return the new LoadState of the interface object
 */
LoadState handleLoadStateMachine(int objectIdx, const byte* data, int len)
{
    // FIXME at least these "interface objects" should support their load states.
    // userEeprom.loadState[OT_ADDR_TABLE]
    // userEeprom.loadState[OT_ASSOC_TABLE]
    // userEeprom.loadState[OT_APPLICATION]

    LoadControl loadcontrol = LoadControl(data[0] & 7);
    switch (loadcontrol) // the control state
    {
    case LC_START_LOADING: // Load
        return LS_LOADING; // reply: Loading

    case LC_LOAD_COMPLETED: // Load completed
        return LS_LOADED; // reply: Loaded

    case LC_ADDITIONAL_LOAD_CONTROLS: // Load data: handled below
        return LS_LOADCOMPLETING; // returning LS_LOADCOMPLETING is a hack for flow control in int loadProperty()

    case LC_UNLOAD: // Unload
        return LS_UNLOADED;  // reply: Unloaded

    default:
        IF_DUMP_PROPERTIES(printObjectIdx(objectIdx); serial.println(" unknown loadcontrol=0x", loadcontrol, HEX, 2););
        IF_DUMP_PROPERTIES(fatalError()); // this will "halt the cpu"
        return LS_ERROR;  // reply: Error
    }
}

/**
 * handles SegmentType ST_ALLOC_ABS_TASK_SEG of a
 *
 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3 p.115
 *
 * @param objectIdx - interface object index (ObjectType)
 * @param data - the telegram data bytes
 * @param len - the length of the data
 * @param offset - optional offset (offset=1 for a APCI_MEMORY_WRITE_PDU, otherwise 0)
 *
 * @return new LoadState of the interface object objectIdx
 */
LoadState handleAllocAbsTaskSegment(int objectIdx, const byte* data, int len, int offset)
{
    int addr = makeWord(data[2 + offset], data[3 + offset]);
    // addr is used for address table, association table and communication object table
    switch (objectIdx)
    {
        case OT_ADDR_TABLE:
            userEeprom.addrTabAddr = addr;
            IF_DUMP_PROPERTIES(serial.println(" userEeprom.addrTabAddr=0x", userEeprom.addrTabAddr, HEX, 2););
            break;

        case OT_ASSOC_TABLE:
            userEeprom.assocTabAddr = addr;
            IF_DUMP_PROPERTIES(serial.println(" userEeprom.assocTabAddr=0x", userEeprom.assocTabAddr, HEX, 2););
            break;

        case OT_APPLICATION:
#           if BCU_TYPE == BIM112_TYPE
                userEeprom.commsTabAddr = addr;
                IF_DUMP_PROPERTIES(serial.println(" userEeprom.commsTabAddr == 0x", userEeprom.commsTabAddr, HEX, 2););
#           endif
            userEeprom.appPeiType = data[4 + offset];
            userEeprom.manufacturerH = data[5 + offset];
            userEeprom.manufacturerL = data[6 + offset];
            userEeprom.deviceTypeH = data[7 + offset];
            userEeprom.deviceTypeL = data[8 + offset];
            userEeprom.version = data[9 + offset];
            break;

        // TODO this actually doesn't work with MemoryWrite APCI_MEMORY_WRITE_PDU
        // should work for most of the apps using <LdCtrlTaskSegment LsmIdx="5" Address="xxxx" />
        case OT_KNX_OBJECT_ASSOCIATATION_TABLE:
#           if BCU_TYPE == BIM112_TYPE
                userEeprom.commsTabAddr = addr;
                IF_DUMP_PROPERTIES(serial.println(" userEeprom.commsTabAddr = 0x", userEeprom.commsTabAddr, HEX, 2));
#           endif

        default:
            IF_DUMP_PROPERTIES(serial.println(" unknown objectIdx"););
            return LS_ERROR;

        userEeprom.modified();
    }
    return LS_LOADING;
}

int loadProperty(int objectIdx, const byte* data, int len)
{
    // See KNX 3/5/2, 3.27 DM_LoadStateMachineWrite
    // See KNX 6/6 Profiles, p. 101 for load states
    // See BCU2 help System Architecture > Load Procedure

    LoadState newLoadState = handleLoadStateMachine(objectIdx, data, len);

    IF_DUMP_PROPERTIES(printObjectIdx(objectIdx); serial.print(" "); printLoadState(newLoadState););

    if (newLoadState != LS_LOADCOMPLETING)
    {
        IF_DUMP_PROPERTIES(serial.println(););
        return newLoadState;
    }

    //
    // Load data (newLoadState == LS_LOADCOMPLETING )
    //
    int segmentType = data[1];
    int addr = makeWord(data[2 + PROP_LOAD_OFFSET], data[3 + PROP_LOAD_OFFSET]);

    IF_DUMP_PROPERTIES(serial.print(" ");printSegmentType(segmentType););

    switch (segmentType)
    {
    case ST_ALLOC_ABS_DATA_SEG:  // ignored, Allocate absolute code/data segment (LdCtrlAbsSegment)
        // See KNX 3/5/2 p.84 (A_Memory_Write) and KNX 3/5/2 p.113 (A_PropertyValue_Write)
        // data[2..3]: address
        // data[4..5]: length
        // data[6]: read/write access level
        // data[7]: memory type (1=Low RAM, 2=RAM, 3=EEPROM)
        // data[8]: bit 7: enable checksum control
        IF_DUMP_PROPERTIES(serial.println(" ignored!"););
        break;

    case ST_ALLOC_ABS_STACK_SEG:  // Allocate absolute stack segment (ignored)
        IF_DUMP_PROPERTIES(serial.println(" ignored!"););
        break;

    case ST_ALLOC_ABS_TASK_SEG:  // Segment control record (LdCtrlTaskSegment)
        return handleAllocAbsTaskSegment(objectIdx, data, len, PROP_LOAD_OFFSET);

    case ST_TASK_PTR:  // Task pointer (ignored)
        // data[2+3]: app init function
        // data[4+5]: app save function
        // data[6+7]: custom PEI handler function
        IF_DUMP_PROPERTIES(serial.println(" ignored!"););
        break;

    case ST_TASK_CTRL_1:  // Task control 1
        userEeprom.eibObjAddr = addr;       // nowhere used
        userEeprom.eibObjCount = data[4];   // nowhere used
        IF_DUMP_PROPERTIES(serial.print  (" userEeprom.eibObjAddr=0x", userEeprom.eibObjAddr, HEX, 2););
        IF_DUMP_PROPERTIES(serial.println(" userEeprom.eibObjCount=0x", userEeprom.eibObjCount, HEX, 2););
        break;

    case ST_TASK_CTRL_2:  // Task control 2
        // data[2+3]: app callback function
        userEeprom.commsTabAddr = makeWord(data[4], data[5]);
        userEeprom.commsSeg0Addr = makeWord(data[6], data[7]); // commsSeg0Addr & commsSeg1Addr are nowhere used
        userEeprom.commsSeg1Addr = makeWord(data[8], data[9]); // TODO was .commsSeg0Addr, which would override above assignment of it, maybe bug?
        IF_DUMP_PROPERTIES(serial.print  (" userEeprom.commsTabAddr=0x", userEeprom.commsTabAddr, HEX, 2););
        IF_DUMP_PROPERTIES(serial.print  (" userEeprom.commsSeg0Addr=0x", userEeprom.commsSeg0Addr, HEX, 2););
        IF_DUMP_PROPERTIES(serial.println(" userEeprom.commsSeg1Addr=0x", userEeprom.commsSeg1Addr, HEX, 2););
        break;

    case ST_RELATIVE_ALLOCATION: // relative allocation (ignored)
        IF_DUMP_PROPERTIES(serial.println(" ignored!"););
        break;

    case ST_DATA_RELATIVE_ALLOCATION: // data relative allocation (ignored)
        IF_DUMP_PROPERTIES(serial.println(" ignored!"););
        break;

    default:
        IF_DUMP_PROPERTIES(serial.println(" FATAL ERROR."););
        IF_DEBUG(fatalError());
        return LS_ERROR; // reply: Error
    }

    return LS_LOADING;
}

bool propertyValueReadTelegram(int objectIdx, PropertyID propertyId, int count, int start)
{
    const PropertyDef* def = propertyDef(objectIdx, propertyId);
    if (!def) return false; // not found

    PropertyDataType type = (PropertyDataType) (def->control & PC_TYPE_MASK);
    byte* valuePtr = def->valuePointer();

    --start;
    int size = def->size();
    int len = count * size;
    if(len > 12) return false; // length error

    if (type < PDT_CHAR_BLOCK)
        reverseCopy(bcu.sendTelegram + 12, valuePtr + start * size, len);
    else memcpy(bcu.sendTelegram + 12, valuePtr + start * size, len);

    bcu.sendTelegram[5] += len;

    return true;
}

bool propertyValueWriteTelegram(int objectIdx, PropertyID propertyId, int count, int start)
{
    const PropertyDef* def = propertyDef(objectIdx, propertyId);
    if (!def)
        return false; // not found

    if (!(def->control & PC_WRITABLE))
        return false; // not writable

    PropertyDataType type = def->type();
    byte* valuePtr = def->valuePointer();

    const byte* data = bus.telegram + 12;
    int state, len;

    if (type == PDT_CONTROL)
    {
        len = bus.telegramLen - 13;
        state = loadProperty(objectIdx, data, len);
        userEeprom.loadState[objectIdx] = state;
        bcu.sendTelegram[12] = state;
        len = 1;
    }
    else
    {
        len = count * def->size();
        --start;

        reverseCopy(valuePtr + start * type, data, len);
        reverseCopy(bcu.sendTelegram + 12, valuePtr + start * type, len);

        if (def->isEepromPointer())
            userEeprom.modified();
    }

    bcu.sendTelegram[5] += len;
    return true;
}

bool propertyDescReadTelegram(int objectIdx, PropertyID propertyId, int index)
{
    const PropertyDef* def;

    if (propertyId)
        def = propertyDef(objectIdx, propertyId);
    else def = &propertiesTab[objectIdx][index];

    bcu.sendTelegram[10] = index;

    if (!def || !def->id)
    {
        bcu.sendTelegram[9] = propertyId;
        bcu.sendTelegram[11] = 0;
        bcu.sendTelegram[12] = 0;
        bcu.sendTelegram[13] = 0;
        bcu.sendTelegram[14] = 0;
        return false; // not found
    }

    int numElems;
    if ((def->control & PC_ARRAY_POINTER) == PC_ARRAY_POINTER)
        numElems = *def->valuePointer();
    else numElems = 1;

    bcu.sendTelegram[9] = def->id;
    bcu.sendTelegram[11] = def->control & (PC_TYPE_MASK | PC_WRITABLE);
    bcu.sendTelegram[12] = (numElems >> 8) & 15;
    bcu.sendTelegram[13] = numElems;
    bcu.sendTelegram[14] = def->control & PC_WRITABLE ? 0xf1 : 0x50; // wild guess from bus traces

    return true;
}

#endif /* BCU_TYPE != BCU1_TYPE */
