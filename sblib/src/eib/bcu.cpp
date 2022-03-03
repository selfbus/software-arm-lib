/*
 *  bcu.cpp - EIB bus coupling unit.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#define INSIDE_BCU_CPP
#include <sblib/eib/bcu.h>
#include <sblib/eib/knx_tpdu.h>
#include <sblib/eib/apci.h>
#include <sblib/internal/functions.h>
#include <sblib/eib/com_objects.h>
#include <sblib/internal/variables.h>
#include <sblib/mem_mapper.h>

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
extern volatile unsigned int systemTime;

void BCU::_begin()
{
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
bool BCU::processGroupAddressTelegram(unsigned char *telegram, unsigned short telLength)
{
    unsigned short destAddr = (telegram[3] << 8) | telegram[4];
    unsigned short apci = ((telegram[6] & 3) << 8) | telegram[7];

    DB_COM_OBJ(
        serial.println();
        serial.print("BCU1 grp addr: 0x", (unsigned int)destAddr, HEX, 4);
        serial.println(" error state:  0x",(unsigned int)bus.receivedTelegramState(), HEX, 4);
    );

    processGroupTelegram(destAddr, apci & APCI_GROUP_MASK, telegram);
    return (true);
}

bool BCU::processBroadCastTelegram(unsigned char *telegram, unsigned short telLength)
{
    unsigned short apci = ((telegram[6] & 3) << 8) | telegram[7];
    if (programmingMode()) // we are in programming mode
    {
        if (apci == APCI_INDIVIDUAL_ADDRESS_WRITE_PDU)
        {
            setOwnAddress((telegram[8] << 8) | telegram[9]);
        }
        else if (apci == APCI_INDIVIDUAL_ADDRESS_READ_PDU)
        {
            sendTelegram[0] = 0xb0 | (telegram[0] & 0x0c); // Control byte
            // 1+2 contain the sender address, which is set by bus.sendTelegram()
            sendTelegram[3] = 0x00;  // Zero target address, it's a broadcast
            sendTelegram[4] = 0x00;
            sendTelegram[5] = 0xe1;
            sendTelegram[6] = 0x01;  // APCI_INDIVIDUAL_ADDRESS_RESPONSE_PDU (0x0140)
            sendTelegram[7] = 0x40;

            // on a productive bus without this delay, a lot of "more than one device is in programming mode" errors can occur
            // 03.03.2022 tested 10 different licensed KNX-devices (MDT, Gira, Siemens, Merten), only one MDT line-coupler had also this problem
            // other devices responded with a delay of 8-40 milliseconds, so we go for ~5ms + 2*4ms = ~13ms
            // 4.000usec is max. we can delay with delayMicroseconds, so call it twice
            delayMicroseconds(MAX_DELAY_MICROSECONDS);
            delayMicroseconds(MAX_DELAY_MICROSECONDS);
            bus.sendTelegram(sendTelegram, 8);
        }
    }
    return (true);
}

bool BCU::processDeviceDescriptorReadTelegram(int id)
{
    if (id == 0)
    {
        unsigned short version = maskVersion();
        sendTelegram[5] = (bus.telegram[5] & 0xf0) | 0x03; // routing count in high nibble, length in low nibble already set from caller
        sendTelegram[6] = 0x43;
        sendTelegram[7] = 0x40;
        sendTelegram[8] = version >> 8;
        sendTelegram[9] = version;
        return (true);
    }
    return (false); // unknown device descriptor
}

static void cpyToUserRam(unsigned int address, unsigned char * buffer, unsigned int count)
{
    address -= getUserRamStart();
    if ((address > 0x60) || ((address + count) < 0x60))
    {
        memcpy(userRamData + address, buffer, count);
    }
    else
    {
        while (count--)
        {
            if (address == 0x60)
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
    address -= getUserRamStart();
    if ((address > 0x60) || ((address + count) < 0x60))
    {
        memcpy(buffer, userRamData + address, count);
    }
    else
    {
        while (count--)
        {
            if (address == 0x60)
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
            if ((addressStart >= USER_EEPROM_START) &&  (addressEnd < USER_EEPROM_END))
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
            else if ((addressStart >= USER_EEPROM_START) && (addressStart < USER_EEPROM_END))
            {
                // start is in USER_EEPROM, but payLoad is too long, we need to cut it down to fit
                const int copyCount = USER_EEPROM_END - addressStart;
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
            if ((addressStart >= getUserRamStart()) && (addressEnd <= getUserRamEnd()))
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
            userEeprom.loadState[objectIdx] = loadProperty(objectIdx, &payLoad[0], lengthPayLoad);
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

unsigned char BCU::processApci(int apci, const int senderAddr, const int senderSeqNo, bool *sendResponse, unsigned char *telegram, unsigned short telLength)
{
    int count;
    int address;
    int index;
#if BCU_TYPE != BCU1_TYPE
    bool found;
    int id;
#endif
    unsigned char sendAckTpu = 0;
    *sendResponse = false;

    sendTelegram[6] = 0;

    int apciCommand = apci & APCI_GROUP_MASK;
    switch (apciCommand)
    {
    case APCI_ADC_READ_PDU: ///\todo implement ADC service for bus voltage and PEI,
                            //!> Estimation of the current bus via the AD-converter channel 1 and the AdcRead-service.
                            //!  The value read can be converted to a voltage value by using the following formula: Voltage = ADC_Value * 0,15V
        index = bus.telegram[7] & 0x3f;  // ADC channel
        count = bus.telegram[8];         // number of samples
        sendTelegram[5] = 0x64;
        sendTelegram[6] = 0x41;
        sendTelegram[7] = 0xc0 | (index & 0x3f);   // channel
        sendTelegram[8] = count;                   // read count
        sendTelegram[9] = 0;                       // FIXME dummy - ADC value high byte
        sendTelegram[10] = 0;                      // FIXME dummy - ADC value low byte
        *sendResponse = true;
        break;

    case APCI_MEMORY_READ_PDU:
    case APCI_MEMORY_WRITE_PDU:
        count = bus.telegram[7] & 0x0f; // number of data bytes
        address = (bus.telegram[8] << 8) | bus.telegram[9]; // address of the data block

        if (apciCommand == APCI_MEMORY_WRITE_PDU)
        {
            if (processApciMemoryWritePDU(address, &bus.telegram[10], count))
            {
#if BCU_TYPE != BCU1_TYPE
                if (userRam.deviceControl & DEVCTRL_MEM_AUTO_RESPONSE)
                {
                    // only on successful write
                    apciCommand = APCI_MEMORY_READ_PDU;
                }
#endif
            }
            sendAckTpu = T_ACK_PDU;
        }

        if (apciCommand == APCI_MEMORY_READ_PDU)
        {
            if (!processApciMemoryReadPDU(address, &sendTelegram[10], count))
            {
                // address space unreachable, need to respond with count 0
                count = 0;
            }

            // send a APCI_MEMORY_RESPONSE_PDU response
            sendTelegram[5] = 0x63 + count;
            sendTelegram[6] = 0x42;
            sendTelegram[7] = 0x40 | count;
            sendTelegram[8] = address >> 8;
            sendTelegram[9] = address;
            *sendResponse = true;
        }
        break;

    case APCI_DEVICEDESCRIPTOR_READ_PDU:
        if (processDeviceDescriptorReadTelegram(apci & 0x3f))
        {
            *sendResponse = true;
        }
        else
        {
            sendAckTpu = T_NACK_PDU; ///\todo this needs a check. On TL4 Style 1 rationalised only T_ACK is allowed
        }
        break;

    default:
        switch (apci)
        {
        case APCI_BASIC_RESTART_PDU:
            softSystemReset();
            break; // we should never land on this break

        case APCI_MASTER_RESET_PDU:
            if (processApciMasterResetPDU(apci, senderSeqNo, bus.telegram[8], bus.telegram[9]))
            {
                softSystemReset(); // perform a basic restart;
            }
            // APCI_MASTER_RESET_PDU was not processed successfully send prepared response telegram
            *sendResponse = true;
            break;

        case APCI_AUTHORIZE_REQUEST_PDU:
            sendTelegram[5] = 0x62;
            sendTelegram[6] = 0x43;
            sendTelegram[7] = 0xd2;
            sendTelegram[8] = 0x00;
            *sendResponse = true;
            break;

#if BCU_TYPE != BCU1_TYPE
        case APCI_PROPERTY_VALUE_READ_PDU:
        case APCI_PROPERTY_VALUE_WRITE_PDU:
            sendTelegram[5] = 0x65;
            sendTelegram[6] = 0x40 | (APCI_PROPERTY_VALUE_RESPONSE_PDU >> 8);
            sendTelegram[7] = APCI_PROPERTY_VALUE_RESPONSE_PDU & 0xff;
            index = sendTelegram[8] = bus.telegram[8];
            id = sendTelegram[9] = bus.telegram[9];
            count = (sendTelegram[10] = bus.telegram[10]) >> 4;
            address = ((bus.telegram[10] & 15) << 4) | (sendTelegram[11] = bus.telegram[11]);

            if (apci == APCI_PROPERTY_VALUE_READ_PDU)
                found = propertyValueReadTelegram(index, (PropertyID) id, count, address);
            else found = propertyValueWriteTelegram(index, (PropertyID) id, count, address);
            if (!found) sendTelegram[10] = 0;
            *sendResponse = true;
            break;

        case APCI_PROPERTY_DESCRIPTION_READ_PDU:
            sendTelegram[5] = 0x68;
            sendTelegram[6] = 0x64 | (APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU >> 8);
            sendTelegram[7] = APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU & 0xff;
            index = sendTelegram[8] = bus.telegram[8];
            id = sendTelegram[9] = bus.telegram[9];
            address = (sendTelegram[10] = bus.telegram[10]);
            propertyDescReadTelegram(index, (PropertyID) id, address);
            *sendResponse = true;
            break;
#endif /*BCU_TYPE != BCU1_TYPE*/

        default:
            ///\todo this needs a check. On TL4 Style 1 rationalised only T_ACK is allowed
            sendAckTpu = T_NACK_PDU;  // Command not supported
            break;
        }
        break;
    }
    return (sendAckTpu);
}

