/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Main Group Description
 * @defgroup SBLIB_SUB_GROUP_1 Sub Group 1 Description
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   prot_network_layer.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include "protocol.h"

typedef struct
{
    unsigned char  state;
    bool           connected;
    unsigned short ownAddress;
} ProtocolTestState;

static ProtocolTestState protoState[2];

#define VaS(s) ((ProtocolTestState *) (s))

static void tc_eepromSetup(void)
{
    bcuUnderTest->userEeprom->commsTabPtr() = 0x9A;
    bcuUnderTest->userEeprom->addrTabSize() = 0x02;
    bcuUnderTest->userEeprom->assocTabPtr() = 0x1B;

    bcuUnderTest->userEeprom->userEepromData[0x119 - EEPROM_START] = 0x10;
    bcuUnderTest->userEeprom->userEepromData[0x11A - EEPROM_START] = 0x01;
    bcuUnderTest->userEeprom->userEepromData[0x11B - EEPROM_START] = 0x14; // <<--- assocTabPtr
    bcuUnderTest->userEeprom->userEepromData[0x11C - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x11D - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x11E - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x11F - EEPROM_START] = 0x01;
    bcuUnderTest->userEeprom->userEepromData[0x120 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x121 - EEPROM_START] = 0x02;
    bcuUnderTest->userEeprom->userEepromData[0x122 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x123 - EEPROM_START] = 0x03;
    bcuUnderTest->userEeprom->userEepromData[0x124 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x125 - EEPROM_START] = 0x04;
    bcuUnderTest->userEeprom->userEepromData[0x126 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x127 - EEPROM_START] = 0x05;
    bcuUnderTest->userEeprom->userEepromData[0x128 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x129 - EEPROM_START] = 0x06;
    bcuUnderTest->userEeprom->userEepromData[0x12A - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x12B - EEPROM_START] = 0x07;
    bcuUnderTest->userEeprom->userEepromData[0x12C - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x12D - EEPROM_START] = 0x08;
    bcuUnderTest->userEeprom->userEepromData[0x12E - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x12F - EEPROM_START] = 0x09;
    bcuUnderTest->userEeprom->userEepromData[0x130 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x131 - EEPROM_START] = 0x0A;
    bcuUnderTest->userEeprom->userEepromData[0x132 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x133 - EEPROM_START] = 0x0B;
    bcuUnderTest->userEeprom->userEepromData[0x134 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x135 - EEPROM_START] = 0x0C;
    bcuUnderTest->userEeprom->userEepromData[0x136 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x137 - EEPROM_START] = 0x0D;
    bcuUnderTest->userEeprom->userEepromData[0x138 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x139 - EEPROM_START] = 0x0E;
    bcuUnderTest->userEeprom->userEepromData[0x13A - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x13B - EEPROM_START] = 0x0F;
    bcuUnderTest->userEeprom->userEepromData[0x13C - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x13D - EEPROM_START] = 0x10;
    bcuUnderTest->userEeprom->userEepromData[0x13E - EEPROM_START] = 0x01; // <<---
    bcuUnderTest->userEeprom->userEepromData[0x13F - EEPROM_START] = 0x11;
    bcuUnderTest->userEeprom->userEepromData[0x140 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x141 - EEPROM_START] = 0x12;
    bcuUnderTest->userEeprom->userEepromData[0x142 - EEPROM_START] = 0xFE;
    bcuUnderTest->userEeprom->userEepromData[0x143 - EEPROM_START] = 0x13;

    bcuUnderTest->userEeprom->userEepromData[0x19A - EEPROM_START] = 0x14; // <<--- commsTabPtr
    bcuUnderTest->userEeprom->userEepromData[0x19B - EEPROM_START] = 0xD6;
    bcuUnderTest->userEeprom->userEepromData[0x19C - EEPROM_START] = 0x54;
    bcuUnderTest->userEeprom->userEepromData[0x19D - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x19E - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x19F - EEPROM_START] = 0x55;
    bcuUnderTest->userEeprom->userEepromData[0x1A0 - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1A1 - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1A2 - EEPROM_START] = 0x56;
    bcuUnderTest->userEeprom->userEepromData[0x1A3 - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1A4 - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1A5 - EEPROM_START] = 0x57;
    bcuUnderTest->userEeprom->userEepromData[0x1A6 - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1A7 - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1A8 - EEPROM_START] = 0x58;
    bcuUnderTest->userEeprom->userEepromData[0x1A9 - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1AA - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1AB - EEPROM_START] = 0x59;
    bcuUnderTest->userEeprom->userEepromData[0x1AC - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1AD - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1AE - EEPROM_START] = 0x5A;
    bcuUnderTest->userEeprom->userEepromData[0x1AF - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1B0 - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1B1 - EEPROM_START] = 0x5B;
    bcuUnderTest->userEeprom->userEepromData[0x1B2 - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1B3 - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1B4 - EEPROM_START] = 0x5C;
    bcuUnderTest->userEeprom->userEepromData[0x1B5 - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1B6 - EEPROM_START] = 0x01;
    bcuUnderTest->userEeprom->userEepromData[0x1B7 - EEPROM_START] = 0x5D;
    bcuUnderTest->userEeprom->userEepromData[0x1B8 - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1B9 - EEPROM_START] = 0x01;
    bcuUnderTest->userEeprom->userEepromData[0x1BA - EEPROM_START] = 0x5E;
    bcuUnderTest->userEeprom->userEepromData[0x1BB - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1BC - EEPROM_START] = 0x01;
    bcuUnderTest->userEeprom->userEepromData[0x1BD - EEPROM_START] = 0x5F;
    bcuUnderTest->userEeprom->userEepromData[0x1BE - EEPROM_START] = 0x93;
    bcuUnderTest->userEeprom->userEepromData[0x1BF - EEPROM_START] = 0x01;
    bcuUnderTest->userEeprom->userEepromData[0x1C0 - EEPROM_START] = 0x60;
    bcuUnderTest->userEeprom->userEepromData[0x1C1 - EEPROM_START] = 0xCB;
    bcuUnderTest->userEeprom->userEepromData[0x1C2 - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1C3 - EEPROM_START] = 0x61;
    bcuUnderTest->userEeprom->userEepromData[0x1C4 - EEPROM_START] = 0xCB;
    bcuUnderTest->userEeprom->userEepromData[0x1C5 - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1C6 - EEPROM_START] = 0x62;
    bcuUnderTest->userEeprom->userEepromData[0x1C7 - EEPROM_START] = 0xCB;
    bcuUnderTest->userEeprom->userEepromData[0x1C8 - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1C9 - EEPROM_START] = 0x63;
    bcuUnderTest->userEeprom->userEepromData[0x1CA - EEPROM_START] = 0xCB;
    bcuUnderTest->userEeprom->userEepromData[0x1CB - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1CC - EEPROM_START] = 0x64;
    bcuUnderTest->userEeprom->userEepromData[0x1CD - EEPROM_START] = 0xCB;
    bcuUnderTest->userEeprom->userEepromData[0x1CE - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1CF - EEPROM_START] = 0x65; // <--
    bcuUnderTest->userEeprom->userEepromData[0x1D0 - EEPROM_START] = 0x9F;
    bcuUnderTest->userEeprom->userEepromData[0x1D1 - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1D2 - EEPROM_START] = 0x66;
    bcuUnderTest->userEeprom->userEepromData[0x1D3 - EEPROM_START] = 0xCB;
    bcuUnderTest->userEeprom->userEepromData[0x1D4 - EEPROM_START] = 0x00;
    bcuUnderTest->userEeprom->userEepromData[0x1D5 - EEPROM_START] = 0x67;
}

