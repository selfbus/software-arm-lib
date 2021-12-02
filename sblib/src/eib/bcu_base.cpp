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
#include <sblib/eib/user_memory.h>
#include <sblib/eib/addr_tables.h>
#include <sblib/internal/functions.h>
#include <sblib/internal/variables.h>
#include <sblib/internal/iap.h>
#include <string.h>

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

// The interrupt handler for the EIB bus access object
BUS_TIMER_INTERRUPT_HANDLER(TIMER16_1_IRQHandler, bus);

extern unsigned int writeUserEepromTime;
extern volatile unsigned int systemTime;

BcuBase::BcuBase()
:progButtonDebouncer()
{
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
    IF_DEBUG(serial.println("BCU_TYPE: 0x", BCU_TYPE, HEX, 2));
    IF_DEBUG(serial.println("MASK_VERSION: 0x", MASK_VERSION, HEX, 2));
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

    userRam.status = BCU_STATUS_LL | BCU_STATUS_TL | BCU_STATUS_AL | BCU_STATUS_USR;
    userRam.deviceControl = 0;
    userRam.runState = 1;

    userEeprom.runError = 0xff;
    userEeprom.portADDR = 0;

    userEeprom.manufacturerH = manufacturer >> 8;
    userEeprom.manufacturerL = manufacturer;

    userEeprom.deviceTypeH = deviceType >> 8;
    userEeprom.deviceTypeL = deviceType;

    userEeprom.version = version;

#if BCU_TYPE != BCU1_TYPE
    unsigned int partID;
    unsigned int useOldSerialStyle;
    byte uniqueID[IAP_UID_LENGTH];

    useOldSerialStyle = 1;
    if (iapReadUID(&uniqueID[0]) == IAP_SUCCESS)
    {
        // https://community.nxp.com/t5/LPC-Microcontrollers/IAP-C-code-example-query/m-p/596139/highlight/true#M22963
        // Unfortunately the details of what go into the 128-bit GUID cannot be disclosed.
        // It can be said, however, that the 128-bit GUIDs are not random, nor are they sequential.
        // Thus to ensure there are no collisions with other devices, the full 128 bits should be used.

        // create a 48bit serial/hash from the 128bit Guid
        useOldSerialStyle = !hashUID(&uniqueID[0], sizeof(uniqueID), &userEeprom.serial[0], sizeof(userEeprom.serial));

        /** //TODO this could be a alternative for ETS checking the serial number
         *         it seems that the first two bytes are the Manufacturer-ID
         *         and setting the last 4 bytes to 0xFF the ETS doesn't check the serial number at all
         *         memset (userEeprom.serial, 0xFF, 6);             // set serial to 0xFFFFFFFFFFFF
         *         userEeprom.serial[0] = userEeprom.manufacturerH; // set Manufacturer-ID
         *         userEeprom.serial[1] = userEeprom.manufacturerL;
         */
    }

    if (useOldSerialStyle)
    {
        iapReadPartID(&partID);
        memcpy (userEeprom.serial, &partID, 4);
        userEeprom.serial[4] = SBLIB_VERSION >> 8;
        userEeprom.serial[5] = SBLIB_VERSION;
    }
    userRam.peiType = 0;     // PEI type: 0=no adapter connected to PEI.
    userEeprom.appType = 0;  // Set to BCU2 application. ETS reads this when programming.
#endif

    writeUserEepromTime = 0;
    enabled = true;
    bus.begin();
    progButtonDebouncer.init(1);
}

void BcuBase::_begin()
{

}

void BcuBase::end()
{
    enabled = false;

    bus.end();
}

void BcuBase::setOwnAddress(int addr)
{
    userEeprom.addrTab[0] = addr >> 8;
    userEeprom.addrTab[1] = addr;
#if BCU_TYPE != BCU1_TYPE
    if (userEeprom.loadState[OT_ADDR_TABLE] == LS_LOADING)
    {
        byte * addrTab =  addrTable() + 1;

        * (addrTab + 0)  = addr >> 8;
        * (addrTab + 1)  = addr;
    }
#endif
    userEeprom.modified();

    bus.ownAddr = addr;
}

