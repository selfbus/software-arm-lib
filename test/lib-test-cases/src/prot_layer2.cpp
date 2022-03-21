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

TLayer4* bcuUnderTest = (TLayer4*) &bcu;

typedef struct
{
    unsigned char  state;
    bool           connected;
    TLayer4::TL4State machineState;
    int connectedAddr;
    byte seqNoSend;
    byte seqNoRcv;
    bool telegramReadyToSend;
} ProtocolTestState;

static ProtocolTestState protoState[2];

#define VaS(s) ((ProtocolTestState *) (s))

static void tc_setup(void)
{
    bcu.setTL4State(TLayer4::CLOSED); // to "reset" connection for next test
    bcu.setOwnAddress(0xA000); // set own address to 10.00.000
}

static void connect(void * state, unsigned int param)
{
    REQUIRE(param > 0);
    VaS(state)->connectedAddr = param;
    VaS(state)->machineState = TLayer4::OPEN_IDLE;
    VaS(state)->seqNoSend = 0;
    VaS(state)->seqNoRcv = 0;
    VaS(state)->telegramReadyToSend = false;
}

/**
 * \todo when L2->L4 callback L2_ACK sent is implemented
 *       delete this function and replace in with telegrams with connectedOpenIdle
 */
static void connectedOpenIdleOrWait(void * state, unsigned int param)
{
    REQUIRE(param > 0);
    VaS(state)->connectedAddr = param;
    VaS(state)->machineState = TLayer4::OPEN_WAIT;
}

static void connectedOpenIdle(void * state, unsigned int param)
{
    REQUIRE(param > 0);
    VaS(state)->connectedAddr = param;
    VaS(state)->machineState = TLayer4::OPEN_IDLE;
}

static void connectedOpenWait(void * state, unsigned int param)
{
    REQUIRE(param > 0);
    VaS(state)->connectedAddr = param;
    VaS(state)->machineState = TLayer4::OPEN_WAIT;
}

static void disconnectClosed(void * state, unsigned int param)
{
    VaS(state)->connectedAddr = 0;
    VaS(state)->machineState = TLayer4::CLOSED;
}

static void gatherProtocolStateCustomCheckRepeatedFlag(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->machineState = bcuUnderTest->state;
    state->connectedAddr = bcuUnderTest->connectedTo();

    if(refState)
    {
        REQUIRE(state->machineState == refState->machineState);
        REQUIRE(state->connectedAddr == refState->connectedAddr);
    }
}

static Telegram testCaseTelegrams_CustomCheckRepeatedFlag[] =
{
    // Test is not in KNX Spec.
    // send the same telegram twice, second time with repeated flag set
    // second repeated telegram should be filtered by class TLayer4
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0xA001, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. DeviceDescriptorRead(DescType=00) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  8, 0, 0xA001, connectedOpenIdleOrWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    // 3. Check T_ACK response for the DeviceDescriptorRead, bcu.loop() once, so DeviceDescriptorResponse will be send and state set to OPEN_WAIT
    {TEL_TX,  7, 1, 0xA001, connectedOpenIdleOrWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}}, // T_ACK_PDU
    // 4. Check DeviceDescriptorResponse Maskversion 0x0012
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, MASK_VERSION_HIGH, MASK_VERSION_LOW}}, // DeviceDescriptorResponse(DescType=00, Descriptor=0x0012)
    // 5. Check for empty TX
    {CHECK_TX_BUFFER,  0, 0, 0, NULL, {}},
    // 6. with repeated flag set DeviceDescriptorRead(DescType=00) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  8, 0, 0xA001, connectedOpenWait, {0x90, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    // 7. Check for empty TX-Response
    {CHECK_TX_BUFFER,  0, 0, 0, NULL, {}},
    // 8. T_DISCONNECT_PDU (0x81) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x81}},
    {END}
};

static Test_Case testCaseTelegramSequence_CustomCheckRepeatedFlag =
{
    "Repeated DeviceDescriptorRead",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolStateCustomCheckRepeatedFlag,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_CustomCheckRepeatedFlag
};

TEST_CASE("Layer 2 protocol", "[protocol][L2]")
{
   executeTest(& testCaseTelegramSequence_CustomCheckRepeatedFlag);
}
