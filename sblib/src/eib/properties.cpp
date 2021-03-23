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


#define DMP_LOADSTATE_MACHINE_WRITE_RCO_IO_LENGTH  10 //!> Data length of a valid DMP_LoadStateMachineWrite_RCo_IO telegram
#define DMP_LOADSTATE_MACHINE_WRITE_RCO_MEM_LENGTH  11 //!> Data length of a valid DMP_LoadStateMachineWrite_RCo_Mem telegram (deprecated)

#define DMP_LOADSTATE_MACHINE_WRITE_RCO_IO_PAYLOAD_OFFSET 2 //!> offset for RCo_IO mode, where the real data for Additional Load Controls starts
#define DMP_LOADSTATE_MACHINE_WRITE_RCO_MEM_PAYLOAD_OFFSET 3 //!> offset for RCo_Mem mode, where the real data for Additional Load Controls starts

#define DMP_LOADSTATE_MACHINE_WRITE_RCO_PAYLOAD_LENGTH 7 //!> the length of the payload for both implementations

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

    if (defFound == nullptr)
    {
        IF_DUMP_PROPERTIES(serial.print("findProperty: "); printObjectIdx(table->id); serial.print(" "); printPropertyID(propertyId); serial.println(" not implemented"););
    }

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
        IF_DUMP_PROPERTIES(serial.print("propertyDef: ");printObjectIdx(objectIdx); serial.println(" not implemented!"););
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
LoadState handleLoadStateMachine(const int objectIdx, const byte* data, const int len)
{
    // FIXME at least these "interface objects" should support their load states.
    // userEeprom.loadState[OT_ADDR_TABLE]
    // userEeprom.loadState[OT_ASSOC_TABLE]
    // userEeprom.loadState[OT_APPLICATION]

    LoadState newLoadState = LS_ERROR;

    LoadControl loadcontrol = LoadControl(data[0] & 7);
    switch (loadcontrol) // the control state
    {
    case LC_START_LOADING: // Load
        newLoadState = LS_LOADING; // reply: Loading
        break;

    case LC_LOAD_COMPLETED: // Load completed
        newLoadState =  LS_LOADED; // reply: Loaded
        break;

    case LC_ADDITIONAL_LOAD_CONTROLS: // Load data: handled below
        newLoadState =  LS_LOADCOMPLETING; // returning LS_LOADCOMPLETING is a hack for flow control in int loadProperty()
        break;

    case LC_UNLOAD: // Unload
        newLoadState =  LS_UNLOADED;  // reply: Unloaded
        break;

    default:
        newLoadState =  LS_ERROR;  // reply: Error
        IF_DUMP_PROPERTIES(printObjectIdx(objectIdx); serial.println(" unknown loadcontrol=0x", loadcontrol, HEX, 2);serial.println(););
        IF_DUMP_PROPERTIES(fatalError()); // this will "halt the cpu"
        break;
    }

    IF_DUMP_PROPERTIES(serial.print("handleLoadStateMachine: "); printObjectIdx(objectIdx); serial.print(" "); printLoadState(newLoadState);serial.println(););
    return newLoadState;
}

/**
 * handles Additional Load Control: LoadEvent: AllocAbsTaskSeg (segment type 2) of a
 *
 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3.4 p.115
 *
 * @param objectIdx - interface object index (ObjectType)
 * @param payLoad - the telegram data bytes
 * @param len - the length of the payLoad
 *
 * @return new LoadState of the interface object objectIdx
 */
