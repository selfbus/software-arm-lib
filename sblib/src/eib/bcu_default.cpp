/*
 *  bcu_default.cpp
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#define INSIDE_BCU_CPP
#include <sblib/io_pin_names.h>
#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/bcu_default.h>
#include <sblib/eib/addr_tables.h>
#include <sblib/internal/variables.h>
#include <sblib/internal/iap.h>
#include <sblib/eib/userEeprom.h>
#include <sblib/eib/userRam.h>
#include <sblib/eib/apci.h>
#include <sblib/utils.h>
#include <string.h>
#include <sblib/eib/bus.h>

// static Bus* timerBusObj;
// The interrupt handler for the EIB bus access object
// BUS_TIMER_INTERRUPT_HANDLER(TIMER16_1_IRQHandler, (*timerBusObj))

extern volatile unsigned int systemTime;

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
    BcuBase::_begin();

#ifdef DUMP_TELEGRAMS ///\todo move to Bus::begin()
    IF_DEBUG(serial.println("Telegram dump enabled."));
#endif

#ifdef DUMP_PROPERTIES ///\todo move to BCU2::begin(...)
    IF_DEBUG(serial.println("Properties dump enabled."));
#endif

#ifdef DEBUG_BUS ///\todo move to Bus::begin()
    IF_DEBUG(serial.println("DEBUG_BUS dump enabled."));
#endif

#ifdef DEBUG_BUS_BITLEVEL ///\todo move to Bus::begin()
    IF_DEBUG(serial.println("DEBUG_BUS_BITLEVEL dump enabled."));
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
        IF_DEBUG(serial.print("userRam    start: 0x", userRam->userRamStart, HEX, 4); serial.print(" "););
        IF_DEBUG(serial.print("end: 0x", userRam->userRamEnd, HEX, 4); serial.print(" "););
        IF_DEBUG(serial.println("size: 0x", userRam->userRamSize, HEX, 4));
    }
    if (userEeprom != nullptr)
    {
        IF_DEBUG(serial.print("userEeprom start: 0x", userEeprom->userEepromStart, HEX, 4); serial.print(" "););
        IF_DEBUG(serial.print("end: 0x", userEeprom->userEepromEnd, HEX, 4); serial.print(" "););
        IF_DEBUG(serial.print("size: 0x", userEeprom->userEepromSize, HEX, 4); serial.print(" "););
        IF_DEBUG(serial.println("flashSize: 0x", userEeprom->userEepromFlashSize, HEX, 4););
    }
    IF_DEBUG(serial.println());
#endif
    sendGrpTelEnabled = true;
    groupTelSent = millis();

    // set limit to max of 28 telegrams per second (wait 35ms) -  to avoid risk of thermal destruction of the sending circuit
    groupTelWaitMillis = DEFAULT_GROUP_TEL_WAIT_MILLIS ;
}

void BcuDefault::setOwnAddress(uint16_t addr)
{
    BcuBase::setOwnAddress(addr);
}

void BcuDefault::loop()
{
	if (!enabled)
		return;


#ifdef DUMP_TELEGRAMS
	extern unsigned char telBuffer[];
	extern unsigned int telLength ; // db_state;
	extern unsigned char txtelBuffer[];
	extern unsigned int txtelLength;
	extern unsigned int tx_rep_count;
	extern unsigned int tx_busy_rep_count;
    extern unsigned int tx_telrxerror;

	//extern unsigned int telRXtime;
	extern unsigned int telRXStartTime;
	extern unsigned int telTXStartTime;
	extern unsigned int telRXEndTime;
	extern unsigned int telTXEndTime;
	extern unsigned int telTXAck;
	extern unsigned int telRXWaitInitTime; // Wait for 50 bit time after last RX/TX telegram, could be less- rx will be ignored
	extern unsigned int telRXWaitIdleTime; // bus is in idle after 50 bit times, now wait for next start of RX/TX
	//extern unsigned int telRXTelStartTime; // Start Time of a telegram
	extern unsigned int telRXTelByteStartTime; // Start Time of a byte within a telegram
	extern unsigned int telRXTelBitTimingErrorL; // Bit error - falling rx edge was not in the expected window -7 till +33us
	extern unsigned int telRXTelBitTimingErrorE; // Bit error - falling rx edge was not in the expected window -7 till +33us
    extern unsigned int telrxerror;

	extern bool telcollision;
	static unsigned int telLastRXEndTime =0;
	static unsigned int telLastTXEndTime =0;

	if (telTXAck)
	{
	    serial.print("TXAck:");
		serial.println(telTXAck, HEX, 2);
		telTXAck = 0;
	}

	//dump transmitting part
	if (telTXEndTime) // we transmitted a tel
	{
			serial.print("TX: (S");
			serial.print(telTXStartTime, DEC, 6);
			serial.print(" E");
			serial.print(telTXEndTime, DEC, 6);

			serial.print(" err: 0x");
			serial.print(tx_telrxerror, HEX, 4);


		if (telLastRXEndTime)
		{
			// print time in between last rx-tel and current tx-tel
			serial.print(" dt RX-TX:");
			serial.print(( telTXStartTime - telLastRXEndTime), DEC, 6);
			telLastRXEndTime = 0;

		}else if(telLastTXEndTime)
		{
			// print time in between last tx-tel and current tx-tel
			serial.print(" dt TX-TX:" );
			serial.print(( telTXStartTime - telLastTXEndTime), DEC, 6);
			telLastTXEndTime = 0;
		}
		serial.print(" rep:");
		serial.print(( tx_rep_count), DEC, 1);
		serial.print(" brep:");
		serial.print(( tx_busy_rep_count), DEC, 1);
		serial.print(") ");

		//dump tx tel data
		for (unsigned int i = 0; i < txtelLength; ++i)
		{
			if (i) serial.print(" ");
			serial.print(txtelBuffer[i], HEX, 2);
		}
		serial.println();

		telLastTXEndTime = telTXEndTime;
		telTXEndTime = 0;
		telTXStartTime = 0;
	}


/*
//dump wait for idle stuff
	if (telRXWaitInitTime)
	{
		if (telLastTXEndTime)
		{
		serial.print("WI-TX:");
		serial.println(( telRXWaitInitTime -telLastTXEndTime), DEC, 6);
		//serial.println("");
		telRXWaitInitTime = 0;
		}
		else if (telLastRXEndTime)
		{
		serial.print("WI-RX:");
		serial.println(( telRXWaitInitTime - telLastRXEndTime), DEC, 6);
		//serial.println("");
		telRXWaitInitTime = 0;
		}
		else
		{
		serial.print("WI:");
		serial.println(( telRXWaitInitTime ), DEC, 6);
		//serial.print(" ");
		}
		telRXWaitInitTime = 0;
	}
*/
/*
	if (telRXWaitIdleTime)
	{
		if (telLastTXEndTime)
		{
			serial.print("WID-TX:");
			serial.print(( telRXWaitIdleTime - telLastTXEndTime), DEC, 6);
			serial.print(" ");
		} else if (telLastRXEndTime)
		{
			serial.print("WID-RX:");
			serial.print(( telRXWaitIdleTime - telLastRXEndTime), DEC, 6);
			serial.print(" ");
		} else
		{
			serial.print("WID:");
			serial.print(( telRXWaitIdleTime ), DEC, 6);
			serial.print(" ");
		}
		telRXWaitIdleTime = 0;
	}
*/

