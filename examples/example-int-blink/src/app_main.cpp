/*****************************************************************************************************//**
 * @file    app_main.cpp
 * @brief   A simple application that blinks the Info LED of the 4TE-ARM Controller (on pin PIO2.6)
 *          using a timer and its timer interrupt.
 *
 *          needs BCU1 version of the sblib library
 *
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2014
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 *****************************************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/core.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/io_pin_names.h>

#define BLINK_LED PIN_INFO ///> Info LED of the 4TE-ARM Controller

/*
 * Handler for the timer interrupt.
 */
extern "C" void TIMER32_0_IRQHandler()
{
    // Toggle the Info LED
    digitalWrite(BLINK_LED, !digitalRead(BLINK_LED));

    // Clear the timer interrupt flags, otherwise the interrupt handler is called again immediately after returning.
    timer32_0.resetFlags();
}

/**
 * Initialize the application.
 */
void setup()
{
	pinMode(BLINK_LED, OUTPUT);
	pinMode(PIN_RUN, OUTPUT);	// Run LED

    // Enable the timer interrupt
    enableInterrupt(TIMER_32_0_IRQn);

    // Begin using the timer
    timer32_0.begin();

    // Let the timer count milliseconds
    timer32_0.prescaler((SystemCoreClock / 1000) - 1);

    // On match of MAT1, generate an interrupt and reset the timer
    timer32_0.matchMode(MAT1, RESET | INTERRUPT);

    // Match MAT1 when the timer reaches this value (in milliseconds)
    timer32_0.match(MAT1, 500);

    timer32_0.start();
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    // Sleep until the next interrupt happens
    __WFI();
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
