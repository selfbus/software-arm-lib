/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Selfbus library
 * @defgroup SBLIB_UNIT_TEST Unit tests
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   Catch unit test for APCI processing
 * @details
 *
 *
 * @{
 *
 * @file   test_prot_apci.cpp
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
#include <sblib/eib/apci.h>

// #include "tc_tlayer4_stepfunction.h"

#define MANUFACTURER (0x0004)
#define DEVICE       (0x2060)
#define VERSION      (0x01)

#define OWN_KNX_ADDRESS (0xA000) // own address 10.0.0

TLayer4 *bcuTL42 = (TLayer4*) &bcu;

static void tc_setup(void)
{
    bcuTL42->setTL4State(TLayer4::CLOSED); // to "reset" connection for next test
    bcuTL42->setOwnAddress(OWN_KNX_ADDRESS);
}

static Telegram apciAdcReadPduTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_ADC_READ_PDU
    {TEL_RX, 8, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x64, 0x01, 0x80}},
    // 3. Check T_ACK, loop() once so APCI_ADC_RESPONSE_PDU will be send
    {TEL_TX, 7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4.  APCI_ADC_RESPONSE_PDU
    {TEL_TX, 11, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x64, 0x41, 0xC0, 0xAB, 0x00, 0x00}},
    {END}
};

static Telegram apciMemoryReadTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_MEMORY_READ_PDU count = 2, address = 0x105 (UserEeprom.deviceTypeH and UserEeprom.deviceTypeL)
    {TEL_RX, 10, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x64, 0x42, 0x02, 0x01, 0x05}},
    // 3. Check T_ACK, loop() once so APCI_MEMORY_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. APCI_MEMORY_RESPONSE_PDU count = 2, address = 0x105, value = DEVICE = 0x2060
    {TEL_TX, 12, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x65, 0x42, 0x42, 0x01, 0x05, 0x20, 0x60}},
    {END}
};

static Telegram apciMemoryWriteTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_MEMORY_WRITE_PDU count = 2, address = 0x105 value = 0xCDEF
    {TEL_RX, 12, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x66, 0x42, 0x82, 0x01, 0x05, 0xCD, 0xEF}},
    // 3. Check T_ACK, loop() once
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. APCI_MEMORY_READ_PDU count = 2, address = 0x105 (UserEeprom.deviceTypeH and UserEeprom.deviceTypeL)
    {TEL_RX, 10, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x64, 0x46, 0x02, 0x01, 0x05}},
    // 5. Check T_ACK, loop() once so APCI_MEMORY_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC6}},
    // 6. APCI_MEMORY_RESPONSE_PDU count = 2, address = 0x105, value = 0xCDEF
    {TEL_TX, 12, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x65, 0x42, 0x42, 0x01, 0x05, 0xCD, 0xEF}},
    {END}
};

static Telegram apciDeviceDescriptorReadTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_DEVICEDESCRIPTOR_READ_PDU
    {TEL_RX,  8, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x00}},
    // 3. Check T_ACK, loop() once so APCI_DEVICEDESCRIPTOR_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. DeviceDescriptorResponse
    {TEL_TX, 10, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, MASK_VERSION_HIGH, MASK_VERSION_LOW}},
    {END}
};

static Telegram apciBasicRestartTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_BASIC_RESTART_PDU
    {TEL_RX,  8, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x80}},
    // 3. Check T_ACK
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    {END}
};

static Telegram apciMasterResetTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_MASTER_RESET_PDU eraseCode = BOOTLOADER_MAGIC_ERASE, channelNumber = BOOTLOADER_MAGIC_CHANNEL
    {TEL_RX, 10, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0x81,  BOOTLOADER_MAGIC_ERASE, BOOTLOADER_MAGIC_CHANNEL}},
    // 3. Check T_ACK, loop() once so APCI_MASTER_RESET_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. APCI_MASTER_RESET_RESPONSE_PDU => T_RESTART_NO_ERROR with 1 second
    {TEL_TX, 11, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x64, 0x43, 0xA1, 0x00, 0x00, 0x01}},
    {END}
};

static Telegram apciAuthorizeRequestTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_AUTHORIZE_REQUEST_PDU
    {TEL_RX, 8, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0xD1}},
    // 3. Check T_ACK, loop() once so APCI_AUTHORIZE_RESPONSE_PDU will be send
    {TEL_TX, 7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. APCI_AUTHORIZE_RESPONSE_PDU
    {TEL_TX, 9, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x62, 0x43, 0xD2, 0x00}},
    {END}
};

static Test_Case testCaseApciAdcReadPdu =
{
    "APCI_ADC_READ_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciAdcReadPduTelegrams
};

static Test_Case testCaseApciMemoryReadPdu =
{
    "APCI_MEMORY_READ_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciMemoryReadTelegrams
};

static Test_Case testCaseApciMemoryWritePdu =
{
    "APCI_MEMORY_WRITE_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciMemoryWriteTelegrams
};

static Test_Case testCaseApciDeviceDescriptorReadPdu =
{
    "APCI_DEVICEDESCRIPTOR_READ_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciDeviceDescriptorReadTelegrams
};

static Test_Case testCaseApciBasicRestartPdu =
{
    "APCI_BASIC_RESTART_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciBasicRestartTelegrams
};

static Test_Case testCaseApciMasterResetPdu =
{
    "APCI_MASTER_RESET_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciMasterResetTelegrams
};

static Test_Case testCaseApciAuthorizeRequestPdu =
{
    "APCI_AUTHORIZE_REQUEST_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciAuthorizeRequestTelegrams
};

TEST_CASE("APCI processing", "[protocol][APCI]")
{
    executeTest(&testCaseApciAdcReadPdu);
    executeTest(&testCaseApciMemoryReadPdu);
    executeTest(&testCaseApciMemoryWritePdu);
    executeTest(&testCaseApciDeviceDescriptorReadPdu);
    executeTest(&testCaseApciBasicRestartPdu);
    executeTest(&testCaseApciMasterResetPdu);
    executeTest(&testCaseApciAuthorizeRequestPdu);
}