//dump receiving bit error stuff
//rx bit timing errors
	if (telRXTelBitTimingErrorL)
	{
		serial.print(" ERL:", telRXTelBitTimingErrorL, DEC, 6);
		serial.print(" ");
		telRXTelBitTimingErrorL =0;
	}
	if (telRXTelBitTimingErrorE)
	{
		serial.print(" ERE:", telRXTelBitTimingErrorE, DEC, 6);
		serial.print(" ");
		telRXTelBitTimingErrorE =0;
	}

//dump  tel receiving part
	if (telLength > 0)
	{
	    serial.print("RCV: (S");
		serial.print(telRXStartTime, DEC, 6 );
		serial.print(" E");
		serial.print(telRXEndTime, DEC, 6);
	//	serial.print(") ");
	//	serial.print(", LRXE:");
	//	serial.print(telLastRXEndTime, DEC, 9);
	//	serial.print(", LTXE:");
	//	serial.print(telLastTXEndTime, DEC,9);
		//serial.print(") ");

		if (telLastTXEndTime)
		{
			// print time in between last tx-tel and current rx-tel
			serial.print(" dt TX-RX:");
			serial.print(( telRXStartTime - telLastTXEndTime), DEC, 6);
			telLastTXEndTime = 0;
		}
		else if(telLastRXEndTime)
		{
			// print time in between last rx-tel and current rx-tel
			serial.print(" dt RX-RX:");
			serial.print(( telRXStartTime - telLastRXEndTime), DEC, 6);
		//	serial.println(") ");
			//telLastRXEndTime = 0;
		}

		serial.print(" err: 0x");
		serial.print(telrxerror, HEX, 4);
		serial.print(") ");

		if (telcollision)  serial.print("collision");
		//dump tel data
		for (unsigned int i = 0; i < telLength; ++i)
		{
			if (i) serial.print(" ");
			serial.print(telBuffer[i], HEX, 2);
		}
		serial.println();
//reset all debug data
		telLength = 0;
		telLastRXEndTime = telRXEndTime;
		telRXEndTime = 0;
		telrxerror = 0;
		telRXTelByteStartTime = 0;
	}
