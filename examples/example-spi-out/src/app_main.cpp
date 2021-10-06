/**************************************************************************//**
 * @file    app_main.cpp
 * @brief   A simple example for SPI.
 *          This example configures SPI for output and sends a byte every second.
 *
 *          We use SPI port 0 in this example.
 *
 *          Pinout:
 *
 *          PIO0_2:  SSEL0
 *          PIO0_9:  MOSI0
 *          PIO2_11: SCK0
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
#include <sblib/ioports.h>
#include <sblib/io_pin_names.h>
#include <sblib/spi.h>

SPI spi(SPI_PORT_0);

// #define BLINK_PIN PIN_IO2
#define BLINK_PIN PIN_INFO

/**
 * Initialize the application.
 */
void setup()
{
    pinMode(BLINK_PIN,  OUTPUT);

    pinMode(PIO0_2,  OUTPUT | SPI_SSEL);
    pinMode(PIO0_9,  OUTPUT | SPI_MOSI);
    pinMode(PIO2_11, OUTPUT | SPI_CLOCK);

    spi.setClockDivider(128);
    spi.begin();
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    static int val = 0;

    val++;
    val &= 255;

    digitalWrite(BLINK_PIN, true);
    spi.transfer(val);
    delay(200);

    digitalWrite(BLINK_PIN, false);
    delay(800);
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
