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
#include <sblib/bits.h>
#include "protocol.h"

extern unsigned int wfiSystemTimeInc;

const uint8_t dummyMaskVersionHigh = 0xDE;
const uint8_t dummyMaskVersionLow = 0xAD;
uint16_t bcuEepromStartAddress;

BcuDefault* bcuUnderTest = nullptr;

uint16_t getTotalStepCount(Test_Case* tc)
{
    unsigned int totalStepCount = 0;
    Telegram* tel = tc->telegram;
    while (tel->type != END)
    {
        totalStepCount++;
        tel++;
    }
    totalStepCount++;
    return totalStepCount;
}

void telegramPreparation(BcuDefault* testBcu, Telegram* tel, uint16_t telCount)
{
    const uint16_t bcu1MemoryOffset = 0x100;

    uint8_t maskVersionHigh = testBcu->getMaskVersion() >> 8;
    uint8_t maskVersionLow = testBcu->getMaskVersion() & 0xff;

    ///\todo very dirty hack to modify test telegrams
    // dirty hack to set expected mask version and memory addresses in telegrams
    for (uint16_t i = 0; i < telCount; i++, tel++)
    {
        if (tel->length < 10)
        {
            continue;
        }

        if ((tel->type == TEL_TX) &&
            (tel->bytes[6] & 0x03 == 0x03) &&
            (tel->bytes[7] == 0x40))
        {
            // mask version APCI_DEVICEDESCRIPTOR_RESPONSE_PDU
            // {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x63, 0x43, 0x40, bcuMaskVersionHigh, bcuMaskVersionLow}
            tel->bytes[8] = maskVersionHigh;
            tel->bytes[9] = maskVersionLow;
            continue;
        }

        // check for APCI_MEMORY_READ_PDU, APCI_MEMORY_RESPONSE_PDU, APCI_MEMORY_WRITE_PDU
        uint16_t shortApci = makeWord(tel->bytes[6], tel->bytes[7]);
        shortApci &= 0x3f0;  // only bits 0 and 1 of bytes[6] and bits 4-7 of bytes[7]

        bool isMemoryRead = (tel->type == TEL_RX) && (shortApci == 0x200);
        bool isMemoryWrite = (tel->type == TEL_RX) && (shortApci == 0x280);
        bool isMemoryRespone = (tel->type == TEL_TX) && (shortApci == 0x240);

        if ((isMemoryRead) || (isMemoryWrite) || (isMemoryRespone))  // memory operation
        {
            if (testBcu->getMaskVersion() == 0x12)
            {
                // no need to change memory addresses for BCU1
                continue;
            }

            uint16_t memAddress = makeWord(tel->bytes[8], tel->bytes[9]);
            if (memAddress < bcu1MemoryOffset)
            {
                // is in userRam
                continue;
            }

            memAddress -= bcu1MemoryOffset; // all memory addresses are for BCU1 so "normalize" them
            memAddress += testBcu->userEeprom->startAddr(); // move by current Eeprom start

            tel->bytes[8] = memAddress >> 8;
            tel->bytes[9] = memAddress & 0xff;
        }
    }
}

