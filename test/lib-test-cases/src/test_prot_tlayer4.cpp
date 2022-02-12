/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Selfbus library
 * @defgroup SBLIB_UNIT_TEST Unit tests
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   Catch unit test for the transport layer 4
 * @details
 *
 *
 * @{
 *
 * @file   prot_tlayer4.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <catch.hpp>
#include <protocol.h>
#include <sblib/timer.h>

#include "tc_tlayer4_telegram.h"


#define MANUFACTURER (0x0004)
#define DEVICE       (0x2060)
#define VERSION      (0x01)

#define OWN_KNX_ADDRESS (0xA000) // own address 10.0.0

TLayer4* bcuTL4 = (TLayer4*) &bcu;

static void tc_setup(void)
{
    bcuTL4->setTL4State(TLayer4::CLOSED); // to "reset" connection for next test
    bcuTL4->setOwnAddress(OWN_KNX_ADDRESS);
}

static void tc_setup_1001(void)
{
    bcuTL4->state = TLayer4::CLOSED; // to "reset" connection for next test
    bcuTL4->setOwnAddress(0x1001);
}

static void tc_setup_OpenWait(void)
{
    tc_setup();
    bcuTL4->connectedTime = millis(); // "start connection timeout timer"
    bcuTL4->state = TLayer4::OPEN_WAIT;
    bcuTL4->seqNoRcv = 0;
    bcuTL4->seqNoSend = 0;
}

static void tc_setup_OpenWait_A007(void)
{
    tc_setup_OpenWait();
    bcuTL4->connectedAddr = 0xA007;
}

static void tc_setup_OpenWait_A001(void)
{
    tc_setup_OpenWait();
    bcuTL4->connectedAddr = 0xA001;
}

static void gatherProtocolState_01(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->connected  = bcuTL4->directConnection();
    state->ownAddress = bcuTL4->ownAddress();

    state->connectedAddrNew = bcuTL4->connectedTo();
    state->machineState = bcuTL4->state;
    state->seqNoSend = bcuTL4->seqNoSend;
    state->seqNoRcv = bcuTL4->seqNoRcv;
    state->telegramReadyToSend = bcuTL4->telegramReadyToSend;

    if(refState)
    {
        REQUIRE(state->ownAddress == refState->ownAddress);
        REQUIRE(state->machineState == refState->machineState);
        REQUIRE(state->connectedAddrNew == refState->connectedAddrNew);
        REQUIRE(state->seqNoSend == refState->seqNoSend);
        REQUIRE(state->seqNoRcv == refState->seqNoRcv);
        REQUIRE(state->telegramReadyToSend == refState->telegramReadyToSend);
    }
}

static void gatherProtocolState_02(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->ownAddress = bcuTL4->ownAddress();
    state->machineState = bcuTL4->state;

    if(refState)
    {
        REQUIRE(state->ownAddress == refState->ownAddress);
        REQUIRE(state->machineState == refState->machineState);
    }
}

static void gatherProtocolState_03(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->ownAddress = bcuTL4->ownAddress();
    state->machineState = bcuTL4->state;

    if(refState)
    {
        REQUIRE(state->ownAddress == refState->ownAddress);
        REQUIRE(state->machineState == refState->machineState);
    }
}

static void gatherProtocolState_04(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->ownAddress = bcuTL4->ownAddress();
    state->machineState = bcuTL4->state;
    state->connectedAddrNew = bcuTL4->connectedTo();

    if(refState)
    {
        REQUIRE(state->ownAddress == refState->ownAddress);
        REQUIRE(state->machineState == refState->machineState);
        REQUIRE(state->connectedAddrNew == refState->connectedAddrNew);
    }
}

static void gatherProtocolState_stillConnected(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->ownAddress = bcuTL4->ownAddress();
    state->machineState = bcuTL4->state;
    state->connectedAddrNew = bcuTL4->connectedTo();

    if(refState)
    {
        REQUIRE(state->ownAddress == refState->ownAddress);
        REQUIRE(state->machineState == refState->machineState);
        REQUIRE(state->connectedAddrNew == refState->connectedAddrNew);
    }
}

static void gatherProtocolState_machineState(ProtocolTestState * state, ProtocolTestState * refState)
{
    state->machineState = bcuTL4->state;

    if(refState)
    {
        REQUIRE(state->machineState == refState->machineState);
    }
}

static Test_Case testCaseTelegramSequence_01 =
{
    "TelSeq 1 Connect from a remote device with initial state CLOSED",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    (StateFunction *) gatherProtocolState_01,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_01
};

static Test_Case testCaseTelegramSequence_02 =
{
    "TelSeq 2 Connect from remote device with initial state OPEN_IDLE/OPEN_WAIT",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    (StateFunction *) gatherProtocolState_02,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_02
};

static Test_Case testCaseTelegramSequence_03 =
{
    "TelSeq 3 Connect from remote device with initial state OPEN_WAIT (style 1)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_03,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_03
};

static Test_Case testCaseTelegramSequence_04 =
{
    "TelSeq 4 Connect while connected, with initial state OPEN_IDLE (style 1/3)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_04,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_04
};

static Test_Case testCaseTelegramSequence_05 =
{
    "TelSeq 5 Connect while connected, with initial state OPEN_WAIT (style 1/3)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A007,
    (StateFunction *) gatherProtocolState_stillConnected,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_05
};

static Test_Case testCaseTelegramSequence_06 =
{
    "TelSeq 6 Disconnect from a remote device, with initial state CLOSED (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_06
};

static Test_Case testCaseTelegramSequence_07 =
{
    "TelSeq 7 Disconnect from a remote device, with initial state OPEN_IDLE (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_07
};

static Test_Case testCaseTelegramSequence_08 =
{
    "TelSeq 8 Disconnect from a remote device, with initial state OPEN_WAIT (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A001,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_08
};

static Test_Case testCaseTelegramSequence_09 =
{
    "TelSeq 9 Disconnect from a foreign device, with initial state OPEN_IDLE (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_stillConnected,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_09
};

static Test_Case testCaseTelegramSequence_10 =
{
    "TelSeq 10 Disconnect from a foreign device, with initial state OPEN_WAIT (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A007,
    (StateFunction *) gatherProtocolState_stillConnected,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_10
};

static Test_Case testCaseTelegramSequence_18 =
{
    "TelSeq 18 Connection timeout, with initial state OPEN_IDLE (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_18
};

static Test_Case testCaseTelegramSequence_19 =
{
    "TelSeq 19 Acknowledgment timeout, with initial state OPEN_WAIT (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A001,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_19
};

static Test_Case testCaseTelegramSequence_20a =
{
    "TelSeq 20a Reception of correct N_Data_Individual, with initial state OPEN_IDLE (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_20a
};

static Test_Case testCaseTelegramSequence_20b =
{
    "TelSeq 20b Reception of correct N_Data_Individual, with initial state OPEN_WAIT (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_1001,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_20b
};

static Test_Case testCaseTelegramSequence_21 =
{
    "TelSeq 21 Reception of a repeated N_Data_Individual, with initial state OPEN_IDLE (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_1001,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_21
};

static Test_Case testCaseTelegramSequence_22 =
{
    "TelSeq 22 Reception of a repeated N_Data_Individual, with initial state OPEN_WAIT (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A001,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_22
};

static Test_Case testCaseTelegramSequence_23 =
{
    "TelSeq 23 Reception of N_Data_Individual wrong sequence number, with initial state OPEN_IDLE (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_23
};

static Test_Case testCaseTelegramSequence_24 =
{
    "TelSeq 24 Reception of N_Data_Individual wrong sequence number, with initial state OPEN_WAIT (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A001,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_24
};

static Test_Case testCaseTelegramSequence_25 =
{
    "TelSeq 25 Reception of N_Data_Individual wrong source address, with initial state OPEN_IDLE (style 1)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_25
};

static Test_Case testCaseTelegramSequence_26 =
{
    "TelSeq 25 Reception of N_Data_Individual wrong source address, with initial state OPEN_WAIT (style 1)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A007,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_26
};

static Test_Case testCaseTelegramSequence_31 =
{
    "TelSeq 31 Reception of T_ACK with wrong sequence number, with initial state OPEN_IDLE (style 1/2)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_31
};

static Test_Case testCaseTelegramSequence_32 =
{
    "TelSeq 32 Reception of T_ACK with wrong sequence number, with initial state OPEN_WAIT (style 1/3)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A001,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_32
};

static Test_Case testCaseTelegramSequence_33 =
{
    "TelSeq 33 Reception of T_ACK with wrong connection address, with initial state OPEN_IDLE (style 1)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_33
};

static Test_Case testCaseTelegramSequence_34 =
{
    "TelSeq 34 Reception of T_ACK with wrong connection address, with initial state OPEN_WAIT (style 1)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A007,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_34
};

static Test_Case testCaseTelegramSequence_35 =
{
    "TelSeq 35 Reception of T_NACK with wrong sequence number, with initial state OPEN_IDLE (style 1,2)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_35
};

static Test_Case testCaseTelegramSequence_36 =
{
    "TelSeq 36 Reception of T_NACK with wrong sequence number, with initial state OPEN_WAIT (style 1,3)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A001,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_36
};

static Test_Case testCaseTelegramSequence_37 =
{
    "TelSeq 37 Reception of T_NACK with correct sequence number, with initial state OPEN_IDLE (style 1,3)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_37
};

static Test_Case testCaseTelegramSequence_38 =
{
    "TelSeq 38 Reception of T_NACK with correct sequence number and max. repetitions not reached, with initial state OPEN_WAIT (all styles)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A001,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_38
};

static Test_Case testCaseTelegramSequence_39 =
{
    "TelSeq 39 Reception of T_NACK with correct sequence number and max. repetitions reached, with initial state OPEN_WAIT (all styles/optional style 1 rationalised)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A001,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_39
};

static Test_Case testCaseTelegramSequence_40 =
{
    "TelSeq 40 Reception of T_NACK with wrong connection address, with initial state OPEN_IDLE (style 1)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_40
};

static Test_Case testCaseTelegramSequence_41 =
{
    "TelSeq 41 Reception of T_NACK with wrong connection address, with initial state OPEN_WAIT (style 1)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup_OpenWait_A007,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_41
};

static Test_Case testCaseTelegramSequence_42 =
{
    "TelSeq 42 T_DATA_CONNECTED, T_ACK, T_NACK, with initial state CLOSED (style 1)",
    MANUFACTURER, DEVICE, VERSION,
    0,
    NULL,
    tc_setup,
    (StateFunction *) gatherProtocolState_machineState,
    (TestCaseState *) &protoState[0],
    (TestCaseState *) &protoState[1],
    testCaseTelegrams_42
};

TEST_CASE("Transport layer 4 protocol", "[protocol][L4]")
{
    // See KNX Spec. 2.1 8/3/4 for all test details
    executeTest(& testCaseTelegramSequence_01);
    executeTest(& testCaseTelegramSequence_02);
    executeTest(& testCaseTelegramSequence_03);
    executeTest(& testCaseTelegramSequence_04);
    executeTest(& testCaseTelegramSequence_05);
    executeTest(& testCaseTelegramSequence_06);
    executeTest(& testCaseTelegramSequence_07);
    executeTest(& testCaseTelegramSequence_08);
    executeTest(& testCaseTelegramSequence_09);
    executeTest(& testCaseTelegramSequence_10);
    // test cases 11-17 not implemented, they are for client only, (connect/disconnect from the local user)
    executeTest(& testCaseTelegramSequence_18);
    executeTest(& testCaseTelegramSequence_19);
    executeTest(& testCaseTelegramSequence_20a);

    ///\todo implement test case 20b correctly
    // executeTest(& testCaseTelegramSequence_20b);

    executeTest(& testCaseTelegramSequence_21);
    executeTest(& testCaseTelegramSequence_22);
    executeTest(& testCaseTelegramSequence_23);
    executeTest(& testCaseTelegramSequence_24);
    executeTest(& testCaseTelegramSequence_25);
    executeTest(& testCaseTelegramSequence_26);
    // test cases 27-30 not implemented, they are for client only, (T_DATA_CONNECTED from the local user)
    executeTest(& testCaseTelegramSequence_31); // from KNX Spec note: "Current System 1 implementations (see volume 6) transmit faulty telegrams. This is not allowed for updated versions or new developed system 1 implementations."
    executeTest(& testCaseTelegramSequence_32);
    executeTest(& testCaseTelegramSequence_33);
    executeTest(& testCaseTelegramSequence_34);
    executeTest(& testCaseTelegramSequence_35);
    executeTest(& testCaseTelegramSequence_36);
    executeTest(& testCaseTelegramSequence_37);
    executeTest(& testCaseTelegramSequence_38);
    executeTest(& testCaseTelegramSequence_39);
    executeTest(& testCaseTelegramSequence_40);
    executeTest(& testCaseTelegramSequence_41);
    executeTest(& testCaseTelegramSequence_42);

}

