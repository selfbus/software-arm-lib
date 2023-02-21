/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_SERIAL_BUS_2 Serial Bus Monitor 2 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief   A second Bus monitor that outputs all received KNX-Bus bytes to the serial port
 * @details This is a bus monitor that outputs all received KNX-Bus bytes to the serial port.<br/>
 *          It uses the debug macros of the sblib for that.<br/>
 *          The serial port is used with 576,000 baud, 8 data bits, no parity, 1 stop bit.<br/>
 *          Serial pins for a 4TE Controller: Tx-pin PIO3.0, Rx-pin is PIO3.1<br/>
 *          Serial pins for a TS_ARM Controller: Tx-pin PIO2.8, Rx-pin is PIO2.7<br/>
 *
 *          links against Debug BCU1 version of the sblib library
 *
 * @warn    #define BUSMONITOR and #define DUMP_TELEGRAMS must be "enabled" in @ref sblib/inc/libconfig.h
 *
 * @{
 *
 * @file   app_main.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include "config.h"
#include <sblib/eibBCU1.h>
#include <sblib/serial.h>
#include <sblib/io_pin_names.h>
#include <sblib/timeout.h>

Timeout blinky; ///< Timeout to blink the Led

BCU1 bcu = BCU1();

/**
 * Initialize the application.
 */
BcuBase* setup()
{
    serial.setTxPin(PIN_SERIAL_TX);
    serial.setRxPin(PIN_SERIAL_RX);
    serial.begin(SERIAL_BAUD_RATE);
    serial.println("Selfbus Bus Monitor 2");

    bcu.begin(2, 1, 1); // ABB, dummy something device

    pinMode(PIN_BLINK, OUTPUT);
    blinky.start(1);
    return (&bcu);
}

/**
 * The main processing loop.
 */
void loop()
{
    pinMode(PIN_BLINK, OUTPUT);
    if (blinky.expired ())
    {
        blinky.start (BLINK_TIME_MS);
        digitalWrite(PIN_BLINK, !digitalRead(PIN_BLINK));
    }
    // Sleep until the next 1 msec timer interrupt occurs (or shorter)
    waitForInterrupt();
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{

}
/** @}*/
