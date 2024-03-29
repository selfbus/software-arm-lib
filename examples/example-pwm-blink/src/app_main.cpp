/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_PWM_BLINK_1 PWM blink example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Flashes a LED on digital pin PIO2.6 using the PWM output of the 16bit timer #1
 * @details To test, connect: PIO2.6 --[ 470 Ohm resistor ]---[ LED ]--- GND<br/>
 *          The LED to blink has to be connected to pin PIN_INFO (PIO2.6)
 *          We cannot blink the LED on the LPCxpresso board with PWM.<br/>
 *
 * @{
 *
 * @file   app_main.cpp
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2014
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/eibBCU1.h>
#include <sblib/io_pin_names.h>

/*
 * NOTICE
 *
 * The LED to blink has to be connected to pin PIN_INFO (PIO2.6)
 * We cannot blink the LED on the LPCxpresso board with PWM.
 *
 */

BCU1 bcu = BCU1();

/**
 * Initialize the application.
 */
BcuBase* setup()
{
    pinMode(PIN_INFO, OUTPUT_MATCH);  // configure digital pin PIN_INFO to match MAT1 of timer32 #0

    // Begin using the timer.
    timer32_0.begin();

    timer32_0.prescaler((SystemCoreClock / 1000) - 1); // let the timer count milliseconds
    timer32_0.matchMode(MAT1, SET);  // set the output of PIN_INFO to 1 when the timer matches MAT1
    timer32_0.match(MAT1, 800);      // match MAT1 when the timer reaches this value
    timer32_0.pwmEnable(MAT1);       // enable PWM for match channel MAT1

    // Reset the timer when the timer matches MAT3 and generate an interrupt.
    timer32_0.matchMode(MAT3, RESET | INTERRUPT);
    timer32_0.match(MAT3, 1000);     // match MAT3 after 1000 milliseconds

    timer32_0.start();

    return (&bcu);
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    __WFI(); // sleep until the next interrupt occurs
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
/** @}*/
