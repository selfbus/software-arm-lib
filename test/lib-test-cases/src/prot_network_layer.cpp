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
    userEeprom.commsTabPtr = 0x9A;
    userEeprom.addrTabSize = 0x02;
    userEeprom.assocTabPtr = 0x1B;


    userEeprom[0x119] = 0x10;
    userEeprom[0x11A] = 0x01;
    userEeprom[0x11B] = 0x14; // <<--- assocTabPtr
    userEeprom[0x11C] = 0xFE;
    userEeprom[0x11D] = 0x00;
    userEeprom[0x11E] = 0xFE;
    userEeprom[0x11F] = 0x01;
    userEeprom[0x120] = 0xFE;
    userEeprom[0x121] = 0x02;
    userEeprom[0x122] = 0xFE;
    userEeprom[0x123] = 0x03;
    userEeprom[0x124] = 0xFE;
    userEeprom[0x125] = 0x04;
    userEeprom[0x126] = 0xFE;
    userEeprom[0x127] = 0x05;
    userEeprom[0x128] = 0xFE;
    userEeprom[0x129] = 0x06;
    userEeprom[0x12A] = 0xFE;
    userEeprom[0x12B] = 0x07;
    userEeprom[0x12C] = 0xFE;
    userEeprom[0x12D] = 0x08;
    userEeprom[0x12E] = 0xFE;
    userEeprom[0x12F] = 0x09;
    userEeprom[0x130] = 0xFE;
    userEeprom[0x131] = 0x0A;
    userEeprom[0x132] = 0xFE;
    userEeprom[0x133] = 0x0B;
    userEeprom[0x134] = 0xFE;
    userEeprom[0x135] = 0x0C;
    userEeprom[0x136] = 0xFE;
    userEeprom[0x137] = 0x0D;
    userEeprom[0x138] = 0xFE;
    userEeprom[0x139] = 0x0E;
    userEeprom[0x13A] = 0xFE;
    userEeprom[0x13B] = 0x0F;
    userEeprom[0x13C] = 0xFE;
    userEeprom[0x13D] = 0x10;
    userEeprom[0x13E] = 0x01; // <<---
    userEeprom[0x13F] = 0x11;
    userEeprom[0x140] = 0xFE;
    userEeprom[0x141] = 0x12;
    userEeprom[0x142] = 0xFE;
    userEeprom[0x143] = 0x13;

    userEeprom[0x19A] = 0x14; // <<--- commsTabPtr
    userEeprom[0x19B] = 0xD6;
    userEeprom[0x19C] = 0x54;
    userEeprom[0x19D] = 0x93;
    userEeprom[0x19E] = 0x00;
    userEeprom[0x19F] = 0x55;
    userEeprom[0x1A0] = 0x93;
    userEeprom[0x1A1] = 0x00;
    userEeprom[0x1A2] = 0x56;
    userEeprom[0x1A3] = 0x93;
    userEeprom[0x1A4] = 0x00;
    userEeprom[0x1A5] = 0x57;
    userEeprom[0x1A6] = 0x93;
    userEeprom[0x1A7] = 0x00;
    userEeprom[0x1A8] = 0x58;
    userEeprom[0x1A9] = 0x93;
    userEeprom[0x1AA] = 0x00;
    userEeprom[0x1AB] = 0x59;
    userEeprom[0x1AC] = 0x93;
    userEeprom[0x1AD] = 0x00;
    userEeprom[0x1AE] = 0x5A;
    userEeprom[0x1AF] = 0x93;
    userEeprom[0x1B0] = 0x00;
    userEeprom[0x1B1] = 0x5B;
    userEeprom[0x1B2] = 0x93;
    userEeprom[0x1B3] = 0x00;
    userEeprom[0x1B4] = 0x5C;
    userEeprom[0x1B5] = 0x93;
    userEeprom[0x1B6] = 0x01;
    userEeprom[0x1B7] = 0x5D;
    userEeprom[0x1B8] = 0x93;
    userEeprom[0x1B9] = 0x01;
    userEeprom[0x1BA] = 0x5E;
    userEeprom[0x1BB] = 0x93;
    userEeprom[0x1BC] = 0x01;
    userEeprom[0x1BD] = 0x5F;
    userEeprom[0x1BE] = 0x93;
    userEeprom[0x1BF] = 0x01;
    userEeprom[0x1C0] = 0x60;
    userEeprom[0x1C1] = 0xCB;
    userEeprom[0x1C2] = 0x00;
    userEeprom[0x1C3] = 0x61;
    userEeprom[0x1C4] = 0xCB;
    userEeprom[0x1C5] = 0x00;
    userEeprom[0x1C6] = 0x62;
    userEeprom[0x1C7] = 0xCB;
    userEeprom[0x1C8] = 0x00;
    userEeprom[0x1C9] = 0x63;
    userEeprom[0x1CA] = 0xCB;
    userEeprom[0x1CB] = 0x00;
    userEeprom[0x1CC] = 0x64;
    userEeprom[0x1CD] = 0xCB;
    userEeprom[0x1CE] = 0x00;
    userEeprom[0x1CF] = 0x65; // <--
    userEeprom[0x1D0] = 0x9F;
    userEeprom[0x1D1] = 0x00;
    userEeprom[0x1D2] = 0x66;
    userEeprom[0x1D3] = 0xCB;
    userEeprom[0x1D4] = 0x00;
    userEeprom[0x1D5] = 0x67;
}

