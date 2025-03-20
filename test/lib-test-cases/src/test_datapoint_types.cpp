/*
 *  datapoint_types_test.cpp - Tests for the datapoint types
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <catch.hpp>
#include <sblib/eib/datapoint_types.h>

#include <limits.h>


TEST_CASE("Datapoint type conversion: floatToDpt9","[SBLIB]")
{
    REQUIRE(floatToDpt9(0) == 0);

    REQUIRE(floatToDpt9(1) == 1);
    REQUIRE(floatToDpt9(2) == 2);
    REQUIRE(floatToDpt9(2047) == 0x07ff);
    REQUIRE(floatToDpt9(2048) == 0x0c00);
    REQUIRE(floatToDpt9(2050) == 0x0c01);
    REQUIRE(floatToDpt9(4096) == 0x1400);
    REQUIRE(floatToDpt9(8192) == 0x1c00);
    REQUIRE(floatToDpt9(67043328) == 0x7ffe);
    REQUIRE(floatToDpt9(67076095) == 0x7ffe);
    REQUIRE(floatToDpt9(67076096) == 0x7fff);
    REQUIRE(floatToDpt9(67076097) == 0x7fff);
    REQUIRE(floatToDpt9(INT_MAX) == 0x7fff);

    REQUIRE(floatToDpt9(-1) == 0x87ff);
    REQUIRE(floatToDpt9(-2) == 0x87fe);
    REQUIRE(floatToDpt9(-2048) == 0x8000);
    REQUIRE(floatToDpt9(-4096) == 0x8800);
    REQUIRE(floatToDpt9(-67108864) == 0xf800);
    REQUIRE(floatToDpt9(-67108865) == 0x7fff);
    REQUIRE(floatToDpt9(INT_MIN) == 0x7fff);
}

TEST_CASE("Datapoint type conversion: dpt9ToFloat","[SBLIB]")
{
    REQUIRE(dpt9ToFloat(0) == 0);

    REQUIRE(dpt9ToFloat(1) == 1);
    REQUIRE(dpt9ToFloat(2) == 2);
    REQUIRE(2047 == dpt9ToFloat(0x07ff));
    REQUIRE(2048 == dpt9ToFloat(0x0c00));
    REQUIRE(4096 == dpt9ToFloat(0x1400));
    REQUIRE(8192 == dpt9ToFloat(0x1c00));
    REQUIRE(67043328 == dpt9ToFloat(0x7ffe));
    REQUIRE(INVALID_DPT_FLOAT == dpt9ToFloat(0x7fff));

    REQUIRE(-1 == dpt9ToFloat(0x87ff));
    REQUIRE(-2 == dpt9ToFloat(0x87fe));
    REQUIRE(-2048 == dpt9ToFloat(0x8000));
    REQUIRE(-4096 == dpt9ToFloat(0x8800));
    REQUIRE(-67108864 == dpt9ToFloat(0xf800));
}