static void _handleBusSendingInterrupt(BcuDefault* currentBcu)
{
     //    (currentBcu->bus->state == Bus::RECV_WAIT_FOR_STARTBIT_OR_TELEND) ||
     if (currentBcu->bus->state == Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE)
     {
         currentBcu->bus->state = Bus::SEND_START_BIT;
     }

     if (currentBcu->bus->state == Bus::SEND_START_BIT)
     {
        currentBcu->bus->state = Bus::SEND_END_OF_TX;
        currentBcu->bus->timerInterruptHandler();
        if (currentBcu->bus->state == Bus::SEND_WAIT_FOR_RX_ACK_WINDOW)  // device request an ACK -> so inject one
        {
            currentBcu->bus->telegram[0]  = currentBcu->bus->currentByte = SB_BUS_ACK;
            currentBcu->bus->telegramLen  = 1; // why tel->length? we just simulate the 0xCC?
            currentBcu->bus->state        = Bus::RECV_WAIT_FOR_STARTBIT_OR_TELEND;
            currentBcu->bus->nextByteIndex = 1;
            currentBcu->bus->parity        = 1;
            _LPC_TMR16B1.IR  = 0;
            currentBcu->bus->timerInterruptHandler ();
            currentBcu->bus->telegramLen = 0;
            LPC_TMR16B1->IR = 0x00;
        }
    }

    REQUIRE(((currentBcu->bus->state == Bus::IDLE) || (currentBcu->bus->state == Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE)));
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

static void _handleRx(BcuDefault* currentBcu, Test_Case * tc, Telegram * tel, unsigned int testStep)
{
    tel->length++; // add one byte for checksum
    addChecksum(tel->bytes, tel->length);
	memcpy(currentBcu->bus->telegram, tel->bytes, tel->length);
	currentBcu->bus->telegramLen = tel->length;
	currentBcu->processTelegram(currentBcu->bus->telegram, currentBcu->bus->telegramLen);
	REQUIRE(currentBcu->bus->telegramLen == 0);
}

static void _checkSendTelegram(BcuDefault* currentBcu, Test_Case * tc, Telegram * tel, unsigned int testStep)
{
    int i;
    int mismatches = 0;
    char msg[1025];
    char numbers[23 * 3 + 1] = { 0 };
    char received[23 * 3 + 1] = { 0 };
    char expected[23 * 3 + 1] = { 0 };
    char temp[1025];

    snprintf(msg, 1024, "%s: Number of bytes in send telegram %d expected %d, sent %d", tc->name, testStep,
        tel->length, currentBcu->bus->sendTelegramLen - 1);
    INFO(msg);
    REQUIRE(tel->length == (currentBcu->bus->sendTelegramLen - 1));

    snprintf(msg, 1024, "%s: Send telegram %d mismatch at byte(s) ", tc->name, testStep);
    for (i = 0; i < tel->length; i++)
    {
        snprintf(temp, 255, " %2d", i + 1);
        strcat(numbers, temp);
        snprintf(temp, 255, " %02x", currentBcu->bus->sendCurTelegram[i]);
        strcat(received, temp);
        snprintf(temp, 255, " %02x", tel->bytes[i]);
        strcat(expected, temp);
        if (tel->bytes[i] != currentBcu->bus->sendCurTelegram[i])
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

static void _handleTx(BcuDefault* currentBcu, Test_Case * tc, Telegram * tel, unsigned int testStep)
{
    if (currentBcu->bus->state == Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE)
    {
        currentBcu->bus->timerInterruptHandler();
    }

    /*
    ///\todo special handling of DeviceDescriptorResponse, we change expected maskVersion to maskVersion of current bcu
    // check for a APCI_DEVICEDESCRIPTOR_RESPONSE_PDU
    if ((tel->length = 9) && (tel->bytes[0] == 0xB0) && ((tel->bytes[6] & 0x03) == 0x03) && (tel->bytes[7] == 0x40))
    {
        // telegram seems to look like this
        // {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x63, 0x43, 0x40, bcuMaskVersionHigh, bcuMaskVersionLow}}
        tel->bytes[8] = bcuMaskVersionHigh;
        tel->bytes[9] = bcuMaskVersionLow;
    }
    */
    _checkSendTelegram(currentBcu, tc, tel, testStep);

    _handleBusSendingInterrupt(currentBcu);

    REQUIRE(currentBcu->bus->sendNextTel == NULL);
}

static void _handleCheckTx(BcuDefault* currentBcu, Test_Case * tc, Telegram * tel, unsigned int tn)
{
    unsigned int s = 0;
    if (currentBcu->bus->sendCurTelegram) s++;
    if (currentBcu->bus->sendNextTel) s++;
    INFO("Check if additional telegrams should be sent");
    REQUIRE(s == tel->variable);
}

static void _handleTime(BcuDefault* currentBcu, Test_Case * tc, Telegram * tel, unsigned int testStep)
{
	unsigned int s = 0;
    if (currentBcu->bus->sendCurTelegram) s++;
    if (currentBcu->bus->sendNextTel) s++;
    //INFO("Ensure that no outgoing telegram is in the queue");
    if (s)
    {
        int i;
        char msg[1025];
        char numbers[23 * 3 + 1] = { 0 };
        char received[23 * 3 + 1] = { 0 };
        char temp[1025];

        currentBcu->bus->timerInterruptHandler();
        snprintf(msg, 1024, "Unexpected telegram\n");
        for (i = 0; i < currentBcu->bus->sendTelegramLen - 1; i++)
        {
            snprintf(temp, 255, " %2d", i + 1);
            strcat(numbers, temp);
            snprintf(temp, 255, " %02x", currentBcu->bus->sendCurTelegram[i]);
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

static void _loop(BcuDefault* currentBcu, const int loopCount)
{
    for (int i = 0; i < loopCount; i++)
    {
        currentBcu->loop();
    }
}

static void _handleLoop (BcuDefault* currentBcu, Test_Case * tc, Telegram * tel, unsigned int testStep)
{
    _loop(currentBcu, tel->loopCount);
}

static void checkBcuInitialisation(BcuDefault* currentBcu, Test_Case* tc)
{
    REQUIRE(currentBcu->state == TLayer4::CLOSED);
    REQUIRE(currentBcu->bus->state == Bus::INIT);
    REQUIRE(tc->manufacturer == makeWord(currentBcu->userEeprom->manufacturerH(), currentBcu->userEeprom->manufacturerL()));
    REQUIRE(tc->deviceType == makeWord(currentBcu->userEeprom->deviceTypeH(), currentBcu->userEeprom->deviceTypeL()));
    REQUIRE(tc->version == currentBcu->userEeprom->version());
}

static void checkBcuEepromSettings(BcuDefault* currentBcu, Test_Case * tc)
{
    switch (currentBcu->getMaskVersion())
    {
        case 0x12:
            REQUIRE(currentBcu->userEeprom->startAddr() == 0x100);
            REQUIRE(currentBcu->userEeprom->endAddr() == 0x1ff);
            REQUIRE(currentBcu->userEeprom->size() == 256);
            break;

        case 0x20:
            REQUIRE(currentBcu->userEeprom->startAddr() == 0x100);
            REQUIRE(currentBcu->userEeprom->endAddr() == 0x4ff);
            REQUIRE(currentBcu->userEeprom->size() == 1024);
            break;

        case 0x701:
        case 0x705:
            REQUIRE(currentBcu->userEeprom->startAddr() == 0x3f00);
            REQUIRE(currentBcu->userEeprom->endAddr() == 0x4aff);
            REQUIRE(currentBcu->userEeprom->size() == 3072);
            break;

        case 0x7B0:
            REQUIRE(currentBcu->userEeprom->startAddr() == 0x3300);
            REQUIRE(currentBcu->userEeprom->endAddr() == 0x3eff);
            REQUIRE(currentBcu->userEeprom->size() == 3072);
            break;

        default:
            FAIL("Can't test unknown BCU type.");
    }
}

void executeTestOnBcu(BcuDefault* currentBcu, Test_Case * tc)
{
    bcuUnderTest = currentBcu;
    char msg[1025];
    snprintf(msg, sizeof(msg)-1, "Running test on %s mask 0x%04x", currentBcu->getBcuType(), currentBcu->getMaskVersion());
    INFO(msg);

    Telegram * tel;
    unsigned int testStep = 1;
    void * refState  = tc->refState;
    void * stepState = tc->stepState;

    uint16_t totalStepCount = getTotalStepCount(tc);

    IAP_Init_Flash(0xFF);
    if(tc->eepromSetup) tc->eepromSetup();

    checkBcuEepromSettings(currentBcu, tc);
    memcpy(FLASH_BASE_ADDRESS + iapFlashSize() - FLASH_SECTOR_SIZE, currentBcu->userEeprom->userEepromData, 0x100);
    currentBcu->begin(tc->manufacturer, tc->deviceType, tc->version);
    checkBcuInitialisation(currentBcu, tc);

    _LPC_TMR16B1.IR  = 0;
    currentBcu->bus->timerInterruptHandler(); // move the ISR out of INIT state
    REQUIRE(currentBcu->bus->state == Bus::IDLE);

    systemTime  = 0;
    wfiSystemTimeInc = 1;
    setup();
    wfiSystemTimeInc = 0;
    if (tc->powerOnDelay)
    {
        REQUIRE(tc->powerOnDelay == systemTime);
    }

    if (tc->setup) tc->setup(tc->telegram, totalStepCount);
    if (tc->gatherState) tc->gatherState(refState, NULL);
    tel = tc->telegram;
    while (tel->type != END)
    {
        // clear the "interrupts" to allow sending of a new telegram
        LPC_TMR16B1->IR = 0x00;
        currentBcu->userEeprom->userEepromModified = false;
        INFO("Step " << testStep << "/" << totalStepCount << " of test case " << tc->name);
        if (BREAK == tel->type)
                testStep = _handleBreak (tc, tel, testStep);
        else if (TEL_RX == tel->type)
                _handleRx(currentBcu, tc, tel, testStep);
        else if (TEL_TX == tel->type)
                _handleTx(currentBcu, tc, tel, testStep);
        else if (CHECK_TX_BUFFER == tel->type)
                _handleCheckTx(currentBcu, tc, tel, testStep);
        else if (TIMER_TICK == tel->type)
                _handleTime(currentBcu, tc, tel, testStep);
        else if (LOOP == tel->type)
                _handleLoop(currentBcu, tc, tel, testStep);

        if (tel->stepFunction) tel->stepFunction(refState, tel->variable);
        if (tc->gatherState) tc->gatherState(stepState, refState);

        // loopCount > 0 specifies to loop the bcu x times
        if ((tel->loopCount > 0) && (tel->type != LOOP) && (tel->type != BREAK))
        {
            _loop(currentBcu, tel->loopCount);
        }

        testStep++;
        tel++;
    }
}

void copyTelegram(Telegram* telCopy, const Telegram* telOriginal)
{
    telCopy->type = telOriginal->type;
    telCopy->length = telOriginal->length;
    telCopy->loopCount = telOriginal->loopCount;
    telCopy->variable = telOriginal->variable;
    telCopy->stepFunction = telOriginal->stepFunction;
    uint16_t byteCount = sizeof(telOriginal->bytes)/sizeof(telOriginal->bytes[0]);
    memcpy(&telCopy->bytes, &telOriginal->bytes, byteCount);
}

void copyAllTelegrams(Telegram* telCopy, const Telegram* telOriginal, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++, telCopy++, telOriginal++)
    {
        copyTelegram(telCopy, telOriginal);
    }
}

void executeTest(BcuType testBcuType, Test_Case * tc)
{
    BcuDefault* bcuToTest = nullptr;
    switch (testBcuType)
    {
        case BCU_1:
            bcuToTest = new BCU1();
            break;

        case BCU_2:
            bcuToTest = new BCU2();
            break;

        case BCU_0701:
            bcuToTest = new MASK0701();
            break;

        case BCU_0705:
            bcuToTest = new MASK0705();
            break;

        case BCU_07B0:
            bcuToTest = new SYSTEMB();
            break;

        default:
            FAIL("unknown BCU type");
    }

    Telegram* telOriginal = tc->telegram;
    uint16_t totalStepCount = getTotalStepCount(tc);
    Telegram telCopy[totalStepCount];
    copyAllTelegrams(&telCopy[0], telOriginal, totalStepCount);
    tc->telegram = &telCopy[0];
    executeTestOnBcu(bcuToTest, tc);
    tc->telegram = telOriginal;
    delete bcuToTest;
}
