/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_SERIAL_PORT_1 Serial port example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Shows the use of the serial port.
 * @details This example application shows the use of the serial port.<br />
 *          Connect a terminal program to the ARM's serial port.<br />
 *          The serial port is used with 115200 baud, 8 data bits, no parity, 1 stop bit.<br />
 *          Tx-pin is PIO1.7, Rx-pin is PIO1.6<br />
 *
 * @{
 *
 * @file    app_main.cpp
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
#include <sblib/serial.h>
#include <sblib/internal/iap.h>


#define PAUSE_MS 1000 ///> pause in milliseconds between each transmission

BCU1 bcu = BCU1();

/**
 * Initialize the application.
 */
BcuBase* setup()
{
    serial.setRxPin(PIO1_6); // @ swd/jtag connector
    serial.setTxPin(PIO1_7); // @ swd/jtag connector

    // serial.setRxPin(PIO2_7); // @ 4TE-ARM Controller pin 1 on connector SV3 (ID_SEL)
    // serial.setTxPin(PIO2_8); // @ 4TE-ARM Controller pin 2 on connector SV3 (ID_SEL)

    serial.begin(115200);

    serial.println("Selfbus serial port example");

    serial.print("Target MCU has ", iapFlashSize() / 1024);
    serial.println("k flash");
    serial.println();
    return (&bcu);
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    static int value = 0;
    unsigned char rxChar;

    // Write some text to the serial port
    serial.print("Counter value: b");
    serial.println(++value, BIN, 8);

    // handle incoming data from the serial port and echo it back
    while (serial.available())
    {
        rxChar = serial.read(); // character from receive-buffer
        serial.write(rxChar); // echo received character back
    }
    delay(PAUSE_MS);
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
/** @}*/
