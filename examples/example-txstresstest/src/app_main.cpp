/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_BIM112_TX_STRESSTEST BCU BIM112 Maskversion 0x0701 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief   A simple application which stress tests the KNX-Tx driver.
 * @details The mask version of this example is 0x0701.
 *
 *          links against BIM112_71 version of the sblib library
 * @{
 *
 * @file   app_main.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/knx_tpdu.h>
#include <sblib/eib/apci.h>
#include <sblib/eib.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/io_pin_names.h>
#include <sblib/timeout.h>
#include <string.h>

#define KNX_PHYS_ADDRESS    (0xAFFE)    //!< own physical KNX address (10.15.254)
#define MANUFACTURER        (0x83)      //!< Manufacturer ID (MDT)
#define DEVICETYPE          (0x0030)    //!< Device Type (Binary input 16f)
#define APPVERSION          (0x20)      //!< Application Version

#define TEST_FILLBYTE       (0x00)  //!< Value the payload bytes should be filled with
#define TEST_DELAY_MS       (1)    //!< Delay in milliseconds between two test telegrams
#define TEST_TELEGRAM_SIZE  (24)    //!< Length of the test telegram, including trailing 0x00 byte and checksum byte (sblib supports up to 24 Bytes)
#define TEST_STARTUP_DELAY_MS (1000)//!< Delay in milliseconds before the test starts


const unsigned char hardwareVersion[6] = {0, 0, 0, 0, 0x00, 0x1E}; //!< The hardware identification number
Timeout sendTimeout;

APP_VERSION("SBSTRES", "0", "01");

byte testTelegram[TEST_TELEGRAM_SIZE];
unsigned int testTelegramSize = sizeof(testTelegram)/sizeof(testTelegram[0]) - 2; // last 2 bytes reserved for KNX checksum and trailing 0x00 byte

void initTestTelegram()
{
    int count = (testTelegramSize - 7);
    int memoryAddress = 0;
    memset(testTelegram, TEST_FILLBYTE, testTelegramSize);
    initLpdu(testTelegram, PRIORITY_SYSTEM, false); // init control byte
    setDestinationAddress(testTelegram, KNX_PHYS_ADDRESS - 1); // set destination address
    // send a APCI_MEMORY_RESPONSE_PDU response
    testTelegram[5] = 0x60 + count;
    testTelegram[6] = (APCI_MEMORY_RESPONSE_PDU >> 8);
    setSequenceNumber(testTelegram, 0);
    testTelegram[7] = (APCI_MEMORY_RESPONSE_PDU & 0xff) | (count - 2);
    testTelegram[8] = memoryAddress >> 8;
    testTelegram[9] = memoryAddress;
}

void sendTestTelegram()
{
    // prepare test telegram
    initTestTelegram();

    bus.sendTelegram(testTelegram, testTelegramSize);
    sendTimeout.start(TEST_DELAY_MS);
}

/**
 * @brief This function is called by the Selfbus's library main
 *        when the processor is started or reset.
 *
 * @note  You must implement this function in your code.
 */
void setup(void)
{
    bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION);
    bcu.setOwnAddress(KNX_PHYS_ADDRESS);
    bus.maxSendTries(1);
    memcpy(userEeprom.order, &hardwareVersion, sizeof(hardwareVersion));

    sendTimeout.start(TEST_STARTUP_DELAY_MS);
}

/**
 * @brief The main processing loop while a KNX-application is loaded.
 *
 */
void loop(void)
{
    if (bus.idle() && sendTimeout.expired())
    {
        sendTestTelegram();
    }

    // Sleep up to 1 millisecond if there is nothing to do
    if (bus.idle())
        waitForInterrupt();
}

/**
 * @brief The processing loop while no KNX-application is loaded.
 */
void loop_noapp(void)
{
    loop();
}

/** @}*/
