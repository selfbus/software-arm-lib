/*
 *  protocol.cpp - protocol tests
 *
 *  Copyright (C) 2014-2015 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/internal/variables.h>
#include <sblib/internal/iap.h>
#include "protocol.h"

extern volatile byte userEepromModified;
extern int sndStartIdx;
extern unsigned int wfiSystemTimeInc;

static void _handleBusSendingInterrupt()
{
     //    (bus.state == Bus::RECV_WAIT_FOR_STARTBIT_OR_TELEND) ||
     if (bus.state == Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE)
     {
         bus.state = Bus::SEND_START_BIT;
     }

     if (bus.state == Bus::SEND_START_BIT)
     {
        bus.state = Bus::SEND_END_OF_TX;
        bus.timerInterruptHandler();
        if (bus.state == Bus::SEND_WAIT_FOR_RX_ACK_WINDOW)  // device request an ACK -> so inject one
        {
            bus.telegram[0]  = bus.currentByte = SB_BUS_ACK;
            bus.telegramLen  = 1; // why tel->length? we just simulate the 0xCC?
            bus.state        = Bus::RECV_WAIT_FOR_STARTBIT_OR_TELEND;
            bus.nextByteIndex = 1;
            bus.parity        = 1;
            _LPC_TMR16B1.IR  = 0;
            bus.timerInterruptHandler ();
            bus.telegramLen = 0;
            LPC_TMR16B1->IR = 0x00;
        }
    }

    REQUIRE(((bus.state == Bus::IDLE) || (bus.state == Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE)));
}

static void addChecksum(unsigned char * telegram, unsigned int telLength)
{
    unsigned char checksum = 0xff;
    for (unsigned short i = 0; i < (telLength-1); i++)
    {
        checksum ^= telegram[i];
    }
    telegram[telLength-1] = checksum;
}

static void _handleRx(Test_Case * tc, Telegram * tel, unsigned int testStep)
{
    tel->length++; // add one byte for checksum
    addChecksum(tel->bytes, tel->length);
	memcpy(bus.telegram, tel->bytes, tel->length);
	bus.telegramLen = tel->length;
	bcu.processTelegram(bus.telegram, bus.telegramLen);
	REQUIRE(bus.telegramLen == 0);
}

static void _checkSendTelegram(Test_Case * tc, Telegram * tel, unsigned int testStep)
{
    int i;
    int mismatches = 0;
    char msg[1025];
    char numbers[23 * 3 + 1] = { 0 };
    char received[23 * 3 + 1] = { 0 };
    char expected[23 * 3 + 1] = { 0 };
    char temp[1025];

    snprintf(msg, 1024, "%s: Number of bytes in send telegram %d expected %d, sent %d", tc->name, testStep,
        tel->length, bus.sendTelegramLen - 1);
    INFO(msg);
    REQUIRE(tel->length == (bus.sendTelegramLen - 1));

    snprintf(msg, 1024, "%s: Send telegram %d mismatch at byte(s) ", tc->name, testStep);
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
}

static void _handleTx(Test_Case * tc, Telegram * tel, unsigned int testStep)
{
    if (bus.state == Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE)
    {
        bus.timerInterruptHandler();
    }

    _checkSendTelegram(tc, tel, testStep);

    _handleBusSendingInterrupt();

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

static void _handleTime(Test_Case * tc, Telegram * tel, unsigned int testStep)
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

static unsigned int _handleBreak (Test_Case * tc, Telegram * tel, unsigned int testStep)
{
    if (tel->variable)
        return testStep;
    return testStep - 1;
}

static void _loop(const int loopCount)
{
    for (int i = 0; i < loopCount; i++)
    {
        bcu.loop();
    }
}

static void _handleLoop (Test_Case * tc, Telegram * tel, unsigned int testStep)
{
    _loop(tel->loopCount);
}

void executeTest(Test_Case * tc)
{
    Telegram * tel;
    unsigned int testStep = 1;
    void * refState  = tc->refState;
    void * stepState = tc->stepState;

    unsigned int totalStepCount = 0;
    tel = tc->telegram;
    while (tel->type != END)
    {
        totalStepCount++;
        tel++;
    }
    tel = tc->telegram;

    IAP_Init_Flash(0xFF);
    if(tc->eepromSetup) tc->eepromSetup();
    memcpy(FLASH_BASE_ADDRESS + iapFlashSize() - FLASH_SECTOR_SIZE, userEepromData, USER_EEPROM_SIZE);
    bcu.begin(tc->manufacturer, tc->deviceType, tc->version);

    REQUIRE(bus.state == Bus::INIT);
    _LPC_TMR16B1.IR  = 0;
    bus.timerInterruptHandler(); // move the ISR out of INIT state
    REQUIRE(bus.state == Bus::IDLE);

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
        INFO("Step " << testStep << "/" << totalStepCount << " of test case " << tc->name);
        if (BREAK == tel->type)
                testStep = _handleBreak (tc, tel, testStep);
        else if (TEL_RX == tel->type)
                _handleRx(tc, tel, testStep);
        else if (TEL_TX == tel->type)
                _handleTx(tc, tel, testStep);
        else if (CHECK_TX_BUFFER == tel->type)
                _handleCheckTx(tc, tel, testStep);
        else if (TIMER_TICK == tel->type)
                _handleTime(tc, tel, testStep);
        else if (LOOP == tel->type)
                _handleLoop(tc, tel, testStep);

        if (tel->stepFunction) tel->stepFunction(refState, tel->variable);
        if (tc->gatherState) tc->gatherState(stepState, refState);

        // loopCount > 0 specifies to loop the bcu x times
        if ((tel->loopCount > 0) && (tel->type != LOOP) && (tel->type != BREAK))
        {
            _loop(tel->loopCount);
        }

        testStep++;
        tel++;
    }
}
