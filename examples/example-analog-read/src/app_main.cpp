/**************************************************************************//**
 * @file    app_main.cpp
 * @brief   A simple application that reads the analog channel AD0 (PIO0.11) and
 *          prints the read value to the serial port.
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

/**
 * Initialize the application.
 */
void setup()
{
    analogBegin();
    pinMode(PIO0_11, INPUT_ANALOG); // AD0 @ PIO0.11

    // Enable the serial port with 115200 baud, no parity, 1 stop bit
    // Tx: PIO1.7, Rx: PIO1.6
    serial.begin(115200);
    serial.println("Analog read example");
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    int value = analogRead(AD0);

    serial.println(value);

    delay(500);
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
