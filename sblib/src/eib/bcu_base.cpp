/*
 *  bcu_base.cpp
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/io_pin_names.h>
#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/bcu_base.h>
#include <sblib/eib/addr_tables.h>
#include <sblib/internal/variables.h>
#include <sblib/internal/iap.h>
#include <sblib/eib/userEeprom.h>
#include <sblib/eib/userRam.h>
#include <sblib/eib/apci.h>
#include <sblib/utils.h>
#include <string.h>
#include <sblib/eib/bus.h>

static Bus* timerBusObj;
// The interrupt handler for the EIB bus access object
BUS_TIMER_INTERRUPT_HANDLER(TIMER16_1_IRQHandler, (*timerBusObj))

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

extern volatile unsigned int systemTime;

BcuBase::BcuBase(UserRam* userRam, AddrTables* addrTables) :
        TLayer4(maxTelegramSize()),
        bus(new Bus(this, timer16_1, PIN_EIB_RX, PIN_EIB_TX, CAP0, MAT0)),
        progPin(PIN_PROG),
        progPinInv(true),
        userRam(userRam),
        addrTables(addrTables),
        comObjects(nullptr),
        progButtonDebouncer()
{
    timerBusObj = bus;
    setFatalErrorPin(progPin);
    setKNX_TX_Pin(bus->txPin);
}

void BcuBase::_begin()
{
    TLayer4::_begin();
    bus->begin();
    progButtonDebouncer.init(1);
}

void BcuBase::loop()
{
    TLayer4::loop();
    bool telegramInQueu = bus->telegramReceived();
    telegramInQueu &= (!bus->sendingTelegram());
	if (telegramInQueu && (bus->state == Bus::IDLE) && (userRam->status() & BCU_STATUS_TRANSPORT_LAYER))
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
			userRam->status() ^= 0x81;  // toggle programming mode and checksum bit
		}
		pinMode(progPin, OUTPUT);
		digitalWrite(progPin, (userRam->status() & BCU_STATUS_PROGRAMMING_MODE) ^ progPinInv);
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
        userRam->status() |= 0x81;  // set programming mode and checksum bit
    }
    else
    {
        userRam->status() &= 0x81;  // clear programming mode and checksum bit
    }
    pinMode(progPin, OUTPUT);
    digitalWrite(progPin, (userRam->status() & BCU_STATUS_PROGRAMMING_MODE) ^ progPinInv);
    return true;
}

void BcuBase::sendApciIndividualAddressReadResponse()
{
    initLpdu(sendTelegram, PRIORITY_SYSTEM, false, FRAME_STANDARD);
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    setDestinationAddress(sendTelegram, 0x0000); // Zero target address, it's a broadcast
    sendTelegram[5] = 0xe0 + 1; // address type & routing count in high nibble + response length in low nibble
    setApciCommand(sendTelegram, APCI_INDIVIDUAL_ADDRESS_RESPONSE_PDU, 0);

    // on a productive bus without this delay, a lot of "more than one device is in programming mode" errors can occur
    // 03.03.2022 tested 10 different licensed KNX-devices (MDT, Gira, Siemens, Merten), only one MDT line-coupler had also this problem
    // other devices responded with a delay of 8-40 milliseconds, so we go for ~5ms + 2*4ms = ~13ms
    // 4.000usec is max. we can delay with delayMicroseconds, so call it twice
    delayMicroseconds(MAX_DELAY_MICROSECONDS);
    delayMicroseconds(MAX_DELAY_MICROSECONDS);
    delayMicroseconds(MAX_DELAY_MICROSECONDS - ((systemTime % (MAX_DELAY_MICROSECONDS/100)) * 10)); // "randomize response delay"
    bus->sendTelegram(sendTelegram, 8);
}

void BcuBase::end()
{
    enabled = false;
    bus->end();
}

bool BcuBase::programmingMode() const
{
    return (userRam->status() & BCU_STATUS_PROGRAMMING_MODE) == BCU_STATUS_PROGRAMMING_MODE;
}

int BcuBase::maxTelegramSize()
{
    return 23;
}

void BcuBase::discardReceivedTelegram()
{
    bus->discardReceivedTelegram();
}

void BcuBase::send(unsigned char* telegram, unsigned short length)
{
    bus->sendTelegram(telegram, length);
}

void BcuBase::softSystemReset()
{
    NVIC_SystemReset();
}

void BcuBase::setProgPin(int prgPin) {
    progPin=prgPin;
    setFatalErrorPin(progPin);
}

void BcuBase::setProgPinInverted(int prgPinInv) {
    progPinInv=prgPinInv;
}

