/**************************************************************************//**
 * @file    app_main.cpp
 * @brief   A simple application that sends pulses on a digital port.
 *
 * By default PIO0.7 is used, which is the LED on the LPCxpresso board.
 * needs BCU1 version of the sblib library.
 *
 * This example is meant to be a test for the timing of delayMicroseconds().
 *
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2015
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
#include <sblib/digital_pin.h>
#include <sblib/io_pin_names.h>

/**
 * The pin that is used for this example, see sblib/io_pin_names.h for other options
 * e.g. PIN_IO1, PIN_RUN, PIN_INFO
 */
int pin = PIO0_7;
// int pin = PIN_IO1;
// int pin = PIN_RUN;
// int pin = PIN_INFO;

int mask = digitalPinToBitMask(pin);
LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin)];


/**
 * Initialize the application.
 */
void setup()
{
    pinMode(pin, OUTPUT);
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    /* To be as accurate as possible, we directly access the IO pin and do not
     * use digitalWrite().
     *
     * This is for exact testing of delayMicroseconds() and should not be used
     * in your own code!
     */
    port->MASKED_ACCESS[mask] = mask;
    delayMicroseconds(10);
    port->MASKED_ACCESS[mask] = 0;
    delayMicroseconds(50);

    port->MASKED_ACCESS[mask] = mask;
    delayMicroseconds(100);
    port->MASKED_ACCESS[mask] = 0;
    delayMicroseconds(500);

    port->MASKED_ACCESS[mask] = mask;
    delayMicroseconds(1000);
    port->MASKED_ACCESS[mask] = 0;
    delayMicroseconds(5000);

    port->MASKED_ACCESS[mask] = mask;
    delayMicroseconds(10000);
    port->MASKED_ACCESS[mask] = 0;
    delayMicroseconds(50000);

    port->MASKED_ACCESS[mask] = mask;
    delayMicroseconds(100000);
    port->MASKED_ACCESS[mask] = 0;
    delay(500);
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
