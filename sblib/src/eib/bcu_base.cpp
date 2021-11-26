/*
 *  bcu.h - EIB bus coupling unit.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#define INSIDE_BCU_CPP
#include <sblib/io_pin_names.h>
#include <sblib/eib/bcu_base.h>
#include <sblib/eib/addr_tables.h>
#include <sblib/internal/variables.h>
#include <sblib/internal/iap.h>
#include <sblib/eib/userEeprom.h>
#include <sblib/eib/userRam.h>
#include <sblib/eib/apci.h>
#include <string.h>
#include <sblib/eib/bus.h>

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

static Bus* timerBusObj;
// The interrupt handler for the EIB bus access object
BUS_TIMER_INTERRUPT_HANDLER(TIMER16_1_IRQHandler, (*timerBusObj));

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

// Enable informational debug statements
#if defined(INCLUDE_SERIAL)
#   define DB(x) x
#else
#   define DB(x)
#endif

extern volatile unsigned int systemTime;

BcuBase::BcuBase(UserRam* userRam, UserEeprom* userEeprom, ComObjects* comObjects, AddrTables* addrTables) :
		sendTelegram(new byte[TelegramSize()]),
		userEeprom(userEeprom),
		userRam(userRam),
		comObjects(comObjects),
		addrTables(addrTables),
		bus(new Bus(this, timer16_1, PIN_EIB_RX, PIN_EIB_TX, CAP0, MAT0)),
		progButtonDebouncer()
{
	timerBusObj = bus;
    progPin = PIN_PROG;
    setFatalErrorPin(progPin);
    progPinInv = true;
    enabled = false;
}

/*
 * Creates a len_hash wide hash of the uid.
 * Hash will be generated in provided hash buffer
 */
int BcuBase::hashUID(byte* uid, const int len_uid, byte* hash, const int len_hash)
{
    const int MAX_HASH_WIDE = 16;
    uint64_t BigPrime48 = 281474976710597u;  // FF FF FF FF FF C5
    uint64_t a, b;
    unsigned int mid;

    if ((len_uid <= 0) || (len_uid > MAX_HASH_WIDE))  // maximum of 16 bytes can be hashed by this function
        return 0;
    if ((len_hash <= 0) || (len_hash > len_uid))
        return 0;

    mid = len_uid/2;
    memcpy (&a, &uid[0], mid);          // copy first half of uid-bytes to a
    memcpy (&b, &uid[mid], len_uid-mid); // copy second half of uid-bytes to b

    // do some modulo a big primenumber
    a = a % BigPrime48;
    b = b % BigPrime48;
    a = a^b;
    // copy the generated hash to provided buffer
    for (int i = 0; i<len_hash; i++)
        hash[i] = uint64_t(a >> (8*i)) & 0xFF;
    return 1;
}

