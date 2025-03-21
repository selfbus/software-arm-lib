/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Selfbus KNX-Library
 * @defgroup SBLIB_SUB_GROUP_TEST KNX LPDU Unit Test
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   test_knx_lpdu.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <catch.hpp>
#include <sblib/eib/knx_lpdu.h>
#include <string.h>

const int testTelLength = 24;
unsigned char testTelegram[testTelLength];

TEST_CASE("LPDU processing","[SBLIB][KNX][LPDU]")
{

    byte testByte;
    char msg[200];

    for (int i = 0; i <= 0xFF; i++)
    {
        // fill telegram with some data
        for (int j = 0; j < testTelLength; j++)
        {
            testTelegram[j] = j;
        }
        // set controlByte data
        testTelegram[0] = i;
        snprintf(msg, sizeof(msg)/sizeof(msg[0]) - 1, "LPDU controlByte=0x%02X", testTelegram[0]);
        INFO(msg);

        // now let's test some stuff
        // check for correct returned controlByte data
        REQUIRE(controlByte(testTelegram) == i);

        // set repeated flag
        testTelegram[0] = testTelegram[0] & 0b11011111; // 5.bit 0 = repeated
        REQUIRE(isRepeated(testTelegram) == true);

        // unset repeated flag
        testTelegram[0] |= 0b00100000; // 5.bit 1 = not repeated
        REQUIRE(isRepeated(testTelegram) == false);

        // check setting of repeated = false
        testTelegram[0] = i;
        testByte = testTelegram[0] | 0b00100000; // 5.bit 1 = not repeated
        setRepeated(testTelegram, false);
        REQUIRE(testByte == testTelegram[0]);

        // check setting of repeated = true
        testTelegram[0] = i;
        testByte = testTelegram[0] & 0b11011111; // 5.bit 0 = repeated
        setRepeated(testTelegram, true);
        REQUIRE(testByte == testTelegram[0]);

        // check setting and getting of priority low
        testTelegram[0] = i;
        testByte = testTelegram[0] | 0b00001100; // 2. & 3.bit set = PRIORITY_LOW
        setPriority(testTelegram, PRIORITY_LOW);
        REQUIRE(testByte == testTelegram[0]);
        REQUIRE(PRIORITY_LOW == priority(testTelegram));

        // check setting and getting of priority high
        testTelegram[0] = i;
        testByte = (testTelegram[0] | 0b00000100) & (0b11110111); // 2.bit set & 3.bit unset = PRIORITY_HIGH
        setPriority(testTelegram, PRIORITY_HIGH);
        REQUIRE(testByte == testTelegram[0]);
        REQUIRE(PRIORITY_HIGH == priority(testTelegram));

        // check setting and getting of priority alarm
        testTelegram[0] = i;
        testByte = (testTelegram[0] | 0b00001000) & (0b11111011);; // 2.bit unset & 3.bit set = PRIORITY_ALARM
        setPriority(testTelegram, PRIORITY_ALARM);
        REQUIRE(testByte == testTelegram[0]);
        REQUIRE(PRIORITY_ALARM == priority(testTelegram));

        // check setting and getting of priority system
        testTelegram[0] = i;
        testByte = testTelegram[0] & 0b11110011; // 2. & 3.bit unset = PRIORITY_SYSTEM
        setPriority(testTelegram, PRIORITY_SYSTEM);
        REQUIRE(testByte == testTelegram[0]);
        REQUIRE(PRIORITY_SYSTEM == priority(testTelegram));

        // set extended frame type
        testTelegram[0] = testTelegram[0] & 0b01111111; // 7.bit 0 = extended frame length
        REQUIRE(frameType(testTelegram) == FRAME_EXTENDED);

        // set standard frame type
        testTelegram[0] |= 0b10000000; // 7.bit 1 = standard frame length
        REQUIRE(frameType(testTelegram) == FRAME_STANDARD);

        // check setting of standard frame type
        testTelegram[0] = i;
        testByte = testTelegram[0] | 0b10000000; // 7.bit 1 = standard frame length
        setFrameType(testTelegram, FRAME_STANDARD);
        REQUIRE(testByte == testTelegram[0]);

        // check setting of extended frame type
        testTelegram[0] = i;
        testByte = testTelegram[0] & 0b01111111; // 7.bit 0 = extended frame length
        setFrameType(testTelegram, FRAME_EXTENDED);
        REQUIRE(testByte == testTelegram[0]);

    }
}

TEST_CASE("LPDU sender/destination address processing","[SBLIB][KNX][LPDU]")
{
    unsigned short addr;
    byte highByte;
    byte lowByte;

    for (int i = 0; i <= 255; i++)
    {
        highByte = i;
        for (int j = 0; j <= 255; j++)
        {
            lowByte = j;
            addr = (highByte << 8) | lowByte;
            setDestinationAddress(testTelegram, addr);
            // check setting of destination address
            REQUIRE(addr == ((testTelegram[3] << 8) | testTelegram[4]));
            // check getting of destination address
            REQUIRE(addr == destinationAddress(testTelegram));
            // check getting of sender address
            testTelegram[1] = highByte;
            testTelegram[2] = lowByte;
            REQUIRE(addr == senderAddress(testTelegram));
        }
    }
}

TEST_CASE("LPDU initialization","[SBLIB][KNX][LPDU]")
{
    unsigned char testTelegram[24];
    memset(testTelegram, 0, sizeof(testTelegram));
    initLpdu(testTelegram, PRIORITY_LOW, false, FRAME_STANDARD);
    REQUIRE(testTelegram[0] ==  0xbc);
}


/** @}*/
