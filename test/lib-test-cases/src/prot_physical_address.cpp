/*
 *  prot_parameter.cpp - Test case for programming the parameters
 *
 *  Copyright (c) 2014 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include "protocol.h"

typedef struct
{
    unsigned char  state;
    bool           connected;
    unsigned short ownAddress;
} ProtocolTestState;

static ProtocolTestState protoState[2];

#define VaS(s) ((ProtocolTestState *) (s))

static void tc_setup(Telegram* tel, uint16_t telCount)
{
    bcuUnderTest->setOwnAddress(0x11C9); // set own address to 1.1.102
    bcuUnderTest->userRam->status ^= 0x81; // set the programming mode
    telegramPreparation(bcuUnderTest, tel, telCount);
}

static void connect(void * state, unsigned int param)
{
    VaS(state)->connected = true;
}

static void disconnect(void * state, unsigned int param)
{
    VaS(state)->connected = false;
}

static void phy_addr_changed(void * state, unsigned int param)
{
    VaS(state)->ownAddress = bcuUnderTest->userEeprom->addrTab()[0] << 8 | bcuUnderTest->userEeprom->addrTab()[1];
}

static void clearProgMode(void * state, unsigned int param)
{
    if (bcuUnderTest->userRam->status & BCU_STATUS_PROG)
    {
        bcuUnderTest->userRam->status   ^= 0x81; // clear the programming mode
        VaS(state)->state = bcuUnderTest->userRam->status;
    }
}

static Telegram testCaseTelegrams[] =
{
        //Step     type   len var. StepFunction        bytes[23];
        //  1.  0.0.1 -> 0.0.0 APCI_INDIVIDUAL_ADDRESS_READ_PDU broadcast
        {TEL_RX,  8, 0, 1, NULL, {0xB0, 0x00, 0x01, 0x00, 0x00, 0xE1, 0x01, 0x00}},
        //  2.  own 1.1.102 -> 0.0.0 APCI_INDIVIDUAL_ADDRESS_RESPONSE_PDU
        {TEL_TX,  8, 0, 0, NULL, {0xB0, 0x11, 0xC9, 0x00, 0x00, 0xE1, 0x01, 0x40}},
        //  3.  other responding
        {TEL_RX,  7, 0, 0, NULL, {0xB0, 0x00, 0x01, 0x11, 0x12, 0x60, 0x80}},
        //  4.
        {TEL_RX,  8, 0, 0, NULL, {0xB0, 0x00, 0x01, 0x11, 0x12, 0x61, 0x43, 0x00}},
        //  5.
        {TEL_RX,  7, 0, 0, NULL, {0xB0, 0x00, 0x01, 0x11, 0x12, 0x60, 0x81}},
        //  6.  0.0.1 -> 0.0.0 APCI_INDIVIDUAL_ADDRESS_WRITE_PDU  1.1.12
        {TEL_RX, 10, 0, 0, phy_addr_changed, {0xB0, 0x00, 0x01, 0x00, 0x00, 0xE3, 0x00, 0xC0, 0x11, 0x12}},
        //  7.
        {TEL_RX,  8, 0, 1, NULL, {0xB0, 0x00, 0x01, 0x00, 0x00, 0xE1, 0x01, 0x00}},
        //  8.
        {TEL_TX,  8, 0, 0, NULL, {0xB0, 0x11, 0x12, 0x00, 0x00, 0xE1, 0x01, 0x40}},
        //  9.  T_CONNECT 0.0.1 -> 1.1.12
        {TEL_RX,  7, 0, 0, connect, {0xB0, 0x00, 0x01, 0x11, 0x12, 0x60, 0x80}},
        // 10.
        {TEL_RX,  8, 0, 2, NULL, {0xB0, 0x00, 0x01, 0x11, 0x12, 0x61, 0x43, 0x00}},
        // 11.  1.1.12 -> 0.0.1 T_ACK
        {TEL_TX,  7, 1, 0, NULL, {0xB0, 0x11, 0x12, 0x00, 0x01, 0x60, 0xC2}},
        // 12.
        {TEL_TX, 10, 0, 0, NULL, {0xB0, 0x11, 0x12, 0x00, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},
        // 13.
        {TEL_RX,  7, 0, 0, NULL, {0xB0, 0x00, 0x01, 0x11, 0x12, 0x60, 0xC2}},
        // 14.
        {TEL_RX,  8, 0, 0, clearProgMode, {0xB0, 0x00, 0x01, 0x11, 0x12, 0x61, 0x47, 0x80}},
        // 15.
        {TEL_RX,  7, 0, 0, disconnect, {0xB0, 0x00, 0x01, 0x11, 0x12, 0x60, 0x81}},
        // 16.
        {TEL_RX,  8, 0, 0, NULL, {0xB0, 0x00, 0x01, 0x00, 0x00, 0xE1, 0x01, 0x00}},
        // 17.
        {END}
};

static void gatherProtocolState(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->state      = bcuUnderTest->userRam->status;
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
    "Phy Addr Prog",
    0x0004, 0x2060, 0x01,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams,
};

TEST_CASE("Programming of the physical address", "[protocol][address]")
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