static Telegram testCaseTelegrams_GroupOriented[] =
{
    // KNX Spec. 2.1 8/3/3 3.1 Group oriented communication
    // routing count 6,5,4,3,2,1,0, 7 always answer with routing count 6
    // according to Spec. we should receive 0xE2 but thats not correct, it must be 0xE1
    // 1. routing count 6, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x81, 0x00, 0x00}},
    // 2. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 3. routing count 5, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x91, 0x00, 0x00}},
    // 4. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 5. routing count 4, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xA1, 0x00, 0x00}},
    // 6. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 7. routing count 3, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xB1, 0x00, 0x00}},
    // 8. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 9. routing count 2, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xC1, 0x00, 0x00}},
    // 10. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 11. routing count 1, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xD1, 0x00, 0x00}},
    // 12. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 13. routing count 0, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX, 8, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xE1, 0x00, 0x00}},
    // 14. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},

    // 15. routing count 7, GroupvalueRead 2/0/1 from 10.15.254
    {TEL_RX,  8, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0xF1, 0x00, 0x00}},
    // 16. Check for routing count 6 in GroupvalueResponse routing count
    {TEL_TX, 8, 0, NULL, {0xBC, 0x10, 0x01, 0x10, 0x01, 0xE1, 0x00, 0x40}},
    {END}
};

static Telegram testCaseTelegrams_ConnectionOriented[] =
{
    ///\todo implement full test after routing count is working in lib
    // KNX Spec. 2.1 8/3/3 3.2 Connection oriented communication
    // routing count 6,5,4,3,2,1,0, 7 always answer with routing count 6
    // checking against maskversion 0x0012 not like in Spec 0x0020

    // 1. establish connection oriented communication
    {TEL_RX,  7, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x60, 0x80}}, //T_CONNECT
    // 2. routing count 6 MaskVersionRead()
    {TEL_RX,  8, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x61, 0x43, 0x00}},
    // 3.
    {TEL_TX,  7, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x60, 0xC2}}, // T-Ack(Seq=0) ///\todo T_ACK always with system priority first byte 0xB0, change after TLayer4 is in sblib
    // 4.
    {TEL_TX, 10, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x63, 0x43, 0x40, 0x00, 0x12}}, // MaskVersionResponse(Type=00, Version=12)


    // 5. routing count 7 MaskVersionRead()
    {TEL_RX,  8, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x51, 0x47, 0x00}},
    // 6.
    {TEL_TX,  7, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x50, 0xC6}}, // T-Ack(Seq=1) ///\todo T_ACK always with system priority first byte 0xB0, change after TLayer4 is in sblib
    // 7.
    {TEL_TX, 10, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x53, 0x47, 0x40, 0x00, 0x12}}, // MaskVersionResponse(Type=00, Version=12)



     // . Check for routing count 6 in GroupvalueResponse routing count

    // . disconnect
    {TEL_RX,  7, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x60, 0x81}}, //T_DISCONNECT
    {END}
};

static Telegram testCaseTelegrams_ConnectionLess[] =
{
    // KNX Spec. 2.1 8/3/3 3.3 Device oriented communication connectionless
    // routing count 6,5,4,3,2,1,0, 7 always answer with routing count 6
    ///\todo implement full test after connectionless is working in lib
    // 1. routing count 6, PropertyRead(Obj=00, Prop=01, Count=1, Start=001)
    {TEL_RX, 12, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x65, 0x03, 0xD5, 0x00, 0x01, 0x10, 0x01}},
    // 2. Check for routing count 6 in PropertyResponse(Obj=00, Prop=01, Count=1, Start=001, Data=00 00 )
    {TEL_TX, 14, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x67, 0x03, 0xD6, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00}},

    ///\todo implement routing count 5...0

    // x. routing count 7, PropertyRead(Obj=00, Prop=01, Count=1, Start=001)
    {TEL_RX, 12, 0, NULL, {0xBC, 0xAF, 0xFE, 0x10, 0x01, 0x75, 0x03, 0xD5, 0x00, 0x01, 0x10, 0x01}},
    // x. Check for routing count 6 in PropertyResponse(Obj=00, Prop=01, Count=1, Start=001, Data=00 00 )
    {TEL_TX, 14, 0, NULL, {0xBC, 0x10, 0x01, 0xAF, 0xFE, 0x77, 0x03, 0xD6, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00}},


    {END}
};

static void tc_setup(void)
{
    bcu.setOwnAddress(0x1001); // set own address to 1.0.1
}

static void gatherProtocolState(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->state      = userRam.status;
    state->ownAddress = userEeprom.addrTab[0] << 8 | userEeprom.addrTab[1];

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


TEST_CASE("Network Layer", "[protocol][L3]")
{
    // executeTest(& testCaseGroupOriented);
    // executeTest(& testCaseConnectionOriented);
    executeTest(& testCaseConnectionLess);
}

/** @}*/
