/*
 * Tests for the memory.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <catch.hpp>
#include "test_memory.h"


struct TestRange
{
    uint32_t start;
    uint32_t size;
};

static std::vector<TestRange> testRanges = {
    { 0,1 },               // lower limit
    { 0, UINT32_MAX },     // upper limit size
    { UINT32_MAX - 1, 1 }, // upper limit address
    { 0x100, 256 },        // BCU 1
    { 0x100, 1024 },       // BCU 2
    { 0x3f00, 3072 },      // Mask 0x0701, 0x0705
    { 0x3300, 3072 }       // System B
};

TEST_CASE("Memory Constructor", "[UserEeprom]")
{
    TestMemory* testMemory;
    // Test declared ranges
    for (const auto& [start, size] : testRanges)
    {
        testMemory = new TestMemory(start, size);
        REQUIRE(testMemory->startAddr() == start);
        REQUIRE(testMemory->size() == size);
        REQUIRE(testMemory->endAddr() == start + size - 1);
        delete testMemory;
    }

    // Test some arbitrary ranges
    for (uint32_t start = 0; start < 0xffff; start += 131) // 131 is some arbitrary increment
    {
        for (uint32_t size = 1; size < 0xffff; size += 251) // 251 is some arbitrary increment
        {
            testMemory = new TestMemory(start, size);
            REQUIRE(testMemory->startAddr() == start);
            REQUIRE(testMemory->size() == size);
            REQUIRE(testMemory->endAddr() == start + size - 1);
            delete testMemory;
        }
    }
}

TEST_CASE("Memory normalizeAddress", "[UserEeprom]")
{
    // Test declared ranges
    for (const auto& [start, size] : testRanges)
    {
        TestMemory testMemory(start, size);
        const uint32_t maxIterations = std::min(testMemory.endAddr(), static_cast<uint32_t>(0xffff));
        for (uint32_t i = start; i <= maxIterations; i++)
        {
            uint32_t addressToTest = i + start;
            testMemory.normalizeAddress(&addressToTest);
            REQUIRE(addressToTest == i);
        }
    }
}

TEST_CASE("Memory inRange(start, end)", "[UserEeprom]")
{
    // Test declared ranges
    for (const auto& [start, size] : testRanges)
    {
        TestMemory testMemory(start, size);
        REQUIRE(testMemory.inRange(start - 1, start) == false);
        REQUIRE(testMemory.inRange(start, start) == true);
        REQUIRE(testMemory.inRange(start, testMemory.endAddr()) == true);
        REQUIRE(testMemory.inRange(start, testMemory.endAddr() - 1) == (size > 1));
        REQUIRE(testMemory.inRange(start, testMemory.endAddr()) == true);
        const uint32_t maxIterations = std::min(testMemory.endAddr(), static_cast<uint32_t>(0xffff));
        for (uint32_t i = start; i <= maxIterations; i++)
        {
            REQUIRE(testMemory.inRange(i, maxIterations) == true);
        }

        for (uint32_t i = 0; i <= maxIterations; i--)
        {
            REQUIRE(testMemory.inRange(start, testMemory.endAddr() - i) == true);
        }
    }
}

TEST_CASE("Memory inRange(address)", "[UserEeprom]")
{
    // Test declared ranges
    for (const auto& [start, size] : testRanges)
    {
        TestMemory testMemory(start, size);
        REQUIRE(testMemory.inRange(start - 1) == false);
        REQUIRE(testMemory.inRange(testMemory.endAddr() + 1) == false);

        const uint32_t maxIterations = std::min(testMemory.endAddr(), static_cast<uint32_t>(0xffff));
        for (uint32_t i = start; i <= maxIterations; i++)
        {
            REQUIRE(testMemory.inRange(i) == true);
        }

        for (uint32_t i = 0; i <= maxIterations; i--)
        {
            REQUIRE(testMemory.inRange(testMemory.endAddr() - i) == true);
        }
    }
}

TEST_CASE("Memory overridden dummys", "[UserEeprom]")
{
    // Test declared ranges
    for (const auto& [start, size] : testRanges)
    {
        TestMemory testMemory(start, size);
        // These checks are just too check that the methods are implemented.
        REQUIRE(testMemory[start] == 0);
        REQUIRE(testMemory.getUInt8(start) == 0);
        REQUIRE(testMemory.getUInt16(start) == 0);
    }
}