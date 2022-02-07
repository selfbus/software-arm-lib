/*
 *  protocol.cpp - protocol tests
 *
 *  Copyright (C) 2014-2015 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include "protocol.h"
#include "sblib/internal/variables.h"
#include <sblib/internal/iap.h>

extern byte userEepromModified;
extern int sndStartIdx;
extern unsigned int wfiSystemTimeInc;

/**
 * @fn void _handleRx(Test_Case*, Telegram*, unsigned int)
 * @brief handle a simulated telegram received from the bus
 *
 * @param tc
 * @param tel
 * @param tn
 */
static void _handleRx(Test_Case * tc, Telegram * tel, unsigned int tn)
{
	memcpy(bus.telegram, tel->bytes, tel->length);
	bus.telegramLen = tel->length;
    if ((bus.state == Bus::INIT) || (bus.state == Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE))
    {
        bus.timerInterruptHandler(); // move the Isr "forward"
    }
	bcu.processTelegram();
	REQUIRE(bus.telegramLen == 0);
}

static void _handleTx(Test_Case * tc, Telegram * tel, unsigned int tn)
{
    int i;
    int mismatches = 0;
    char msg[1025];
    char numbers[23 * 3 + 1] = { 0 };
    char received[23 * 3 + 1] = { 0 };
    char expected[23 * 3 + 1] = { 0 };
    char temp[1025];

    bus.timerInterruptHandler();
    snprintf(msg, 1024, "%s: Number of bytes in send telegram %d expected %d, sent %d", tc->name, tn,
        tel->length, bus.sendTelegramLen - 1);
    INFO(msg);
    REQUIRE(tel->length == (bus.sendTelegramLen - 1));

    snprintf(msg, 1024, "%s: Send telegram %d mismatch at byte(s) ", tc->name, tn);
    for (i = 0; i < tel->length; i++)
    {
        snprintf(temp, 255, " %2d", i + 1);
        strcat(numbers, temp);
        snprintf(temp, 255, " %02x", bus.sendCurTelegram[i]);
        strcat(received, temp);
        snprintf(temp, 255, " %02x", tel->bytes[i]);
        strcat(expected, temp);
        if (tel->bytes[i] != bus.sendCurTelegram[i])
        {
            mismatches++;
            snprintf(temp, 1024, "%d, ", i + 1);
            strcat(msg, temp);
        }
    }
    msg[strlen(msg) - 2] = '\n'; // remove the last ', ' sequence
    snprintf(temp, 1024, "          %s\n expected: %s\n sent:     %s", numbers, expected, received);
    strcat(msg, temp);
    INFO(msg);
    REQUIRE(mismatches == 0);

    bus.state = Bus::SEND_END_OF_TX;
    bus.timerInterruptHandler();
    if (bus.state == Bus::SEND_WAIT_FOR_RX_ACK_WINDOW) { // device request an ACK -> so inject one
        ///\todo maybe need to save control byte, otherwise the test can override telegram priority
        // byte telegramControlByte = bus.telegram[0];
        bus.telegram[0]  = bus.currentByte = SB_BUS_ACK;
        bus.telegramLen  = tel->length;
    	bus.state        = Bus::RECV_WAIT_FOR_STARTBIT_OR_TELEND;
    	bus.nextByteIndex = 1;
    	bus.parity        = 1;
    	_LPC_TMR16B1.IR  = 0;
        bus.timerInterruptHandler ();
        // bus.telegram[0] = telegramControlByte; // restore the state
    }
    REQUIRE(bus.sendNextTel == NULL);
}

static void _handleCheckTx(Test_Case * tc, Telegram * tel, unsigned int tn)
{
    unsigned int s = 0;
    if (bus.sendCurTelegram) s++;
    if (bus.sendNextTel) s++;
    INFO("Check if additional telegrams should be sent");
    REQUIRE(s == tel->variable);
}

static void _handleTime(Test_Case * tc, Telegram * tel, unsigned int tn)
{
	unsigned int s = 0;
    if (bus.sendCurTelegram) s++;
    if (bus.sendNextTel) s++;
    //INFO("Ensure that no outgoing telegram is in the queue");
    if (s)
    {
        int i;
        char msg[1025];
        char numbers[23 * 3 + 1] = { 0 };
        char received[23 * 3 + 1] = { 0 };
        char temp[1025];

        bus.timerInterruptHandler();
        snprintf(msg, 1024, "Unexpected telegram\n");
        for (i = 0; i < bus.sendTelegramLen - 1; i++)
        {
            snprintf(temp, 255, " %2d", i + 1);
            strcat(numbers, temp);
            snprintf(temp, 255, " %02x", bus.sendCurTelegram[i]);
            strcat(received, temp);
        }
        snprintf(temp, 1024, "          %s\n sent:     %s", numbers, received);
        strcat(msg, temp);
        FAIL(msg);
    }
    systemTime += tel->length;
}

static unsigned int _handleBreak (Test_Case * tc, Telegram * tel, unsigned int tn)
{
    if (tel->variable)
        return tn;
    return tn - 1;
}

static void _handleLoop (Test_Case * tc, Telegram * tel, unsigned int tn)
{
    for (unsigned int i = 0; i < tel->variable; i++)
    {
        if ((bus.state == Bus::INIT) || (bus.state == Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE))
        {
            bus.timerInterruptHandler(); // move the Isr "forward"
        }
        bcu.loop();
    }
}

void executeTest(Test_Case * tc)
{
    Telegram * tel = tc->telegram;
    unsigned int tn = 1;
    void * refState  = tc->refState;
    void * stepState = tc->stepState;

    IAP_Init_Flash(0xFF);
    if(tc->eepromSetup) tc->eepromSetup();
    memcpy(FLASH_BASE_ADDRESS + iapFlashSize() - FLASH_SECTOR_SIZE, userEepromData, USER_EEPROM_SIZE);
    bcu.begin(tc->manufacturer, tc->deviceType, tc->version);
    bus.timerInterruptHandler(); // move the ISR out of INIT state
    sndStartIdx = 0;
    systemTime  = 0;
    wfiSystemTimeInc = 1;
    setup();
    wfiSystemTimeInc = 0;
    if (tc->powerOnDelay)
    {
        REQUIRE(tc->powerOnDelay == systemTime);
    }
    if (tc->setup) tc->setup();
    if (tc->gatherState) tc->gatherState(refState, NULL);
    while (tel->type != END)
    {
        // clear the "interrupts" to allow sending of a new telegram
        LPC_TMR16B1->IR = 0x00;
        userEepromModified = 0;
        INFO("Step " << tn << " of test case " << tc->name);
        if (BREAK == tel->type)
                tn = _handleBreak (tc, tel, tn);
        else if (TEL_RX == tel->type)
                _handleRx(tc, tel, tn);
        else if (TEL_TX == tel->type)
                _handleTx(tc, tel, tn);
        else if (CHECK_TX_BUFFER == tel->type)
                _handleCheckTx(tc, tel, tn);
        else if (TIMER_TICK == tel->type)
                _handleTime(tc, tel, tn);
        else if (LOOP == tel->type)
                _handleLoop(tc, tel, tn);

        if (tel->stepFunction) tel->stepFunction(refState, tel->variable);
        if (tc->gatherState) tc->gatherState(stepState, refState);
        tn++;
        tel++;
    }
}
