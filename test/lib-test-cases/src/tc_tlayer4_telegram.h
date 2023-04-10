/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Selfbus library
 * @defgroup SBLIB_UNIT_TEST Unit tests
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   Collection of Testcase KNX telegram for transport layer unit test
 * @details 
 *
 *
 * @{
 *
 * @file   tc_telegram.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef TC_TELEGRAM_H_
#define TC_TELEGRAM_H_

#include "tc_tlayer4_stepfunction.h"

static Telegram testCaseTelegrams_01[] =
{
    // KNX Spec. 8/3/4 2.2.1.1 p.12 (all styles)
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 1, 0xA001, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_02[] =
{
    // KNX Spec. 8/3/4 2.2.1.2 p.13 (style 3, remain in OPEN_IDLE)
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0xA001, connect,               {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. a second T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0, while already connected
    // Style 2/3: BDUT remains in OPEN_IDLE, BDUT sends no T_Disconnect.ind and no Disconnect on the bus.
    {TEL_RX,  7, 0, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_03[] =
{
    // KNX Spec. 8/3/4 2.2.1.3 p.14 (style 3, remain in OPEN_WAIT)
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.1 to destAddr=10.0.0
    // steps 0.x done in tc_setup_OpenWait_A001

    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    // Style 2/3: BDUT remains in OPEN_WAIT, BDUT sends no T_Disconnect.ind and no Disconnect on the bus.
    {TEL_RX,  7, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. Check for empty TX-Response
    {CHECK_TX_BUFFER,  0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_04[] =
{
    // KNX Spec. 8/3/4 2.2.2.1 p.15
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.7 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0xA007, connect,                {0xB0, 0xA0, 0x07, 0xA0, 0x00, 0x60, 0x80}},
    // 2. a second T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0, while already connected
    {TEL_RX,  7, 0, 0xA007, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 3. Check TX-Response for a T_DISCONNECT_PDU (0x81) to 10.0.1. The BDUT persists in State 'OPEN_IDLE'.
    {TEL_TX,  7, 0, 0xA007, connectedOpenIdle, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0x81}},
    {END}
};

static Telegram testCaseTelegrams_05[] =
{
    // KNX Spec. 8/3/4 2.2.2.2 p.16
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.7 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.7 to destAddr=10.0.0
    // steps 0.x done in tc_setup_OpenWait_A007

    // 1. a "second" T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0, while already connected in state OPEN_WAIT
    {TEL_RX, 7, 0, 0xA007, connectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. Check TX-Response for a T_DISCONNECT_PDU (0x81) to 10.0.1. The BDUT persists in State 'OPEN_WAIT'.
    {TEL_TX, 7, 0, 0xA007, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0x81}},
    {END}
};

static Telegram testCaseTelegrams_06[] =
{
    // KNX Spec. 8/3/4 2.2.3.1 p.16/17
    // 1. T_DISCONNECT_PDU (0x81) from sourceAddr=10.0.7 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x81}},
    // 2. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_07[] =
{
    // KNX Spec. 8/3/4 2.2.3.2 p.17
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA001, connect,     {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. T_DISCONNECT_PDU (0x81) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x81}},
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_08[] =
{
    // KNX Spec. 8/3/4 2.2.3.3 p.18
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.1 to destAddr=10.0.0
    // steps 0.x done in tc_setup_OpenWait_A001

    // 1. T_DISCONNECT_PDU (0x81) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x81}},
    // 2. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_09[] =
{
    // KNX Spec. 8/3/4 2.2.4.1 p.18/19
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA001, connect,     {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. foreign T_DISCONNECT_PDU (0x81) from sourceAddr=10.0.7 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA001, disconnectWhileConnectedOpenIdle, {0xB0, 0xA0, 0x07, 0xA0, 0x00, 0x60, 0x81}},
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_10[] =
{
    // KNX Spec. 8/3/4 2.2.4.2 p.19
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.7 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.7 to destAddr=10.0.0
    // steps 0.x done in tc_setup_OpenWait_A007

    // 1. foreign T_DISCONNECT_PDU (0x81) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA007, disconnectWhileConnectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x81}},
    // 2. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_18[] =
{
    // KNX Spec. 8/3/4 2.2.10.1 p.26
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA001, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. simulate 6000 milliseconds passing, loop bcu once
    {TIMER_TICK, 6000, 1, 0, NULL, {}},
    // 3. Check TX-Response for a T_DISCONNECT_PDU (0x81) to 10.0.1
    {TEL_TX, 7, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0x81}},
    // 4. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_19[] =
{
    // KNX Spec. 8/3/4 2.2.11.1 p.27
    // 0. Setup connection, get request, send T_ACK, send response
    {TEL_RX,  7, 0, 0, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    {TEL_RX,  8, 0, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    {TEL_TX,  7, 1, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},

    // 1. simulate 3000 milliseconds passing, loop bcu once
    {TIMER_TICK, 3000, 1, 0, NULL, {}},
    // 2. Check TX-Response for repetition #1 of response
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},
    // 3. simulate 3000 milliseconds passing, loop bcu once
    {TIMER_TICK, 3000, 1, 0, NULL, {}},
    // 4. Check TX-Response for repetition #2 of response
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},
    // 5. simulate 3000 milliseconds passing, loop bcu once
    {TIMER_TICK, 3000, 1, 0, NULL, {}},
    // 6. Check TX-Response for repetition #3 of response
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},
    // 7. simulate 3000 milliseconds passing, loop bcu once
    {TIMER_TICK, 3000, 1, 0, NULL, {}},
    // 8. Check TX-Response for a T_DISCONNECT_PDU (0x81) to 10.0.1
    {TEL_TX, 7, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0x81}},
    // 9. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_20a[] =
{
    // KNX Spec. 8/3/4 2.3.1 p.28
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA001, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. T_DATA_CONNECTED_REQ from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 8, 0, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}}, // MaskVersionRead()
    // 3. Check T_ACK response for the T_DATA_CONNECTED_REQ
    {TEL_TX, 7, 1, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}}, // T_ACK_PDU
    {END}
};

static Telegram testCaseTelegrams_20b[] =
{
    // KNX Spec. 8/3/4 2.3.2 p.29/30
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.15.254 to destAddr=1.0.1
    {TEL_RX, 7, 0, 0x1001, connect, {0xB0, 0xAF, 0xFE, 0x10, 0x01, 0x60, 0x80}},
    // 2. DeviceDescriptorRead(DescType=00) from sourceAddr=10.15.254 to destAddr=1.0.1 (MaskVersionRead)
    {TEL_RX, 8, 0, 0xAFFE, connectedOpenIdle, {0xB0, 0xAF, 0xFE, 0x10, 0x01, 0x61, 0x43, 0x00}},
    // 3. Check T_ACK response for the DeviceDescriptorRead, loop bcu once
    {TEL_TX, 7, 1, 0xAFFE, connectedOpenIdle, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x60, 0xC2}}, // T_ACK_PDU
    // 4. Check DeviceDescriptorResponse
    {TEL_TX, 10, 0, 0xAFFE, connectedOpenWait, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}}, // DeviceDescriptorResponse(DescType=00, Descriptor=??)
    // 5. simulate 3500 milliseconds passing
    {TIMER_TICK, 3500, 0, 0, connectedOpenWait, {}},
    // 6. MemoryRead(Count=1, Addr=0x01FE) from sourceAddr=10.15.254 to destAddr=1.0.1
    {TEL_RX, 8, 0, 0xAFFE, connectedOpenWait, {0xB0, 0xAF, 0xFE, 0x10, 0x01, 0x63, 0x46, 0x01, 0x01, 0xFE}},
    // 7. Check T_ACK response for the MemoryRead, loop bcu once
    {TEL_TX, 7, 1, 0xAFFE, connectedOpenWait, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x60, 0xC6}}, // T_ACK_PDU
    // 8. Check second try of DeviceDescriptorResponse
    {TEL_TX, 10, 0, 0xAFFE, connectedOpenWait, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}}, // DeviceDescriptorResponse(DescType=00, Descriptor=??)
    // 9. simulate 6500 milliseconds passing
    {TIMER_TICK, 6500, 0, 0, connectedOpenWait, {}},
    // 10. Receive T-Ack(Seq=0)
    {TEL_RX, 7, 0, 0xAFFE, connectedOpenIdle, {0xB0, 0xAF, 0xFE, 0x10, 0x01, 0x60, 0xC2}},
    // 11. Check MemoryReadResponse
    {TEL_TX, 10, 0, 0xAFFE, connectedOpenWait, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x64, 0x46, 0x41, 0x01, 0xFE, 0x00}}, // MemoryResponse(Count=01, Addr=01FE, Data=?? )
    // 12. simulate 3500 milliseconds passing
    {TIMER_TICK, 3500, 0, 0, connectedOpenWait, {}},
    // 13. Check MemoryReadResponse repetition 1
    {TEL_TX, 10, 0, 0xAFFE, connectedOpenWait, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x64, 0x46, 0x41, 0x01, 0xFE, 0x00}}, // MemoryResponse(Count=01, Addr=01FE, Data=?? )
    // 14. simulate 3500 milliseconds passing
    {TIMER_TICK, 3500, 0, 0, connectedOpenWait, {}},
    // 15. Check MemoryReadResponse repetition 2
    {TEL_TX, 10, 0, 0xAFFE, connectedOpenWait, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x64, 0x46, 0x41, 0x01, 0xFE, 0x00}}, // MemoryResponse(Count=01, Addr=01FE, Data=?? )
    // 16. simulate 3500 milliseconds passing
    {TIMER_TICK, 3500, 0, 0, connectedOpenWait, {}},
    // 17. Check MemoryReadResponse repetition 3
    {TEL_TX, 10, 0, 0xAFFE, connectedOpenWait, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x64, 0x46, 0x41, 0x01, 0xFE, 0x00}}, // MemoryResponse(Count=01, Addr=01FE, Data=?? )
    // 18. simulate 3500 milliseconds passing
    {TIMER_TICK, 3500, 0, 0, connectedOpenWait, {}},
    // 19. Check TX-Response for a T_DISCONNECT_PDU (0x81) to 10.0.1
    {TEL_TX, 7, 0, 0, disconnectClosed, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x60, 0x81}},
    {END}
};

