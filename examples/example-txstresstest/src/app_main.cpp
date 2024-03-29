/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_BIM112_TX_STRESSTEST BCU BIM112 Maskversion 0x0701 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief   A simple application which stress tests the KNX-Tx driver.
 * @details The mask version of this example is 0x0701.
 *
 * @{
 *
 * @file   app_main.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2023
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#include <sblib/eibMASK0701.h>
#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/knx_tpdu.h>
#include <sblib/eib/apci.h>
#include <sblib/io_pin_names.h>

#include <sblib/timeout.h>
#include <string.h>

// #define KNX_PHYS_ADDRESS    (0xAFFE)    //!< own physical KNX address (10.15.254)
#define KNX_PHYS_ADDRESS    (0x0002)    //!< own physical KNX address (0.0.2)
#define MANUFACTURER        (0x83)      //!< Manufacturer ID (MDT)
#define DEVICETYPE          (0x0030)    //!< Device Type (Binary input 16f)
#define APPVERSION          (0x20)      //!< Application Version

#define TEST_FILLBYTE       (0x00)      //!< Value the payload bytes should be filled with
#define TEST_DELAY_MS       (1)         //!< Delay in milliseconds between two test telegrams
#define TEST_TELEGRAM_SIZE  (24)        //!< Length of the test telegram, including trailing 0x00 byte and checksum byte (sblib supports up to 24 Bytes)
#define TEST_MAX_SEND_RETRIES (100)     //!< maximum retries in case of no ACK
#define TEST_STARTUP_DELAY_MS (1000)    //!< Delay in milliseconds before the test starts


const unsigned char hardwareVersion[6] = {0, 0, 0, 0, 0x00, 0x1E}; //!< The hardware identification number

MASK0701 bcu;

Timeout sendTimeout;

APP_VERSION("SBSTRES", "0", "01");

byte testTelegram[TEST_TELEGRAM_SIZE];
unsigned int testTelegramSize = sizeof(testTelegram)/sizeof(testTelegram[0]) - 2; // last 2 bytes reserved for KNX checksum and trailing 0x00 byte

void initTestTelegram()
{
    int count = (testTelegramSize - 7);
    int memoryAddress = 0;
    memset(testTelegram, TEST_FILLBYTE, testTelegramSize);
    // send a APCI_MEMORY_RESPONSE_PDU response
    initLpdu(testTelegram, PRIORITY_SYSTEM, false, FRAME_STANDARD); // init control byte
    setDestinationAddress(testTelegram, KNX_PHYS_ADDRESS - 1); // set destination address
    testTelegram[5] = 0x60 + count; // hopcount + length of payload
    setApciCommand(testTelegram, APCI_MEMORY_RESPONSE_PDU, (count - 2));
    setSequenceNumber(testTelegram, 0);
    testTelegram[8] = memoryAddress >> 8;
    testTelegram[9] = memoryAddress;
}

void sendTestTelegram()
{
    initLpdu(testTelegram, PRIORITY_SYSTEM, false, FRAME_STANDARD); // init control byte
    bcu.bus->sendTelegram(testTelegram, testTelegramSize);
    sendTimeout.start(TEST_DELAY_MS);
}

/**
 * @brief This function is called by the Selfbus's library main
 *        when the processor is started or reset.
 *
 * @note  You must implement this function in your code.
 */
BcuBase* setup()
{
    bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION);
    bcu.setOwnAddress(KNX_PHYS_ADDRESS);
    bcu.bus->maxSendRetries(TEST_MAX_SEND_RETRIES);
    bcu.setHardwareType(hardwareVersion, sizeof(hardwareVersion));
    // prepare test telegram
    initTestTelegram();
    sendTimeout.start(TEST_STARTUP_DELAY_MS);
    return (&bcu);
}

/**
 * @brief The main processing loop while a KNX-application is loaded.
 *
 */
void loop(void)
{
    if (!bcu.bus->sendingTelegram() && ((TEST_DELAY_MS <= 0) || (sendTimeout.expired())))
    {
        sendTestTelegram();
    }
    else
    {
        waitForInterrupt();
    }
}

/**
 * @brief The processing loop while no KNX-application is loaded.
 */
void loop_noapp(void)
{
    loop();
}

/** @}*/
