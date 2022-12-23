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

#define MANUFACTURER (0x0004)
#define DEVICE       (0x2060)
#define VERSION      (0x01)

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
    bcuUnderTest->userRam->status() ^= 0x81; // set the programming mode
    telegramPreparation(bcuUnderTest, tel, telCount);
}

static void tc_setupWithSerialNumber(Telegram* tel, uint16_t telCount)
{
   tc_setup(tel, telCount);
   UserEepromBCU2* eeprom = (UserEepromBCU2*)(bcuUnderTest->userEeprom);
   eeprom->serial()[0] = 0xC8;
   eeprom->serial()[1] = 0xAC;
   eeprom->serial()[2] = 0x02;
   eeprom->serial()[3] = 0x4C;
   eeprom->serial()[4] = 0x4F;
   eeprom->serial()[5] = 0x98;
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
    if (bcuUnderTest->userRam->status() & BCU_STATUS_PROGRAMMING_MODE)
    {
        bcuUnderTest->userRam->status()   ^= 0x81; // clear the programming mode
        VaS(state)->state = bcuUnderTest->userRam->status();
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

static Telegram apciIndividualAddressSerialNumberWriteTelegrams[] =
{
    // 1. APCI_INDIVIDUALADDRESS_SERIALNUMBER_WRITE_PDU
    //      from sourceAddr=10.0.1 to destAddr=0.0.0
    //      SerialNumber C8 AC 02 4C 4F 98
    //      new phys. address 15.15.255
    {TEL_RX, 20 , 0, 0, phy_addr_changed, {0xB0, 0xA0, 0x01, 0x00, 0x00, 0xED, 0x03, 0xDE, 0xC8, 0xAC, 0x02, 0x4C, 0x4F, 0x98, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00}},
    // 2. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}

    //{TEL_RX, 20 , 0, 0, NULL, {0xB0, 0x31, 0xE7, 0x00, 0x00, 0xED, 0x03, 0xDE, 0xC8, 0xAC, 0x02, 0x4C, 0x4F, 0x98, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00}},
};

static Telegram apciIndividualAddressSerialNumberReadTelegrams[] =
{
    // 1. APCI_INDIVIDUALADDRESS_SERIALNUMBER_READ_PDU
    //      from sourceAddr=10.0.1 to destAddr=0.0.0
    //      SerialNumber C8 AC 02 4C 4F 98
    {TEL_RX, 14 , 0, 0, NULL, {0xB0, 0x31, 0xE7, 0x00, 0x00, 0xE7, 0x03, 0xDC, 0xC8, 0xAC, 0x02, 0x4C, 0x4F, 0x98}},
    // 2. APCI_INDIVIDUALADDRESS_SERIALNUMBER_RESPONSE_PDU
    //      from sourceAddr 1.1.102 to destAddr = 0.0.0
    //      SerialNumber C8 AC 02 4C 4F 98
    //      domain address 00 00
    //      last 2 reserved 00 00
    {TEL_TX, 18, 0, 0, NULL, {0xB0, 0x11, 0xC9, 0x00, 0x00, 0xEB, 0x03, 0xDD, 0xC8, 0xAC, 0x02, 0x4C, 0x4F, 0x98, 0x00, 0x00, 0x00, 0x00}},
    {END}
};

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
    "APCI_INDIVIDUAL_ADDRESS_WRITE_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams,
};

static Test_Case testCaseApciIndividualAddressSerialNumberWritePdu =
{
    "APCI_INDIVIDUALADDRESS_SERIALNUMBER_WRITE_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setupWithSerialNumber,
    (StateFunction *) gatherProtocolState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    apciIndividualAddressSerialNumberWriteTelegrams
};

static Test_Case testCaseApciIndividualAddressSerialNumberReadPdu =
{
    "APCI_INDIVIDUALADDRESS_SERIALNUMBER_READ_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setupWithSerialNumber,
    (StateFunction *) gatherProtocolState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    apciIndividualAddressSerialNumberReadTelegrams
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

TEST_CASE("Programming of the physical address with serial number", "[protocol][address]")
{
    // BCU 1 doesn't support programming via serial number

    Test_Case* tcWrite = &testCaseApciIndividualAddressSerialNumberWritePdu;
    Test_Case* tcRead = &testCaseApciIndividualAddressSerialNumberReadPdu;
    SECTION("BCU 2") {
        executeTest(BCU_2, tcWrite);
        executeTest(BCU_2, tcRead);
    }

    SECTION("BCU 0x0701 (BIM112)") {
        executeTest(BCU_0701, tcWrite);
        executeTest(BCU_0701, tcRead);
    }

    SECTION("BCU 0x0705 (BIM112)") {
        executeTest(BCU_0705, tcWrite);
        executeTest(BCU_0705, tcRead);
    }

    SECTION("BCU 0x07B0") {
        executeTest(BCU_07B0, tcWrite);
        executeTest(BCU_07B0, tcRead);
    }
}