static Telegram testCaseTelegrams_GroupOriented[] =
{
    // KNX Spec. 2.1 8/3/3 3.1 Group oriented communication
    // routing count 6,5,4,3,2,1,0, 7 always answer with routing count 6
    // according to Spec. we should receive 0xE2 but thats not correct, it must be 0xE1
    // 1. routing count 6, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x81, 0x00, 0x00}},
    // 2. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 3. routing count 5, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x91, 0x00, 0x00}},
    // 4. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 5. routing count 4, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xA1, 0x00, 0x00}},
    // 6. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 7. routing count 3, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xB1, 0x00, 0x00}},
    // 8. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 9. routing count 2, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xC1, 0x00, 0x00}},
    // 10. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 11. routing count 1, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xD1, 0x00, 0x00}},
    // 12. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 13. routing count 0, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xE1, 0x00, 0x00}},
    // 14. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 15. routing count 7, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xF1, 0x00, 0x00}},
    // 16. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},
    {END}
};

static Telegram testCaseTelegrams_ConnectionOriented[] =
{
    ///\todo implement full test after routing count is working in lib
    // KNX Spec. 2.1 8/3/3 3.2 Connection oriented communication
    // routing count 6,5,4,3,2,1,0, 7 always answer with routing count 6
    // checking against maskversion 0x0012 not like in Spec 0x0020

    // 1. establish connection oriented communication
    {TEL_RX,  7, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x60, 0x80}}, //T_CONNECT
    // 2. routing count 6 MaskVersionRead()
    {TEL_RX,  8, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x61, 0x43, 0x00}},
    // 3.
    {TEL_TX,  7, 1, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x60, 0xC2}}, // T-Ack(Seq=0) ///\todo T_ACK always with system priority first byte 0xB0, change after TLayer4 is in sblib
    // 4.
    {TEL_TX, 10, 0, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x63, 0x43, 0x40, 0x00, 0x12}}, // MaskVersionResponse(Type=00, Version=12)


    // 5. routing count 7 MaskVersionRead()
    {TEL_RX,  8, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x51, 0x47, 0x00}},
    // 6.
    {TEL_TX,  7, 1, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x50, 0xC6}}, // T-Ack(Seq=1) ///\todo T_ACK always with system priority first byte 0xB0, change after TLayer4 is in sblib
    // 7.
    {TEL_TX, 10, 0, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x53, 0x47, 0x40, 0x00, 0x12}}, // MaskVersionResponse(Type=00, Version=12)



     // . Check for routing count 6 in GroupvalueResponse routing count

    // . disconnect
    {TEL_RX,  7, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x60, 0x81}}, //T_DISCONNECT
    {END}
};

