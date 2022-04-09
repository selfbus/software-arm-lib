/*
 *  bcu.cpp - replaced by classes BCU1, BCU2, MASK0701, MASK0705, SYSTEMB
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

// replaced by classes BCU1, BCU2, MASK0701, MASK0705, SYSTEMB
#if 0
#define INSIDE_BCU_CPP
#include <sblib/eib/bcu.h>
#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/knx_tpdu.h>
#include <sblib/eib/apci.h>
#include <sblib/eib/com_objects.h>
#include <sblib/internal/variables.h>
#include <sblib/mem_mapper.h>
#include <sblib/utils.h>

#include <string.h>

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

// Enable informational debug statements
#if defined(DUMP_MEM_OPS)
#   define DB_MEM_OPS(x) x
#else
#   define DB_MEM_OPS(x)
#endif

#if defined(DUMP_PROPERTIES)
#   define DB_PROPERTIES(x) x
#else
#   define DB_PROPERTIES(x)
#endif

#if defined(DUMP_COM_OBJ)
#   define DB_COM_OBJ(x) x
#else
#   define DB_COM_OBJ(x)
#endif

#if !defined(INCLUDE_SERIAL)
#   define DB_MEMP_OPS(x)
#   define DB_PROPERTIES(x)
#   define DB_COM_OBJ(x)
#endif

extern volatile unsigned int writeUserEepromTime;

void BCU::_begin()
{
    BcuBase::_begin();
    readUserEeprom();
    sendGrpTelEnabled = true;
    groupTelSent = millis();

    // set limit to max of 28 telegrams per second (wait 35ms) -  to avoid risk of thermal destruction of the sending circuit
    groupTelWaitMillis = DEFAULT_GROUP_TEL_WAIT_MILLIS ;
}

void BCU::end()
{
    if (usrCallback)
        usrCallback->Notify(USR_CALLBACK_BCU_END);
    BcuBase::end();
    writeUserEeprom();
    if (memMapper)
    {
        memMapper->doFlash();
    }
}


/**
 *  main loop for Receiving and transmitting and receiving telegrams toward the interrupt driven bus/physical layer
 *
 *  todo mapp sending results to the objects error state
 *
 */
void BCU::loop()
{
    if (!enabled)
        return;
    BcuBase::loop();

    // if bus is not sending (telegramm buffer is empty) check for next telegram to be send
    if (sendGrpTelEnabled && !bus.sendingTelegram())
    {
        // Send group telegram if group telegram rate limit not exceeded
        if (elapsed(groupTelSent) >= groupTelWaitMillis)
        {
        	// check in com_objects method for waiting objects from user app and store in telegrambuffer
        	// and call bus.sendTelegram
         if (sendNextGroupTelegram())
             groupTelSent = millis();

        }
        // To prevent overflows if no telegrams are sent for a long time - todo: better reload with systemTime - groupTelWaitMillis
        if (elapsed(groupTelSent) >= 2000)
        {
            groupTelSent += 1000;
        }
    }

    if (userEeprom.isModified() && bus.idle() && bus.telegramLen == 0 && connectedTo() == 0)
    {
        if (writeUserEepromTime)
        {
            if ((int)millis() - (int)writeUserEepromTime > 0)
            {
                if (usrCallback)
                    usrCallback->Notify(USR_CALLBACK_FLASH);
                writeUserEeprom();
                if (memMapper)
                {
                    memMapper->doFlash();
                }
            }
        }
        else writeUserEepromTime = millis() + 50;
    }
}

/**
 * todo check for RX status and inform upper layer if needed
 */
bool BCU::processGroupAddressTelegram(ApciCommand apciCmd, uint16_t groupAddress, unsigned char *telegram, uint8_t telLength)
{
    DB_COM_OBJ(
        serial.println();
        serial.print("BCU1 grp addr: 0x", (unsigned int)destAddr, HEX, 4);
        serial.println(" error state:  0x",(unsigned int)bus.receivedTelegramState(), HEX, 4);
    );

    processGroupTelegram(groupAddress, apciCmd & APCI_GROUP_MASK, telegram);
    return (true);
}

