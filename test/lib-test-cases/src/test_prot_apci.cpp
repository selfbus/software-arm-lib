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

static void tc_setup(Telegram* tel, uint16_t telCount)
{
    // bcuUnderTest->setTL4State(TLayer4::CLOSED); // to "reset" connection for next test
    bcuUnderTest->setOwnAddress(OWN_KNX_ADDRESS);
    telegramPreparation(bcuUnderTest, tel, telCount);
}

static Telegram apciAdcReadPduTelegrams[] =
{
    // 1. APCI_ADC_READ_PDU
    {TEL_RX, 9, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x62, 0x01, 0x80, 0x89}},
    // 2. APCI_ADC_RESPONSE_PDU
    {TEL_TX, 11, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x64, 0x01, 0xC0, 0x89, 0x00, 0x00}},
    {END}
};

static Telegram apciAdcReadPduConnectedTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_ADC_READ_PDU
    {TEL_RX, 9, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x62, 0x41, 0x80, 0x89}},
    // 3. Check T_ACK, loop() once so APCI_ADC_RESPONSE_PDU will be send
    {TEL_TX, 7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. APCI_ADC_RESPONSE_PDU
    {TEL_TX, 11, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x64, 0x41, 0xC0, 0x89, 0x00, 0x00}},
    {END}
};

static Telegram apciMemoryReadTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_MEMORY_READ_PDU count = 2, address = (BCU1: 0x105, BIM112: 0x3F05) (UserEeprom.deviceTypeH and UserEeprom.deviceTypeL)
    {TEL_RX, 10, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x64, 0x42, 0x02, 0x01, 0x05}},
    // 3. Check T_ACK, loop() once so APCI_MEMORY_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. APCI_MEMORY_RESPONSE_PDU count = 2, address = (BCU1: 0x105, BIM112: 0x3F05), value = DEVICE = 0x2060
    {TEL_TX, 12, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x65, 0x42, 0x42, 0x01, 0x05, 0x20, 0x60}},
    {END}
};

static Telegram apciMemoryWriteTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX, 7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_MEMORY_WRITE_PDU count = 2, address = (BCU1: 0x105, BIM112: 0x3F05), value = 0xCDEF
    {TEL_RX, 12, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x66, 0x42, 0x82, 0x01, 0x05, 0xCD, 0xEF}},
    // 3. Check T_ACK, loop() once
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. APCI_MEMORY_READ_PDU count = 2, address = (BCU1: 0x105, BIM112: 0x3F05) (UserEeprom.deviceTypeH and UserEeprom.deviceTypeL)
    {TEL_RX, 10, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x64, 0x46, 0x02, 0x01, 0x05}},
    // 5. Check T_ACK, loop() once so APCI_MEMORY_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC6}},
    // 6. APCI_MEMORY_RESPONSE_PDU count = 2, address = (BCU1: 0x105, BIM112: 0x3F05), value = 0xCDEF
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
    {TEL_TX, 10, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x63, 0x43, 0x40, dummyMaskVersionHigh, dummyMaskVersionLow}},
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
    // 4. APCI_MASTER_RESET_RESPONSE_PDU => T_RESTART_NO_ERROR with 1 second, loop() once so T_DISCONNECT_PDU will be sent
    {TEL_TX, 11, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x64, 0x43, 0xA1, 0x00, 0x00, 0x01}},
    // 5. T_DISCONNECT_PDU
    {TEL_TX, 7, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0x81}},
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

static Telegram apciPropertyDescriptionReadTelegrams_BCU1[] =
{
    // special BCU 1 version, which should not respond
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_PROPERTY_DESCRIPTION_READ_PDU, objIdx = 0x00 (deviceObjectProps) , propId = 0x0C (PID_MANUFACTURER_ID), index = 0x00
    {TEL_RX, 11, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0xD8, 0x00, 0x0C, 0x00}},
    // 3. Check T_ACK, loop() once so APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    // 5.
    {END}
};

static Telegram apciPropertyDescriptionReadTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_PROPERTY_DESCRIPTION_READ_PDU, objIdx = 0x00 (deviceObjectProps) , propId = 0x0C (PID_MANUFACTURER_ID), index = 0x00
    {TEL_RX, 11, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0xD8, 0x00, 0x0C, 0x00}},
    // 3. Check T_ACK, loop() once so APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU
    {TEL_TX, 15, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x68, 0x43, 0xD9, 0x00, 0x0C, 0x00, 0x12, 0x00, 0x01, 0x50}},
    // 5. T_ACK for APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC2}},
    // 6. APCI_PROPERTY_DESCRIPTION_READ_PDU, objIdx = 0x03 (appObjectProps) , propId = 0x0D (PID_PROG_VERSION), index = 0x00
    {TEL_RX, 11, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x47, 0xD8, 0x03, 0x0D, 0x00}},
    // 7. Check T_ACK, loop() once so APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC6}},
    // 8. APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU
    {TEL_TX, 15, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x68, 0x47, 0xD9, 0x03, 0x0D, 0x00, 0x15, 0x00, 0x01, 0x50}},
    // 9. T_ACK for APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC6}},
    // 10.
    {END}
};

static Telegram apciPropertyValueReadTelegrams_BCU1[] =
{   // special BCU 1 version, which should not respond
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_PROPERTY_VALUE_READ_PDU, objIdx = 0x00 (deviceObjectProps) , propId = 0x0C (PID_MANUFACTURER_ID), count = 1, start = 1
    {TEL_RX, 12, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0xD5, 0x00, 0x0C, 0x10, 0x01}},
    // 3. Check T_ACK, loop() once so APCI_PROPERTY_VALUE_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    // 5.
    {END}
};

static Telegram apciPropertyValueReadTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_PROPERTY_VALUE_READ_PDU, objIdx = 0x00 (deviceObjectProps) , propId = 0x0C (PID_MANUFACTURER_ID), count = 1, start = 1
    {TEL_RX, 12, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0xD5, 0x00, 0x0C, 0x10, 0x01}},
    // 3. Check T_ACK, loop() once so APCI_PROPERTY_VALUE_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. APCI_PROPERTY_VALUE_RESPONSE_PDU (Manufacturer)
    {TEL_TX, 14, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x67, 0x43, 0xD6, 0x00, 0x0C, 0x10, 0x01, (MANUFACTURER >> 8), (MANUFACTURER & 0xFF)}},
    // 5. T_ACK for APCI_PROPERTY_VALUE_RESPONSE_PDU
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC2}},

    // 6. APCI_PROPERTY_VALUE_READ_PDU, objIdx = 0x03 (appObjectProps) , propId = 0x0D (PID_PROG_VERSION), count = 1, start = 1
    {TEL_RX, 12, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x47, 0xD5, 0x03, 0x0D, 0x10, 0x01}},
    // 7. Check T_ACK, loop() once so APCI_PROPERTY_VALUE_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC6}},
    // 8. APCI_PROPERTY_VALUE_RESPONSE_PDU (Manufacturer, Device, Version)
    {TEL_TX, 17, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x6A, 0x47, 0xD6, 0x03, 0x0D, 0x10, 0x01, (MANUFACTURER >> 8), (MANUFACTURER & 0xFF), (DEVICE >> 8), (DEVICE & 0xFF), VERSION}},
    // 9. T_ACK for APCI_PROPERTY_VALUE_RESPONSE_PDU
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC6}},
    // 10.
    {END}
};

static Telegram apciPropertyValueWriteTelegrams_BCU1[] =
{   // special BCU 1 version, which should not respond
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_PROPERTY_VALUE_WRITE_PDU, objIdx = 0x00 (deviceObjectProps) , propId = 0x4E (PID_HARDWARE_TYPE), count = 1, start = 1, length 6
    {TEL_RX, 18, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0xD7, 0x00, 0x4E, 0x10, 0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06}},
    // 3. Check T_ACK, loop() once so APCI_PROPERTY_VALUE_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. Check for empty TX-Response
    {CHECK_TX_BUFFER, 0, 0, 0, NULL, {}},
    // 5.
    {END}
};

