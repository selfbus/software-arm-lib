/*
 *  bcu_default.cpp
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/bcu_default.h>
#include <string.h>
#include <sblib/eib/bus.h>

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

BcuDefault::BcuDefault(UserRam* userRam, UserEeprom* userEeprom, ComObjects* comObjects, AddrTables* addrTables) :
        BcuBase(userRam, addrTables),
        userEeprom(userEeprom),
        memMapper(nullptr),
		usrCallback(nullptr),
		sendGrpTelEnabled(false),
		groupTelWaitMillis(DEFAULT_GROUP_TEL_WAIT_MILLIS),
		groupTelSent(millis())
{
    this->comObjects = comObjects;
}

void BcuDefault::_begin()
{
#ifndef ROUTER
    if (ownAddress() == 0) // 0.0.0 is not allowed for normal devices, only routers
    {
        setOwnAddress(PHY_ADDR_DEFAULT); //set default address 15.15.255
    }
#endif
    BcuBase::_begin();

#ifdef DUMP_PROPERTIES ///\todo move to BCU2::begin(...)
    IF_DEBUG(serial.println("Properties dump enabled."));
#endif

#if defined(INCLUDE_SERIAL)
    IF_DEBUG(serial.print("BCU Name: "););
    IF_DEBUG(serial.print(getBcuType()); serial.print(" "););
    IF_DEBUG(serial.println(" MaskVersion: 0x", getMaskVersion(), HEX, 4));
#   ifdef LOAD_CONTROL_ADDR
        IF_DEBUG(serial.println("LOAD_CONTROL_ADDR: 0x", LOAD_CONTROL_ADDR, HEX, 4));
#   endif
#   ifdef LOAD_STATE_ADDR
        IF_DEBUG(serial.println("LOAD_STATE_ADDR: 0x", LOAD_STATE_ADDR, HEX, 4));
#   endif
#   ifdef EXTRA_USER_RAM_SIZE
        IF_DEBUG(serial.println("EXTRA_USER_RAM_SIZE: 0x", EXTRA_USER_RAM_SIZE, HEX, 4));
#   endif

#   ifdef USER_RAM_SHADOW_SIZE
        IF_DEBUG(serial.println("USER_RAM_SHADOW_SIZE: 0x", USER_RAM_SHADOW_SIZE, HEX, 4));
#   endif
    if (userRam != nullptr)
    {
        IF_DEBUG(serial.print("userRam    start: 0x", (unsigned int)userRam->startAddr(), HEX, 4); serial.print(" "););
        IF_DEBUG(serial.print("end: 0x", (unsigned int)userRam->endAddr(), HEX, 4); serial.print(" "););
        IF_DEBUG(serial.println("size: 0x", (unsigned int)userRam->size(), HEX, 4));
    }
    if (userEeprom != nullptr)
    {
        IF_DEBUG(serial.print("userEeprom start: 0x", (unsigned int)userEeprom->startAddr(), HEX, 4); serial.print(" "););
        IF_DEBUG(serial.print("end: 0x", (unsigned int)userEeprom->endAddr(), HEX, 4); serial.print(" "););
        IF_DEBUG(serial.print("size: 0x", (unsigned int)userEeprom->size(), HEX, 4); serial.print(" "););
        IF_DEBUG(serial.println("flashSize: 0x", (unsigned int)userEeprom->flashSize(), HEX, 4););
    }
    IF_DEBUG(serial.println());
#endif
    sendGrpTelEnabled = true;
    groupTelSent = millis();

    // set limit to max of 28 telegrams per second (wait 35ms) -  to avoid risk of thermal destruction of the sending circuit
    groupTelWaitMillis = DEFAULT_GROUP_TEL_WAIT_MILLIS ;
}

void BcuDefault::begin(int manufacturer, int deviceType, int version)
{
    setOwnAddress(makeWord(userEeprom->addrTab()[0], userEeprom->addrTab()[1]));
    userRam->status() = BCU_STATUS_LINK_LAYER | BCU_STATUS_TRANSPORT_LAYER | BCU_STATUS_APPLICATION_LAYER | BCU_STATUS_USER_MODE;
    userRam->deviceControl() = 0;
    userRam->runState() = 1;

    ///\todo why we touch runError here ? KNX spec 2.1 9/4/1 4.1.10.3.13 p.31 states "They shall be explicitly cleared by a management-tool."
    userEeprom->runError() = 0xff;
    userEeprom->portADDR() = 0;

    userEeprom->manufacturerH() = HIGH_BYTE(manufacturer);
    userEeprom->manufacturerL() = lowByte(manufacturer);

    userEeprom->deviceTypeH() = HIGH_BYTE(deviceType);
    userEeprom->deviceTypeL() = lowByte(deviceType);

    userEeprom->version() = version;

    userEeprom->modified(false);
}

void BcuDefault::setOwnAddress(uint16_t addr)
{
    if (addr != makeWord(userEeprom->addrTab()[0], userEeprom->addrTab()[1]))
    {
        userEeprom->addrTab()[0] = HIGH_BYTE(addr);
        userEeprom->addrTab()[1] = lowByte(addr);
        userEeprom->modified(true);
    }
    BcuBase::setOwnAddress(addr);
}

void BcuDefault::loop()
{
	if (!enabled)
		return;

    BcuBase::loop(); // check processTelegram and programming button state

    // Rest of this function is only relevant if currently able to send another telegram.
    if (bus->sendingTelegram())
    {
        return;
    }

    // handle group object telegrams only if application is runnning
    // check for next telegram to be send
    if (sendGrpTelEnabled && applicationRunning())
    {
        // Send group telegram if group telegram rate limit not exceeded
        if (elapsed(groupTelSent) >= groupTelWaitMillis)
        {
            // check for possible next comobject to be send
         if (comObjects->sendNextGroupTelegram())
             groupTelSent = millis();

        }
        // To prevent overflows if no telegrams are sent for a long time
        ///\todo better reload with systemTime - groupTelWaitMillis
        if (elapsed(groupTelSent) >= 2000)
        {
            groupTelSent += 1000;
        }
    }

    if (userEeprom->isModified() && !directConnection() && userEeprom->writeDelayElapsed())
    {
        flushUserMemory(UsrCallbackType::flash, true);
    }
}

void BcuDefault::end()
{
    flushUserMemory(UsrCallbackType::bcu_end, true);
    BcuBase::end();
}

byte* BcuDefault::userMemoryPtr(unsigned int addr)
{
    if (userEeprom->inRange(addr))
    {
        return &(*userEeprom)[addr];
    }
    else if (userRam->inRange(addr))
    {
        return &(*userRam)[addr];
    }
    return nullptr;
}

void BcuDefault::setMemMapper(MemMapper *mapper)
{
    memMapper = mapper;
}

MemMapper* BcuDefault::getMemMapper()
{
    return memMapper;
}

void BcuDefault::setUsrCallback(UsrCallback *callback)
{
    usrCallback = callback;
}

void BcuDefault::enableGroupTelSend(bool enable)
{
    sendGrpTelEnabled = enable;
}

void BcuDefault::setGroupTelRateLimit(unsigned int limit)
{
 if ((limit > 0) && (limit <= MAX_GROUP_TEL_PER_SECOND))
     groupTelWaitMillis = 1000/limit;
 else
     groupTelWaitMillis = DEFAULT_GROUP_TEL_WAIT_MILLIS ;
}

void BcuDefault::setRxPin(int rxPin) {
    bus->rxPin=rxPin;
}

void BcuDefault::setTxPin(int txPin) {
    bus->txPin=txPin;
    setKNX_TX_Pin(bus->txPin);
}

void BcuDefault::setTimer(Timer& timer) {
    bus->timer=timer;
}

void BcuDefault::setCaptureChannel(TimerCapture captureChannel) {
    bus->captureChannel=captureChannel;
}

/**
 * todo check for RX status and inform upper layer if needed
 */