static Telegram testCaseTelegrams_ConnectionLess[] =
{
    // KNX Spec. 2.1 8/3/3 3.3 Device oriented communication connectionless
    // routing count 6,5,4,3,2,1,0, 7 always answer with routing count 6
    ///\todo implement full test after connectionless is working in lib
    // 1. routing count 6, PropertyRead(Obj=00, Prop=01, Count=1, Start=001)
    {TEL_RX, 12, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x65, 0x03, 0xD5, 0x00, 0x01, 0x10, 0x01}},
    // 2. Check for routing count 6 in PropertyResponse(Obj=00, Prop=01, Count=1, Start=001, Data=00 00 )
    {TEL_TX, 14, 0, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x67, 0x03, 0xD6, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00}},

    ///\todo implement routing count 5...0

    // x. routing count 7, PropertyRead(Obj=00, Prop=01, Count=1, Start=001)
    {TEL_RX, 12, 0, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x75, 0x03, 0xD5, 0x00, 0x01, 0x10, 0x01}},
    // x. Check for routing count 6 in PropertyResponse(Obj=00, Prop=01, Count=1, Start=001, Data=00 00 )
    {TEL_TX, 14, 0, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x77, 0x03, 0xD6, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00}},
    {END}
};

static void tc_setup(Telegram* tel, uint16_t telCount)
{
    bcuUnderTest->setOwnAddress(0x1001); // set own address to 1.0.1
    telegramPreparation(bcuUnderTest, tel, telCount);
}

static void gatherProtocolState(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->state      = bcuUnderTest->userRam->status();
    state->ownAddress = bcuUnderTest->userEeprom->addrTab()[0] << 8 | bcuUnderTest->userEeprom->addrTab()[1];

    if(refState)
    {
        REQUIRE(state->state      == refState->state);
        REQUIRE(state->ownAddress == refState->ownAddress);
        REQUIRE(state->connected  == refState->connected);
    }
}

static Test_Case testCaseGroupOriented =
{
  "Group oriented communication",
  0x0004, 0x2060, 0x01,
  0,
  tc_eepromSetup,
  tc_setup,
  (StateFunction *) gatherProtocolState,
  (TestCaseState *) &protoState[0],
  (TestCaseState *) &protoState[1],
  testCaseTelegrams_GroupOriented
};

static Test_Case testCaseConnectionOriented =
{
  "Connection oriented communication",
  0x0004, 0x2060, 0x01,
  0,
  NULL,
  tc_setup,
  (StateFunction *) gatherProtocolState,
  (TestCaseState *) &protoState[0],
  (TestCaseState *) &protoState[1],
  testCaseTelegrams_ConnectionOriented
};

static Test_Case testCaseConnectionLess =
{
  "Connectionless communication",
  0x0004, 0x2060, 0x01,
  0,
  NULL,
  tc_setup,
  (StateFunction *) gatherProtocolState,
  (TestCaseState *) &protoState[0],
  (TestCaseState *) &protoState[1],
  testCaseTelegrams_ConnectionLess
};

///\todo TEST doesn't work at all
/*
TEST_CASE("Network Layer", "[protocol][L3]")
{

    SECTION("BCU 1") {
        executeTest(BCU_1, &testCaseGroupOriented);
        executeTest(BCU_1, &testCaseConnectionOriented);
        executeTest(BCU_1, &testCaseConnectionLess);
    }

    SECTION("BCU 2") {
        executeTest(BCU_2, &testCaseGroupOriented);
        executeTest(BCU_2, &testCaseConnectionOriented);
        executeTest(BCU_2, &testCaseConnectionLess);
    }

    SECTION("BCU 0x0701 (BIM112)") {
        executeTest(BCU_0701, &testCaseGroupOriented);
        executeTest(BCU_0701, &testCaseConnectionOriented);
        executeTest(BCU_0701, &testCaseConnectionLess);
    }

    SECTION("BCU 0x0705 (BIM112)") {
        executeTest(BCU_0705, &testCaseGroupOriented);
        executeTest(BCU_0705, &testCaseConnectionOriented);
        executeTest(BCU_0705, &testCaseConnectionLess);
    }

    SECTION("BCU 0x07B0 (BIM112)") {
        // executeTest(BCU_07B0, &testCaseGroupOriented);
        // executeTest(BCU_07B0, &testCaseConnectionOriented);
        // executeTest(BCU_07B0, &testCaseConnectionLess);
    }
}
*/
/** @}*/