bool BCU::processBroadCastTelegram(ApciCommand apciCmd, unsigned char *telegram, uint8_t telLength)
{
    if (programmingMode()) // we are in programming mode
    {
        if (apciCmd == APCI_INDIVIDUAL_ADDRESS_WRITE_PDU)
        {
            setOwnAddress(MAKE_WORD(telegram[8], telegram[9]));
        }
        else if (apciCmd == APCI_INDIVIDUAL_ADDRESS_READ_PDU)
        {
            sendApciIndividualAddressReadResponse();
        }
    }
    return (true);
}

bool BCU::processDeviceDescriptorReadTelegram(int id)
{
    if (id != 0)
    {
        return (false); // unknown device descriptor
    }

    sendTelegram[5] = 0x60 + 3; // routing count in high nibble + response length in low nibble
    setApciCommand(sendTelegram, APCI_DEVICEDESCRIPTOR_RESPONSE_PDU, 0);
    sendTelegram[8] = HIGH_BYTE(getMaskVersion());
    sendTelegram[9] = LOW_BYTE(getMaskVersion());
    return (true);
}

static void BCU::cpyToUserRam(unsigned int address, unsigned char * buffer, unsigned int count)
{
    if ((address == USER_RAM_SYSTEM_STATE_ADDRESS) && (count == 1))
    {
        userRam.status = *buffer;
        return;
    }

    address -= getUserRamStart();
    if ((address > USER_RAM_SYSTEM_STATE_ADDRESS) || ((address + count) < USER_RAM_SYSTEM_STATE_ADDRESS))
    {
        memcpy(userRamData + address, buffer, count);
    }
    else
    {
        while (count--)
        {
            if (address == USER_RAM_SYSTEM_STATE_ADDRESS)
                userRam.status = * buffer;
            else
                userRamData[address] = * buffer;
            buffer++;
            address++;
        }
    }
}

static void cpyFromUserRam(unsigned int address, unsigned char * buffer, unsigned int count)
{
    if ((address == USER_RAM_SYSTEM_STATE_ADDRESS) && (count == 1))
    {
        *buffer = userRam.status;
        return;
    }

    address -= getUserRamStart();
    if ((address > USER_RAM_SYSTEM_STATE_ADDRESS) || ((address + count) < USER_RAM_SYSTEM_STATE_ADDRESS))
    {
        memcpy(buffer, userRamData + address, count);
    }
    else
    {
        while (count--)
        {
            if (address == USER_RAM_SYSTEM_STATE_ADDRESS)
                * buffer = userRam.status;
            else
                * buffer = userRamData[address];
            buffer++;
            address++;
        }
    }
}