static Telegram apciPropertyValueWriteTelegrams[] =
{
    // 1. T_CONNECT_PDU (0x80) from sourceAddr=10.0.1 to destAddr=10.0.0
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0x80}},
    // 2. APCI_PROPERTY_VALUE_WRITE_PDU, objIdx = 0x00 (deviceObjectProps) , propId = 0x4E (PID_HARDWARE_TYPE), count = 1, start = 1, length 6
    {TEL_RX, 18, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x43, 0xD7, 0x00, 0x4E, 0x10, 0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06}},
    // 3. Check T_ACK, loop() once so APCI_PROPERTY_VALUE_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC2}},
    // 4. APCI_PROPERTY_VALUE_RESPONSE_PDU objIdx = 0x00 (deviceObjectProps) , propId = 0x4E (PID_HARDWARE_TYPE)
    {TEL_TX, 18, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x6B, 0x43, 0xD6, 0x00, 0x4E, 0x10, 0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06}},
    // 5. T_ACK for APCI_PROPERTY_VALUE_RESPONSE_PDU
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC2}},
    // 6. APCI_PROPERTY_VALUE_WRITE_PDU, objIdx = 0x03 (appObjectProps) , propId = 0x1B (PID_MCB_TABLE), count = 1, start = 1, length 8
    {TEL_RX, 20, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x61, 0x47, 0xD7, 0x03, 0x1B, 0x10, 0x01, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80}},
    // 7. Check T_ACK, loop() once so APCI_PROPERTY_VALUE_RESPONSE_PDU will be send
    {TEL_TX,  7, 1, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x60, 0xC6}},
    // 8. APCI_PROPERTY_VALUE_RESPONSE_PDU objIdx = 0x03 (appObjectProps) , propId = 0x1B (PID_MCB_TABLE)
    {TEL_TX, 20, 0, 0, NULL, {0xB0, 0xA0, 0x00, 0xA0, 0x01, 0x6D, 0x47, 0xD6, 0x03, 0x1B, 0x10, 0x01, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80}},
    // 9. T_ACK for APCI_PROPERTY_VALUE_RESPONSE_PDU
    {TEL_RX,  7, 0, 0, NULL, {0xB0, 0xA0, 0x01, 0xA0, 0x00, 0x60, 0xC6}},
    // 10.
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

