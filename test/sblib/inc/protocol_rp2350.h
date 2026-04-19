/*
 *  protocol_rp2350.h - Test protocol for KnxBusInterface-based BCU tests.
 *
 *  This is the RP2350/interface-based counterpart to the original protocol.h,
 *  which directly manipulates LPC Bus class internals. This version uses
 *  MockKnxBus to test the BCU stack through the abstract KnxBusInterface.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */
#ifndef PROTOCOL_RP2350_H_
#define PROTOCOL_RP2350_H_

#include <catch.hpp>

// Same hack as protocol.h to access protected BCU members in tests
#define private   public
#define protected public
#   include <sblib/eib/bcu_base.h>
#   include <sblib/eib/bcu_default.h>
#   include <sblib/eib/bcu1.h>
#   include <sblib/eib/bcu2.h>
#   include <sblib/eib/mask0701.h>
#   include <sblib/eib/mask0705.h>
#   include <sblib/eib/systemb.h>
#undef private
#undef protected

#include <sblib/eib/knx_bus_interface.h>
#include "mock_knx_bus.h"
#include "iap_emu.h"

#include <string.h>
#include <stdio.h>

extern BcuDefault* bcuUnderTest;
extern const uint8_t dummyMaskVersionHigh;
extern const uint8_t dummyMaskVersionLow;
extern uint16_t bcuEepromStartAddress;

// Compatibility macros shared with protocol.h
#define EEPROM_START (bcuUnderTest->userEeprom->startAddr())
#define ADD_EEPROM_START(address) ((address + bcuEepromStartAddress))
#define HIGH_BYTE2(b) (static_cast<unsigned char>(b >> 8))
#define LOW_BYTE2(b) (static_cast<unsigned char>(b & 0xff))
#define HIGH_ADDR(address) (static_cast<unsigned char>((ADD_EEPROM_START(address) >> 8)))
#define LOW_ADDR(address) (static_cast<unsigned char>(ADD_EEPROM_START(address) & 0xFF))

#define HIGH_MASK (HIGH_BYTE(bcu->getMaskVersion()))
#define LOW_MASK (LOW_BYTE(bcu->getMaskVersion()))

// Reuse the same telegram types and structures as original protocol.h
enum BcuType
{
    BCU_1   = 0x01,
    BCU_2   = 0x02,
    BCU_0701 = 0x04,
    BCU_0705 = 0x08,
    BCU_07B0 = 0x10,
};

enum TelegramType
{
    TEL_RX,
    TEL_TX,
    TIMER_TICK,
    CHECK_TX_BUFFER,
    LOOP,
    BREAK,
    END,
};

typedef void (TestCaseEepromSetup)(void);
typedef void (StateFunction)(void* state, void* refState);
typedef void (StepFunction)(void* state, unsigned int var);

struct Telegram
{
    TelegramType   type;
    int            length;
    unsigned int   loopCount;
    unsigned int   variable;
    StepFunction*  stepFunction;
    unsigned char  bytes[24];
};

typedef void (TestCaseSetup)(Telegram* tel, uint16_t telCount);

struct TestCaseState
{
    unsigned int dummy;
};

struct Test_Case
{
    const char*           name;
    int                   manufacturer;
    int                   deviceType;
    int                   version;
    unsigned int          powerOnDelay;
    TestCaseEepromSetup*  eepromSetup;
    TestCaseSetup*        setup;
    StateFunction*        gatherState;
    TestCaseState*        refState;
    TestCaseState*        stepState;
    Telegram*             telegram;
};

/**
 * Execute a test using the KnxBusInterface-based path with MockKnxBus.
 */
void executeTest(BcuType testBcuType, Test_Case* tc);

/**
 * Prepare telegram bytes (adjust mask version, EEPROM addresses).
 */
void telegramPreparation(BcuDefault* testBcu, Telegram* tel, uint16_t telCount);

#endif /* PROTOCOL_RP2350_H_ */
