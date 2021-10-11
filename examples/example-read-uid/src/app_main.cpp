/**************************************************************************//**
 * @file    app_main.cpp
 * @brief   A simple application which reads the UID (GUID) of the
 *          LPCxx processor and sends it to the serial port
 *
 *          Connect a terminal program to the ARM's serial port:
 *          The serial port is used with 19200 baud, 8 data bits, no parity, 1 stop bit.
 *          Tx-pin is PIO1.6, Rx-pin is PIO1.7
 *
 * needs BCU1 version of the sblib library
 *
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

#include <sblib/core.h>
#include <sblib/internal/iap.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/serial.h>
#include <sblib/timer.h>

#define PAUSE_MS 1000                   //!> pause in milliseconds between each transmission

#define RX_PIN SERIAL_RX_PIN            //!> default serial Rx-Pin, defined in sblib config.h
#define TX_PIN SERIAL_TX_PIN            //!> default serial Tx-Pin, defined in sblib config.h

#define SERIAL_SPEED (19200)           //!> serial speed

#define UID_BYTES_FOR_BUSUPDATER 12    //!> number of byte the bus-updater needs for option -uid

/**
 * Initialize the application.
 */
void setup()
{
    serial.setRxPin(RX_PIN);
    serial.setTxPin(TX_PIN);
    serial.begin(SERIAL_SPEED);
    serial.println("Selfbus read UID example");
}

void sendUIDtoSerialPort(Serial &serialPort, byte* uid, unsigned int length)
{
    for (unsigned int i = 0; i < length; ++i)
    {
        serialPort.print(uid[i], HEX, 2);
        if (i < (length - 1))
        {
            serialPort.print(":");
        }
    }
    serialPort.println();
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    byte uniqueID[IAP_UID_LENGTH];

    if (iapReadUID(&uniqueID[0]) == IAP_SUCCESS)
    {
        // send the uid/guid we received from iapReadUID
        serial.print("Target UID is   :      ");
        sendUIDtoSerialPort(serial, &uniqueID[0], IAP_UID_LENGTH);
        serial.print("Busupdater needs: -uid ");
        sendUIDtoSerialPort(serial, &uniqueID[0], UID_BYTES_FOR_BUSUPDATER);
        serial.println();
    }
    else
    {
        serial.println("Error reading Target UID.");
    }
    delay(PAUSE_MS);
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