bool BCU::processApciMemoryOperation(int addressStart, byte *payLoad, int lengthPayLoad, const bool readMem)
{
    const int addressEnd = addressStart + lengthPayLoad - 1;
    bool startNotFound = false; // we need this as a exit condition, in case memory range is no where found

    if (lengthPayLoad == 0)
    {
        DB_MEM_OPS(serial.println(" Error processApciMemoryOperation : lengthPayLoad: 0x", lengthPayLoad , HEX, 2));
        return false;
    }

    while ((!startNotFound) && (addressStart <= addressEnd))
    {
        startNotFound = true;
        // check if we have a memMapper and if payLoad is handled by it
        if (memMapper != nullptr)
        {
            if (memMapper->isMappedRange(addressStart, addressEnd))
            {
                // start & end fit into memMapper
                bool operationResult = false;
                if (readMem)
                    operationResult = memMapper->readMemPtr(addressStart, &payLoad[0], lengthPayLoad) == MEM_MAPPER_SUCCESS;
                else
                    operationResult = memMapper->writeMemPtr(addressStart, &payLoad[0], lengthPayLoad) == MEM_MAPPER_SUCCESS;

                if (operationResult)
                {
                    DB_MEM_OPS(serial.println(" -> memmapped ", lengthPayLoad, DEC));
                    return true;
                }
                else
                    return false;
            }
            else if (memMapper->isMapped(addressStart))
            {
                // we only know that start is mapped so lets do memory operation byte by byte
                for (int i = 0; i < lengthPayLoad; i++)
                {
                    bool operationResult = memMapper->isMapped(addressStart);
                    if (operationResult)
                    {
                        if (readMem)
                            operationResult = memMapper->readMemPtr(addressStart, &payLoad[0], 1) == MEM_MAPPER_SUCCESS;
                        else
                            operationResult = memMapper->writeMemPtr(addressStart, &payLoad[0], 1) == MEM_MAPPER_SUCCESS;
                    }

                    if (operationResult)
                    {
                        // address is mapped and memory operation was successful
                        addressStart++;
                        payLoad++;
                        startNotFound = false;
                    }
                    else
                    {
                        DB_MEM_OPS(serial.println(" -> memmapped processed : ", i , DEC));
                        break;
                    }
                }
            }
        }

        // check if payLoad is in USER_EEPROM
        if (addressStart <= addressEnd)
        {
            if ((addressStart >= userEeprom->userEepromStart) &&  (addressEnd < userEeprom->userEepromEnd))
            {
                // start & end are in USER_EEPROM
                if (readMem)
                {
                    memcpy(&payLoad[0], &userEeprom[addressStart], addressEnd - addressStart + 1);
                }
                else
                {
                    memcpy(&userEeprom[addressStart], &payLoad[0], addressEnd - addressStart + 1);
                    userEeprom.modified();
                }
                DB_MEM_OPS(serial.println(" -> EEPROM ", addressEnd - addressStart + 1, DEC));
                return true;
            }
            else if ((addressStart >= userEeprom->userEepromStart) && (addressStart < userEeprom->userEepromEnd))
            {
                // start is in USER_EEPROM, but payLoad is too long, we need to cut it down to fit
                const int copyCount = userEeprom->userEepromEnd - addressStart;
                if (readMem)
                {
                    memcpy(&payLoad[0], &userEeprom[addressStart], copyCount);
                }
                else
                {
                    memcpy(&userEeprom[addressStart], &payLoad[0], copyCount);
                    userEeprom.modified();
                }
                addressStart += copyCount;
                payLoad += copyCount;
                startNotFound = false;
                DB_MEM_OPS(serial.println(" -> EEPROM processed: ", copyCount, DEC));
            }
        }

        // check if payLoad is in UserRam
        if (addressStart <= addressEnd)
        {
            if (((addressStart >= getUserRamStart()) && (addressEnd <= getUserRamEnd())) ||
                 ((addressStart == USER_RAM_SYSTEM_STATE_ADDRESS) && (addressEnd == USER_RAM_SYSTEM_STATE_ADDRESS))) // USER_RAM_STATUS_ADDRESS (0x60) is system state specific
            {
                // start & end are in UserRAM
                if (readMem)
                    cpyFromUserRam(addressStart, &payLoad[0], addressEnd - addressStart + 1);
                else
                    cpyToUserRam(addressStart, &payLoad[0], addressEnd - addressStart + 1);
                DB_MEM_OPS(serial.println(" -> UserRAM ", addressEnd - addressStart + 1, DEC));
                return true;
            }
            else if ((addressStart >= getUserRamStart()) && (addressStart <= getUserRamEnd()))
            {
                // start is in UserRAM, but payLoad is too long, we need to cut it down to fit
                const int copyCount = getUserRamEnd() - addressStart + 1;
                if (readMem)
                    cpyFromUserRam(addressStart, &payLoad[0], copyCount);
                else
                    cpyToUserRam(addressStart, &payLoad[0], copyCount);
                addressStart += copyCount;
                payLoad += copyCount;
                startNotFound = false;
                DB_MEM_OPS(serial.println(" -> UserRAM processed: ", copyCount, DEC));
            }
        }

        // ok, lets prepare for another try, maybe we find the remaining bytes in another memory
        lengthPayLoad = addressEnd - addressStart + 1;
        if (!startNotFound)
        {
            DB_MEM_OPS(
                (readMem) ? serial.print(" -> MemoryRead :", addressStart, HEX, 4) : serial.print(" -> MemoryWrite:", addressStart, HEX, 4);
                serial.print(" Data:");
                for(int i=0; i<lengthPayLoad ; i++)
                {
                    serial.print(" ", payLoad[i], HEX, 2);
                }
                serial.println(" count: ", lengthPayLoad, DEC);
            );
        }
    }

    DB_MEM_OPS(
       if (lengthPayLoad != 0)
       {
           serial.print(" not found start: 0x", addressStart, HEX, 4);
           serial.print(" end: 0x", addressEnd, HEX, 4);
           serial.println(" lengthPayLoad:", lengthPayLoad);
       }
    );

    return (lengthPayLoad == 0);
}