bool BcuDefault::processGroupAddressTelegram(ApciCommand apciCmd, uint16_t groupAddress, unsigned char *telegram, uint8_t telLength)
{
    DB_COM_OBJ(
        serial.println();
        serial.print("BCU grp addr: 0x", (unsigned int)groupAddress, HEX, 4);
    );

    comObjects->processGroupTelegram(groupAddress, apciCmd & APCI_GROUP_MASK, telegram);
    return (true);
}

bool BcuDefault::processBroadCastTelegram(ApciCommand apciCmd, unsigned char *telegram, uint8_t telLength)
{
    if (!programmingMode())
    {
        return (true);
    }

    // we are in programming mode
    switch (apciCmd)
    {
        case APCI_INDIVIDUAL_ADDRESS_WRITE_PDU:
            setOwnAddress(makeWord(telegram[8], telegram[9]));
            break;

        case APCI_INDIVIDUAL_ADDRESS_READ_PDU:
            sendApciIndividualAddressReadResponse();
            break;

        default :
            return (false);
    }
    return (true);
}

bool BcuDefault::processApciMemoryWritePDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    DB_MEM_OPS(
       serial.print("ApciMemoryWritePDU: 0x", addressStart, HEX, 4);
       serial.print(" Data:");
       for(int i=0; i<lengthPayLoad ; i++)
       {
           serial.print(" ", payLoad[i], HEX, 2);
       }
       serial.print(" count: ", lengthPayLoad, DEC);
    );
    return processApciMemoryOperation(addressStart, payLoad, lengthPayLoad, false);
}