static Telegram testCaseTelegrams_21[] =
{
    // specified in KNX Spec. 8/3/4 2.3.3.1 p.30
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.15.254 to destAddr=1.0.1
    {TEL_RX, 7, 0, 0x1001, connect, {0xB0, 0xAF, 0xFE, 0x10, 0x01, 0x60, 0x80}},
    // 2. DeviceDescriptorRead(DescType=00) from sourceAddr=10.15.254 to destAddr=1.0.1 with outdated SeqNo
    {TEL_RX, 8, 0, 0x1001, connectedOpenIdle, {0xB0, 0xAF, 0xFE, 0x10, 0x01, 0x61, 0x7F, 0x00}},
    // 3. Check T_ACK response for the DeviceDescriptorRead
    {TEL_TX, 7, 0, 0x1001, connectedOpenIdle, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x60, 0xFE}}, // T_ACK_PDU Seq=F
    // 4. simulate 6000 milliseconds passing, loop bcu once
    {TIMER_TICK,  6000, 1, 0, NULL, {}},
    // 5. Check TX-Response for a T_DISCONNECT_PDU (0x81) to 10.0.1
    {TEL_TX, 7, 0, 0, disconnectClosed, {0xB0, 0x10, 0x01, 0xAF, 0xFE, 0x60, 0x81}},
    // 6. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_22[] =
{
    // specified in KNX Spec. 8/3/4 2.3.3.2 p.31
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.1 to destAddr=10.0.0
    // steps 0.x done in tc_setup_OpenWait_A001

    // 1. DeviceDescriptorRead(DescType=00) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  8, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    // 2. Check T_ACK response for the DeviceDescriptorRead
    {TEL_TX,  7, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}}, // T_ACK_PDU Seq=0
    // 3. repeated DeviceDescriptorRead(DescType=00) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  8, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    // 4. Check T_ACK response for the second DeviceDescriptorRead
    {TEL_TX,  7, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}}, // T_ACK_PDU Seq=0
    {END}
};