#endif

#ifdef DEBUG_BUS
	// trace buffer content:
	// trace point id (start with s) followed by trace data, coding: sittee
	// i: state machine trace point code
	//  0000-3999  one timer value
	//  4000-5999 one hex value
	//  6000-7999 one dec value
	//  8000-8999 all timer values at interrupt
	//  9000 - rx tel data values
	// tt: trace point number within certain state
	// ee: state of state machine at trace point



	static unsigned int t,l, l1, lt,lt1, s, cv,tv, tmv;
	bool cf;
	l=0; l1=0;
	while (tb_in != tb_out && l1 < 5) {
	//while (tb_in != tb_out) {
		l1++;
		s= td_b[tb_out].ts;
		t= td_b[tb_out].tt;
		tv= td_b[tb_out].ttv;
		cv= td_b[tb_out].tcv;
		tmv= td_b[tb_out].ttmv;
		cf= td_b[tb_out].tc;
		if ((s>=8000 && s<=8999) ) {
			serial.println();
			serial.print("s");
			serial.print( (unsigned int) s, DEC, 3);
			serial.print(" t");
			serial.print( (unsigned int) t, DEC, 6);
			serial.print(" dt");
			serial.print( (unsigned int) t-lt, DEC,4);
			serial.print(" f");
			serial.print((unsigned int)cf, DEC, 1);
			serial.print(" c");
			serial.print((unsigned int)cv, DEC, 4);
			serial.print(" t");
			serial.print((unsigned int)tv, DEC, 4);
			serial.print(" m");
			serial.print((unsigned int)tmv, DEC,4);
/*			serial.print(" i");
			serial.print((unsigned int)tb_in, DEC,3);
			serial.print(" o");
			serial.print((unsigned int)tb_out, DEC,3);
*/
			//			serial.print("*");
			l=1;
			lt = t;
			lt1= t;
		}
		else if ( s>=9000) {
			serial.println();
			serial.print("s");
			serial.print( (unsigned int) s, DEC, 3);
			serial.print(" c/v");
			serial.print((unsigned int)cf, HEX, 2);
			serial.print(" L");
			serial.print((unsigned int)tmv, DEC, 2);
			serial.print(" t");
			serial.print( (unsigned int) t, HEX, 8);
			serial.print(" ");
			serial.print((unsigned int)cv, HEX, 4);
			serial.print(" ");
			serial.print((unsigned int)tv, HEX, 4);
			//serial.print("*");
		}else if ( s>=9005) {
			serial.println();
			serial.print("s");
			serial.print( (unsigned int) s, DEC, 3);
			serial.print(" ");
			serial.print((unsigned int)tmv, HEX,4);
			serial.print(" ");
			serial.print( (unsigned int) t, HEX, 8);
			serial.print(" ");
			serial.print((unsigned int)cv, HEX, 4);
			serial.print(" ");
			serial.print((unsigned int)tv, HEX, 4);
			serial.print(" d");
			serial.print((unsigned int)cf, HEX, 4);
			//serial.print("*");
		}
		else  if (s < 4000) { // one  delta timer
			serial.print("s");
			serial.print( (unsigned int) s -2000, DEC, 3);
			serial.print(" dt");
			serial.print( (unsigned int) t-lt1, DEC, 6);
			lt1 = t;
			l++;
		}
		else if (s < 5000) { // one hex
			serial.print("s");
			serial.print( (unsigned int) s- 4000, DEC, 3);
			serial.print(" h");
			serial.print( (unsigned int) t,HEX,4);
			l++;
		}
		else if (s < 6000) { // one dec
			serial.print("s");
			serial.print( (unsigned int) s- 5000, DEC, 3);
			serial.print(" d");
			serial.print( (unsigned int) t,DEC,4);
			l++;
		}
		if(l >5) {
			l=0;
			serial.println();
//			serial.print("* ");
		} else  serial.print(" ");
		if (++tb_out >= tb_lngth){ tb_out =0; tb_in_ov = false;}
		if(tb_in_ov && tb_out <= tb_in)  serial.print(" !!OV**");
	}