bool BCU::processApciMemoryWritePDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    DB_MEM_OPS(
       serial.print("ApciMemoryWritePDU:", addressStart, HEX, 4);
       serial.print(" Data:");
       for(int i=0; i<lengthPayLoad ; i++)
       {
           serial.print(" ", payLoad[i], HEX, 2);
       }
       serial.print(" count: ", lengthPayLoad, DEC);
    );

#ifdef LOAD_CONTROL_ADDR
    // special handling of DMP_LoadStateMachineWrite_RCo_Mem (APCI_MEMORY_WRITE_PDU)
    // See KNX Spec. 3/5/2 3.28.2 p.109 (deprecated)
    if (addressStart == LOAD_CONTROL_ADDR)
    {
        unsigned int objectIdx = payLoad[0] >> 4;
        DB_PROPERTIES(serial.println(" LOAD_CONTROL_ADDR: objectIdx:", objectIdx, HEX));
        if (objectIdx < INTERFACE_OBJECT_COUNT)
        {
            userEeprom.loadState[objectIdx] = (uint8_t)loadProperty(objectIdx, &payLoad[0], lengthPayLoad);
            userEeprom.modified();
            DB_PROPERTIES(serial.println());
            return true;
        }
        else
        {
            DB_PROPERTIES(serial.println(" not found"));
            return false;
        }
    }
#endif

    return processApciMemoryOperation(addressStart, payLoad, lengthPayLoad, false);
}

bool BCU::processApciMemoryReadPDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    DB_MEM_OPS(
       serial.print("ApciMemoryReadPDU :", addressStart, HEX, 4);
       serial.print(" count: ", lengthPayLoad, DEC);
    );

#ifdef LOAD_STATE_ADDR
    // special handling of DMP_LoadStateMachineRead_RCo_Mem (APCI_MEMORY_READ_PDU)
    // See KNX Spec. 3/5/2 3.30.2 p.121  (deprecated)
    if (addressStart >= LOAD_STATE_ADDR && addressStart < LOAD_STATE_ADDR + INTERFACE_OBJECT_COUNT)
    {
        memcpy(payLoad, userEeprom.loadState + (addressStart - LOAD_STATE_ADDR), lengthPayLoad);
        DB_PROPERTIES(serial.println(" LOAD_STATE_ADDR: ", addressStart, HEX));
        return true;
    }
#endif

    bool result = processApciMemoryOperation(addressStart, payLoad, lengthPayLoad, true);

    DB_MEM_OPS(
       if (result)
       {
           serial.print("           result :", addressStart, HEX, 4);
           serial.print(" Data:");
           for(int i=0; i<lengthPayLoad ; i++)
           {
               serial.print(" ", payLoad[i], HEX, 2);
           }
           serial.println(" count: ", lengthPayLoad, DEC);
       }
    );
    return result;
}