static Telegram testCaseTelegrams_23[] =
{
    // KNX Spec. 8/3/4 2.3.4.1 p.32
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA001, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. DeviceDescriptorRead(DescType=00) from sourceAddr=10.0.1 to destAddr=1.0.1 with wrong Seq=5
    {TEL_RX, 8, 0, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x57, 0x00}},
    // 3. Check T_NACK_PDU (Seq=5)
    {TEL_TX, 7, 0, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xD7}},
    // 4. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_24[] =
{
    // KNX Spec. 8/3/4 2.3.4.2 p.33
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.1 to destAddr=10.0.0
    // steps 0.x done in tc_setup_OpenWait_A001

    // 1. DeviceDescriptorRead(DescType=00) from sourceAddr=10.0.1 to destAddr=10.0.0 with wrong Seq=5
    {TEL_RX, 8, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x57, 0x00}},
    // 2. Check T_NACK_PDU (Seq=5)
    {TEL_TX, 7, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xD7}},
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_25[] =
{
    // KNX Spec. 8/3/4 2.3.5.1 p.34
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.7 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA007, connect, {0xB0, 0xA0, 0x07, 0xA0, 0x00, 0x60, 0x80}},
    // 2. DeviceDescriptorRead(DescType=00) from wrong sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 8, 0, 0xA007, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    // Style 2/3 - BDUT sends no Disconnect on the bus.
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_26[] =
{
    // KNX Spec. 8/3/4 2.3.5.2 p.34/35
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.7 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.7 to destAddr=10.0.0
    // steps 0.x done in tc_setup_OpenWait_A007

    // 1. DeviceDescriptorRead(DescType=00) from wrong sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 8, 0, 0xA007, connectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    // Style 2/3 - BDUT sends no Disconnect on the bus.
    // 2. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_30[] =
{
    // KNX Spec. 8/3/4 2.4.2.1 p.38/39
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA001, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. T_ACK_PDU (Seq=0)
    {TEL_RX, 8, 0, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC2}},
    // Style 3 - BDUT remains in OPEN_IDLE, BDUT sends no T_Disconnect.ind and no Disconnect on the bus.
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_31[] =
{
    // KNX Spec. 8/3/4 2.4.3.1 p.39/40
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA001, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. T_ACK with wrong sequence number (5)
    {TEL_RX, 8, 0, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xD6}}, // T-Ack(Seq=5)
    // Style 3 - BDUT remains in OPEN_IDLE, BDUT sends no T_Disconnect.ind and no Disconnect on the bus.
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_32[] =
{
    // KNX Spec. 8/3/4 2.4.3.2 p.40
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.0 to destAddr=10.0.1
    // steps 0.x done in tc_setup_OpenWait_A001

    // 1. T_ACK with wrong sequence number (5)
    {TEL_RX, 8, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xD6}}, // T-Ack(Seq=5)
    // 2. Check T_DISCONNECT_PDU
    {TEL_TX, 7, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0x81}},
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_33[] =
{
    // KNX Spec. 8/3/4 2.4.4.1 p.41
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.7 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA007, connect, {0xB0, 0xA0, 0x07, 0xA0, 0x00, 0x60, 0x80}},
    // 2. T_ACK with wrong source addressAddr=10.0.1
    {TEL_RX, 8, 0, 0xA007, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC2}}, // T-Ack(Seq=0)
    // Style 2/3 - BDUT sends no Disconnect on the bus.
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_34[] =
{
    // KNX Spec. 8/3/4 2.4.4.2 p.42
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.7 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.0 to destAddr=10.0.7
    // steps 0.x done in tc_setup_OpenWait_A007

    // 1. T_ACK with wrong source addressAddr=10.0.1
    {TEL_RX, 8, 0, 0xA007, connectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC2}}, // T-Ack(Seq=0)
    // Style 2/3 - BDUT sends no Disconnect on the bus.
    // 2. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_35[] =
{
    // KNX Spec. 8/3/4 2.4.5.1 p.43
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA001, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. T_NACK with wrong sequence number 5
    {TEL_RX, 8, 0, 0, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xD7}}, // T-Nack(Seq=5)
    // Style 3 - BDUT remains in OPEN_IDLE, BDUT sends no T_Disconnect.ind and no Disconnect on	the bus.
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_36[] =
{
    // KNX Spec. 8/3/4 2.4.5.2 p.44
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.0 to destAddr=10.0.1
    // steps 0.x done in tc_setup_OpenWait_A001

    // 1. T_NACK with wrong sequence number 5
    {TEL_RX, 8, 0, 0, connectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xD7}}, // T-Nack(Seq=5)
    // Style 2 - BDUT remains in OPEN_WAIT, BDUT sends no T_Disconnect.ind and no Disconnect on the bus.
    // According to 8/3/4 2.4.5.2, the BDUT switches to state CLOSED and sends a Disconnect in Style 3.
    // According to 3/3/4 5.4.3, event E11 it stays in OPEN_WAIT and does nothing.
    // 2. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_37[] =
{
    // KNX Spec. 8/3/4 2.4.6 p.45
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA001, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. T_NACK with correct sequence number 0
    {TEL_RX, 8, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC3}}, // T-Nack(Seq=0)
    // 3. Check T_DISCONNECT_PDU
    {TEL_TX, 7, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0x81}},
    // 4. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_38[] =
{
    // KNX Spec. 8/3/4 2.4.7 p.46/47
    // 0. Setup connection, get request, send T_ACK, send response
    {TEL_RX,  7, 0, 0, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    {TEL_RX,  8, 0, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    {TEL_TX,  7, 1, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},

    // 1. T_NACK with correct sequence number 0
    {TEL_RX, 8, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC3}}, // T-Nack(Seq=0)
    // 2. The BDUT sends a telegram with NSDU=T_DATA_CONNECTED.req (stored message).
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},
    {END}
};

