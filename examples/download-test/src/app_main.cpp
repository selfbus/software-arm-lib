/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_DOWLOAD_TEST_1 Download-Test example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Blinks the RUN/INFO LED's and sends a simple message over RS232.
 * @details The serial port is used with 115200 baud, 8 data bits, no parity, 1 stop bit.<br/>
 *          Tx-pin is PIO1.7, Rx-pin is PIO1.6<br/>
 *          used on the Selfbus 4TE-ARM-Controller the RUN (PIO3.3) and INFO (PIO2.6) LED will blink<br/>
 *
 * @{
 *
 * @file   app_main.cpp
 * @author Martin Glueck <martin@mangari.org> Copyright (c) 2015
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
#include <sblib/timeout.h>
#include <sblib/serial.h>

Timeout blinky;

APP_VERSION("exDownlo", "0", "10")

BCU1 bcu = BCU1();

/**
 * Initialize the application.
 */
BcuBase* setup()
{
    pinMode(PIN_INFO, OUTPUT);
    pinMode(PIN_RUN,  OUTPUT);
    return (&bcu);
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    digitalWrite(PIN_RUN, 1);
    SysTick_Config(SystemCoreClock / 1000);

    blinky.start (1000);
    serial.setRxPin(PIO1_6);
    serial.setTxPin(PIO1_7);
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
/** @}*/
