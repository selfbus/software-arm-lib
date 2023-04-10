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

#include <string.h> // for memcpy in propertyValueReadTelegram()

#include <sblib/eib/propertiesSYSTEMB.h>
#include <sblib/eib/systemb.h>
#include <sblib/eib/bus.h>

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
LoadState PropertiesSYSTEMB::handleAllocAbsDataSegment(const int objectIdx, const byte* payLoad, const int len)
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
LoadState PropertiesSYSTEMB::handleDataRelativeAllocation(const int objectIdx, const byte* payLoad, const int len)
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

    unsigned int reqMemSize = ((payLoad[0] << 24) | (payLoad[1] << 16) | (payLoad[2] << 8) | payLoad[3]);
    word*    tableAddress[] = {&bcu->userEeprom->addrTabAddr(), &bcu->userEeprom->assocTabAddr(), &bcu->userEeprom->commsTabAddr(),
	                       &bcu->userEeprom->eibObjAddr(), &bcu->userEeprom->commsSeg0Addr()};

    UserEepromSYSTEMB* userEeprom = (UserEepromSYSTEMB*) bcu->userEeprom;

    byte* tableSize[] = {&userEeprom->addrTabMcb()[0], &userEeprom->assocTabMcb()[0], &userEeprom->commsTabMcb()[0],
                         &userEeprom->eibObjMcb()[0], &userEeprom->commsSeg0Mcb()[0]};

    word virtMemAddr = 0x3A9E; ///\todo get rid of magic number USER_EEPROM_START + USER_EEPROM_SIZE
    for (int i = 0; i < 5; i++)
    {
        if ((*tableAddress[i] != 0) && (*tableAddress[i] < virtMemAddr))
        virtMemAddr = *tableAddress[i];
    }
    *tableAddress[objectIdx -1] = virtMemAddr - reqMemSize;
    byte* tabSiz = tableSize[objectIdx -1];
    tabSiz[0] = payLoad[0];
    tabSiz[1] = payLoad[1];
    tabSiz[2] = payLoad[2];
    tabSiz[3] = payLoad[3];

    // Mode = 0x01 => fill allocated memory
    if (payLoad[4] > 0)
    {
        byte* physMemAddr = (byte*)(bcu->userEeprom->userEepromData + (virtMemAddr - bcu->userEeprom->startAddr()));
        byte fillByte = payLoad[5];
        for (uint32_t i = 0; i < reqMemSize; i++)
            physMemAddr[i] = fillByte;
    }

    return LS_LOADING;
}

uint16_t PropertiesSYSTEMB::crc16(uint8_t* ptr, int len)
{
  //int len = 9;
  //uint8_t data[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,  0x39};
  //uint8_t* ptr = &data[0];
    char i;
    uint16_t crc = 0x1D0F;
    while (--len >= 0)
    {
       crc = crc ^ (uint16_t) *ptr++ << 8;
       i = 8;
       do
       {
          if (crc & 0x8000)
             crc = crc << 1 ^ 0x1021;
          else
             crc = crc << 1;
       } while(--i);
    }
  return crc;
}

int PropertiesSYSTEMB::loadProperty(int objectIdx, const byte* data, int len)
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

    // When memory load is complete, calculate the crc 16 and store it in the mcb
    if (newLoadState == LS_LOADED)
    {
        const PropertyDef* def = propertyDef(objectIdx, PID_TABLE_REFERENCE);
        byte* valuePtr = def->valuePointer(bcu);
        uint16_t virtMemStart = ((valuePtr[1] << 8) + valuePtr[0]);
        byte* memStart = (byte*)(bcu->userEeprom->userEepromData + (virtMemStart - bcu->userEeprom->startAddr()));

        def = propertyDef(objectIdx, PID_MCB_TABLE);
        byte* mcbPtr = def->valuePointer(bcu);
        uint16_t memSize = (mcbPtr[2] << 8) + mcbPtr[3];

        uint16_t crc = crc16(memStart, memSize);
        mcbPtr[6] = (byte)(crc >> 8);
        mcbPtr[7] = (byte)crc;

        return newLoadState;
    }

    if (newLoadState == LS_UNLOADED)
    {
        // clear table reference address on unload
        const PropertyDef* def = propertyDef(objectIdx, PID_TABLE_REFERENCE);
        byte* valuePtr = def->valuePointer(bcu);
        valuePtr[1] = 0;
        valuePtr[0] = 0;
        return newLoadState;
    }

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

        case ST_RELATIVE_ALLOCATION: // relative allocation
            return handleRelativeAllocation(objectIdx, payload, len);

        case ST_DATA_RELATIVE_ALLOCATION: // data relative allocation (ignored)
            return handleDataRelativeAllocation(objectIdx, payload, len);

        default:
            DB_PROPERTIES(serial.println(" FATAL ERROR."););
            IF_DEBUG(fatalError());
            return LS_ERROR; // reply: Error
    }
    return LS_LOADING;
}

bool PropertiesSYSTEMB::propertyValueReadTelegram(int objectIdx, PropertyID propertyId, int count, int start, uint8_t * sendBuffer)
{
    // IF_DUMP_PROPERTIES(serial.print("propertyValueReadTelegram: "); printObjectIdx(objectIdx); serial.print(" "); printPropertyID(propertyId);serial.println(););
    const PropertyDef* def = propertyDef(objectIdx, propertyId);
    if (!def) return false; // not found

    PropertyDataType type = (PropertyDataType) (def->control & PC_TYPE_MASK);
    byte* valuePtr = def->valuePointer(bcu);

    --start;
    int size = def->size();
    int len = count * size;
    if(len > 12) return false; // length error

    if (type < PDT_CHAR_BLOCK)
    {
        if (propertyId == PID_TABLE_REFERENCE)
        {
            sendBuffer[12] = 0;
            sendBuffer[13] = 0;
            reverseCopy(sendBuffer + 14, valuePtr + start * size, len);
            len += 2;
        }
        else
            reverseCopy(sendBuffer + 12, valuePtr + start * size, len);
    }
    else memcpy(sendBuffer + 12, valuePtr + start * size, len);

    sendBuffer[5] += len;

    return true;
}

bool PropertiesSYSTEMB::propertyValueWriteTelegram(int objectIdx, PropertyID propertyId, int count, int start, uint8_t * sendBuffer)
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
        if (propertyId == PID_MCB_TABLE)
        {
            memcpy(valuePtr + start * size, data, len);
            memcpy(sendBuffer + 12, valuePtr + start * size, len);
        }
        else
        {
            reverseCopy(valuePtr + start * size, data, len);
            reverseCopy(sendBuffer + 12, valuePtr + start * size, len);
        }
        if (def->isEepromPointer())
            bcu->userEeprom->modified(true);
    }

    sendBuffer[5] += len;
    return true;
}
