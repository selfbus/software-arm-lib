/*
 *  bcu.h - EIB bus coupling unit.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  last change: 10. April 2021 HoRa:
 *  	call to bus:setSendAck() in "connect/disconnect command at Layer4" removed- not needed, will lead to undefined effects when the
 *  	bus state machine is sending an ack in parallel due to asynchronous interrupt process
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#define INSIDE_BCU_CPP
#include <sblib/eib/bcu.h>
#include <sblib/eib/apci.h>
#include <sblib/internal/functions.h>
#include <sblib/eib/com_objects.h>
#include <string.h>
#include <sblib/internal/variables.h>
#include <sblib/mem_mapper.h>

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

// Enable informational debug statements
#if defined(INCLUDE_SERIAL)
#   define DB(x) x
#else
#   define DB(x)
#endif

extern unsigned int writeUserEepromTime;
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

    // Send a disconnect after 6 seconds inactivity
    if (connectedAddr && elapsed(connectedTime) > 6000)
    {
        sendConControlTelegram(T_DISCONNECT_PDU, 0);
        connectedAddr = 0;
    }

    if (userEeprom.isModified() && bus.idle() && bus.telegramLen == 0 && connectedAddr == 0)
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

void BCU::sendConControlTelegram(int cmd, int senderSeqNo)
{
    if (cmd & 0x40)  // Add the sequence number if the command shall contain it
        cmd |= senderSeqNo & 0x3c;

    sendCtrlTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    sendCtrlTelegram[3] = connectedAddr >> 8;
    sendCtrlTelegram[4] = connectedAddr;
    sendCtrlTelegram[5] = 0x60;
    sendCtrlTelegram[6] = cmd;

    bus.sendTelegram(sendCtrlTelegram, 7);
}


/**
 * BUS process has receive a telegram - now process it
 *
 * called from BCUBase-loop
 *
 * todo check for RX status and inform upper layer if needed
 *
 */
void BCU::processTelegram()
{
    unsigned short destAddr = (bus.telegram[3] << 8) | bus.telegram[4];
    unsigned char tpci = bus.telegram[6] & 0xc3; // Transport control field (see KNX 3/3/4 p.6 TPDU)
    unsigned short apci = ((bus.telegram[6] & 3) << 8) | bus.telegram[7];

    DB(serial.println());
	DB(serial.print("BCU1 grp addr: ");)
	DB(serial.print((unsigned int)destAddr, HEX, 4);)
	DB(serial.print(" error state:  ");)
	DB(serial.println((unsigned int)bus.receivedTelegramState(), HEX, 4);)

    if (destAddr == 0) // a broadcast
    {
        if (programmingMode()) // we are in programming mode
        {
            if (apci == APCI_INDIVIDUAL_ADDRESS_WRITE_PDU)
            {
                setOwnAddress((bus.telegram[8] << 8) | bus.telegram[9]);
            }
            else if (apci == APCI_INDIVIDUAL_ADDRESS_READ_PDU)
            {
                sendTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
                // 1+2 contain the sender address, which is set by bus.sendTelegram()
                sendTelegram[3] = 0x00;  // Zero target address, it's a broadcast
                sendTelegram[4] = 0x00;
                sendTelegram[5] = 0xe1;
                sendTelegram[6] = 0x01;  // APCI_INDIVIDUAL_ADDRESS_RESPONSE_PDU (0x0140)
                sendTelegram[7] = 0x40;
                bus.sendTelegram(sendTelegram, 8);
            }
        }
    }
    else if ((bus.telegram[5] & 0x80) == 0) // a physical destination address
    {
        if (destAddr == bus.ownAddress()) // it's our physical address
        {
            if (tpci & 0x80)  // A connection control command
            {
                processConControlTelegram(bus.telegram[6]);
            }
            else
            {
                processDirectTelegram(apci);
            }
        }
    }
    else if (tpci == T_GROUP_PDU) // a group destination address and multicast
    {
        processGroupTelegram(destAddr, apci & APCI_GROUP_MASK, bus.telegram);
    }

    // At the end: discard the received telegram
    bus.discardReceivedTelegram();
}

