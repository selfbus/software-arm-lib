/**************************************************************************//**
 * @file    Download-Test.cpp
 * @brief   A simple application that blinks the RUN/INFO LED's
 *          and sends a simple message over RS232.
 *          The serial port is used with 115200 baud, 8 data bits, no parity, 1 stop bit.
 *          Tx-pin is PIO1.7, Rx-pin is PIO1.6
 *
 *          needs BCU1 version of the sblib library
 *          used on the Selfbus 4TE-ARM-Controller the RUN (PIO3.3) and INFO (PIO2.6) LED will blink
 *
 * @author Martin Glueck <martin@mangari.org> Copyright (c) 2015
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
#include <sblib/io_pin_names.h>
#include <sblib/timeout.h>
#include <sblib/serial.h>

Timeout blinky;

/**
 * Initialize the application.
 */
void setup()
{
    pinMode(PIN_INFO, OUTPUT);
    pinMode(PIN_RUN,  OUTPUT);
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    digitalWrite(PIN_RUN, 1);
    SysTick_Config(SystemCoreClock / 1000);

    blinky.start (1000);
    serial.begin(115200); // Tx: PIO1.7, Rx: PIO1.6
    serial.println("Hello World via KNX download!");
    // TODO: insert code here

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
        digitalWrite(PIN_INFO, i & 0x00080000);
        if (blinky.expired())
        {
        	blinky.start (1000);
            digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));
        }
    }
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
