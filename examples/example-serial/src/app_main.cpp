/**************************************************************************//**
 * @file    app_main.cpp
 * @brief   This example application shows the use of the serial port.
 *          Connect a terminal program to the ARM's serial port:
 *          The serial port is used with 115200 baud, 8 data bits, no parity, 1 stop bit.
 *          Tx-pin is PIO1.7, Rx-pin is PIO1.6
 *
 *          needs BCU1 version of the sblib library
 *
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2014
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/core.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/serial.h>
#include <sblib/internal/iap.h>


#define PAUSE_MS 1000 ///> pause in milliseconds between each transmission

/**
 * Initialize the application.
 */
void setup()
{
    // serial.setRxPin(PIO2_7); // uncomment and change PIO for another RX-pin
    // serial.setTxPin(PIO2_8); // uncomment and change PIO for another TX-pin
    serial.begin(115200);

    serial.println("Selfbus serial port example");

    serial.print("Target MCU has ");
    serial.print(iapFlashSize() / 1024);
    serial.println("k flash");
    serial.println();
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    static int value = 0;
    unsigned char rxChar;

    // Write some text to the serial port
    serial.print("Counter value: B");
    serial.println(++value, BIN, 8);

    // handle incoming data from the serial port and echo it back
    while (serial.available())
    {
        rxChar = serial.read(); // character from receive-buffer
        serial.write(rxChar); // echo received character back
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