bool BCU::processDeviceDescriptorReadTelegram(int id)
{
    if (id == 0)
    {
        int version = maskVersion();

        sendTelegram[5] = 0x63;
        sendTelegram[6] = 0x43;
        sendTelegram[7] = 0x40;
        sendTelegram[8] = version >> 8;
        sendTelegram[9] = version;
        return true;
    }

    return false; // unknown device descriptor
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
        DB(serial.println(" Error processApciMemoryOperation : lengthPayLoad: 0x", lengthPayLoad , HEX, 2));
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
                    DB(serial.println(" -> memmapped ", lengthPayLoad, DEC));
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
                        DB(serial.println(" -> memmapped processed : ", i , DEC));
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
                    memcpy(&payLoad[0], userEepromData + (addressStart - USER_EEPROM_START), addressEnd - addressStart + 1);
                }
                else
                {
                    memcpy(userEepromData + (addressStart - USER_EEPROM_START), &payLoad[0], addressEnd - addressStart + 1);
                    userEeprom.modified();
                }
                DB(serial.println(" -> EEPROM ", addressEnd - addressStart + 1, DEC));
                return true;
            }
            else if ((addressStart >= USER_EEPROM_START) && (addressStart < USER_EEPROM_END))
            {
                // start is in USER_EEPROM, but payLoad is too long, we need to cut it down to fit
                const int copyCount = USER_EEPROM_END - addressStart;
                if (readMem)
                {
                    memcpy(&payLoad[0], userEepromData + (addressStart - USER_EEPROM_START), copyCount);
                }
                else
                {
                    memcpy(userEepromData + (addressStart - USER_EEPROM_START), &payLoad[0], copyCount);
                    userEeprom.modified();
                }
                addressStart += copyCount;
                payLoad += copyCount;
                startNotFound = false;
                DB(serial.println(" -> EEPROM processed: ", copyCount, DEC));
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
                DB(serial.println(" -> UserRAM ", addressEnd - addressStart + 1, DEC));
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
                DB(serial.println(" -> UserRAM processed: ", copyCount, DEC));
            }
        }

        // ok, lets prepare for another try, maybe we find the remaining bytes in another memory
        lengthPayLoad = addressEnd - addressStart + 1;
        if (!startNotFound)
        {
            DB((readMem) ? serial.print(" -> MemoryRead :", addressStart, HEX, 4) : serial.print(" -> MemoryWrite:", addressStart, HEX, 4);
                serial.print(" Data:");
                for(int i=0; i<lengthPayLoad ; i++)
                {
                    serial.print(" ", payLoad[i], HEX, 2);
                }
                serial.println(" count: ", lengthPayLoad, DEC);
            );
        }
    }

    DB(if (lengthPayLoad != 0)
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
    DB(serial.print("ApciMemoryWritePDU:", addressStart, HEX, 4);
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
        DB(serial.println(" LOAD_CONTROL_ADDR: objectIdx:", objectIdx, HEX));
        if (objectIdx < INTERFACE_OBJECT_COUNT)
        {
            userEeprom.loadState[objectIdx] = loadProperty(objectIdx, &payLoad[0], lengthPayLoad);
            userEeprom.modified();
            DB(serial.println());
            return true;
        }
        else
        {
            DB(serial.println(" not found"));
            return false;
        }
    }
#endif

    return processApciMemoryOperation(addressStart, payLoad, lengthPayLoad, false);
}

bool BCU::processApciMemoryReadPDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    DB(serial.print("ApciMemoryReadPDU :", addressStart, HEX, 4);
       serial.print(" count: ", lengthPayLoad, DEC);
    );