unsigned char BCU::processApci(ApciCommand apciCmd, const uint16_t senderAddr, const int8_t senderSeqNo, bool *sendResponse, unsigned char *telegram, uint8_t telLength)
{
    uint8_t count;
    uint16_t address;
    uint8_t index;
#if BCU_TYPE != BCU1_TYPE
    bool found;
    uint8_t id;
#endif
    unsigned char sendAckTpu = 0;
    *sendResponse = false;

    switch (apciCmd)
    {
    case APCI_ADC_READ_PDU: ///\todo implement ADC service for bus voltage and PEI,
                            //!> Estimation of the current bus via the AD-converter channel 1 and the AdcRead-service.
                            //!  The value read can be converted to a voltage value by using the following formula: Voltage = ADC_Value * 0,15V
        index = telegram[7] & 0x3f;  // ADC channel
        count = telegram[8];         // number of samples
        sendTelegram[5] = 0x60 + 4;  // routing count in high nibble + response length in low nibble
        setApciCommand(sendTelegram, APCI_ADC_RESPONSE_PDU, index);
        sendTelegram[8] = count;     // read count
        sendTelegram[9] = 0;         // ADC value high byte
        sendTelegram[10] = 0;        // ADC value low byte
        *sendResponse = true;
        break;

    case APCI_MEMORY_READ_PDU:
    case APCI_MEMORY_WRITE_PDU:
        count = telegram[7] & 0x0f; // number of data bytes
        address = MAKE_WORD(telegram[8], telegram[9]); // address of the data block

        if (apciCmd == APCI_MEMORY_WRITE_PDU)
        {
            if (processApciMemoryWritePDU(address, &telegram[10], count))
            {
#if BCU_TYPE != BCU1_TYPE
                if (userRam.deviceControl & DEVCTRL_MEM_AUTO_RESPONSE)
                {
                    // only on successful write
                    apciCmd = APCI_MEMORY_READ_PDU;
                }
#endif
            }
            sendAckTpu = T_ACK_PDU;
        }

        if (apciCmd == APCI_MEMORY_READ_PDU)
        {
            if (!processApciMemoryReadPDU(address, &sendTelegram[10], count))
            {
                // address space unreachable, need to respond with count 0
                count = 0;
            }

            // send a APCI_MEMORY_RESPONSE_PDU response
            sendTelegram[5] = 0x60 + count + 3; // routing count in high nibble + response length in low nibble
            setApciCommand(sendTelegram, APCI_MEMORY_RESPONSE_PDU, count);
            sendTelegram[8] = HIGH_BYTE(address);
            sendTelegram[9] = LOW_BYTE(address);
            *sendResponse = true;
        }
        break;

    case APCI_DEVICEDESCRIPTOR_READ_PDU:
        if (processDeviceDescriptorReadTelegram(telegram[7] & 0x3f))
        {
            *sendResponse = true;
        }
        else
        {
            sendAckTpu = T_NACK_PDU; // KNX Spec. 3/3/4 5.5.4 p.26 "TL4 Style 1 Rationalised" No Sending of T_NAK frames
        }
        break;

    case APCI_BASIC_RESTART_PDU:
        softSystemReset();
        break; // we should never land on this break

    case APCI_MASTER_RESET_PDU:
        if (processApciMasterResetPDU(telegram, senderSeqNo, telegram[8], telegram[9]))
        {
            softSystemReset(); // perform a basic restart;
        }
        // APCI_MASTER_RESET_PDU was not processed successfully send prepared response telegram
        *sendResponse = true;
        break;

    case APCI_AUTHORIZE_REQUEST_PDU:
        sendTelegram[5] = 0x60 + 2; // routing count in high nibble + response length in low nibble
        setApciCommand(sendTelegram, APCI_AUTHORIZE_RESPONSE_PDU, 0);
        sendTelegram[8] = 0x00;
        *sendResponse = true;
        break;

#if BCU_TYPE != BCU1_TYPE
    case APCI_PROPERTY_VALUE_READ_PDU:
    case APCI_PROPERTY_VALUE_WRITE_PDU:
        index = telegram[8];
        id = telegram[9];
        count = telegram[10] >> 4;
        address = ((telegram[10] & 0x0f) << 4);
        address = (uint16_t)(address | (telegram[11]));

        sendTelegram[5] = 0x60 + 5; // routing count in high nibble + response length in low nibble
        setApciCommand(sendTelegram, APCI_PROPERTY_VALUE_RESPONSE_PDU, 0);
        sendTelegram[8] = index;
        sendTelegram[9] = id;
        sendTelegram[10] = telegram[10];
        sendTelegram[11] = telegram[11];

        if (apciCmd == APCI_PROPERTY_VALUE_READ_PDU)
            found = propertyValueReadTelegram(index, (PropertyID) id, count, address);
        else
            found = propertyValueWriteTelegram(index, (PropertyID) id, count, address);
        if (!found) sendTelegram[10] = 0;
        *sendResponse = true;
        break;

    case APCI_PROPERTY_DESCRIPTION_READ_PDU:
        index = telegram[8];
        id = telegram[9];
        address = telegram[10];
        sendTelegram[5] = 0x60 + 8; // routing count in high nibble + response length in low nibble
        setApciCommand(sendTelegram, APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU, 0);
        sendTelegram[8] = index;
        sendTelegram[9] = id;
        sendTelegram[10] = (uint8_t)address;
        propertyDescReadTelegram(index, (PropertyID) id, address);
        *sendResponse = true;
        break;
#endif /*BCU_TYPE != BCU1_TYPE*/

    default:
        sendAckTpu = T_NACK_PDU;  // Command not supported, KNX Spec. 3/3/4 5.5.4 p.26 "TL4 Style 1 Rationalised" No Sending of T_NAK frames
        break;
    }
    return (sendAckTpu);
}

