/*
 *  protocol.h - Main include file for defining protocol related tests
 *
 *  Copyright (c) 2014 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */


#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "catch.hpp"

#define private   public
#define protected public
#   include <sblib/eib/bus.h>
#   include <sblib/eib/BCU1.h>
#   include <sblib/eib/BCU2.h>
#   include <sblib/eib/mask0701.h>
#   include <sblib/eib/mask0705.h>
#   include <sblib/eib/systemb.h>
#undef private
#undef protected
#include "iap_emu.h"

#include <string.h>
#include <stdio.h>

extern BcuDefault* bcuUnderTest;
extern const uint8_t dummyMaskVersionHigh;
extern const uint8_t dummyMaskVersionLow;

// #define GET_LENGTH(array) (sizeof(array)/sizeof(array[0]))

#define EEPROM_START (bcuUnderTest->userEeprom->userEepromStart)
#define ADD_EEPROM_START(address) ((address + bcuEepromStartAddress))
#define HIGH_BYTE2(b) (static_cast<unsigned char>(b >> 8))
#define LOW_BYTE2(b) (static_cast<unsigned char>(b & 0xff))
#define HIGH_ADDR(address) (static_cast<unsigned char>((ADD_EEPROM_START(address) >> 8)))
#define LOW_ADDR(address) (static_cast<unsigned char>(ADD_EEPROM_START(address) & 0xFF))

#define HIGH_MASK (HIGH_BYTE(bcu->getMaskVersion()))
#define LOW_MASK (LOW_BYTE(bcu->getMaskVersion()))

enum BcuType
{
    BCU_1 = 0x01,
    BCU_2 = 0x02,
    BCU_0701 = 0x04,
    BCU_0705 = 0x08,
    BCU_07B0 = 0x10,
};

enum TelegramType
{
    TEL_RX,             //!> simulated telegram received from the bus, at least the length must be specified, loopCount can be used to call bcu->loop() after stepFunction is evaluated
    TEL_TX,             //!> simulated telegram to transmit to the bus, at least the length must be specified
    TIMER_TICK,         //!> simulated timer tick by increasing system time
    CHECK_TX_BUFFER,
    LOOP,               //!> simulates bcu->loop()
    BREAK,
    END,                //!> test case end
};


typedef void (TestCaseEepromSetup) (void);

typedef void (StateFunction) (void * state, void * refState);
typedef void (StepFunction)  (void * state, unsigned int var);



struct Telegram // adding something to Telegram also requires extension of function copyTelegram(const Telegram * telOriginal)
{
    TelegramType     type;
    int              length;
    unsigned int     loopCount;
    unsigned int     variable;
    StepFunction   * stepFunction;
    unsigned char    bytes[24]; ///\todo more space for extended frames
};

typedef void (TestCaseSetup) (Telegram* tel, uint16_t telCount);

struct TestCaseState
{
    unsigned int dummy;
};

struct Test_Case
{
    const char            * name;
          int               manufacturer;
          int               deviceType;
          int               version;
          unsigned int      powerOnDelay;
          TestCaseEepromSetup   * eepromSetup;
          TestCaseSetup   * setup;
          StateFunction   * gatherState;
          TestCaseState   * refState;
          TestCaseState   * stepState;
          Telegram        * telegram;
};

void executeTest(BcuType testBcuType, Test_Case * tc);
void telegramPreparation(BcuDefault* testBcu, Telegram* tel, uint16_t telCount);

#endif /* PROTOCOL_H_ */