#ifdef LOAD_STATE_ADDR
    // special handling of DMP_LoadStateMachineRead_RCo_Mem (APCI_MEMORY_READ_PDU)
    // See KNX Spec. 3/5/2 3.30.2 p.121  (deprecated)
    if (addressStart >= LOAD_STATE_ADDR && addressStart < LOAD_STATE_ADDR + INTERFACE_OBJECT_COUNT)
    {
        memcpy(payLoad, userEeprom.loadState + (addressStart - LOAD_STATE_ADDR), lengthPayLoad);
        DB(serial.println(" LOAD_STATE_ADDR: ", addressStart, HEX));
        return true;
    }
#endif

    bool result = processApciMemoryOperation(addressStart, payLoad, lengthPayLoad, true);

    DB(if (result)
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

void BCU::processDirectTelegram(int apci)
{
    const int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];
    const int senderSeqNo = bus.telegram[6] & 0x3c;
    int count, address, index;
#if BCU_TYPE != BCU1_TYPE
    bool found;
    int id;
#endif
    unsigned char sendAck = 0;
    bool sendTel = false;

    if (connectedAddr != senderAddr) // ensure that the sender is correct
        return;

    connectedTime = systemTime;
    sendTelegram[6] = 0;

    int apciCmd = apci & APCI_GROUP_MASK;
    switch (apciCmd)  // ADC / memory commands use the low bits for data
    {
    case APCI_ADC_READ_PDU: // todo adc service  to be implemented for bus voltage and PEI
        index = bus.telegram[7] & 0x3f;  // ADC channel
        count = bus.telegram[8];         // number of samples
        sendTelegram[5] = 0x64;
        sendTelegram[6] = 0x41;
        sendTelegram[7] = 0xc0 | (index & 0x3f);   // channel
        sendTelegram[8] = count;                   // read count
        sendTelegram[9] = 0;                       // FIXME dummy - ADC value high byte
        sendTelegram[10] = 0;                      // FIXME dummy - ADC value low byte
        sendTel = true;
        break;

    case APCI_MEMORY_READ_PDU:
    case APCI_MEMORY_WRITE_PDU:
        count = bus.telegram[7] & 0x0f; // number of data bytes
        address = (bus.telegram[8] << 8) | bus.telegram[9]; // address of the data block

        if (apciCmd == APCI_MEMORY_WRITE_PDU)
        {
            if (processApciMemoryWritePDU(address, &bus.telegram[10], count))
            {
#if BCU_TYPE != BCU1_TYPE
                if (userRam.deviceControl & DEVCTRL_MEM_AUTO_RESPONSE)
                {
                    // only on successful write
                    apciCmd = APCI_MEMORY_READ_PDU;
                }
#endif
            }
            sendAck = T_ACK_PDU;
        }

        if (apciCmd == APCI_MEMORY_READ_PDU)
        {
            if (!processApciMemoryReadPDU(address, &sendTelegram[10], count))
            {
                // address space unreachable, need to respond with count 0
                count = 0;
            }

            // send a the read response
            sendTelegram[5] = 0x63 + count;
            sendTelegram[6] = 0x42;
            sendTelegram[7] = 0x40 | count;
            sendTelegram[8] = address >> 8;
            sendTelegram[9] = address;
            sendTel = true;
        }
        break;

    case APCI_DEVICEDESCRIPTOR_READ_PDU:
        if (processDeviceDescriptorReadTelegram(apci & 0x3f))
            sendTel = true;
        else sendAck = T_NACK_PDU;
        break;

    default:
        switch (apci)
        {
        case APCI_RESTART_PDU:
        case APCI_RESTART_TYPE1_PDU:
            if(apci&1)
            {
                unsigned int erase   = bus.telegram[8];
                unsigned int channel = bus.telegram[9];

                if(erase == 0 && channel == 255)
                {
                    unsigned int * magicWord = (unsigned int *) 0x10000000;
                    *magicWord = 0x5E1FB055;
                }
            }
            if (usrCallback)
                usrCallback->Notify(USR_CALLBACK_RESET);
            writeUserEeprom();   // Flush the EEPROM before resetting
            if (memMapper)
            {
                memMapper->doFlash();
            }
            NVIC_SystemReset();  // Software Reset
            break;

        case APCI_AUTHORIZE_REQUEST_PDU:
            sendTelegram[5] = 0x62;
            sendTelegram[6] = 0x43;
            sendTelegram[7] = 0xd2;
            sendTelegram[8] = 0x00;
            sendTel = true;
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
            sendTel = true;
            break;

        case APCI_PROPERTY_DESCRIPTION_READ_PDU:
            sendTelegram[5] = 0x68;
            sendTelegram[6] = 0x64 | (APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU >> 8);
            sendTelegram[7] = APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU & 0xff;
            index = sendTelegram[8] = bus.telegram[8];
            id = sendTelegram[9] = bus.telegram[9];
            address = (sendTelegram[10] = bus.telegram[10]);
            propertyDescReadTelegram(index, (PropertyID) id, address);
            sendTel = true;
            break;
#endif /*BCU_TYPE != BCU1_TYPE*/

        default:
            sendAck = T_NACK_PDU;  // Command not supported
            break;
        }
        break;
    }

    if (sendTel)
        sendAck = T_ACK_PDU;

    if (sendAck)
        sendConControlTelegram(sendAck, senderSeqNo);
    else sendCtrlTelegram[0] = 0;

    if (sendTel)
    {
        sendTelegram[0] = 0xb0 | (bus.telegram[0] & 0x0c); // Control byte
        // 1+2 contain the sender address, which is set by bus.sendTelegram()
        sendTelegram[3] = connectedAddr >> 8;
        sendTelegram[4] = connectedAddr;

        if (sendTelegram[6] & 0x40) // Add the sequence number if applicable
        {
            sendTelegram[6] &= ~0x3c;
            sendTelegram[6] |= connectedSeqNo;
            incConnectedSeqNo = true;
        }
        else incConnectedSeqNo = false;

        bus.sendTelegram(sendTelegram, telegramSize(sendTelegram));
    }
}

