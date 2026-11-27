/*
 * Test class for the memory.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#ifndef TEST_SBLIB_KNX_MEMORY_H_
#define TEST_SBLIB_KNX_MEMORY_H_

#include <sblib/eib/memory.h>


class TestMemory final : public Memory
{
public:
    TestMemory() = delete;
    TestMemory(const uint32_t start, const uint32_t size) :
        Memory(start, size) {}

    //using Memory::startAddr;
    //using Memory::endAddr;
    //using Memory::size;
    using Memory::normalizeAddress;
    //using Memory::inRange;

    // Implement dummy abstract methods
    byte& operator[](const uint32_t address) override { return dummyByte; }
    [[nodiscard]] uint8_t getUInt8(uint32_t address) const override { return 0;}
    [[nodiscard]] uint16_t getUInt16(uint32_t address) const override { return 0;}

private:
    uint8_t dummyByte = 0;
};

#endif /* TEST_SBLIB_KNX_MEMORY_H_ */
