/*
 * Tests for the BCU1 address table (addr_tablesBCU1.h)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <catch.hpp> // If possible, include catch.hpp as last header

#include <cstdint>
#include <sblib/eib/addr_tablesBCU1.h>
#include <vector>

namespace
{
    // Backing beyond the counted table makes a read past it observable without UB
    class TestAddrTablesBCU1 final : public AddrTablesBCU1
    {
    public:
        TestAddrTablesBCU1() : AddrTablesBCU1(nullptr), bytes(1 + 2 * 255 + 2, 0)
        {
            address(0, 0x1165);
        }

        uint8_t* addrTable() override
        {
            return bytes.data();
        }

        void address(const int index, const int value)
        {
            uint8_t* entry = bytes.data() + 1 + 2 * index;
            entry[0] = value >> 8;
            entry[1] = value & 255;
        }

        std::vector<uint8_t> bytes;
    };
}

TEST_CASE("BCU1 address table count includes the individual address", "[addr_tables_bcu1]")
{
    TestAddrTablesBCU1 addrTables;

    SECTION("count zero is empty")
    {
        addrTables.bytes[0] = 0;
        addrTables.address(1, 0x1401);
        CHECK(addrTables.indexOfAddr(0x1165) == -1);
        CHECK(addrTables.indexOfAddr(0x1401) == -1);
    }

    SECTION("count one holds only the individual address")
    {
        addrTables.bytes[0] = 1;
        addrTables.address(1, 0x1401);
        CHECK(addrTables.indexOfAddr(0x1165) == -1);
        CHECK(addrTables.indexOfAddr(0x1401) == -1);
    }

    SECTION("entry behind the counted table is not found")
    {
        addrTables.bytes[0] = 2;
        addrTables.address(1, 0x0064);
        addrTables.address(2, 0x1401);
        CHECK(addrTables.indexOfAddr(0x0064) == 1);
        CHECK(addrTables.indexOfAddr(0x1401) == -1);
        CHECK(addrTables.indexOfAddr(0x1165) == -1);
        CHECK(addrTables.indexOfAddr(0x2222) == -1);
    }

    SECTION("maximum count of the 256 byte BCU1 EEPROM")
    {
        // The count is at offset 0x16, the addresses start at 0x17. 116 entries end at 0xfe, 0xff is the checksum.
        constexpr int count = 116;
        addrTables.bytes[0] = count;
        for (int i = 1; i < count; ++i)
        {
            addrTables.address(i, 0x2000 + i);
        }
        addrTables.address(count, 0xa55a); // checksum bytes, never a table member
        CHECK(addrTables.indexOfAddr(0x2001) == 1);
        CHECK(addrTables.indexOfAddr(0x2000 + count - 1) == count - 1);
        CHECK(addrTables.indexOfAddr(0xa55a) == -1);
        CHECK(addrTables.indexOfAddr(0x3333) == -1);
    }

    SECTION("maximum encoded count in synthetic extended backing")
    {
        // 255 is not a valid BCU1 EEPROM layout, this only checks the loop bound
        constexpr int count = 255;
        addrTables.bytes[0] = count;
        for (int i = 1; i < count; ++i)
        {
            addrTables.address(i, 0x3000 + i);
        }
        addrTables.address(count, 0xb66b);
        CHECK(addrTables.indexOfAddr(0x3000 + count - 1) == count - 1);
        CHECK(addrTables.indexOfAddr(0xb66b) == -1);
    }
}
