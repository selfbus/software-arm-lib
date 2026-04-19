/*
 *  protocol_rp2350.cpp - Test executor using MockKnxBus + KnxBusInterface.
 *
 *  This is the RP2350/interface-based counterpart to protocol.cpp.
 *  Instead of directly manipulating the LPC Bus class, it injects and
 *  inspects telegrams through the MockKnxBus which implements KnxBusInterface.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "protocol_rp2350.h"
#include <sblib/eib/knx_npdu.h>
#include <sblib/internal/iap.h>
#include <sblib/eib/bus_const.h>
#include <sblib/bits.h>

extern unsigned int wfiSystemTimeInc;

const uint8_t dummyMaskVersionHigh = 0xDE;
const uint8_t dummyMaskVersionLow = 0xAD;
uint16_t bcuEepromStartAddress;

BcuDefault* bcuUnderTest = nullptr;

// Shared MockKnxBus instance used by the test executor
static MockKnxBus testMockBus;

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

    for (uint16_t i = 0; i < telCount; i++, tel++)
    {
        if (tel->length < 10)
            continue;

        if ((tel->type == TEL_TX) &&
            ((tel->bytes[6] & 0x03) == 0x03) &&
            (tel->bytes[7] == 0x40))
        {
            tel->bytes[8] = maskVersionHigh;
            tel->bytes[9] = maskVersionLow;
            continue;
        }

        uint16_t shortApci = makeWord(tel->bytes[6], tel->bytes[7]);
        shortApci &= 0x3f0;

        bool isMemoryRead = (tel->type == TEL_RX) && (shortApci == 0x200);
        bool isMemoryWrite = (tel->type == TEL_RX) && (shortApci == 0x280);
        bool isMemoryResponse = (tel->type == TEL_TX) && (shortApci == 0x240);

        if (isMemoryRead || isMemoryWrite || isMemoryResponse)
        {
            if (testBcu->getMaskVersion() == 0x12)
                continue;

            uint16_t memAddress = makeWord(tel->bytes[8], tel->bytes[9]);
            if (memAddress < bcu1MemoryOffset)
                continue;

            memAddress -= bcu1MemoryOffset;
            memAddress += testBcu->userEeprom->startAddr();

            tel->bytes[8] = memAddress >> 8;
            tel->bytes[9] = memAddress & 0xff;
        }
    }
}

static void addChecksum(unsigned char* telegram, unsigned int telLength)
{
    unsigned char checksum = 0xff;
    for (unsigned short i = 0; i < (telLength - 1); i++)
        checksum ^= telegram[i];
    telegram[telLength - 1] = checksum;
}

static void _handleRx(BcuDefault* currentBcu, Test_Case* tc, Telegram* tel, unsigned int testStep)
{
    tel->length++;  // add checksum byte
    addChecksum(tel->bytes, tel->length);

    // Inject the telegram via MockKnxBus and deliver immediately
    testMockBus.injectAndDeliver(tel->bytes, tel->length);
}

static void _checkSentTelegram(BcuDefault* currentBcu, Test_Case* tc, Telegram* tel, unsigned int testStep)
{
    REQUIRE(testMockBus.sentTelegramCount() > 0);

    const uint8_t* sentData = testMockBus.lastSentTelegram();
    uint16_t sentLen = testMockBus.lastSentTelegramLength();

    int mismatches = 0;
    char msg[1025];
    char numbers[23 * 3 + 1] = { 0 };
    char received[23 * 3 + 1] = { 0 };
    char expected[23 * 3 + 1] = { 0 };
    char temp[1025];

    snprintf(msg, 1024, "%s: Number of bytes in send telegram %d expected %d, sent %d",
             tc->name, testStep, tel->length, sentLen);
    INFO(msg);
    REQUIRE(tel->length == sentLen);

    snprintf(msg, 1024, "%s: Send telegram %d mismatch at byte(s) ", tc->name, testStep);
    for (int i = 0; i < tel->length; i++)
    {
        snprintf(temp, 255, " %2d", i + 1);
        strcat(numbers, temp);
        snprintf(temp, 255, " %02x", sentData[i]);
        strcat(received, temp);
        snprintf(temp, 255, " %02x", tel->bytes[i]);
        strcat(expected, temp);
        if (tel->bytes[i] != sentData[i])
        {
            mismatches++;
            snprintf(temp, 1024, "%d, ", i + 1);
            strcat(msg, temp);
        }
    }
    msg[strlen(msg) - 2] = '\n';
    snprintf(temp, 1024, "          %s\n expected: %s\n sent:     %s", numbers, expected, received);
    strcat(msg, temp);
    INFO(msg);
    REQUIRE(mismatches == 0);
}

static void _handleTx(BcuDefault* currentBcu, Test_Case* tc, Telegram* tel, unsigned int testStep)
{
    // The BCU should have queued a telegram via sendTelegram().
    // MockKnxBus::sendTelegram() stores it immediately and calls onTelegramSent().
    _checkSentTelegram(currentBcu, tc, tel, testStep);
    testMockBus.clearSentTelegrams();
}

static void _handleCheckTx(BcuDefault* currentBcu, Test_Case* tc, Telegram* tel, unsigned int testStep)
{
    size_t hasPending = testMockBus.sentTelegramCount();
    INFO("Check if additional telegrams should be sent");
    REQUIRE(hasPending == tel->variable);
}

static void _handleTime(BcuDefault* currentBcu, Test_Case* tc, Telegram* tel, unsigned int testStep)
{
    if (testMockBus.sentTelegramCount() > 0)
    {
        char msg[1025];
        char numbers[23 * 3 + 1] = { 0 };
        char received[23 * 3 + 1] = { 0 };
        char temp[1025];

        const uint8_t* sentData = testMockBus.lastSentTelegram();
        uint16_t sentLen = testMockBus.lastSentTelegramLength();

        snprintf(msg, 1024, "Unexpected telegram\n");
        for (int i = 0; i < sentLen; i++)
        {
            snprintf(temp, 255, " %2d", i + 1);
            strcat(numbers, temp);
            snprintf(temp, 255, " %02x", sentData[i]);
            strcat(received, temp);
        }
        snprintf(temp, 1024, "          %s\n sent:     %s", numbers, received);
        strcat(msg, temp);
        FAIL(msg);
    }
    setMillis(millis() + tel->length);
}

static unsigned int _handleBreak(Test_Case* tc, Telegram* tel, unsigned int testStep)
{
    if (tel->variable)
        return testStep;
    return testStep - 1;
}

static void _loop(BcuDefault* currentBcu, const int loopCount)
{
    for (int i = 0; i < loopCount; i++)
        currentBcu->loop();
}

static void _handleLoop(BcuDefault* currentBcu, Test_Case* tc, Telegram* tel, unsigned int testStep)
{
    _loop(currentBcu, tel->loopCount);
}

static void checkBcuInitialisation(BcuDefault* currentBcu, Test_Case* tc)
{
    REQUIRE(currentBcu->state == TLayer4::CLOSED);
    REQUIRE(tc->manufacturer == makeWord(currentBcu->userEeprom->manufacturerH(), currentBcu->userEeprom->manufacturerL()));
    REQUIRE(tc->deviceType == makeWord(currentBcu->userEeprom->deviceTypeH(), currentBcu->userEeprom->deviceTypeL()));
    REQUIRE(tc->version == currentBcu->userEeprom->version());
}

static void checkBcuEepromSettings(BcuDefault* currentBcu, Test_Case* tc)
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

void executeTestOnBcu(BcuDefault* currentBcu, Test_Case* tc)
{
    bcuUnderTest = currentBcu;
    char msg[1025];
    snprintf(msg, sizeof(msg) - 1, "Running test (interface) on %s mask 0x%04x",
             currentBcu->getBcuType(), currentBcu->getMaskVersion());
    INFO(msg);

    Telegram* tel;
    unsigned int testStep = 1;
    void* refState  = tc->refState;
    void* stepState = tc->stepState;

    uint16_t totalStepCount = getTotalStepCount(tc);

    IAP_Init_Flash(0xFF);
    if (tc->eepromSetup) tc->eepromSetup();

    checkBcuEepromSettings(currentBcu, tc);
    memcpy(FLASH_BASE_ADDRESS + iapFlashSize() - FLASH_SECTOR_SIZE,
           currentBcu->userEeprom->userEepromData, 0x100);

    // The BCU was constructed with a KnxBusInterface* (MockKnxBus).
    // begin() will call busInterface->begin() internally.
    currentBcu->begin(tc->manufacturer, tc->deviceType, tc->version);
    checkBcuInitialisation(currentBcu, tc);

    testMockBus.reset();

    setMillis(0);
    wfiSystemTimeInc = 1;
    setup();
    wfiSystemTimeInc = 0;
    if (tc->powerOnDelay)
    {
        REQUIRE(tc->powerOnDelay == millis());
    }

    if (tc->setup) tc->setup(tc->telegram, totalStepCount);
    if (tc->gatherState) tc->gatherState(refState, nullptr);
    tel = tc->telegram;
    while (tel->type != END)
    {
        testMockBus.clearSentTelegrams();
        INFO("Step " << testStep << "/" << totalStepCount << " of test case " << tc->name);

        if (BREAK == tel->type)
            testStep = _handleBreak(tc, tel, testStep);
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

        if ((tel->loopCount > 0) && (tel->type != LOOP) && (tel->type != BREAK))
            _loop(currentBcu, tel->loopCount);

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
    uint16_t byteCount = sizeof(telOriginal->bytes) / sizeof(telOriginal->bytes[0]);
    memcpy(&telCopy->bytes, &telOriginal->bytes, byteCount);
}

void copyAllTelegrams(Telegram* telCopy, const Telegram* telOriginal, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++, telCopy++, telOriginal++)
        copyTelegram(telCopy, telOriginal);
}

void executeTest(BcuType testBcuType, Test_Case* tc)
{
    BcuDefault* bcuToTest = nullptr;

    // Create BCU with MockKnxBus (KnxBusInterface* constructor)
    switch (testBcuType)
    {
        case BCU_1:   bcuToTest = new BCU1(&testMockBus);   break;
        case BCU_2:   bcuToTest = new BCU2(&testMockBus);   break;
        case BCU_0701: bcuToTest = new MASK0701(&testMockBus); break;
        case BCU_0705: bcuToTest = new MASK0705(&testMockBus); break;
        case BCU_07B0: bcuToTest = new SYSTEMB(&testMockBus); break;
        default:
            FAIL("unknown BCU type");
    }

    Telegram* telOriginal = tc->telegram;
    uint16_t totalStepCount = getTotalStepCount(tc);
    Telegram* telCopy = new Telegram[totalStepCount];
    copyAllTelegrams(telCopy, telOriginal, totalStepCount);
    tc->telegram = telCopy;
    executeTestOnBcu(bcuToTest, tc);
    tc->telegram = telOriginal;
    delete[] telCopy;
    delete bcuToTest;
}