void BcuBase::loop()
{
	if (!enabled)
		return;


#ifdef DUMP_TELEGRAMS
	extern unsigned char telBuffer[];
	extern unsigned int telLength ; // db_state;
	extern unsigned int telRXtime;
	extern unsigned int telRXStartTime;
	extern unsigned int telTXStartTime;
	extern unsigned int telRXEndTime;
	extern unsigned int telTXEndTime;
	extern unsigned int telTXAck;
	extern bool telcollision;
	static unsigned int telLastRXEndTime =0;
	static unsigned int telLastTXEndTime =0;

	if (telTXAck)
	{
	    serial.print("TXAck:");
		serial.println(telTXAck, HEX, 2);
		telTXAck = 0;
	}

	if (telTXStartTime)
	{
/*		serial.print(" LTXE:");
		serial.print(telLastTXEndTime, DEC, 9);
		serial.print(", LRXE:");
		serial.print(telLastRXEndTime, DEC, 9);
		serial.println(") ");
*/
		if (telTXEndTime)
		{
			serial.print("TX: ");
			serial.print(telTXStartTime, DEC, 6);
			serial.print(" ");
			serial.print(telTXEndTime, DEC, 6);

			serial.print(" txerror");
			serial.print((unsigned int)bus.sendTelegramState(), HEX, 4);

			//telTXStartTime = 0;
			telLastTXEndTime = telTXEndTime;
			telTXEndTime = 0;
		}
		else
		{
		serial.print("TXS: ");
		serial.print(telTXStartTime, DEC, 6);
		//serial.println(") ");
		//telTXStartTime = 0;
		}

		if (telLastRXEndTime)
		{
			// print time in between last rx-tel and current tx-tel
			serial.print(" RX-TX:");
			serial.print(( telTXStartTime - telLastRXEndTime), DEC, 6);
	//		serial.println(") ");
			telLastRXEndTime = 0;
		}
		if(telLastTXEndTime)
		{
			// print time in between last tx-tel and current tx-tel
			serial.print(" TX-TX:" );
			serial.print(( telTXStartTime - telLastTXEndTime), DEC, 6);
	//		serial.println(") ");
			telLastTXEndTime = 0;
		}
		serial.println();

		telTXStartTime = 0;

	}

	if (telTXEndTime)
	{
		serial.print("TXE:");
		serial.print(telTXEndTime, DEC, 6);
		serial.print(" tx-error:");
		serial.print((unsigned int)bus.sendTelegramState(), HEX, 4);
		serial.println(") ");

		telLastTXEndTime = telTXEndTime;
		telTXEndTime = 0;
	}

	if (telLength > 0)
	{
	    serial.print("RCV:(");
		serial.print(telRXStartTime, DEC, 6 );
		serial.print(" ");
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
			serial.print(" TX-RX:");
			serial.print(( telRXStartTime - telLastTXEndTime), DEC, 6);
			telLastTXEndTime = 0;
		}
		else if(telLastRXEndTime)
		{
			// print time in between last rx-tel and current rx-tel
			serial.print(" RX-RX:");
			serial.print(( telRXStartTime - telLastRXEndTime), DEC, 6);
		//	serial.println(") ");
			telLastRXEndTime = 0;
		}
		serial.print(") ");


		if (telcollision)  serial.print("collision");

		for (unsigned int i = 0; i < telLength; ++i)
		{
			if (i) serial.print(" ");
			serial.print(telBuffer[i], HEX, 2);
		}
		serial.println();
		telLength = 0;
		telLastRXEndTime = telRXEndTime;
		telRXEndTime = 0;
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

	if (bus.telegramReceived() && (!bus.sendingTelegram()) && (bus.state == Bus::IDLE) && (userRam.status & BCU_STATUS_TL))
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
			userRam.status ^= 0x81;  // toggle programming mode and checksum bit
		}
		pinMode(progPin, OUTPUT);
		digitalWrite(progPin, (userRam.status & BCU_STATUS_PROG) ^ progPinInv);
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
        userRam.status |= 0x81;  // set programming mode and checksum bit
    }
    else
    {
        userRam.status &= 0x81;  // clear programming mode and checksum bit
    }
    pinMode(progPin, OUTPUT);
    digitalWrite(progPin, (userRam.status & BCU_STATUS_PROG) ^ progPinInv);
    return true;
}

void BcuBase::processTelegram()
{
}
