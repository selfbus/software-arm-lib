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

#ifdef DUMP_TELEGRAMS
#include <sblib/serial.h>
#endif

// The interrupt handler for the EIB bus access object
BUS_TIMER_INTERRUPT_HANDLER(TIMER16_1_IRQHandler, bus);

extern unsigned int writeUserEepromTime;
extern volatile unsigned int systemTime;

BcuBase::BcuBase()
:progButtonDebouncer()
{
    progPin = PIN_PROG;
    progPinInv = true;
    enabled = false;
}

// creates from a 128bit uid a 48bit hash
void hashUIDtoSerial(byte* uid, int len_uid, int len_serial)
{
    uint64_t BigPrime48 = 281474976710597u;  // FF FF FF FF FF C5
    uint64_t a, b;
    unsigned int middle, shiftby;
    middle = (len_uid/2);

    memcpy (&a, &uid[0], len_uid/2); // copy first half of uid-bytes to a
    memcpy (&b, &uid[len_uid/2], len_uid/2); // copy second half of uid-bytes to b

    // do some modulo a big primenumber
    a = a % BigPrime48;
    b = b % BigPrime48;
    a = a^b;
    // copy the generated hash back to uid
    for (int i = 0; i<len_serial; i++)
    {
        uid[i] = uint64_t(a >> (8*i)) & 0xFF;
    }
    for (int i = len_serial; i<len_uid; i++)
    {
         uid[i] = 0x00;
    }
}

// The method begin_BCU() is renamed during compilation to indicate the BCU type.
// If you get a link error then the library's BCU_TYPE is different from your application's BCU_TYPE.
void BcuBase::begin_BCU(int manufacturer, int deviceType, int version)
{
	_begin();
#ifdef DUMP_TELEGRAMS
    serial.begin(115200);
    serial.println("Telegram dump enabled");
#endif

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
    byte uniqueID[16];
    if (iapReadUID(&uniqueID[0]) == IAP_SUCCESS)
    {
        // https://community.nxp.com/t5/LPC-Microcontrollers/IAP-C-code-example-query/m-p/596131
        // Unfortunately the details of what go into the 128-bit GUID cannot be disclosed.
        // It can be said, however, that the 128-bit GUIDs are not random, nor are they sequential.
        // Thus to ensure there are no collisions with other devices, the full 128 bits should be used.
        hashUIDtoSerial(&uniqueID[0], sizeof(uniqueID), 6);
        memcpy (&userEeprom.serial, &uniqueID, sizeof(userEeprom.serial));
    }
    else
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
	{
    	extern unsigned char telBuffer[];
    	extern unsigned int telLength ;
    	if (telLength > 0)
    	{
    		serial.print("RCV: ");
            for (int i = 0; i < telLength; ++i)
            {
                if (i) serial.print(" ");
                serial.print(telBuffer[i], HEX, 2);
            }
            serial.println();
            telLength = 0;
    	}
	}
#endif

    if (bus.telegramReceived() && !bus.sendingTelegram() && (userRam.status & BCU_STATUS_TL))
        processTelegram();

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

void BcuBase::processTelegram()
{
}
