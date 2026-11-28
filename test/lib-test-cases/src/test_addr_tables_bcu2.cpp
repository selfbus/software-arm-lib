/*
 * Tests for the BCU2 and SYSTEMB address tables (addr_tablesBCU2.h, addr_tablesSYSTEMB.h)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <catch.hpp> // If possible, include catch.hpp as last header

#include <cstdint>
#include <sblib/eib/addr_tablesBCU2.h>
#include <sblib/eib/addr_tablesSYSTEMB.h>
#include <vector>

namespace
{
    // Backing beyond the counted table makes a read past it observable without UB
    class TestAddrTablesBCU2 final : public AddrTablesBCU2
    {
    public:
        TestAddrTablesBCU2() : AddrTablesBCU2(nullptr), bytes(2 + 2 * 255 + 2, 0) {}

        uint8_t* addrTable() override
        {
            return bytes.data();
        }

        std::vector<uint8_t> bytes;
    };

    class TestAddrTablesSYSTEMB final : public AddrTablesSYSTEMB
    {
    public:
        TestAddrTablesSYSTEMB() : AddrTablesSYSTEMB(nullptr), bytes(2 + 2 * 65535 + 2, 0) {}

        uint8_t* addrTable() override
        {
            return bytes.data();
        }

        std::vector<uint8_t> bytes;
    };

    void putAddress(uint8_t* entries, const int index, const int value)
    {
        entries[index * 2] = value >> 8;
        entries[index * 2 + 1] = value & 255;
    }
}

TEST_CASE("BCU2 address table count includes the individual address", "[addr_tables_bcu2]")
{
    TestAddrTablesBCU2 addrTables;

    SECTION("count zero is empty")
    {
        addrTables.bytes[0] = 0;
        CHECK(addrTables.indexOfAddr(0x1401) == -1);
    }

    SECTION("count one holds only the individual address")
    {
        addrTables.bytes[0] = 1;
        putAddress(addrTables.bytes.data() + 1, 0, 0x1165);
        putAddress(addrTables.bytes.data() + 3, 0, 0x1401);
        CHECK(addrTables.indexOfAddr(0x1165) == -1);
        CHECK(addrTables.indexOfAddr(0x1401) == -1);
    }

    SECTION("entry behind the counted table is not found")
    {
        addrTables.bytes[0] = 2;
        putAddress(addrTables.bytes.data() + 1, 0, 0x1165);
        putAddress(addrTables.bytes.data() + 3, 0, 0x0064);
        putAddress(addrTables.bytes.data() + 5, 0, 0x1401);
        CHECK(addrTables.indexOfAddr(0x0064) == 1);
        CHECK(addrTables.indexOfAddr(0x1401) == -1);
    }

    SECTION("maximum count")
    {
        constexpr int count = 255;
        addrTables.bytes[0] = count;
        putAddress(addrTables.bytes.data() + 1, 0, 0x1165);
        for (int i = 1; i < count; ++i)
        {
            putAddress(addrTables.bytes.data() + 1, i, 0x2000 + i);
        }
        putAddress(addrTables.bytes.data() + 1, count, 0xa55a);
        CHECK(addrTables.indexOfAddr(0x2000 + count - 1) == count - 1);
        CHECK(addrTables.indexOfAddr(0xa55a) == -1);
    }
}

TEST_CASE("SYSTEMB address table count excludes the individual address", "[addr_tables_bcu2]")
{
    TestAddrTablesSYSTEMB addrTables;

    SECTION("count zero is empty")
    {
        addrTables.bytes[0] = 0;
        addrTables.bytes[1] = 0;
        CHECK(addrTables.indexOfAddr(0x1401) == -1);
    }

    SECTION("count one holds one group address")
    {
        addrTables.bytes[0] = 0;
        addrTables.bytes[1] = 1;
        putAddress(addrTables.bytes.data() + 2, 0, 0x0064);
        putAddress(addrTables.bytes.data() + 2, 1, 0x1401);
        CHECK(addrTables.indexOfAddr(0x0064) == 1);
        CHECK(addrTables.indexOfAddr(0x1401) == -1);
    }

    SECTION("maximum count")
    {
        constexpr int count = 65535;
        addrTables.bytes[0] = count >> 8;
        addrTables.bytes[1] = count & 255;
        for (int i = 0; i < count; ++i)
        {
            putAddress(addrTables.bytes.data() + 2, i, 0x2000 + (i & 255));
        }
        putAddress(addrTables.bytes.data() + 2, count, 0xa55a);
        CHECK(addrTables.indexOfAddr(0x20ff) == 256);
        CHECK(addrTables.indexOfAddr(0xa55a) == -1);
    }
}
