/*
 * Tests for the userEeprom.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <catch.hpp>
#include <vector>
#include <cstring>
#include <iap_emu.h>
#include <sblib/internal/iap.h>

#include "test_user_eeprom.h"


struct TestSize
{
    uint32_t start;
    uint32_t size;
    uint32_t flashSize;
    uint32_t resultFlashStart;
};

static std::vector<TestSize> testSizes = {
    { 0, 1, 1, 0xffff },            // lower limit
    { 0x100, 256,  256, 0xff00 },   // BCU 1
    { 0x100, 1024, 1024, 0xfc00 },  // BCU 2
    { 0x3f00, 3072, 4096, 0xf000 }, // Mask 0x0701, 0x0705
    { 0x3300, 3072, 4096, 0xf000 }  // System B
};

TEST_CASE("User EEPROM test", "[userEeprom]")
{
    // Test declared ranges
    for (const auto& [start, size, flashSize, resultFlashStart] : testSizes)
    {
        // Test a new mcu with erased flash
        IAP_Init_Flash(0xff);
        auto testUserEeprom = new TestUserEeprom(start, size, flashSize);
        uint8_t* validPage = testUserEeprom->findValidPage();
        REQUIRE(validPage == nullptr);
        delete testUserEeprom;

        // Test a mcu with erased flash
        IAP_Init_Flash(0x31);
        testUserEeprom = new TestUserEeprom(start, size, flashSize);
        validPage = testUserEeprom->findValidPage();
        // uint32_t i = iapFlashSize();
        // while (i > 0)
        // {
        //     if (&FLASH[i] == validPage)
        //         break;
        //     i--;
        // }
        // printf("i: %x\n", i);
        REQUIRE(validPage == (FLASH + iapFlashSize() - flashSize));
        REQUIRE(validPage == (&FLASH[resultFlashStart]));
        delete testUserEeprom;
    }
}
