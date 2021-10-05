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

enum SystemSpeed {mhz12, mhz24, mhz36, mhz48};

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
 * @fn void connectSystemOscillatorToClkOutPin()
 * @brief Connect the system oscillator to clock-out pin (PIO0.1)
 *        This allows measurement with oscilloscope.
 *
 *        USE WITH CAUTION !
 *        Trying to debug may run into error 'Ee(42). Could not connect to core.'.
 *        This is due to CLKOUT and ISP_Enable share the same pin.
 *        Use Flashmagic to restore the LPC.
 */
void connectSystemOscillatorToClkOutPin()
{
    LPC_IOCON->PIO0_1        &= 0xFFFFFFF8; //clear FUNC bits (sec. 7.4.4)
    LPC_IOCON->PIO0_1        |= 0x01;       //set FUNC bits to CLKOUT function (sec. 7.4.4)
    LPC_GPIO0->DIR           |= 0x01;       //set CLKOUT pin to output (sec. 12.3.2)
    LPC_SYSCON->CLKOUTCLKSEL = 0x3;         //sec. 3.5.21
    LPC_SYSCON->CLKOUTUEN    = 0;           //"CLKOUTUEN" has to be toggled for "CLKOTUCLKSEL" to accept the value written to it
    LPC_SYSCON->CLKOUTUEN    = 1;           //"CLKOUTUEN" has to be toggled for "CLKOTUCLKSEL" to accept the value written to it
    LPC_SYSCON->CLKOUTDIV    = 1;           //set divider to 1 (sec. 3.5.23)
}

/**
 * @fn void setSystemSpeed(SystemSpeed)
 * @brief Changes SystemCoreClock to newSystemSpeed
 *
 * @param[in] newSystemSpeed
 */
void setSystemSpeed(SystemSpeed newSystemSpeed)
{
    unsigned int msel;
    unsigned int psel;

    switch (newSystemSpeed)
    {
        case mhz12 :
            msel = 0x0;
            psel = 0x0;
            break;
        case mhz24 :
            msel = 0x1;
            psel = 0x2;
            break;
        case mhz36 :
            msel = 0x2;
            psel = 0x2;
            break;
        default: // 48MHz
            msel = 0x3;
            psel = 0x1;
            break;
    }

    LPC_SYSCON->SYSPLLCTRL   = (psel << 5) | msel;
    LPC_SYSCON->SYSPLLCLKUEN = 0;               // toggle PLL update register
    LPC_SYSCON->SYSPLLCLKUEN = 1;
    while (!(LPC_SYSCON->SYSPLLSTAT & 0x01));   // wait for update to be finished
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);
}

/**
 * Initialize the application.
 */
void setup()
{
    pinMode(testPin, OUTPUT);
    // setSystemSpeed(mhz48); // test different SystemCoreClocks
    connectSystemOscillatorToClkOutPin();
}

/**
 * Test of delayMicroseconds() method.
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
