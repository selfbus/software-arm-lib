/*
 *  prot_device_info.cpp - Test for querying of the device info
 *
 *  Copyright (c) 2014 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include "protocol.h"
#include <catch.hpp>

typedef struct
{
    unsigned char  state;
    bool           connected;
    unsigned short ownAddress;
} ProtocolTestState;

static ProtocolTestState protoState[2];

#define VaS(s) ((ProtocolTestState *) (s))

static void connect(void * state, unsigned int param)
{
    VaS(state)->connected = true;
}

static void disconnect(void * state, unsigned int param)
{
    VaS(state)->connected = false;
}

static Telegram testCaseTelegrams[] =
{
    {TEL_RX,  7, 0, 0, connect             , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0x80}},       //   1. T_CONNECT
    {TEL_RX,  8, 0, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x61, 0x43, 0x00}}, //   2. DeviceDescriptorRead
    {TEL_TX,  7, 1, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xC2}},       //   3. T_ACK
    {TEL_TX, 10, 0, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}}, // 4. DeviceDescriptorResponse
    {TEL_RX,  7, 0, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xC2}}, //   5. T_ACK
    {TEL_RX, 10, 0, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x46, 0x01, 0x01, 0x04}}, // 6. MemoryRead address=0x104 count=1 (manufacturerH)
    {TEL_TX,  7, 1, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xC6}}, //   7. T_ACK
    {TEL_TX, 11, 0, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x46, 0x41, 0x01, 0x04, 0x04}}, // 8. MemoryReadResponse address=0x104
    {TEL_RX,  7, 0, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xC6}}, //   9. T_ACK
    {TEL_RX,  9, 0, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x62, 0x49, 0x81, 0x08}}, //  10. ADC_Read
    {TEL_TX,  7, 1, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xCA}}, //  11. T_ACK
    {TEL_TX, 11, 0, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x49, 0xC1, 0x08, 0x00, 0x00}}, // 12. ADC read currently only returns 0x0000 0x05, 0xB0}} //  15
    {TEL_RX,  7, 0, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xCA}}, //  13. T_ACK
    {TEL_RX, 10, 0, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x4E, 0x01, 0x00, 0x60}}, //  14. MemoryRead address=0x60 count=1 (system state)
    {TEL_TX,  7, 1, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xCE}}, //  15. T_ACK
    {TEL_TX, 11, 0, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x4E, 0x41, 0x00, 0x60, 0x2E}}, //  16. MemoryReadResponse address=0x60 count=1 value=0x2E
    {TEL_RX,  7, 0, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xCE}}, //  17. T_ACK
    {TEL_RX, 10, 0, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x52, 0x01, 0x01, 0x0D}}, //  18. MemoryRead address=0x10D count=1 (run error)
    {TEL_TX,  7, 1, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xD2}}, //  19. T_ACK
    {TEL_TX, 11, 0, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x52, 0x41, 0x01, 0x0D, 0xFF}}, //  20. MemoryReadResponse address=0x10D count=1 value=0xFF
    {TEL_RX,  7, 0, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xD2}}, //  21. T_ACK
    {TEL_RX,  9, 0, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x62, 0x55, 0x84, 0x08}}, //  22. ADC_Read
    {TEL_TX,  7, 1, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xD6}}, //  23. T_ACK
    {TEL_TX, 11, 0, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x55, 0xC4, 0x08, 0x00, 0x00}}, // 24. ADC read currently only returns 0x0000 0x05, 0xB0}} //   30
    {TEL_RX,  7, 0, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xD6}}, //  25. T_ACK
    {TEL_RX, 10, 0, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x5A, 0x04, 0x01, 0x04}}, //  26. MemoryRead address=0x104 count=4 (manufacturerH, manufacturerL, deviceTypeH, deviceTypeL)
    {TEL_TX,  7, 1, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xDA}}, //  27. T_ACK
    {TEL_TX, 14, 0, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x67, 0x5A, 0x44, 0x01, 0x04, 0x04, 0x20, 0x60, 0x01}}, //  28. MemoryReadResponse address=0x104 count=4
    {TEL_RX,  7, 0, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xDA}}, //  29. T_ACK
    {TEL_RX, 10, 0, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x5E, 0x01, 0x00, 0x60}}, // 30. MemoryRead 0.0.1->1.1.126 count=1 address=0x60
    {TEL_TX,  7, 1, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xDE}}, //  31. T_ACK
    {TEL_TX, 11, 0, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x5E, 0x41, 0x00, 0x60, 0x2E}}, // 32. MemoryReadResponse 1.1.126->0.0.1 count=1 address=0x60
    {TEL_RX,  7, 0, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xDE}}, //  33. T_ACK
    {TEL_RX, 10, 0, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x62, 0x01, 0x01, 0x09}}, // 34. MemoryRead 0.0.1->1.1.126 count=1 address=0x109 (appPeiType)
    {TEL_TX,  7, 1, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xE2}}, //  35. T_ACK
    {TEL_TX, 11, 0, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x62, 0x41, 0x01, 0x09, 0x00}}, // 36. MemoryReadResponse 1.1.126->0.0.1 count=1 address=0x109
    {TEL_RX,  7, 0, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xE2}}, //  37. T_ACK
    {TEL_RX,  7, 0, 0, disconnect          , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0x81}}, //  38. T_DISCONNECT
    {END}, //  39.
};

static void tc_eepromSetup(void)
{
    bcuUnderTest->userEeprom->addrTab()[0] = 0x11;  // sets own address to 1.1.126
    bcuUnderTest->userEeprom->addrTab()[1] = 0x7E;  // sets own address to 1.1.126
    bcuUnderTest->userEeprom->appPeiType() = 0x00;
}

static void tc_setup(Telegram* tel, uint16_t telCount)
{
    telegramPreparation(bcuUnderTest, tel, telCount);
}

static void gatherProtocolState(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->state      = bcuUnderTest->userRam->status();
    state->connected  = bcuUnderTest->directConnection();
    state->ownAddress = bcuUnderTest->userEeprom->addrTab()[0] << 8 | bcuUnderTest->userEeprom->addrTab()[1];

    if(refState)
    {
        REQUIRE(state->state      == refState->state);
        REQUIRE(state->ownAddress == refState->ownAddress);
        REQUIRE(state->connected  == refState->connected);
    }
}

static Test_Case testCase =
{
    "Device Info Test",
    0x0004, 0x2060, 0x01,
    0,
    tc_eepromSetup,
    (TestCaseSetup *) tc_setup,
    (StateFunction *) gatherProtocolState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams
};

TEST_CASE("Protocol tests", "[protocol][device-info]")
{
    SECTION("BCU 1") {
        executeTest(BCU_1, &testCase);
    }

    SECTION("BCU 2") {
        executeTest(BCU_2, &testCase);
    }

    SECTION("BCU 0x0701 (BIM112)") {
        executeTest(BCU_0701, &testCase);
    }

    SECTION("BCU 0x0705 (BIM112)") {
        executeTest(BCU_0705, &testCase);
    }

    SECTION("BCU 0x07B0") {
        executeTest(BCU_07B0, &testCase);
    }
}
