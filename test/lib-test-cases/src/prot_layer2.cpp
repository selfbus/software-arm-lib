/*
 *  prot_layer2.cpp - Test case for layer 2 protocol
 *
 *  Copyright (c) 2022 Darthyson <darth@maptrack.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include "protocol.h"

#define MANUFACTURER (0x0004)
#define DEVICE       (0x2060)
#define VERSION      (0x01)

typedef struct
{
    unsigned char  state;
    bool           connected;
    unsigned short ownAddress;
    int connectedAddr;
} ProtocolTestState;

static ProtocolTestState protoState[2];

#define VaS(s) ((ProtocolTestState *) (s))

static void tc_setup(void)
{
    bcu.setOwnAddress(0xA000); // set own address to 10.00.000
}

static void connect(void * state, unsigned int param)
{
    VaS(state)->connected = true;
    VaS(state)->connectedAddr = 0xA001;
}

static void gatherProtocolState(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->state      = userRam.status;
    state->connected  = bcu.directConnection();
    state->ownAddress = userEeprom.addrTab[0] << 8 | userEeprom.addrTab[1];

    state->connectedAddr = bcu.connectedAddr;

    if(refState)
    {
        REQUIRE(state->state      == refState->state);
        REQUIRE(state->ownAddress == refState->ownAddress);
        REQUIRE(state->connected  == refState->connected);
        REQUIRE(state->connectedAddr == refState->connectedAddr);
    }
}

static Telegram testCaseTelegrams_CustomCheckRepeatedFlag[] =
{
    // Test is not in KNX Spec.
    // send the same telegram twice, second time with repeated flag set
    // second repeated telegram should be filtered by class Bus
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0xA001, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. DeviceDescriptorRead(DescType=00) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  8, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    // 3. Check T_ACK response for the DeviceDescriptorRead
    {TEL_TX,  7, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}}, // T_ACK_PDU
    // 4. Check DeviceDescriptorResponse Maskversion 0x0012
    {TEL_TX,  10, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, 0x00, 0x12}}, // DeviceDescriptorResponse(DescType=00, Descriptor=0x0012)
    // 5. Check for empty TX
    {CHECK_TX_BUFFER,  0, 0, NULL, {}},
    // 6. with repeated flag set DeviceDescriptorRead(DescType=00) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  8, 0, NULL, {0x90, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}}, ///\todo this fails, because class Bus doesn't filter repeated telegrams
    // 7. Check for empty TX-Response
    {CHECK_TX_BUFFER,  0, 0, NULL, {}},
    {END}
};

static Test_Case testCaseTelegramSequence_CustomCheckRepeatedFlag =
{
    "Repeated DeviceDescriptorRead",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_CustomCheckRepeatedFlag
};

TEST_CASE("Layer 2 protocol", "[protocol][L2]")
{
    executeTest(& testCaseTelegramSequence_CustomCheckRepeatedFlag);
}