bool BCU::processApciMasterResetPDU(unsigned char *telegram, const uint8_t senderSeqNo, uint8_t eraseCode, uint8_t channelNumber)
{
    if (!checkApciForMagicWord(eraseCode, channelNumber))
    {
        ///\todo implement proper handling of APCI_MASTER_RESET_PDU for all other Erase Codes
        return (false);
    }

    // create the APCI_MASTER_RESET_RESPONSE_PDU
    initLpdu(sendTelegram, priority(telegram), false, FRAME_STANDARD);
    // sender address will be set by bus.sendTelegram()
    setDestinationAddress(sendTelegram, connectedTo());

    sendTelegram[5] = 0x60 + 4;  // routing count in high nibble + response length in low nibble
    setApciCommand(sendTelegram, APCI_MASTER_RESET_RESPONSE_PDU, 0);
    setSequenceNumber(sendTelegram, sequenceNumberSend());
    sendTelegram[8] = T_RESTART_NO_ERROR;
    sendTelegram[9] = 0; // restart process time 2 byte unsigned integer value expressed in seconds, DPT_TimePeriodSec / DPT7.005
    sendTelegram[10] = 1; // 1 second

    // special version of APCI_MASTER_RESET_PDU used by Selfbus bootloader
    // set magicWord to start after reset in bootloader mode
#ifndef IAP_EMULATION
    unsigned int * magicWord = BOOTLOADER_MAGIC_ADDRESS;
    *magicWord = BOOTLOADER_MAGIC_WORD;

    // send transport layer 4 ACK
    sendConControlTelegram(T_ACK_PDU, connectedTo(), senderSeqNo);
    while (!bus.idle())
        ;
    // send APCI_MASTER_RESET_RESPONSE_PDU
    bus.sendTelegram(sendTelegram, telegramSize(sendTelegram));
    while (!bus.idle())
                ;
    // send disconnect
    sendConControlTelegram(T_DISCONNECT_PDU, connectedTo(), 0);
    while (!bus.idle())
        ;
    softSystemReset();
#endif
    return (true);
}

void BCU::softSystemReset()
{
    if (usrCallback)
    {
        usrCallback->Notify(USR_CALLBACK_RESET);
    }

    writeUserEeprom(); // Flush the EEPROM before resetting

    if (memMapper)
    {
        memMapper->doFlash();
    }
    NVIC_SystemReset();
}
#endif