bool BCU::processApciMasterResetPDU(int apci, const int senderSeqNo, byte eraseCode, byte channelNumber)
{
    ///\todo the following code has been hacked together quick and dirty.
    ///      It needs a rework along with the redesign of processDirectTelegram(..)
    // create the APCI_MASTER_RESET_RESPONSE_PDU
    int senderAddress = connectedTo();
    sendTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    sendTelegram[3] = senderAddress >> 8;
    sendTelegram[4] = senderAddress;
    sendTelegram[5] = 0x64; // length of the telegram is 4 bytes
    sendTelegram[6] = 0x40 | (APCI_MASTER_RESET_RESPONSE_PDU >> 8); // set first byte of apci
    sendTelegram[7] = APCI_MASTER_RESET_RESPONSE_PDU & 0xff; // set second byte of apci
    sendTelegram[7] |= 1; // set restart type to 1

    sendTelegram[8] = T_RESTART_UNSUPPORTED_ERASE_CODE; // set no error response
    // restart process time 2 byte unsigned integer value expressed in seconds
    // DPT_TimePeriodSec / DPT7.005
    sendTelegram[9] = 0; ///\todo set proper restart process time
    sendTelegram[10] = 6; ///\todo set proper restart process time

    if (apci != APCI_MASTER_RESET_PDU)
    {
        return false;
    }
    ///\todo implement proper handling of APCI_MASTER_RESET_PDU for all other Erase Codes

    if (checkApciForMagicWord(apci, eraseCode, channelNumber))
    {
        // special version of APCI_MASTER_RESET_PDU used by Selfbus bootloader
        // set magicWord to start after reset in bootloader mode
        unsigned int * magicWord = BOOTLOADER_MAGIC_ADDRESS;
        *magicWord = BOOTLOADER_MAGIC_WORD;

        // Add the sequence number
        sendTelegram[6] &= ~0x3c;
        sendTelegram[6] |= sequenceNumberSend();
        // set no error
        sendTelegram[8] = T_RESTART_NO_ERROR;

        // send transport layer 4 ACK
        sendConControlTelegram(T_ACK_PDU, senderAddress, senderSeqNo);
        while (!bus.idle())
            ;
        // send APCI_MASTER_RESET_RESPONSE_PDU
        bus.sendTelegram(sendTelegram, telegramSize(sendTelegram));
        while (!bus.idle())
                    ;
        // send disconnect
        sendConControlTelegram(T_DISCONNECT_PDU, senderAddress, 0);
        while (!bus.idle())
            ;
        softSystemReset();
    }

    return false;
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
