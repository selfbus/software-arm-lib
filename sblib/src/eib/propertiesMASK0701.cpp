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

#include <sblib/eib/propertiesMASK0701.h>

#include <sblib/eib/mask0701.h>

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
LoadState PropertiesMASK0701::handleAllocAbsDataSegment(const int objectIdx, const byte* payLoad, const int len)
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
LoadState PropertiesMASK0701::handleAllocAbsTaskSegment(const int objectIdx, const byte* payLoad, const int len)
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
            serial.flush();
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
			// we need this newAddress workaround, see comment @void BcuBase::begin(...) in bcu_base.h
			word newAddress = bcu->getCommObjectTableAddressStatic();
			if (newAddress == 0) // set newAddress, in case bcu doesn't provide a read-only address
			{
				newAddress = addr;
			}

			bcu->userEeprom->commsTabAddr() = newAddress;

			if (bcu->userEeprom->commsTabAddr() != addr)
			    DB_PROPERTIES(serial.println();serial.println("  ----> userEeprom->commsTabAddr MARKED AS READ-ONLY, WON'T CHANGE TO 0x", addr, HEX, 4);serial.println(););

			DB_PROPERTIES(serial.println("  ----> userEeprom->commsTabAddr = 0x", bcu->userEeprom->commsTabAddr(), HEX, 4););
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