LoadState handleAllocAbsTaskSegment(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : start address        (SSSS)
    // payLoad[2]    : PEI type
    // payLoad[3..4] : software manufacturer id (MMMM)
    // payLoad[5..6] : manufacturer specific application software id (TTTT)
    // payLoad[7]    : version of the application software

    if (len < DMP_LOADSTATE_MACHINE_WRITE_RCO_PAYLOAD_LENGTH)
        return LS_ERROR;

    IF_DUMP_PROPERTIES(
            serial.print("handleAllocAbsTaskSegment: ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.print("  --> startaddress: 0x", makeWord(payLoad[0], payLoad[1]), HEX, 4);
            serial.print(" PEI: 0x", payLoad[2], HEX, 2);
            serial.print(" Manufacturer: 0x", makeWord(payLoad[3], payLoad[4]), HEX, 4);
            serial.print(" AppID: 0x", makeWord(payLoad[5], payLoad[6]), HEX, 4);
            serial.println(" Vers.: 0x", payLoad[7], HEX, 2);
    );

    int addr = makeWord(payLoad[0], payLoad[1]);
    // addr is used for address table, association table and communication object table
    switch (objectIdx)
    {
        case OT_ADDR_TABLE:
            userEeprom.addrTabAddr = addr;
            IF_DUMP_PROPERTIES(serial.println("  ----> userEeprom.addrTabAddr=0x", userEeprom.addrTabAddr, HEX, 4); serial.println(););
            break;

        case OT_ASSOC_TABLE:
            userEeprom.assocTabAddr = addr;
            IF_DUMP_PROPERTIES(serial.println("  ----> userEeprom.assocTabAddr=0x", userEeprom.assocTabAddr, HEX, 4); serial.println(););
            break;

        case OT_APPLICATION:
#           if BCU_TYPE == BIM112_TYPE
                userEeprom.commsTabAddr = addr;
                IF_DUMP_PROPERTIES(serial.println("  ----> userEeprom.commsTabAddr == 0x", userEeprom.commsTabAddr, HEX, 4););
#           endif
            userEeprom.appPeiType = payLoad[2];
            userEeprom.manufacturerH = payLoad[3];
            userEeprom.manufacturerL = payLoad[4];
            userEeprom.deviceTypeH = payLoad[5];
            userEeprom.deviceTypeL = payLoad[6];
            userEeprom.version = payLoad[7];
            IF_DUMP_PROPERTIES(
                    serial.println("  ----> userEeprom.appPeiType == 0x", userEeprom.appPeiType, HEX, 2);
                    serial.println("  ----> userEeprom.manufacturerH & L == 0x", makeWord(userEeprom.manufacturerH, userEeprom.manufacturerL), HEX, 4);
                    serial.println("  ----> userEeprom.deviceTypeH & L == 0x", makeWord(userEeprom.deviceTypeH, userEeprom.deviceTypeL), HEX, 4);
                    serial.println("  ----> userEeprom.version == 0x", userEeprom.version, HEX, 2);
                    serial.println();
            );
            break;
        default:
            IF_DUMP_PROPERTIES(serial.println("  ----> unknown objectIdx");serial.println(););
            return LS_ERROR;

        userEeprom.modified();
    }
    return LS_LOADING;
}

/**
 * NOT IMPLEMENTED!
 * should handle Additional Load Control: LoadEvent: AllocAbsDataSeg (segment type 0)
 *
 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3.4 p.114
 *
 * @param objectIdx - interface object index (ObjectType)
 * @param payLoad - the telegram data bytes
 * @param len - the length of the payLoad
 *
 * @return new LoadState of the interface object objectIdx
 */
LoadState handleAllocAbsDataSegment(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : start address        (SSSS)
    // payLoad[2..3] : length (LLLL)        (EEEE-SSSS+1) --> EEEE = SSSS+LLLL-1 // KNX Spec not really clear about that
    // payLoad[4]    : access attributes    (bit 0-3 write access, bit 4-7 read access level)
    // payLoad[5]    : memory type          (bit 0-2,  1=zero page RAM, 2=RAM, 3=EEPROM, bit 3-7 reserved)
    // payLoad[6]    : memory attributes    (bit 0-6 reserved, bit 7=0: checksum control disabled
    // payLoad[7]    : reserved
    unsigned int absDataSegmentStartAddress = makeWord(payLoad[0], payLoad[1]);
    unsigned int absDataSegmentLength = makeWord(payLoad[2], payLoad[3]);
    unsigned int absDataSegmentEndAddress = absDataSegmentStartAddress + absDataSegmentLength - 1;


    IF_DUMP_PROPERTIES(
            serial.print("handleAllocAbsDataSegment NOT IMPLEMENTED! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.print("  --> start: 0x", absDataSegmentStartAddress, HEX, 4);
            serial.print(" length: 0x", absDataSegmentLength, HEX, 4);
            serial.print(" end: 0x", absDataSegmentEndAddress, HEX, 4);
            serial.print(" access: 0x", payLoad[4], HEX, 2);
            serial.print(" memtype: 0x", payLoad[5], HEX, 2);
            serial.println(" attrib: 0x", payLoad[6], HEX, 2);
    );

    byte memStartValid = false;
    byte memEndValid = false;

    // TODO this check should be done in class BCU and is not complete, e.g. memType is ignored

    MemMapper* bcuMemMapper = ((BCU *) &bcu)->getMemMapper();

    memStartValid =  (bcuMemMapper != nullptr) && (bcuMemMapper->isMapped(absDataSegmentStartAddress));
    memStartValid |= (absDataSegmentStartAddress >= USER_EEPROM_START) && (absDataSegmentStartAddress < USER_EEPROM_END);
    memStartValid |= (absDataSegmentStartAddress >= (unsigned int) getUserRamStart()) && (absDataSegmentStartAddress < ((unsigned int) getUserRamStart() + USER_RAM_SIZE));

    memEndValid =  (bcuMemMapper != nullptr) && (bcuMemMapper->isMapped(absDataSegmentEndAddress));
    memEndValid |= (absDataSegmentEndAddress >= USER_EEPROM_START) && (absDataSegmentEndAddress < USER_EEPROM_END);
    memEndValid |= (absDataSegmentEndAddress >= (unsigned int) getUserRamStart()) && (absDataSegmentEndAddress < ((unsigned int) getUserRamStart() + USER_RAM_SIZE));

    if (!memStartValid)
    {
        IF_DUMP_PROPERTIES(serial.println("  ------> invalid start: 0x", absDataSegmentStartAddress, HEX, 4);serial.println(););
        return LS_ERROR;
    }

    if ( !memEndValid)
    {
        IF_DUMP_PROPERTIES(serial.println("  ------> invalid end: 0x", absDataSegmentEndAddress, HEX, 4);serial.println(););
        return LS_ERROR;
    }
    IF_DUMP_PROPERTIES(serial.println(););
    return LS_LOADING;
}

/**
 * NOT IMPLEMENTED!
 * should handle Additional Load Control: LoadEvent: AllocAbsStackSeg (segment type 1) of a
 *
 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3.4 p.115
 *
 * @param objectIdx - interface object index (ObjectType)
 * @param payLoad - the telegram data bytes
 * @param len - the length of the payLoad
 *
 * @return new LoadState of the interface object objectIdx
 */
LoadState handleAllocAbsStackSeg(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : start address        (SSSS)
    // payLoad[2..3] : length               (EEEE-SSSS+1)
    // payLoad[4]    : access attributes    (bit 0-3 write access, bit 4-7 read access level)
    // payLoad[5]    : memory type          (bit 0-2,  1=zero page RAM, 2=RAM, 3=EEPROM, bit 3-7 reserved)
    // payLoad[6]    : memory attributes    (bit 0-6 reserved, bit 7=0: checksum control disabled
    // payLoad[7]    : reserved
    IF_DUMP_PROPERTIES(
            serial.print("handleAllocAbsStackSeg NOT IMPLEMENTED! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.print("  --> start: 0x", makeWord(payLoad[0], payLoad[1]), HEX, 4);
            serial.print(" length: 0x", makeWord(payLoad[2], payLoad[3]), HEX, 4);
            serial.print(" access: 0x", payLoad[4], HEX, 2);
            serial.print(" memtype: 0x", payLoad[5], HEX, 2);
            serial.println(" attrib: 0x", payLoad[6], HEX, 2);
            serial.println();
    );
    return LS_LOADING;
}

/**
 * NOT IMPLEMENTED!
 * should handle Additional Load Control: LoadEvent: TaskPtr (segment type 3)
 *
 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3.4 p.116
 *
 * @param objectIdx - interface object index (ObjectType)
 * @param payLoad - the telegram data bytes
 * @param len - the length of the payLoad
 *
 * @return new LoadState of the interface object objectIdx
 */
LoadState handleTaskPtr(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : app init address           (IIII)
    // payLoad[2..3] : app save address           (SSSS)
    // payLoad[4..5] : custom PEIhandler function (PPPP)
    // payLoad[6..7] : reserved
    IF_DUMP_PROPERTIES(
            serial.print("handleTaskPtr NOT IMPLEMENTED! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.print("  --> init: 0x", makeWord(payLoad[0], payLoad[1]), HEX, 4);
            serial.print(" save: 0x", makeWord(payLoad[2], payLoad[3]), HEX, 4);
            serial.println(" PEI: 0x", makeWord(payLoad[2], payLoad[3]), HEX, 2);
            serial.println();
    );
    return LS_LOADING;
}

/**
 * NOT IMPLEMENTED!
 * for more information https://www.auto.tuwien.ac.at/~mkoegler/eib/doc/Bcu2Help_v12.chm
 * "System Architecture->Interface Objects->User Interface Objects->BCU2 User EIB Objects"
 *
 * should handle Additional Load Control: LoadEvent: TaskCtrl1 (segment type 4)
 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3.4 p.116
 *
 * @param objectIdx - interface object index (ObjectType)
 * @param payLoad - the telegram data bytes
 * @param len - the length of the payLoad
 *
 * @return new LoadState of the interface object objectIdx
 */
LoadState handleTaskCtrl1(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : interface object address
    // payLoad[2]    : nr. of interface objects
    // payLoad[3..7] : reserved
    userEeprom.eibObjAddr = makeWord(payLoad[0], payLoad[1]);
    userEeprom.eibObjCount = payLoad[2];
    IF_DUMP_PROPERTIES(
            serial.print("handleTaskCtrl1 ONLY PARTLY IMPLEMENTED! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.print("  --> userEeprom.eibObjAddr: 0x", userEeprom.eibObjAddr, HEX, 4);
            serial.println(" userEeprom.eibObjCount: 0x", userEeprom.eibObjCount, HEX, 2);
            serial.println();
    );
    return LS_LOADING;
}

/**
 * ONLY PARTLY IMPLEMENTED!
 * should handle Additional Load Control: LoadEvent: TaskCtrl2 (segment type 5)
 *
 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3.4 p.116
 *
 * @param objectIdx - interface object index (ObjectType)
 * @param payLoad - the telegram data bytes
 * @param len - the length of the payLoad
 *
 * @return new LoadState of the interface object objectIdx
 */
LoadState handleTaskCtrl2(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : app callbackAddr (CCCC)
    // payLoad[2..3] : CommObjPtr (OOOO)
    // payLoad[4..5] : CommObjSegPtr1 (1111h)
    // payLoad[6..7] : CommObjSegPtr2 (2222h)
    userEeprom.commsTabAddr = makeWord(payLoad[2], payLoad[3]);
    userEeprom.commsSeg0Addr = makeWord(payLoad[4], payLoad[5]); // commsSeg0Addr is nowhere used in sblib
    userEeprom.commsSeg1Addr = makeWord(payLoad[6], payLoad[7]); // commsSeg1Addr is nowhere used in sblib

    IF_DUMP_PROPERTIES(serial.print  (" userEeprom.commsTabAddr=0x", userEeprom.commsTabAddr, HEX, 4););
    IF_DUMP_PROPERTIES(serial.print  (" userEeprom.commsSeg0Addr=0x", userEeprom.commsSeg0Addr, HEX, 4););
    IF_DUMP_PROPERTIES(serial.println(" userEeprom.commsSeg1Addr=0x", userEeprom.commsSeg1Addr, HEX, 4););
    IF_DUMP_PROPERTIES(
            serial.print("handleTaskCtrl2 ONLY PARTLY IMPLEMENTED! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.print("  --> callbackAddr: 0x", makeWord(payLoad[0], payLoad[1]), HEX, 4);
            serial.print(" userEeprom.commsTabAddr: 0x", userEeprom.commsTabAddr, HEX, 4);
            serial.print(" userEeprom.commsSeg0Addr: 0x", userEeprom.commsSeg0Addr, HEX, 4);
            serial.println(" userEeprom.commsSeg1Addr: 0x", userEeprom.commsSeg1Addr, HEX, 4);
            serial.println();
    );
    return LS_LOADING;
}

/**
 * NOT IMPLEMENTED!
 * should handle Additional Load Control: LoadEvent: RelativeAllocation (segment type 0x0A)
 *
 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3.4 p.116
 *
 * @param objectIdx - interface object index (ObjectType)
 * @param payLoad - the telegram data bytes
 * @param len - the length of the payLoad
 *
 * @return new LoadState of the interface object objectIdx
 */
LoadState handleRelativeAllocation(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : data
    // payLoad[2..7] : fill octects (0x00)
    IF_DUMP_PROPERTIES(
            serial.print("handleRelativeAllocation NOT IMPLEMENTED! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.println("  --> data: 0x", makeWord(payLoad[0], payLoad[1]), HEX, 4);
            serial.println();
    );
    return LS_LOADING;
}

/**
 * NOT IMPLEMENTED!
 * should handle Additional Load Control: LoadEvent: DataRelativeAllocation (segment type 0x0B)
 *
 * DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated) or
 * DMP_LoadStateMachineWrite_RCo_IO (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3.4 p.116
 *
 * @param objectIdx - interface object index (ObjectType)
 * @param payLoad - the telegram data bytes
 * @param len - the length of the payLoad
 *
 * @return new LoadState of the interface object objectIdx
 */
LoadState handleDataRelativeAllocation(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..3] : requested memory size
    // payLoad[4]    : mode (0x00)
    // payLoad[5]    : fill (0x00)
    // payLoad[6..7] : reserved
    IF_DUMP_PROPERTIES(
            serial.print("handleDataRelativeAllocation NOT IMPLEMENTED! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            unsigned int reqMemSize = ((payLoad[0] << 16) | (payLoad[1] << 8) | (payLoad[2]));
            serial.print("  --> requested memory size: 0x", reqMemSize, HEX, 6);
            serial.print(" mode: 0x", payLoad[4], HEX, 2);
            serial.println(" fill: 0x", payLoad[5], HEX, 2);
            serial.println();
    );
    return LS_LOADING;
}

int loadProperty(int objectIdx, const byte* data, int len)
{
    // See KNX 3/5/2, 3.27 DM_LoadStateMachineWrite
    // See KNX 6/6 Profiles, p. 101 for load states
    // See BCU2 help System Architecture > Load Procedure

    // DMP_LoadStateMachineWrite_RCo_IO length of data must be 10  (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3 p.115
    // DMP_LoadStateMachineWrite_RCo_Mem length of data must be 11 (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated)
    if ((len > DMP_LOADSTATE_MACHINE_WRITE_RCO_MEM_LENGTH) || (len < DMP_LOADSTATE_MACHINE_WRITE_RCO_IO_LENGTH))
    {
        IF_DUMP_PROPERTIES(serial.print("loadProperty: "); printObjectIdx(objectIdx); serial.print(" invalid ");printData(data, len););
        return LS_ERROR;
    }

    LoadState newLoadState = handleLoadStateMachine(objectIdx, data, len);

    if (newLoadState != LS_LOADCOMPLETING)
    {
        return newLoadState;
    }

    //
    // from here newLoadState == LS_LOADCOMPLETING
    //
    // Additional Load Control: LoadEvent: segmentType
    //
    int segmentType = data[1]; // this is in both versions always the 2.octet

    byte payloadOffset;
    bool apciPropertyValueWrite = (len == DMP_LOADSTATE_MACHINE_WRITE_RCO_IO_LENGTH); // determine the realization type of DMP_LoadStateMachineWrite_RCo
    if (apciPropertyValueWrite)
        payloadOffset = DMP_LOADSTATE_MACHINE_WRITE_RCO_IO_PAYLOAD_OFFSET; // offset for RCo_IO mode, where the real data for Additional Load Controls starts
    else
        payloadOffset = DMP_LOADSTATE_MACHINE_WRITE_RCO_MEM_PAYLOAD_OFFSET; // offset for RCo_Mem mode, where the real data for Additional Load Controls starts


    const byte *payload  = data + payloadOffset; // "move" to start of payload data
    len -= payloadOffset; // reduce len by payloadOffset

    // IF_DUMP_PROPERTIES(serial.println();serial.print("loadProperty: "); printObjectIdx(objectIdx); serial.print(" "); printLoadState(newLoadState);serial.print(" ");printData(payload, len););
    // IF_DUMP_PROPERTIES(serial.println();serial.print("loadProperty: "); printObjectIdx(objectIdx); serial.print(" "); printLoadState(newLoadState);serial.print(" ");printData(data, len););

    // IF_DUMP_PROPERTIES(serial.print(" ");printSegmentType(segmentType););
    switch (segmentType)
    {
    case ST_ALLOC_ABS_DATA_SEG:  // ignored, Allocate absolute code/data segment (LdCtrlAbsSegment)
        return handleAllocAbsDataSegment(objectIdx, payload, len);

    case ST_ALLOC_ABS_STACK_SEG:  // ignored, Allocate absolute stack segment
        return handleAllocAbsStackSeg(objectIdx, payload, len);

    case ST_ALLOC_ABS_TASK_SEG:  // Segment control record (LdCtrlTaskSegment)
        // See KNX 3/5/2 p.115
        // Additional Load Control: LoadEvent: AllocAbsTaskSeg (segment type 2)
        return handleAllocAbsTaskSegment(objectIdx, payload, len);

    case ST_TASK_PTR:  // Task pointer (ignored)
        return handleTaskPtr(objectIdx, payload, len);

    case ST_TASK_CTRL_1:  // Task control 1
        return handleTaskCtrl1(objectIdx, payload, len);

    case ST_TASK_CTRL_2:  // Task control 2
        return handleTaskCtrl2(objectIdx, payload, len);

    case ST_RELATIVE_ALLOCATION: // relative allocation (ignored)
        return handleRelativeAllocation(objectIdx, payload, len);

    case ST_DATA_RELATIVE_ALLOCATION: // data relative allocation (ignored)
        return handleDataRelativeAllocation(objectIdx, payload, len);

    default:
        IF_DUMP_PROPERTIES(serial.println(" FATAL ERROR."););
        IF_DEBUG(fatalError());
        return LS_ERROR; // reply: Error
    }
    return LS_LOADING;
}

bool propertyValueReadTelegram(int objectIdx, PropertyID propertyId, int count, int start)
{
    // IF_DUMP_PROPERTIES(serial.print("propertyValueReadTelegram: "); printObjectIdx(objectIdx); serial.print(" "); printPropertyID(propertyId);serial.println(););
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
    // IF_DUMP_PROPERTIES(serial.print("propertyValueWriteTelegram: "); printObjectIdx(objectIdx); serial.print(" "); printPropertyID(propertyId);serial.println(););
    const PropertyDef* def = propertyDef(objectIdx, propertyId);
    if (!def)
        return false; // not found

    if (!(def->control & PC_WRITABLE))
    {
        IF_DUMP_PROPERTIES(printObjectIdx(objectIdx); serial.println(" "); printPropertyID(propertyId); serial.println(" not writable!"););
        return false; // not writable
    }

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
        IF_DUMP_PROPERTIES(serial.print("propertyValueWriteTelegram: "); printObjectIdx(objectIdx); serial.print(" "); printPropertyID(propertyId);serial.println(););
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