void BCU::processConControlTelegram(int tpci)
{
    int senderAddr = (bus.telegram[1] << 8) | bus.telegram[2];

    if (tpci & 0x40)  // An acknowledgement
    {
        tpci &= 0xc3;
        if (tpci == T_ACK_PDU) // A positive acknowledgement
        {
            int curSeqNo = bus.telegram[6] & 0x3c;
            if (incConnectedSeqNo && connectedAddr == senderAddr && lastAckSeqNo !=  curSeqNo)
            {
                connectedSeqNo += 4;
                connectedSeqNo &= 0x3c;
                incConnectedSeqNo = false;
                lastAckSeqNo = curSeqNo;
            }
        }
        else if (tpci == T_NACK_PDU)  // A negative acknowledgement
        {
            if (connectedAddr == senderAddr)
            {
                sendConControlTelegram(T_DISCONNECT_PDU, 0);
                connectedAddr = 0;
            }
        }

        incConnectedSeqNo = true;
    }
    else  // A connect/disconnect command
    {
        if (tpci == T_CONNECT_PDU)  // Open a direct data connection
        {
            if (connectedAddr == 0)
            {
                connectedTime = systemTime;
                connectedAddr = senderAddr;
                connectedSeqNo = 0;
                incConnectedSeqNo = false;
                lastAckSeqNo = -1;
                //bus.setSendAck (0); // todo check in spec if needed
            }
        }
        else if (tpci == T_DISCONNECT_PDU)  // Close the direct data connection
        {
            if (connectedAddr == senderAddr)
            {
                connectedAddr = 0;
                //bus.setSendAck (0);  // todo check in spec if needed
            }
        }
    }
}
