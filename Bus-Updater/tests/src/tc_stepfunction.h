/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Selfbus library
 * @defgroup SBLIB_UNIT_TEST Unit tests
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   Collection of Testcase stepfunctions for transport layer unit test
 * @details 
 *
 *
 * @{
 *
 * @file   tc_stepfunction.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef TC_STEPFUNCTION_H_
#define TC_STEPFUNCTION_H_

typedef struct
{
    unsigned char  state;
    bool           connected;
    unsigned short ownAddress;
    int connectedAddr; ///\todo implement all below
    int connectedSeqNo;
    bool incConnectedSeqNo;
    int lastAckSeqNo;

    // from bcu_layer4.h
    BcuLayer4::TL4State machineState;
    byte seqNoSend;
    byte seqNoRcv;
    bool telegramReadyToSend;
} ProtocolTestState;

static ProtocolTestState protoState[2];

#define VaS(s) ((ProtocolTestState *) (s))

static void connect(void * state, unsigned int param)
{
    VaS(state)->connectedAddr = param;
    VaS(state)->machineState = BcuLayer4::OPEN_IDLE;
    VaS(state)->seqNoSend = 0;
    VaS(state)->seqNoRcv = 0;
    VaS(state)->telegramReadyToSend = false;
}

static void connectWhileConnectedClosed(void * state, unsigned int param)
{
    VaS(state)->connectedAddr = param;
    VaS(state)->machineState = BcuLayer4::CLOSED;
}

static void connectedOpenIdle(void * state, unsigned int param)
{
    VaS(state)->connectedAddr = param;
    VaS(state)->machineState = BcuLayer4::OPEN_IDLE;
}

static void connectedOpenWait(void * state, unsigned int param)
{
    VaS(state)->connectedAddr = param;
    VaS(state)->machineState = BcuLayer4::OPEN_WAIT;
}

static void disconnectClosed(void * state, unsigned int param)
{
    VaS(state)->machineState = BcuLayer4::CLOSED;
}

static void disconnectWhileConnectedOpenIdle(void * state, unsigned int param)
{
    VaS(state)->connectedAddr = param;
    VaS(state)->machineState = BcuLayer4::OPEN_IDLE;
}

static void disconnectWhileConnectedOpenWait(void * state, unsigned int param)
{
    VaS(state)->connectedAddr = param;
    VaS(state)->machineState = BcuLayer4::OPEN_WAIT;
}


#endif /* TC_STEPFUNCTION_H_ */
/** @}*/