void BcuBase::loop()
{
	if (!enabled)
		return;


#ifdef DUMP_TELEGRAMS
	extern unsigned char telBuffer[];
	extern unsigned int telLength ; // db_state;
	extern unsigned int telRXtime;
	extern bool telcollision;

	if (telLength > 0)
	{
	    serial.print("RCV: (");

		serial.print(telRXtime, DEC, 8);
		serial.print(") ");
		if (telcollision)  serial.print("collision ");

		for (unsigned int i = 0; i < telLength; ++i)
		{
			if (i) serial.print(" ");
			serial.print(telBuffer[i], HEX, 2);
		}
		serial.println();
		telLength = 0;
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

	if (bus->telegramReceived() && (!bus->sendingTelegram()) && (bus->state == Bus::IDLE) && (userRam->status & BCU_STATUS_TL))
	{
		processTelegram();
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

    // if bus is not sending (telegramm buffer is empty) check for next telegram to be send
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

    // Send a disconnect after 6 seconds inactivity
    if (connectedAddr && elapsed(connectedTime) > 6000)
    {
        sendConControlTelegram(T_DISCONNECT_PDU, 0);
        connectedAddr = 0;
    }

    if (userEeprom->isModified() && bus->idle() && bus->telegramLen == 0 && connectedAddr == 0)
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

bool BcuBase::setProgrammingMode(bool newMode)
{
    if (!progPin)
    {
        return false;
    }

    if (newMode)
    {
        userRam->status |= 0x81;  // set programming mode and checksum bit
    }
    else
    {
        userRam->status &= 0x81;  // clear programming mode and checksum bit
    }
    pinMode(progPin, OUTPUT);
    digitalWrite(progPin, (userRam->status & BCU_STATUS_PROG) ^ progPinInv);
    return true;
}

void BcuBase::processConControlTelegram(int tpci)
{
    int senderAddr = (bus->telegram[1] << 8) | bus->telegram[2];

    if (tpci & 0x40)  // An acknowledgement
    {
        tpci &= 0xc3;
        if (tpci == T_ACK_PDU) // A positive acknowledgement
        {
            int curSeqNo = bus->telegram[6] & 0x3c;
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
                //bus->setSendAck (0); // todo check in spec if needed
                // KNX 3/3/4 3.8 shall try to send the T_CONNECT_REQ_PDU to the remote Transport Layer
            }
        }
        else if (tpci == T_DISCONNECT_PDU)  // Close the direct data connection
        {
            if (connectedAddr == senderAddr)
            {
                connectedAddr = 0;
                //bus->setSendAck (0);  // todo check in spec if needed
                // KNX 3/3/4 3.8 The T_Disconnect service shall neither be acknowledged nor confirmed by the remote Transport Layer entity.
            }
        }
    }
}

bool BcuBase::processApciMemoryReadPDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    DB(serial.print("ApciMemoryReadPDU :", addressStart, HEX, 4);
       serial.print(" count: ", lengthPayLoad, DEC);
    );

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

bool BcuBase::processApciMemoryWritePDU(int addressStart, byte *payLoad, int lengthPayLoad)
{
    DB(serial.print("ApciMemoryWritePDU:", addressStart, HEX, 4);
       serial.print(" Data:");
       for(int i=0; i<lengthPayLoad ; i++)
       {
           serial.print(" ", payLoad[i], HEX, 2);
       }
       serial.print(" count: ", lengthPayLoad, DEC);
    );

    return processApciMemoryOperation(addressStart, payLoad, lengthPayLoad, false);
}

bool BcuBase::processApciMemoryOperation(int addressStart, byte *payLoad, int lengthPayLoad, const bool readMem)
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
                DB(serial.println(" -> EEPROM ", addressEnd - addressStart + 1, DEC));
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
                DB(serial.println(" -> EEPROM processed: ", copyCount, DEC));
            }
        }

        // check if payLoad is in UserRam
        if (addressStart <= addressEnd)
        {
            if ((addressStart >= userRam->userRamStart) && (addressEnd <= userRam->userRamEnd))
            {
                // start & end are in UserRAM
                if (readMem)
                    cpyFromUserRam(addressStart, &payLoad[0], addressEnd - addressStart + 1);
                else
                    cpyToUserRam(addressStart, &payLoad[0], addressEnd - addressStart + 1);
                DB(serial.println(" -> UserRAM ", addressEnd - addressStart + 1, DEC));
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

bool BcuBase::processDeviceDescriptorReadTelegram(int id)
{
    if (id == 0)
    {
        int version = getMaskVersion();

        sendTelegram[5] = 0x63;
        sendTelegram[6] = 0x43;
        sendTelegram[7] = 0x40;
        sendTelegram[8] = version >> 8;
        sendTelegram[9] = version;
        return true;
    }

    return false; // unknown device descriptor
}

void BcuBase::sendConControlTelegram(int cmd, int senderSeqNo)
{
    if (cmd & 0x40)  // Add the sequence number if the command shall contain it
        cmd |= senderSeqNo & 0x3c;

    sendCtrlTelegram[0] = 0xb0 | (bus->telegram[0] & 0x0c); // Control byte
    // 1+2 contain the sender address, which is set by bus->sendTelegram()
    sendCtrlTelegram[3] = connectedAddr >> 8;
    sendCtrlTelegram[4] = connectedAddr;
    sendCtrlTelegram[5] = 0x60;
    sendCtrlTelegram[6] = cmd;

    bus->sendTelegram(sendCtrlTelegram, 7);
}


/**
 * BUS process has receive a telegram - now process it
 *
 * called from BCUBase-loop
 *
 * todo check for RX status and inform upper layer if needed
 *
 */
void BcuBase::processTelegram()
{
    unsigned short destAddr = (bus->telegram[3] << 8) | bus->telegram[4];
    unsigned char tpci = bus->telegram[6] & 0xc3; // Transport control field (see KNX 3/3/4 p.6 TPDU)
    unsigned short apci = ((bus->telegram[6] & 3) << 8) | bus->telegram[7];

    DB(serial.println());
	DB(serial.print("BCU1 grp addr: ");)
	DB(serial.print((unsigned int)destAddr, HEX, 4);)
	DB(serial.print(" error state:  ");)
	DB(serial.println((unsigned int)bus->receivedTelegramState(), HEX, 4);)

    if (destAddr == 0) // a broadcast
    {
        if (programmingMode()) // we are in programming mode
        {
            if (apci == APCI_INDIVIDUAL_ADDRESS_WRITE_PDU)
            {
                setOwnAddress((bus->telegram[8] << 8) | bus->telegram[9]);
            }
            else if (apci == APCI_INDIVIDUAL_ADDRESS_READ_PDU)
            {
                sendTelegram[0] = 0xb0 | (bus->telegram[0] & 0x0c); // Control byte
                // 1+2 contain the sender address, which is set by bus->sendTelegram()
                sendTelegram[3] = 0x00;  // Zero target address, it's a broadcast
                sendTelegram[4] = 0x00;
                sendTelegram[5] = 0xe1;
                sendTelegram[6] = 0x01;  // APCI_INDIVIDUAL_ADDRESS_RESPONSE_PDU (0x0140)
                sendTelegram[7] = 0x40;
                bus->sendTelegram(sendTelegram, 8);
            }
        }
    }
    else if ((bus->telegram[5] & 0x80) == 0) // a physical destination address
    {
        if (destAddr == ownAddr) // it's our physical address
        {
            if (tpci & 0x80)  // A connection control command
            {
                processConControlTelegram(bus->telegram[6]);
            }
            else
            {
                processDirectTelegram(apci);
            }
        }
    }
    else if (tpci == T_GROUP_PDU) // a group destination address and multicast
    {
        comObjects->processGroupTelegram(destAddr, apci & APCI_GROUP_MASK, bus->telegram);
    }

    // At the end: discard the received telegram
    bus->discardReceivedTelegram();
}

void BcuBase::_begin()
{
    userEeprom->readUserEeprom();
    sendGrpTelEnabled = true;
    groupTelSent = millis();

    // set limit to max of 28 telegrams per second (wait 35ms) -  to avoid risk of thermal destruction of the sending circuit
    groupTelWaitMillis = DEFAULT_GROUP_TEL_WAIT_MILLIS ;
}

void BcuBase::end()
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

void BcuBase::cpyToUserRam(unsigned int address, unsigned char * buffer, unsigned int count)
{
    address -= userRam->userRamStart;
    if ((address > 0x60) || ((address + count) < 0x60))
    {
        memcpy((void*)(userRam->userRamData + address), buffer, count);
    }
    else
    {
        while (count--)
        {
            if (address == 0x60)
                userRam->status = * buffer;
            else
                userRam->userRamData[address] = * buffer;
            buffer++;
            address++;
        }
    }
}

void BcuBase::cpyFromUserRam(unsigned int address, unsigned char * buffer, unsigned int count)
{
    address -= userRam->userRamStart;
    if ((address > 0x60) || ((address + count) < 0x60))
    {
        memcpy(buffer, (const void*)(userRam->userRamData + address), count);
    }
    else
    {
        while (count--)
        {
            if (address == 0x60)
                * buffer = userRam->status;
            else
                * buffer = userRam->userRamData[address];
            buffer++;
            address++;
        }
    }
}

byte* BcuBase::userMemoryPtr(int addr)
{
    if (addr >= userEeprom->userEepromStart && addr < userEeprom->userEepromEnd)
        return userEeprom->userEepromData + (addr - userEeprom->userEepromStart);
    else if (addr >= userRam->userRamStart && addr < (userRam->userRamStart + userRam->userRamSize))
        return userRam->userRamData + (addr - userRam->userRamStart);
    return 0;
}

// The method begin_BCU() is renamed during compilation to indicate the BCU type.
// If you get a link error then the library's BCU_TYPE is different from your application's BCU_TYPE.
void BcuBase::begin_BCU(int manufacturer, int deviceType, int version)
{
    _begin();
#if defined(INCLUDE_SERIAL)
    IF_DEBUG(serial.begin(SERIAL_SPEED));
#endif

#ifdef DUMP_TELEGRAMS
    IF_DEBUG(serial.println("Telegram dump enabled."));
#endif

#ifdef DUMP_PROPERTIES
    IF_DEBUG(serial.println("Properties dump enabled."));
#endif

#ifdef DEBUG_BUS
    IF_DEBUG(serial.println("DEBUG_BUS dump enabled."));
#endif

#ifdef DEBUG_BUS_BITLEVEL
    IF_DEBUG(serial.println("DEBUG_BUS_BITLEVEL dump enabled."));
#endif

#if defined(DUMP_TELEGRAMS) || defined (DUMP_PROPERTIES)
    IF_DEBUG(serial.print("BCU_NAME: "));
    IF_DEBUG(serial.println(getBcuType()));
    IF_DEBUG(serial.println("MASK_VERSION: 0x", getMaskVersion(), HEX, 2));
#   ifdef LOAD_CONTROL_ADDR
        IF_DEBUG(serial.println("LOAD_CONTROL_ADDR: 0x", LOAD_CONTROL_ADDR, HEX, 4));
#   endif
#   ifdef LOAD_STATE_ADDR
        IF_DEBUG(serial.println("LOAD_STATE_ADDR: 0x", LOAD_STATE_ADDR, HEX, 4));
#   endif
#   ifdef USER_RAM_START_DEFAULT
        IF_DEBUG(serial.println("USER_RAM_START_DEFAULT: 0x", USER_RAM_START_DEFAULT, HEX, 4));
#   endif
#   ifdef EXTRA_USER_RAM_SIZE
        IF_DEBUG(serial.println("EXTRA_USER_RAM_SIZE: 0x", EXTRA_USER_RAM_SIZE, HEX, 4));
#   endif
#   ifdef USER_RAM_SIZE
        IF_DEBUG(serial.println("USER_RAM_SIZE: 0x", USER_RAM_SIZE, HEX, 4));
#   endif
#   ifdef USER_RAM_SHADOW_SIZE
        IF_DEBUG(serial.println("USER_RAM_SHADOW_SIZE: 0x", USER_RAM_SHADOW_SIZE, HEX, 4));
#   endif
#   ifdef USER_EEPROM_START
        IF_DEBUG(serial.println("USER_EEPROM_START: 0x", USER_EEPROM_START, HEX, 4));
#   endif
#   ifdef USER_EEPROM_SIZE
        IF_DEBUG(serial.println("USER_EEPROM_SIZE: 0x", USER_EEPROM_SIZE, HEX, 4));
#   endif
#   ifdef USER_EEPROM_END
        IF_DEBUG(serial.println("USER_EEPROM_END: 0x", USER_EEPROM_END, HEX, 4));
#   endif
#   ifdef USER_EEPROM_FLASH_SIZE
        IF_DEBUG(serial.println("USER_EEPROM_FLASH_SIZE: 0x", USER_EEPROM_FLASH_SIZE, HEX, 4));
#   endif
    IF_DEBUG(serial.println());
	_begin(); // load flash/rom data to usereeprom, init bcu

#endif

    // set sending buffer to free
    sendTelegram[0] = 0;
    sendCtrlTelegram[0] = 0;

    connectedSeqNo = 0;
    incConnectedSeqNo = false;
    lastAckSeqNo = -1;

    connectedAddr = 0;

    enabled = true;
    bus->begin();
    progButtonDebouncer.init(1);
}

bool BcuBase::programmingMode()
{
    return (userRam->status & BCU_STATUS_PROG) == BCU_STATUS_PROG;
}

int BcuBase::ownAddress() const
{
    return ownAddr;
}

bool BcuBase::directConnection() const
{
    return connectedAddr != 0;
}

word BcuBase::getCommObjectTableAddressStatic() const
{
    return commObjectTableAddressStatic;
}

int BcuBase::connectedTo()
{
    return connectedAddr;
}

void BcuBase::setMemMapper(MemMapper *mapper)
{
    memMapper = mapper;
}

MemMapper* BcuBase::getMemMapper()
{
    return memMapper;
}

void BcuBase::setUsrCallback(UsrCallback *callback)
{
    usrCallback = callback;
}

void BcuBase::enableGroupTelSend(bool enable)
{
    sendGrpTelEnabled = enable;
}
/**
 * The sending circuit of the Controller has limited capability for sending low bits.
 *
 * In order to avoid a thermal destruction (more than 1W power dissipation of the Transistor and 150mW of the 5R6 Resistor
 * a limit of max 28 telegrams per second should be set (assuming an average distribution of 50% low bits within telegrams
 * of max length).
 *
 * @param limit
 */
void BcuBase::setGroupTelRateLimit(unsigned int limit)
{
 if ((limit > 0) && (limit <= MAX_GROUP_TEL_PER_SECOND))
     groupTelWaitMillis = 1000/limit;
 else
     groupTelWaitMillis = DEFAULT_GROUP_TEL_WAIT_MILLIS ;
}

int BcuBase::TelegramSize()
{
	return 24;
}

/**
 * Set RxPin of board, must be called before begin method
 * @param rxPin pin definition
 */
void BcuBase::setRxPin(int rxPin) {
    bus->rxPin=rxPin;
}
/**
 * Set TxPin of board, must be called before begin method
 * @param txPin pin definition
 */
void BcuBase::setTxPin(int txPin) {
    bus->txPin=txPin;
}
/**
 * Set timer class, must be called before begin method
 * @param timer
 */
void BcuBase::setTimer(Timer& timer) {
    bus->timer=timer;
}
/**
 * Set capture channel of processor, must be called before begin method
 * @param capture channel definition of processor
 */
void BcuBase::setCaptureChannel(TimerCapture captureChannel) {
    bus->captureChannel=captureChannel;
}
/**
 * Set ProgPin of board, must be called before begin method
 * @param progPin Pin definition
 */
void BcuBase::setProgPin(int prgPin) {
    progPin=prgPin;
    setFatalErrorPin(progPin);
}
/**
 * Set ProgPin output inverted, must be called before begin method
 * @param progPin output inverted
 */
void BcuBase::setProgPinInverted(int prgPinInv) {
    progPinInv=prgPinInv;
}
