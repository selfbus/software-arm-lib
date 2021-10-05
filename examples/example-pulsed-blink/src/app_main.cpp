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

#include <math.h>
#include <sblib/core.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/io_pin_names.h>


///< microsecond to start the test
#define START_MICROSECOND_TO_TEST 0

///< microseconds to end the test
#define END_MICROSECOND_TO_TEST MAX_DELAY_MICROSECONDS

///< threshold 1, microsecond to speed up the test
#define SPEEDUP_1_MICROSECOND_TO_TEST 100

///< threshold 2, microsecond to speed up the test
#define SPEEDUP_2_MICROSECOND_TO_TEST 5000000

///< Pause between tests in milliseconds
#define TEST_PAUSE 500

///< macro to set the test pin high
#define SET_TESTPIN_HIGH port->MASKED_ACCESS[mask] = mask

///< macro to set the test pin low
#define SET_TESTPIN_LOW port->MASKED_ACCESS[mask] = 0

/**
 * The pin that is used for this example, see sblib/io_pin_names.h for other options
 * e.g. PIN_IO1, PIN_RUN, PIN_INFO
 */
int testPin = PIO0_7;
// int testPin = PIN_IO1;
// int testPin = PIN_RUN;
// int testPin = PIN_INFO;

///< bit mask to set the test pin high
int mask = digitalPinToBitMask(testPin);

///< direct access to the gpioPort of the test pin
LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(testPin)];

/**
 * Initialize the application.
 */
void setup()
{
    pinMode(testPin, OUTPUT);
}

/**
 * delayMicroseconds() method.
 */

void test_delayMicroseconds()
{
    /* To be as accurate as possible, we directly access the IO pin and do not
     * use digitalWrite().
     *
     * pin-state changed takes ~0,3 microseconds at 48MHz SystemCoreClock
     * ==> toggle on and off ~0,6 microseconds
     *
     * This is for exact testing of delayMicroseconds() and should not be used
     * in your own code!
     *
     */

    unsigned int microSecondToTest = START_MICROSECOND_TO_TEST;

    while (microSecondToTest <= END_MICROSECOND_TO_TEST)
    {
        SET_TESTPIN_LOW; // same as port->MASKED_ACCESS[mask] = 0;
        delayMicroseconds(microSecondToTest);
        SET_TESTPIN_HIGH; // same as port->MASKED_ACCESS[mask] = mask;
        delayMicroseconds(microSecondToTest);

        delay(TEST_PAUSE);

        if (microSecondToTest >= SPEEDUP_1_MICROSECOND_TO_TEST)
        {
            int exponent = floor(log10(microSecondToTest));
            if (microSecondToTest < SPEEDUP_2_MICROSECOND_TO_TEST)
            {
                exponent--;
            }
            int speedup = pow(10, exponent);
            microSecondToTest += speedup;
        }
        else
        {
            microSecondToTest++;
        }
    }
}

/**
 * The old test of delayMicroseconds() method.
 */
void test_old_delayMicroseconds()
{
    /* To be as accurate as possible, we directly access the IO pin and do not
     * use digitalWrite().
     *
     * pin-state changed takes ~0,3 microseconds at 48MHz SystemCoreClock
     * ==> toggle on and off ~0,6 microseconds
     *
     * This is for exact testing of delayMicroseconds() and should not be used
     * in your own code!
     *
     */
    SET_TESTPIN_HIGH; // same as port->MASKED_ACCESS[mask] = mask;
    delayMicroseconds(10);
    SET_TESTPIN_LOW;  // same as port->MASKED_ACCESS[mask] = 0;
    delayMicroseconds(50);

    SET_TESTPIN_HIGH;
    delayMicroseconds(100);
    SET_TESTPIN_LOW;
    delayMicroseconds(500);

    SET_TESTPIN_HIGH;
    delayMicroseconds(1000);
    SET_TESTPIN_LOW;
    delayMicroseconds(5000);

    SET_TESTPIN_HIGH;
    delayMicroseconds(10000);
    SET_TESTPIN_LOW;
    delayMicroseconds(50000);

    SET_TESTPIN_HIGH;
    delayMicroseconds(100000);
    SET_TESTPIN_LOW;
    delay(500);
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    test_delayMicroseconds();
    // test_old_delayMicroseconds(); //uncomment to use the old testing method
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