static Telegram testCaseTelegrams_39[] =
{
    // KNX Spec. 8/3/4 2.4.8 p.47/48
    // 0. Setup connection, get request, send T_ACK, send response
    {TEL_RX,  7, 0, 0, connect, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    {TEL_RX,  8, 0, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    {TEL_TX,  7, 1, 0xA001, connectedOpenIdle, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},

    // 1. simulate 3000 milliseconds passing
    {TIMER_TICK, 3000, 1, 0, connectedOpenWait, {}},
    // 2. Repetition #1
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},
    // 3. simulate 3000 milliseconds passing
    {TIMER_TICK, 3000, 1, 0, connectedOpenWait, {}},
    // 4. Repetition #2
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},
    // 5. simulate 3000 milliseconds passing
    {TIMER_TICK, 3000, 1, 0, connectedOpenWait, {}},
    // 6. Repetition #3
    {TEL_TX,  10, 0, 0xA001, connectedOpenWait, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},
    // 7. T_NACK with correct sequence number 0
    {TEL_RX, 8, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC3}}, // T-Nack(Seq=0)
    // 9. Check T_DISCONNECT_PDU
    {TEL_TX, 7, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0x81}},
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_40[] =
{
    // KNX Spec. 8/3/4 2.4.9.1 p.48/49
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.7 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0xA007, connect, {0xB0, 0xA0, 0x07, 0xA0, 0x00, 0x60, 0x80}},
    // 2. T_NACK from wrong sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 8, 0, 0xA007, connectedOpenIdle, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC3}},
    // Style 2/3 - BDUT sends no Disconnect on the bus.
    // 3. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_41[] =
{
    // KNX Spec. 8/3/4 2.4.9.2 p.49
    // 0. T_CONNECT_PDU (0x80) from sourceAddr=10.0.7 to destAddr=10.0.0
    // 0.1 simulate a T_DATA_CONNECTED from sourceAddr=10.0.0 to destAddr=10.0.7
    // steps 0.x done in tc_setup_OpenWait_A007

    // 1. T_NACK from wrong sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 8, 0, 0xA007, connectedOpenWait, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC3}}, // T-Nack(Seq=0)
    // Style 2/3 - BDUT sends no Disconnect on the bus.
    // 2. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

static Telegram testCaseTelegrams_42[] =
{
    // KNX Spec. 8/3/4 2.5.1 p.49/50
    // 1. T_DATA_CONNECTED from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}}, // T_DATA_CONNECTED
    // 2. Style 2/3 - BDUT sends no Disconnect on the bus.
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    // 3. T_ACK from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 8, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC2}}, // T-Ack(Seq=0)
    // 4. Style 2/3 - BDUT sends no Disconnect on the bus.
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    // 5. T_ACK from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 8, 0, 0, disconnectClosed, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC3}}, // T-Nack(Seq=0)
    // 6. Style 2/3 - BDUT sends no Disconnect on the bus.
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    {END}
};

#endif /* TC_TELEGRAM_H_ */
/** @}*/
