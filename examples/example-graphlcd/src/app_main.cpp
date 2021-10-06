/**************************************************************************//**
 * @file    app_main.cpp
 * @brief   An example application for a graphical LCD.
 *          We use the EA DOGS 102, a 102x64 monochrome graphical LCD.
 *
 *          We use SPI port 0 in this example.
 *
 *          Connect the ARM in this way to the EA-DOGS:
 *
 *          PIO0_2:  SSEL0 -> Display CS0 "chip select"
 *          PIO0_9:  MOSI0 -> Display SDA "data in"
 *          PIO2_11: SCK0  -> Display SCK "clock"
 *          PIO0_8:        -> Display CD  "command/data"
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

#include <sblib/lcd/graphical_eadogs.h>
#include <sblib/lcd/font_5x7.h>
#include <sblib/core.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/ioports.h>
#include <sblib/io_pin_names.h>

#define BLINK_PIN PIN_IO2

LcdGraphicalEADOGS display(SPI_PORT_0, PIO0_9, PIO2_11, PIO0_8, PIO0_2, font_5x7);

/**
 * Initialize the application.
 */
void setup()
{
    display.begin();
    pinMode(BLINK_PIN, OUTPUT);

    display.clear();
    display.println("********************");
    display.pos(0, 1);
    display.println("*** Hello World! ***");
    display.pos(0, 2);
    display.println("********************");
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    digitalWrite(BLINK_PIN, !digitalRead(BLINK_PIN));
    delay(1000);
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
