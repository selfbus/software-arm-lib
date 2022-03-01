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
#   include <sblib/eib/bcu.h>
// #   include <sblib/eib/knx_tlayer4.h>
#undef private
#undef protected
#include "iap_emu.h"

#include <string.h>
#include <stdio.h>

#define MASK_VERSION_LOW (MASK_VERSION & 0xFF)
#define MASK_VERSION_HIGH ((MASK_VERSION >> 8) & 0xFF)

typedef void (TestCaseSetup) (void);

typedef void (StateFunction) (void * state, void * refState);
typedef void (StepFunction)  (void * state, unsigned int var);

typedef enum
{
  TEL_RX            //!> simulated telegram received from the bus, at least the length must be specified, loopCount can be used to call bcu.loop() after stepFunction is evaluated
, TEL_TX            //!> simulated telegram to transmit to the bus, at least the length must be specified
, TIMER_TICK        //!> simulated timer tick by increasing system time
, CHECK_TX_BUFFER
, LOOP              //!> simulates bcu.loop()
, BREAK
, END               //!> test case end
} TelegramType;

typedef struct
{
    TelegramType     type;
    int              length;
    unsigned int     loopCount;
    unsigned int     variable;
    StepFunction   * stepFunction;
    unsigned char    bytes[24]; ///\todo more space for extended frames
} Telegram;

typedef struct
{
    unsigned int dummy;
} TestCaseState;

typedef struct
{
    const char            * name;
          int               manufacturer;
          int               deviceType;
          int               version;
          unsigned int      powerOnDelay;
          TestCaseSetup   * eepromSetup;
          TestCaseSetup   * setup;
          StateFunction   * gatherState;
          TestCaseState   * refState;
          TestCaseState   * stepState;
          Telegram        * telegram;
} Test_Case;

void executeTest(Test_Case * tc);

#endif /* PROTOCOL_H_ */