#endif

    TLayer4::loop();
	if (bus->telegramReceived() && (!bus->sendingTelegram()) && (bus->state == Bus::IDLE) && (userRam->status & BCU_STATUS_TL))
	{
        processTelegram(&bus->telegram[0], (uint8_t)bus->telegramLen); // if processed successfully, received telegram will be discarded by processTelegram()
	}

	if (progPin)
	{
		// Detect the falling edge of pressing the prog button
		pinMode(progPin, INPUT|PULL_UP);
		int oldValue = progButtonDebouncer.value();
		if (!progButtonDebouncer.debounce(digitalRead(progPin), 50) && oldValue)
		{
			userRam->status ^= 0x81;  // toggle programming mode and checksum bit
		}
		pinMode(progPin, OUTPUT);
		digitalWrite(progPin, (userRam->status & BCU_STATUS_PROG) ^ progPinInv);
	}

    // if bus is not sending (telegram buffer is empty) check for next telegram to be send
    if (sendGrpTelEnabled && !bus->sendingTelegram())
    {
        // Send group telegram if group telegram rate limit not exceeded
        if (elapsed(groupTelSent) >= groupTelWaitMillis)
        {
        	// check in com_objects method for waiting objects from user app and store in telegrambuffer
        	// and call bus->sendTelegram
         if (comObjects->sendNextGroupTelegram())
             groupTelSent = millis();

        }
        // To prevent overflows if no telegrams are sent for a long time - todo: better reload with systemTime - groupTelWaitMillis
        if (elapsed(groupTelSent) >= 2000)
        {
            groupTelSent += 1000;
        }
    }

    if (userEeprom->isModified() && bus->idle() && bus->telegramLen == 0 && connectedTo() == 0)
    {
        if (userEeprom->writeUserEepromTime)
        {
            if ((int)millis() - (int)userEeprom->writeUserEepromTime > 0)
            {
                if (usrCallback)
                    usrCallback->Notify(USR_CALLBACK_FLASH);
                userEeprom->writeUserEeprom();
                if (memMapper)
                {
                    memMapper->doFlash();
                }
            }
        }
        else userEeprom->writeUserEepromTime = millis() + 50;
    }
}

void BcuDefault::end()
{
    if (usrCallback)
        usrCallback->Notify(USR_CALLBACK_BCU_END);

    enabled = false;

    bus->end();

    userEeprom->writeUserEeprom();
    if (memMapper)
    {
        memMapper->doFlash();
    }
}

///\todo move to class userRam
void BcuDefault::cpyToUserRam(unsigned int address, unsigned char * buffer, unsigned int count)
{
    if ((address == USER_RAM_SYSTEM_STATE_ADDRESS) && (count == 1))
    {
        userRam->status = *buffer;
        return;
    }

    address -= userRam->userRamStart;
    if ((address > USER_RAM_SYSTEM_STATE_ADDRESS) || ((address + count) < USER_RAM_SYSTEM_STATE_ADDRESS))
    {
        memcpy((void*)(userRam->userRamData + address), buffer, count);
    }
    else
    {
        while (count--)
        {
            if (address == USER_RAM_SYSTEM_STATE_ADDRESS)
                userRam->status = * buffer;
            else
                userRam->userRamData[address] = * buffer;
            buffer++;
            address++;
        }
    }
}

///\todo move to class userRam
void BcuDefault::cpyFromUserRam(unsigned int address, unsigned char * buffer, unsigned int count)
{
    if ((address == USER_RAM_SYSTEM_STATE_ADDRESS) && (count == 1))
    {
        *buffer = userRam->status;
        return;
    }

    address -= userRam->userRamStart;
    if ((address > USER_RAM_SYSTEM_STATE_ADDRESS) || ((address + count) < USER_RAM_SYSTEM_STATE_ADDRESS))
    {
        memcpy(buffer, (const void*)(userRam->userRamData + address), count);
    }
    else
    {
        while (count--)
        {
            if (address == USER_RAM_SYSTEM_STATE_ADDRESS)
                * buffer = userRam->status;
            else
                * buffer = userRam->userRamData[address];
            buffer++;
            address++;
        }
    }
}

byte* BcuDefault::userMemoryPtr(unsigned int addr)
{
    if (addr >= userEeprom->userEepromStart && addr < userEeprom->userEepromEnd)
        return &(*userEeprom)[addr];
    else if (addr >= userRam->userRamStart && addr < (userRam->userRamStart + userRam->userRamSize))
        return &(*userRam)[addr];
    return 0;
}

