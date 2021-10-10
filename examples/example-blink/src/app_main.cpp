/**************************************************************************//**
 * @file    app_main.cpp
 * @brief   A simple application that blinks some LED's
 *
 * needs BCU1 version of the sblib library
 * used on a LPCxpresso board the LED on pin PIO0.7 will blink
 * used on a Selfbus ARM-Controller the
 *                  PROGRAM (PIO2.0),
 *                  RUN     (PIO3.3) and
 *                  INFO (PIO2.6)
 *                  LED's will blink
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
#include <sblib/digital_pin.h>
#include <sblib/io_pin_names.h>
#include <sblib/eib/sblib_default_objects.h>

#define BLINK_TIME_1 250 // pause time in milliseconds
#define BLINK_TIME_2 250 // pause time in milliseconds

/**
 * Initialize the application.
 */
void setup()
{
    // set pins to output mode
    pinMode(PIN_IO2, OUTPUT);  // PIO0.7 on the LPCxpresso board
    pinMode(PIN_INFO, OUTPUT); // PIO2.6 on a Selfbus-ARM controller
    pinMode(PIN_RUN, OUTPUT);  // PIO3.3 on a Selfbus-ARM controller
    pinMode(PIN_PROG, OUTPUT); // PIO2.0 on a Selfbus-ARM controller
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    // toggle output PIO0.7 for the LPCxpresso board LED
    digitalWrite(PIO0_7, !digitalRead(PIO0_7));

    // toggle output PIO3.3 for a Selfbus 4TE-ARM-Controller RUN-LED
    digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));

    // toggle output PIO2.0 for a Selfbus ARM-Controller PROGRAM-LED
    digitalWrite(PIN_PROG, !digitalRead(PIN_PROG));

    // pause the program for BLINK_TIME_1 milliseconds
    delay(BLINK_TIME_1);

    // toggle output PIO2.6 for the Selfbus 4TE-ARM-Controller INFO-LED
    digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));

    // pause the program for BLINK_TIME_2 milliseconds
    delay(BLINK_TIME_2);
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
