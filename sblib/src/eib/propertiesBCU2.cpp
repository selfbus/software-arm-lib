/*
 *  properties.cpp - properties of EIB objects for all BCUs except BCU 1
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/core.h>
#include <sblib/eib/bcu1.h>
#include <sblib/internal/variables.h>

#include <cstring> // for memcpy in propertyValueReadTelegram()

#include <sblib/eib/propertiesBCU2.h>
#include <sblib/eib/bcu2.h>
#include <sblib/eib/bus.h>

#define HIGH_RAM_START 0x0900
#define HIGH_RAM_LENGTH 0xBC

// Documentation:
// see KNX 6/6 Profiles, p. 94+
// see KNX 3/7/3 Standardized Identifier Tables, p. 11+

const PropertyDef* PropertiesBCU2::findProperty(PropertyID propertyId, const PropertyDef* table)
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
        DB_PROPERTIES(serial.print("findProperty: "); printObjectIdx(table->id); serial.print(" "); printPropertyID(propertyId); serial.println(" not implemented"););
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
const PropertyDef* PropertiesBCU2::propertyDef(int objectIdx, PropertyID propertyId)
{
    if (objectIdx >= NUM_PROP_OBJECTS)
    {
        DB_PROPERTIES(serial.print("propertyDef: ");printObjectIdx(objectIdx); serial.println(" not implemented!"););
        return nullptr;
    }
    return findProperty(propertyId, propertiesTab()[objectIdx]);
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
LoadState PropertiesBCU2::handleLoadStateMachine(const int objectIdx, const byte* data, const int len)
{
    // FIXME at least these "interface objects" should support their load states.
    // userEeprom->loadState[OT_ADDR_TABLE]
    // userEeprom->loadState[OT_ASSOC_TABLE]
    // userEeprom->loadState[OT_APPLICATION]

    LoadState newLoadState = LS_ERROR;

    LoadControl loadcontrol = LoadControl(data[0] & 7);
    switch (loadcontrol) // the control state
    {
        case LC_START_LOADING: // Load
        {
            newLoadState = LS_LOADING; // reply: Loading
            break;
        }
        case LC_LOAD_COMPLETED: // Load completed
        {
            newLoadState =  LS_LOADED; // reply: Loaded
            break;
        }
        case LC_ADDITIONAL_LOAD_CONTROLS: // Load data: handled below
        {
            newLoadState =  LS_LOADCOMPLETING; // returning LS_LOADCOMPLETING is a hack for flow control in int loadProperty()
            break;
        }
        case LC_UNLOAD: // Unload
        {
            newLoadState =  LS_UNLOADED;  // reply: Unloaded
            break;
        }
        default:
        {
            newLoadState =  LS_ERROR;  // reply: Error
            DB_PROPERTIES(printObjectIdx(objectIdx); serial.println(" unknown loadcontrol=0x", loadcontrol, HEX, 2);serial.println(););
            DB_PROPERTIES(fatalError()); // this will "halt the cpu"
            break;
        }
    }

    DB_PROPERTIES(serial.print("handleLoadStateMachine: "); printObjectIdx(objectIdx); serial.print(" "); printLoadState(newLoadState);serial.println();serial.flush(););
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
LoadState PropertiesBCU2::handleAllocAbsTaskSegment(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : start address        (SSSS)
    // payLoad[2]    : PEI type
    // payLoad[3..4] : software manufacturer id (MMMM)
    // payLoad[5..6] : manufacturer specific application software id (TTTT)
    // payLoad[7]    : version of the application software

    if (len < DMP_LOADSTATE_MACHINE_WRITE_RCO_PAYLOAD_LENGTH)
        return LS_ERROR;

    DB_PROPERTIES(
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
        {
            bcu->userEeprom->addrTabAddr() = addr;
            DB_PROPERTIES(serial.println("  ----> userEeprom->addrTabAddr=0x", bcu->userEeprom->addrTabAddr(), HEX, 4); serial.println(););
            break;
        }
        case OT_ASSOC_TABLE:
        {
            bcu->userEeprom->assocTabAddr() = addr;
            DB_PROPERTIES(serial.println("  ----> userEeprom->assocTabAddr=0x", bcu->userEeprom->assocTabAddr(), HEX, 4); serial.println(););
            break;
        }
        case OT_APPLICATION:
        {
        	bcu->userEeprom->appPeiType() = payLoad[2];
        	bcu->userEeprom->manufacturerH() = payLoad[3];
        	bcu->userEeprom->manufacturerL() = payLoad[4];
        	bcu->userEeprom->deviceTypeH() = payLoad[5];
        	bcu->userEeprom->deviceTypeL() = payLoad[6];
        	bcu->userEeprom->version() = payLoad[7];
        	DB_PROPERTIES(
                    serial.println("  ----> userEeprom->appPeiType = 0x", bcu->userEeprom->appPeiType(), HEX, 2);
                    serial.println("  ----> userEeprom->manufacturerH & L = 0x", makeWord(bcu->userEeprom->manufacturerH(), bcu->userEeprom->manufacturerL()), HEX, 4);
                    serial.println("  ----> userEeprom->deviceTypeH & L = 0x", makeWord(bcu->userEeprom->deviceTypeH(), bcu->userEeprom->deviceTypeL()), HEX, 4);
                    serial.println("  ----> userEeprom->version = 0x", bcu->userEeprom->version(), HEX, 2);
                    serial.println();
            );
            break;
        }
        default:
        {
            DB_PROPERTIES(serial.println("  ----> unknown objectIdx");serial.println(););
            return LS_ERROR;
        }

        bcu->userEeprom->modified(true);
    }
    return LS_LOADING;
}

/**
 * NOT IMPLEMENTED!
 * should handle Additional Load Control: LoadEvent: AllocAbsDataSeg (segment type 0) <LdCtrlAbsSegment>
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
LoadState PropertiesBCU2::handleAllocAbsDataSegment(const int objectIdx, const byte* payLoad, const int len)
{
/*
 *  from KNX Spec. 06 Profiles 4.2.9 RAM cleared
 *  RAM to be cleared by the Management Client during download of an application program:
 *
 *  0x00CE-0x00DF   BCU1, RAM
 *
 *  0x00BD-0x00DF   BCU2, Zero-Page-RAM
 *  0x0972-0x0989   BCU2, High RAM
 *
 *  0x0700-0x????   BIM112, RAM
 *
 *  //XXX sblib ignores this
 */

    // payLoad[0..1] : start address        (SSSS)
    // payLoad[2..3] : length (LLLL)        (EEEE-SSSS+1) --> EEEE = SSSS+LLLL-1 // KNX Spec not really clear about that
    // payLoad[4]    : access attributes    (bit 0-3 write access, bit 4-7 read access level)
    // payLoad[5]    : memory type          (bit 0-2,  1=zero page RAM, 2=RAM, 3=EEPROM, bit 3-7 reserved)
    // payLoad[6]    : memory attributes    (bit 0-6 reserved, bit 7=0: checksum control disabled
    // payLoad[7]    : reserved
    LoadState newLoadState = LS_ERROR;
    unsigned int absDataSegmentStartAddress = makeWord(payLoad[0], payLoad[1]);
    unsigned int absDataSegmentLength = makeWord(payLoad[2], payLoad[3]);
    unsigned int absDataSegmentEndAddress = absDataSegmentStartAddress + absDataSegmentLength - 1;
    MemoryType memType = MemoryType(payLoad[5] & 0x07); // take only bits 0..2

    DB_PROPERTIES(
            serial.print("handleAllocAbsDataSegment only partly implemented! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.print("  --> start: 0x", absDataSegmentStartAddress, HEX, 4);
            serial.print(" length: 0x", absDataSegmentLength, HEX, 4);
            serial.print(" end: 0x", absDataSegmentEndAddress, HEX, 4);
            serial.print(" access: 0x", payLoad[4], HEX, 2);
            serial.print(" memtype: 0x", payLoad[5], HEX, 2);
            serial.print(" checksum: ", ((payLoad[6] & 0x80) >> 7), DEC, 1);
            serial.println(" attrib: 0x", (payLoad[6]), HEX, 2);
    );

    bool memStartValid = false;
    bool memEndValid = false;

    // FIXME this check should be done in class BCU, and if its not available it should be allocated
    switch (memType)
    {
        // there is no real reason to differentiate between memory types,
        // important is, that we can reach it.
        case MT_ZERO_PAGE_RAM:
        case MT_RAM:
        case MT_EEPROM:
        {
            // check against user ram
            memStartValid = bcu->userRam->inRange(absDataSegmentStartAddress);
            memEndValid = bcu->userRam->inRange(absDataSegmentEndAddress);
            // check against user EEPROM
            memStartValid |= bcu->userEeprom->inRange(absDataSegmentStartAddress);
            memEndValid |= bcu->userEeprom->inRange(absDataSegmentEndAddress);
            // check against MemMapper
            MemMapper* bcuMemMapper = bcu->getMemMapper();
            memStartValid |=  (bcuMemMapper != nullptr) && (bcuMemMapper->isMapped(absDataSegmentStartAddress));
            memEndValid |=  (bcuMemMapper != nullptr) && (bcuMemMapper->isMapped(absDataSegmentEndAddress));

                // special handling of the High RAM (BCU 2.0/ 2.1) (0x0900 - 0x09BB),
                // see BCU 2 Help from https://www.auto.tuwien.ac.at/~mkoegler/index.php/bcudoc
                memStartValid |= (absDataSegmentStartAddress >= HIGH_RAM_START) && (absDataSegmentStartAddress < (HIGH_RAM_START + HIGH_RAM_LENGTH));
                memEndValid |= (absDataSegmentEndAddress >= HIGH_RAM_START) && (absDataSegmentEndAddress < (HIGH_RAM_START + HIGH_RAM_LENGTH));

            newLoadState = LS_LOADING;
            break;
        }
        default:
        {
            newLoadState = LS_ERROR;
            break;
        }
    }

    if (!memStartValid)
    {
        DB_PROPERTIES(serial.println("  ------> invalid start: 0x", absDataSegmentStartAddress, HEX, 4);serial.println(););
        newLoadState = LS_ERROR;
    }
    if ( !memEndValid)
    {
        DB_PROPERTIES(serial.println("  ------> invalid end: 0x", absDataSegmentEndAddress, HEX, 4);serial.println(););
        newLoadState = LS_ERROR;
    }
    DB_PROPERTIES(serial.println(););
    return newLoadState;
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
LoadState PropertiesBCU2::handleAllocAbsStackSeg(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : start address        (SSSS)
    // payLoad[2..3] : length               (EEEE-SSSS+1)
    // payLoad[4]    : access attributes    (bit 0-3 write access, bit 4-7 read access level)
    // payLoad[5]    : memory type          (bit 0-2,  1=zero page RAM, 2=RAM, 3=EEPROM, bit 3-7 reserved)
    // payLoad[6]    : memory attributes    (bit 0-6 reserved, bit 7=0: checksum control disabled
    // payLoad[7]    : reserved
    DB_PROPERTIES(
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
LoadState PropertiesBCU2::handleTaskPtr(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : app init address           (IIII)
    // payLoad[2..3] : app save address           (SSSS)
    // payLoad[4..5] : custom PEIhandler function (PPPP)
    // payLoad[6..7] : reserved
    DB_PROPERTIES(
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
LoadState PropertiesBCU2::handleTaskCtrl1(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : interface object address
    // payLoad[2]    : nr. of interface objects
    // payLoad[3..7] : reserved
    bcu->userEeprom->eibObjAddr() = makeWord(payLoad[0], payLoad[1]);
    bcu->userEeprom->eibObjCount() = payLoad[2];
    DB_PROPERTIES(
            serial.print("handleTaskCtrl1 ONLY PARTLY IMPLEMENTED! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.print("  --> userEeprom->eibObjAddr: 0x", bcu->userEeprom->eibObjAddr(), HEX, 4);
            serial.println(" userEeprom->eibObjCount: 0x", bcu->userEeprom->eibObjCount(), HEX, 2);
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
LoadState PropertiesBCU2::handleTaskCtrl2(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : app callbackAddr (CCCC)
    // payLoad[2..3] : CommObjPtr (OOOO)
    // payLoad[4..5] : CommObjSegPtr1 (1111h)
    // payLoad[6..7] : CommObjSegPtr2 (2222h)

    word addr = makeWord(payLoad[2], payLoad[3]);
    // we need this newAddress workaround, see comment @void BcuBase::begin(...) in bcu_base.h
    word newAddress = bcu->getCommObjectTableAddressStatic();
    if (newAddress == 0) // set newAddress, in case bcu doesn't provide a read-only address
    {
        newAddress = addr;
    }

    // XXX is it ok to set .commsTabAddr, .commsSeg0Addr & .commsSeg1Addr without checking objectIdx???
    bcu->userEeprom->commsTabAddr() = newAddress;
    bcu->userEeprom->commsSeg0Addr() = makeWord(payLoad[4], payLoad[5]); // commsSeg0Addr is nowhere used in sblib
    bcu->userEeprom->commsSeg1Addr() = makeWord(payLoad[6], payLoad[7]); // commsSeg1Addr is nowhere used in sblib

    DB_PROPERTIES(
            serial.print("handleTaskCtrl2 ONLY PARTLY IMPLEMENTED! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.print("  --> callbackAddr: 0x", makeWord(payLoad[0], payLoad[1]), HEX, 4);
            if (bcu->userEeprom->commsTabAddr() != addr)
            {
                serial.println();
                serial.println("  ----> userEeprom->commsTabAddr MARKED AS READ-ONLY, WON'T CHANGE TO 0x", addr, HEX, 4);
                serial.println();
            }
            serial.print(" userEeprom->commsTabAddr: 0x", bcu->userEeprom->commsTabAddr(), HEX, 4);
            serial.print(" userEeprom->commsSeg0Addr: 0x", bcu->userEeprom->commsSeg0Addr(), HEX, 4);
            serial.println(" userEeprom->commsSeg1Addr: 0x", bcu->userEeprom->commsSeg1Addr(), HEX, 4);
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
LoadState PropertiesBCU2::handleRelativeAllocation(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..1] : data
    // payLoad[2..7] : fill octects (0x00)
    DB_PROPERTIES(
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
 * PARTIALLY IMPLEMENTED for System_B !
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
LoadState PropertiesBCU2::handleDataRelativeAllocation(const int objectIdx, const byte* payLoad, const int len)
{
    // payLoad[0..3] : requested memory size
    // payLoad[4]    : mode (0x00)
    // payLoad[5]    : fill (0x00)
    // payLoad[6..7] : reserved
    DB_PROPERTIES(
            serial.print("handleDataRelativeAllocation PARTIALLY IMPLEMENTED for System_B! ");
            printObjectIdx(objectIdx);
            serial.print(" ");
            printData(payLoad, len);
            serial.println();
            serial.print("  --> requested memory size: 0x", ((payLoad[0] << 24) | (payLoad[1] << 16) | (payLoad[2] << 8) | payLoad[3]), HEX, 8);
            serial.print(" mode: 0x", payLoad[4], HEX, 2);
            serial.println(" fill: 0x", payLoad[5], HEX, 2);
            serial.println();
    );

    return LS_LOADING;
}

int PropertiesBCU2::loadProperty(int objectIdx, const byte* data, int len)
{
    // See KNX 3/5/2, 3.27 DM_LoadStateMachineWrite
    // See KNX 6/6 Profiles, p. 101 for load states
    // See BCU2 help System Architecture > Load Procedure

    // DMP_LoadStateMachineWrite_RCo_IO length of data must be 10  (ApciPropertyValueWrite) See KNX Spec. 3/5/2 3.28.3 p.115
    // DMP_LoadStateMachineWrite_RCo_Mem length of data must be 11 (APCI_MEMORY_WRITE_PDU) See KNX Spec. 3/5/2 3.28.2 p.109  (deprecated)
    if ((len > DMP_LOADSTATE_MACHINE_WRITE_RCO_MEM_LENGTH) || (len < DMP_LOADSTATE_MACHINE_WRITE_RCO_IO_LENGTH))
    {
        DB_PROPERTIES(serial.print("loadProperty: "); printObjectIdx(objectIdx); serial.print(" invalid ");printData(data, len););
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
    int segmentType = data[1]; // this is in both versions of DMP_LoadStateMachineWrite_RCo always the 2.octet

    byte payloadOffset;
    bool apciPropertyValueWrite = (len == DMP_LOADSTATE_MACHINE_WRITE_RCO_IO_LENGTH); // determine the realization type of DMP_LoadStateMachineWrite_RCo
    if (apciPropertyValueWrite)
        payloadOffset = DMP_LOADSTATE_MACHINE_WRITE_RCO_IO_PAYLOAD_OFFSET; // offset for RCo_IO mode, where the real data for Additional Load Controls starts
    else
        payloadOffset = DMP_LOADSTATE_MACHINE_WRITE_RCO_MEM_PAYLOAD_OFFSET; // offset for RCo_Mem mode, where the real data for Additional Load Controls starts


    const byte *payload  = data + payloadOffset; // "move" to start of payload data
    len -= payloadOffset; // reduce len by payloadOffset

    switch (segmentType)
    {
        case ST_ALLOC_ABS_DATA_SEG:  // Allocate absolute code/data segment (LdCtrlAbsSegment)
            return handleAllocAbsDataSegment(objectIdx, payload, len);

        case ST_ALLOC_ABS_STACK_SEG:  // ignored, Allocate absolute stack segment
            return handleAllocAbsStackSeg(objectIdx, payload, len);

        case ST_ALLOC_ABS_TASK_SEG:  // Segment control record (LdCtrlTaskSegment)
            return handleAllocAbsTaskSegment(objectIdx, payload, len);

        case ST_TASK_PTR:  // Task pointer (ignored)
            return handleTaskPtr(objectIdx, payload, len);

        case ST_TASK_CTRL_1:  // Task control 1
            return handleTaskCtrl1(objectIdx, payload, len);

        case ST_TASK_CTRL_2:  // Task control 2
            return handleTaskCtrl2(objectIdx, payload, len);

        case ST_RELATIVE_ALLOCATION: // relative allocation
            return handleRelativeAllocation(objectIdx, payload, len);

        case ST_DATA_RELATIVE_ALLOCATION: // data relative allocation (ignored)
            return handleDataRelativeAllocation(objectIdx, payload, len);

        default:
            DB_PROPERTIES(serial.println(" FATAL ERROR.");serial.flush(););
            IF_DEBUG(fatalError());
            return LS_ERROR; // reply: Error
    }
    return LS_LOADING;
}

bool PropertiesBCU2::propertyValueReadTelegram(int objectIdx, PropertyID propertyId, int count, int start, uint8_t * sendBuffer)
{
    DB_PROPERTIES(serial.print("propertyValueReadTelegram: "); printObjectIdx(objectIdx); serial.print(" "); printPropertyID(propertyId);serial.println(););
    const PropertyDef* def = propertyDef(objectIdx, propertyId);
    if (!def) return false; // not found

    PropertyDataType type = (PropertyDataType) (def->control & PC_TYPE_MASK);
    byte* valuePtr = def->valuePointer(bcu);

    --start;
    int size = def->size();
    int len = count * size;
    if(len > 10) return false; // length error

    if (type < PDT_CHAR_BLOCK)
    {
        reverseCopy(sendBuffer + 12, valuePtr + start * size, len);
    }
    else
    {
        memcpy(sendBuffer + 12, valuePtr + start * size, len);
    }

    sendBuffer[5] += len;

    return true;
}

bool PropertiesBCU2::propertyValueWriteTelegram(int objectIdx, PropertyID propertyId, int count, int start, uint8_t * sendBuffer)
{
    const PropertyDef* def = propertyDef(objectIdx, propertyId);
    if (!def)
        return false; // not found

    if (!(def->control & PC_WRITABLE))
    {
        DB_PROPERTIES(printObjectIdx(objectIdx); serial.println(" "); printPropertyID(propertyId); serial.println(" not writable!"););
        return false; // not writable
    }

    PropertyDataType type = def->type();
    byte* valuePtr = def->valuePointer(bcu);

    const byte* data = bcu->bus->telegram + 12;
    int state, len;

    if (type == PDT_CONTROL)
    {
        len = bcu->bus->telegramLen - 13;
        state = loadProperty(objectIdx, data, len);
        bcu->userEeprom->loadState()[objectIdx] = state;
        sendBuffer[12] = state;
        len = 1;
    }
    else
    {
        --start;
        int size = def->size();
        len = count * size;
        DB_PROPERTIES(serial.print("propertyValueWriteTelegram: "); printObjectIdx(objectIdx); serial.print(" "); printPropertyID(propertyId);serial.println(););
        reverseCopy(valuePtr + start * size, data, len);
        reverseCopy(sendBuffer + 12, valuePtr + start * size, len);
        if (def->isEepromPointer())
            bcu->userEeprom->modified(true);
    }

    sendBuffer[5] += len;
    return true;
}

bool PropertiesBCU2::propertyDescReadTelegram(int objectIdx, PropertyID propertyId, int index, uint8_t * sendBuffer)
{
    const PropertyDef* def;

    if (propertyId)
        def = propertyDef(objectIdx, propertyId);
    else def = &propertiesTab()[objectIdx][index];

    sendBuffer[10] = index;

    if (!def || !def->id)
    {
        sendBuffer[9] = propertyId;
        sendBuffer[11] = 0;
        sendBuffer[12] = 0;
        sendBuffer[13] = 0;
        sendBuffer[14] = 0;
        return false; // not found
    }

    int numElems;
    if ((def->control & PC_ARRAY_POINTER) == PC_ARRAY_POINTER)
        numElems = *def->valuePointer(bcu);
    else numElems = 1;

    sendBuffer[9] = def->id;
    sendBuffer[11] = def->control & (PC_TYPE_MASK | PC_WRITABLE);
    sendBuffer[12] = (numElems >> 8) & 15;
    sendBuffer[13] = numElems;
    sendBuffer[14] = def->control & PC_WRITABLE ? 0xf1 : 0x50; // wild guess from bus traces

    return true;
}


#ifdef DUMP_PROPERTIES

void PropertiesBCU2::printObjectIdx(int objectIdx)
{
    serial.print("objectIdx=0x", objectIdx, HEX, 2);
    if ((objectIdx >= OT_DEVICE) && (objectIdx <= OT_RF_MEDIUM))
    {
        serial.print(" OT_");
        serial.print(objectType_str[objectIdx]);
    }
    else
    {
        serial.print(" unknown");
    }
}

void PropertiesBCU2::printLoadState(int loadstate)
{
    serial.print("loadstate=0x", loadstate, HEX, 2);
    if ((loadstate >= LS_UNLOADED) && (loadstate <= LS_LOADCOMPLETING))
    {
        serial.print(" LS_");
        serial.print(loadState_str[loadstate]);
    }
    else
    {
        serial.print(" unknown");
    }
}

void PropertiesBCU2::printSegmentType(int segmenttype)
{
    serial.print("segmenttype=0x", segmenttype, HEX, 2);
    if ((segmenttype >= ST_ALLOC_ABS_DATA_SEG) && (segmenttype <= ST_DATA_RELATIVE_ALLOCATION))
    {
        serial.print(" ST_");
        serial.print(segmentType_str[segmenttype]);
    }
    else
    {
        serial.print(" unknown");
    }
}

void PropertiesBCU2::printPropertyID(int propertyid)
{
    serial.print("propertyid=0x", propertyid, HEX, 2);
    if ((propertyid >= PID_OBJECT_TYPE) && (propertyid <= PID_CHANNEL_32_PARAM))
    {
        serial.print(" PID_");
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

void PropertiesBCU2::printData(const byte* data, int len)
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