// The method begin_BCU() is renamed during compilation to indicate the BCU type.
// If you get a link error then the library's BCU_TYPE is different from your application's BCU_TYPE.
void BcuDefault::begin_BCU(int manufacturer, int deviceType, int version)
{

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

void BcuDefault::setProgPin(int prgPin) {
    progPin=prgPin;
    setFatalErrorPin(progPin);
}

void BcuDefault::setProgPinInverted(int prgPinInv) {
    progPinInv=prgPinInv;
}

/**
 * todo check for RX status and inform upper layer if needed
 */
bool BcuDefault::processGroupAddressTelegram(ApciCommand apciCmd, uint16_t groupAddress, unsigned char *telegram, uint8_t telLength)
{
    DB_COM_OBJ(
        serial.println();
        serial.print("BCU grp addr: 0x", (unsigned int)groupAddress, HEX, 4);
        serial.println(" error state:  0x",(unsigned int)bus->receivedTelegramState(), HEX, 4);
    );

    comObjects->processGroupTelegram(groupAddress, apciCmd & APCI_GROUP_MASK, telegram);
    return (true);
}

bool BcuDefault::processBroadCastTelegram(ApciCommand apciCmd, unsigned char *telegram, uint8_t telLength)
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

bool BcuDefault::processApciMemoryWritePDU(int addressStart, byte *payLoad, int lengthPayLoad)
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
    return processApciMemoryOperation(addressStart, payLoad, lengthPayLoad, false);
}

bool BcuDefault::processApciMemoryReadPDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    DB_MEM_OPS(
       serial.print("ApciMemoryReadPDU :", addressStart, HEX, 4);
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

bool BcuDefault::processApciMemoryOperation(unsigned int addressStart, byte *payLoad, int lengthPayLoad, const bool readMem)
{
    const unsigned int addressEnd = addressStart + lengthPayLoad - 1;
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
                    memcpy(&payLoad[0], userEeprom->userEepromData + (addressStart - userEeprom->userEepromStart), addressEnd - addressStart + 1);
                }
                else
                {
                    memcpy(userEeprom->userEepromData + (addressStart - userEeprom->userEepromStart), &payLoad[0], addressEnd - addressStart + 1);
                    userEeprom->modified();
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
                    memcpy(&payLoad[0], userEeprom->userEepromData + (addressStart - userEeprom->userEepromStart), copyCount);
                }
                else
                {
                    memcpy(userEeprom->userEepromData + (addressStart - userEeprom->userEepromStart), &payLoad[0], copyCount);
                    userEeprom->modified();
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
            if (((addressStart >= userRam->userRamStart) && (addressEnd <= userRam->userRamEnd)) ||
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
            else if ((addressStart >= userRam->userRamStart) && (addressStart <= userRam->userRamEnd))
            {
                // start is in UserRAM, but payLoad is too long, we need to cut it down to fit
                const int copyCount = userRam->userRamEnd - addressStart + 1;
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

unsigned char BcuDefault::processApci(ApciCommand apciCmd, const uint16_t senderAddr, const int8_t senderSeqNo, bool *sendResponse, unsigned char *telegram, uint8_t telLength)
{
    uint8_t count;
    uint16_t address;
    uint8_t index;

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

    default:
        sendAckTpu = T_NACK_PDU;  // Command not supported, KNX Spec. 3/3/4 5.5.4 p.26 "TL4 Style 1 Rationalised" No Sending of T_NAK frames
        break;
    }
    return (sendAckTpu);
}

bool BcuDefault::processDeviceDescriptorReadTelegram(int id)
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

bool BcuDefault::processApciMasterResetPDU(unsigned char *telegram, const uint8_t senderSeqNo, uint8_t eraseCode, uint8_t channelNumber)
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
    while (!bus->idle())
        ;
    // send APCI_MASTER_RESET_RESPONSE_PDU
    bus->sendTelegram(sendTelegram, telegramSize(sendTelegram));
    while (!bus->idle())
                ;
    // send disconnect
    sendConControlTelegram(T_DISCONNECT_PDU, connectedTo(), 0);
    while (!bus->idle())
        ;
    softSystemReset();
#endif
    return (true);
}

void BcuDefault::softSystemReset()
{
    if (usrCallback)
    {
        usrCallback->Notify(USR_CALLBACK_RESET);
    }

    userEeprom->writeUserEeprom(); // Flush the EEPROM before resetting

    if (memMapper)
    {
        memMapper->doFlash();
    }
    BcuBase::softSystemReset();
}

