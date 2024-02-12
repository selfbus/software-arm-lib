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
#include <sblib/eib/bus.h>

static Bus* timerBusObj;
// The interrupt handler for the EIB bus access object
BUS_TIMER_INTERRUPT_HANDLER(TIMER16_1_IRQHandler, (*timerBusObj))

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

BcuBase::BcuBase(UserRam* userRam, AddrTables* addrTables) :
        TLayer4(maxTelegramSize()),
        bus(new Bus(this, timer16_1, PIN_EIB_RX, PIN_EIB_TX, CAP0, MAT0)),
        progPin(PIN_PROG),
        progPinInv(true),
        userRam(userRam),
        addrTables(addrTables),
        comObjects(nullptr),
        progButtonDebouncer(),
        restartType(RestartType::None),
        restartDisconnectTimeout(Timeout())
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
    bus->loop();
    TLayer4::loop();

    if (bus->telegramReceived() && !bus->sendingTelegram() && (userRam->status() & BCU_STATUS_TRANSPORT_LAYER))
	{
        processTelegram(bus->telegram, (uint8_t)bus->telegramLen); // if processed successfully, received telegram will be discarded by processTelegram()
	}

	if (progPin)
	{
		// Detect the falling edge of pressing the prog button
		pinMode(progPin, INPUT|PULL_UP);
		int oldValue = progButtonDebouncer.value();
		if (!progButtonDebouncer.debounce(digitalRead(progPin), 50) && oldValue)
		{
			userRam->status() ^= BCU_STATUS_PARITY | BCU_STATUS_PROGRAMMING_MODE;  // toggle programming mode and parity bit
		}
		pinMode(progPin, OUTPUT);
		digitalWrite(progPin, programmingMode() ^ progPinInv);
	}

    // Rest of this function is only relevant if currently able to send another telegram.
    if (bus->sendingTelegram())
    {
        return;
    }

    if (restartType != RestartType::None)
    {
        // Tests require inspection of the sent telegram before calling softSystemReset().
        // So instead of calling the method after disconnect() in the same loop iteration,
        // let's defer that to the next iteration by moving it to an otherwise unneeded
        // else block.
        if (directConnection())
        {
            // KNX spec v2.1 chapter 3/5/2 sections 3.7.1.1 p.63 and 3.7.3 p. 72 say the
            // Management Server should send a T_DISCONNECT and the Management Client
            // should send one as well. So wait a bit to receive one, and only send one
            // out if the connection is not terminated by the client.
            // Although the spec clearly says that clients should ignore T_DISCONNECT
            // messages as well as errors, calimero warns about "negative confirmation"
            // frames in this case. So be nice and try to avoid these warnings.
            if (restartDisconnectTimeout.expired())
            {
                disconnect();
            }
        }
        else
        {
            softSystemReset();
        }
    }
}

bool BcuBase::setProgrammingMode(bool newMode)
{
    if (!progPin)
    {
        return false;
    }

    if (newMode != programmingMode())
    {
        userRam->status() ^= BCU_STATUS_PARITY | BCU_STATUS_PROGRAMMING_MODE;  // toggle programming mode and parity bit
    }
    pinMode(progPin, OUTPUT);
    digitalWrite(progPin, programmingMode() ^ progPinInv);
    return true;
}

bool BcuBase::processApci(ApciCommand apciCmd, unsigned char * telegram, uint8_t telLength, uint8_t * sendBuffer)
{
    switch (apciCmd)
    {
        case APCI_BASIC_RESTART_PDU:
            scheduleRestart(RestartType::Basic);
            return (false);
        default:
            return (TLayer4::processApci(apciCmd, telegram, telLength, sendBuffer));
    }
    return (false);
}

void BcuBase::sendApciIndividualAddressReadResponse()
{
    auto sendBuffer = acquireSendBuffer();
    initLpdu(sendBuffer, PRIORITY_SYSTEM, false, FRAME_STANDARD);
    // 1+2 contain the sender address, which is set by bus.sendTelegram()
    setDestinationAddress(sendBuffer, 0x0000); // Zero target address, it's a broadcast
    sendBuffer[5] = 0xe0 + 1; // address type & routing count in high nibble + response length in low nibble
    setApciCommand(sendBuffer, APCI_INDIVIDUAL_ADDRESS_RESPONSE_PDU, 0);
    sendPreparedTelegram();
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

void BcuBase::scheduleRestart(RestartType type)
{
    restartType = type;
    restartDisconnectTimeout.start(500);
}

void BcuBase::softSystemReset()
{
    bus->end();

    // Set magicWord to start in bootloader mode after reset.
    // As this overwrites the start of the interrupt vector table, disable interrupts.
    if (restartType == RestartType::MasterIntoBootloader)
    {
        noInterrupts();
#ifndef IAP_EMULATION
        unsigned int * magicWord = BOOTLOADER_MAGIC_ADDRESS;
        *magicWord = BOOTLOADER_MAGIC_WORD;
#endif
    }

    NVIC_SystemReset();
}

void BcuBase::setProgPin(int prgPin) {
    progPin=prgPin;
    setFatalErrorPin(progPin);
}

void BcuBase::setProgPinInverted(int prgPinInv) {
    progPinInv=prgPinInv;
}

