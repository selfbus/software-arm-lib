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

#include "catch.hpp"
#include <sblib/eib/knx_lpdu.h>

TEST_CASE("LPDU processing","[SBLIB][KNX][LPDU]")
{
    const int testTelLength = 24;
    unsigned char testTelegram[testTelLength];
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
        testByte = i | 0b00100000; // 5.bit 1 = not repeated
        testTelegram[0] = i;
        testTelegram[0] = setRepeated(controlByte(testTelegram), false);
        REQUIRE(testByte == testTelegram[0]);

        // check setting of repeated = true
        testByte = i & 0b11011111; // 5.bit 1 = not repeated
        testTelegram[0] = i;
        testTelegram[0] = setRepeated(controlByte(testTelegram), true);
        REQUIRE(testByte == testTelegram[0]);
    }
}


/** @}*/