bool BcuDefault::processApciMemoryReadPDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    DB_MEM_OPS(
       serial.print("ApciMemoryReadPDU : 0x", addressStart, HEX, 4);
       serial.print(" count: ", lengthPayLoad, DEC);
    );

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

bool BcuDefault::processApciMemoryOperation(unsigned int addressStart, byte *payLoad, unsigned int lengthPayLoad, const bool &readMem)
{
    if (lengthPayLoad == 0)
    {
        DB_MEM_OPS(serial.println(" Error processApciMemoryOperation : lengthPayLoad == 0"));
        return (false);
    }

    const unsigned int addressEnd = addressStart + lengthPayLoad - 1;

    if (addressEnd < addressStart)
    {
        DB_MEM_OPS(
                serial.print("   --> address overflow start 0x", addressStart, HEX);
                serial.print(" end 0x", addressEnd, HEX);
                serial.println(" lengthPayLoad ", lengthPayLoad, DEC);
                );
        return (false);
    }

    bool startNotFound = false; // we need this as a exit condition, in case memory range is no where found
    bool startFound;
    bool endFound;

    while ((!startNotFound) && (addressStart <= addressEnd))
    {
        startNotFound = true;
        // check if we have a memMapper and if payLoad is handled by it
        if (memMapper != nullptr)
        {
            startFound = memMapper->isMapped(addressStart);
            endFound = memMapper->isMapped(addressEnd);
            if (startFound && endFound)
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
                    return (true);
                }
                else
                    return (false);
            }
            else if (startFound)
            {
                // we only know that start is mapped so lets do memory operation byte by byte
                for (unsigned int i = 0; i < lengthPayLoad; i++)
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

        // check if payLoad is in userEeprom
        if (userEeprom->inRange(addressStart, addressEnd)) // start & end in userEeprom ?
        {
            const int copyCount = addressEnd - addressStart + 1;
            uint8_t* mem = this->userMemoryPtr(addressStart);
            if (readMem)
            {
                memcpy(&payLoad[0], mem, copyCount);
            }
            else
            {
                memcpy(mem, &payLoad[0], copyCount);
                userEeprom->modified(true);
            }
            DB_MEM_OPS(serial.println(" -> EEPROM ", copyCount, DEC));
            return (true);
        }
        else if (userEeprom->inRange(addressStart))
        {
            // start is in USER_EEPROM, but payLoad is too long, we need to cut it down to fit
            const int copyCount = (userEeprom->endAddr() - addressStart + 1);
            uint8_t* mem = this->userMemoryPtr(addressStart);
            if (readMem)
            {
                memcpy(&payLoad[0], mem, copyCount);
            }
            else
            {
                memcpy(mem, &payLoad[0], copyCount);
                userEeprom->modified(true);
            }
            addressStart += copyCount;
            payLoad += copyCount;
            startNotFound = false;
            DB_MEM_OPS(serial.println(" -> EEPROM processed: ", copyCount, DEC));
        }

        // check if payLoad is in UserRam
        if ((userRam->inRange(addressStart, addressEnd)) ||
            ((userRam->isStatusAddress(addressStart)) && (userRam->isStatusAddress(addressEnd)))) // USER_RAM_STATUS_ADDRESS (0x60) is system state specific
        {
            // start & end are in UserRAM
            if (readMem)
                userRam->cpyFromUserRam(addressStart, &payLoad[0], addressEnd - addressStart + 1);
            else
                userRam->cpyToUserRam(addressStart, &payLoad[0], addressEnd - addressStart + 1);
            DB_MEM_OPS(serial.println(" -> UserRAM ", addressEnd - addressStart + 1, DEC));
            return (true);
        }
        else if (userRam->inRange(addressStart))
        {
            // start is in UserRAM, but payLoad is too long, we need to cut it down to fit
            const int copyCount = userRam->endAddr() - addressStart + 1;
            if (readMem)
                userRam->cpyFromUserRam(addressStart, &payLoad[0], copyCount);
            else
                userRam->cpyToUserRam(addressStart, &payLoad[0], copyCount);
            addressStart += copyCount;
            payLoad += copyCount;
            startNotFound = false;
            DB_MEM_OPS(serial.println(" -> UserRAM processed: ", copyCount, DEC));
        }

        // ok, lets prepare for another try, maybe we find the remaining bytes in another memory
        lengthPayLoad = addressEnd - addressStart + 1;
        if (!startNotFound)
        {
            DB_MEM_OPS(
                (readMem) ? serial.print(" -> MemoryRead :", addressStart, HEX, 4) : serial.print(" -> MemoryWrite:", addressStart, HEX, 4);
                serial.print(" Data:");
                for(unsigned int i=0; i<lengthPayLoad ; i++)
                {
                    serial.print(" ", payLoad[i], HEX, 2);
                }
                serial.print(" count: ", lengthPayLoad, DEC);
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
       else
       {
           serial.println();
       }
    );

    return (lengthPayLoad == 0);
}

bool BcuDefault::processApci(ApciCommand apciCmd, unsigned char * telegram, uint8_t telLength, uint8_t * sendBuffer)
{
    uint8_t count;
    uint16_t address;
    uint8_t index;

    switch (apciCmd)
    {
    case APCI_ADC_READ_PDU: ///\todo implement ADC service for bus voltage and PEI,
                            //!> Estimation of the current bus via the AD-converter channel 1 and the AdcRead-service.
                            //!  The value read can be converted to a voltage value by using the following formula: Voltage = ADC_Value * 0,15V
        index = telegram[7] & 0x3f;  // ADC channel
        count = telegram[8];         // number of samples
        sendBuffer[5] = 0x60 + 4;  // routing count in high nibble + response length in low nibble
        setApciCommand(sendBuffer, APCI_ADC_RESPONSE_PDU, index);
        sendBuffer[8] = count;     // read count
        sendBuffer[9] = 0;         // ADC value high byte
        sendBuffer[10] = 0;        // ADC value low byte
        return (true);

    case APCI_MEMORY_READ_PDU:
    case APCI_MEMORY_WRITE_PDU:
        count = telegram[7] & 0x0f; // number of data bytes
        address = makeWord(telegram[8], telegram[9]); // address of the data block

        if (apciCmd == APCI_MEMORY_WRITE_PDU)
        {
            if (processApciMemoryWritePDU(address, &telegram[10], count))
            {
                ///\todo dirty workaround, should be done in subclasses BCU1, MASKVERSION701, MASKVERSION705...
                if ((getMaskVersion() > 0x1F) && (userRam->deviceControl() & DEVCTRL_MEM_AUTO_RESPONSE))
                {
                    // autorespond only on successful write
                    apciCmd = APCI_MEMORY_READ_PDU;
                }
            }
        }

        if (apciCmd == APCI_MEMORY_READ_PDU)
        {
            if (!processApciMemoryReadPDU(address, &sendBuffer[10], count))
            {
                // address space unreachable, need to respond with count 0
                count = 0;
            }

            // send a APCI_MEMORY_RESPONSE_PDU response
            sendBuffer[5] = 0x60 + count + 3; // routing count in high nibble + response length in low nibble
            setApciCommand(sendBuffer, APCI_MEMORY_RESPONSE_PDU, count);
            sendBuffer[8] = HIGH_BYTE(address);
            sendBuffer[9] = lowByte(address);
            return (true);
        }
        break;

    case APCI_DEVICEDESCRIPTOR_READ_PDU:
        return (processDeviceDescriptorReadTelegram(sendBuffer, telegram[7] & 0x3f));

    case APCI_MASTER_RESET_PDU:
        return (processApciMasterResetPDU(sendBuffer, telegram[8], telegram[9]));

    case APCI_AUTHORIZE_REQUEST_PDU:
        sendBuffer[5] = 0x60 + 2; // routing count in high nibble + response length in low nibble
        setApciCommand(sendBuffer, APCI_AUTHORIZE_RESPONSE_PDU, 0);
        sendBuffer[8] = 0x00;
        return (true);

    default:
        return (BcuBase::processApci(apciCmd, telegram, telLength, sendBuffer));

    }
    return (false);
}

bool BcuDefault::processDeviceDescriptorReadTelegram(uint8_t * sendBuffer, int id)
{
    if (id != 0)
    {
        return (false); // unknown device descriptor
    }

    sendBuffer[5] = 0x60 + 3; // routing count in high nibble + response length in low nibble
    setApciCommand(sendBuffer, APCI_DEVICEDESCRIPTOR_RESPONSE_PDU, 0);
    sendBuffer[8] = HIGH_BYTE(getMaskVersion());
    sendBuffer[9] = lowByte(getMaskVersion());
    return (true);
}

bool BcuDefault::processApciMasterResetPDU(uint8_t * sendBuffer, uint8_t eraseCode, uint8_t channelNumber)
{
    if (!checkApciForMagicWord(eraseCode, channelNumber))
    {
        ///\todo implement proper handling of APCI_MASTER_RESET_PDU for all other Erase Codes
        requestedRestartType = RESTART_BASIC;
        return (false);
    }

    // create the APCI_MASTER_RESET_RESPONSE_PDU
    sendBuffer[5] = 0x60 + 4;  // routing count in high nibble + response length in low nibble
    setApciCommand(sendBuffer, APCI_MASTER_RESET_RESPONSE_PDU, 0);
    sendBuffer[8] = T_RESTART_NO_ERROR;
    sendBuffer[9] = 0; // restart process time 2 byte unsigned integer value expressed in seconds, DPT_TimePeriodSec / DPT7.005
    sendBuffer[10] = 1; // 1 second

    // special version of APCI_MASTER_RESET_PDU used by Selfbus bootloader
    // set magicWord to start after reset in bootloader mode
#ifndef IAP_EMULATION
    unsigned int * magicWord = BOOTLOADER_MAGIC_ADDRESS;
    *magicWord = BOOTLOADER_MAGIC_WORD;
#endif
    requestedRestartType = RESTART_MASTER;
    return (true);
}

void BcuDefault::softSystemReset()
{
    bool waitIdle = true;
#ifdef IAP_EMULATION
    waitIdle = false; ///\todo workaround for lib test cases running later into a infinitive loop at ' while (!bus->idle())'
#endif
    flushUserMemory(UsrCallbackType::reset, waitIdle);
    BcuBase::softSystemReset();
}

bool BcuDefault::flushUserMemory(UsrCallbackType reason, bool waitIdle)
{
    if (waitIdle)
    {
        while (!bus->idle()) // wait for an idle bus
        {
            ;
        }
    }

    if (usrCallback)
    {
        usrCallback->Notify(reason);
    }

    userEeprom->writeUserEeprom();

    if (memMapper)
    {
        memMapper->doFlash();
    }
    return (true);
}