static Test_Case testCaseApciAdcReadPduConnected =
{
    "APCI_ADC_READ_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciAdcReadPduConnectedTelegrams
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

static Test_Case testCaseApciPropertyDescriptionReadPdu =
{
    "APCI_PROPERTY_DESCRIPTION_READ_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciPropertyDescriptionReadTelegrams
};

static Test_Case testCaseApciPropertyValueReadPdu =
{
    "APCI_PROPERTY_VALUE_READ_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciPropertyValueReadTelegrams
};

static Test_Case testCaseApciPropertyValueWritePdu =
{
    "APCI_PROPERTY_VALUE_WRITE_PDU",
    MANUFACTURER, DEVICE, VERSION,
    0,    //powerOnDelay
    NULL, // eePromSetup
    tc_setup,
    NULL,
    NULL,
    NULL,
    apciPropertyValueWriteTelegrams
};

TEST_CASE("APCI processing", "[protocol][APCI]")
{
    BcuType bcuTypeToTest;
    // BCU 1 doesn't have properties so use special telegram sets
    testCaseApciPropertyDescriptionReadPdu.telegram = apciPropertyDescriptionReadTelegrams_BCU1;
    testCaseApciPropertyValueReadPdu.telegram = apciPropertyValueReadTelegrams_BCU1;
    testCaseApciPropertyValueWritePdu.telegram = apciPropertyValueWriteTelegrams_BCU1;
    SECTION("BCU 1") {
        bcuTypeToTest = BCU_1;
        executeTest(bcuTypeToTest, &testCaseApciAdcReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciAdcReadPduConnected);
        executeTest(bcuTypeToTest, &testCaseApciMemoryReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciMemoryWritePdu);
        executeTest(bcuTypeToTest, &testCaseApciDeviceDescriptorReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciBasicRestartPdu);
        executeTest(bcuTypeToTest, &testCaseApciMasterResetPdu);
        executeTest(bcuTypeToTest, &testCaseApciAuthorizeRequestPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyDescriptionReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyValueReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyValueWritePdu);
    }

    // all other BCUs have properties so use default telegram sets
    testCaseApciPropertyDescriptionReadPdu.telegram = apciPropertyDescriptionReadTelegrams;
    testCaseApciPropertyValueReadPdu.telegram = apciPropertyValueReadTelegrams;
    testCaseApciPropertyValueWritePdu.telegram = apciPropertyValueWriteTelegrams;
    SECTION("BCU 2") {
        bcuTypeToTest = BCU_2;
        executeTest(bcuTypeToTest, &testCaseApciAdcReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciAdcReadPduConnected);
        executeTest(bcuTypeToTest, &testCaseApciMemoryReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciMemoryWritePdu);
        executeTest(bcuTypeToTest, &testCaseApciDeviceDescriptorReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciBasicRestartPdu);
        executeTest(bcuTypeToTest, &testCaseApciMasterResetPdu);
        executeTest(bcuTypeToTest, &testCaseApciAuthorizeRequestPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyDescriptionReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyValueReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyValueWritePdu);
    }

    SECTION("BCU 0x0701 (BIM112)") {
        bcuTypeToTest = BCU_0701;
        executeTest(bcuTypeToTest, &testCaseApciAdcReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciAdcReadPduConnected);
        executeTest(bcuTypeToTest, &testCaseApciMemoryReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciMemoryWritePdu);
        executeTest(bcuTypeToTest, &testCaseApciDeviceDescriptorReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciBasicRestartPdu);
        executeTest(bcuTypeToTest, &testCaseApciMasterResetPdu);
        executeTest(bcuTypeToTest, &testCaseApciAuthorizeRequestPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyDescriptionReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyValueReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyValueWritePdu);
    }

    SECTION("BCU 0x0705 (BIM112)") {
        bcuTypeToTest = BCU_0705;
        executeTest(bcuTypeToTest, &testCaseApciAdcReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciAdcReadPduConnected);
        executeTest(bcuTypeToTest, &testCaseApciMemoryReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciMemoryWritePdu);
        executeTest(bcuTypeToTest, &testCaseApciDeviceDescriptorReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciBasicRestartPdu);
        executeTest(bcuTypeToTest, &testCaseApciMasterResetPdu);
        executeTest(bcuTypeToTest, &testCaseApciAuthorizeRequestPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyDescriptionReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyValueReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyValueWritePdu);
    }

    SECTION("BCU 0x07B0") {
        bcuTypeToTest = BCU_07B0;
        executeTest(bcuTypeToTest, &testCaseApciAdcReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciAdcReadPduConnected);
        executeTest(bcuTypeToTest, &testCaseApciMemoryReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciMemoryWritePdu);
        executeTest(bcuTypeToTest, &testCaseApciDeviceDescriptorReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciBasicRestartPdu);
        executeTest(bcuTypeToTest, &testCaseApciMasterResetPdu);
        executeTest(bcuTypeToTest, &testCaseApciAuthorizeRequestPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyDescriptionReadPdu);
        executeTest(bcuTypeToTest, &testCaseApciPropertyValueReadPdu);
        ///\todo fails for SYSTEMB
        // executeTest(bcuTypeToTest, &testCaseApciPropertyValueWritePdu);
    }
}

