/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_SERIAL_BUS_1 Serial Bus Monitor Example
 * @ingroup SBLIB_EXAMPLES
 * @brief    Bus monitor that outputs all received telegrams to the serial port
 * @details  This is a bus monitor that outputs all received telegrams to the serial port.<br/>
 *           The serial port is used with 115200 baud, 8 data bits, no parity, 1 stop bit.<br/>
 *           Tx-pin is PIO1.7, Rx-pin is PIO1.6<br/>
 *           In addition, one can send telegram on the bus via the serial port by sending
 *           a sequence of bytes.<br/>
 *           The incoming data from the serial line should be in following byte-format:<br />
 *           - CC aa bb cc dd ee<br />
 *           - CC             the number of telegram-bytes to be send<br />
 *           - aa, bb, cc, dd, ee the telegram data (without the checksum)<br />
 *
 * @{
 *
 * @file   app_main.cpp
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2014
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
#include <sblib/serial.h>
#include <sblib/io_pin_names.h>
#include <sblib/timeout.h>

Timeout blinky; ///< Timeout to blink the RUN Led

BCU1 bcu = BCU1();

/**
 * Initialize the application.
 */
BcuBase* setup()
{
    bcu.begin(2, 1, 1); // ABB, dummy something device

    // Disable telegram processing by the lib
    if (bcu.userRam->status & BCU_STATUS_TRANSPORT_LAYER)
    {
        bcu.userRam->status ^= BCU_STATUS_TRANSPORT_LAYER | BCU_STATUS_PARITY;
    }

    serial.begin(115200); // Tx: PIO1.7, Rx: PIO1.6
    serial.println("Selfbus Bus Monitor");

    pinMode(PIN_INFO, OUTPUT); // Info LED
    pinMode(PIN_RUN, OUTPUT);  // Run LED
    blinky.start(1);
    bcu.setOwnAddress(0x1140); // 1.1.64
    return (&bcu);
}

int receiveCount = -1;
int tLength = 0;
unsigned char sendTelBuffer[32];

/**
 * The main processing loop.
 */
void loop()
{
    if (blinky.expired ())
    {
        blinky.start (500);
        digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));
    }

    // handle the incoming data from the KNX-bus
    if (bcu.bus->telegramReceived())
    {
        serial.print(bcu.bus->telegramLen, HEX, 2);
		for (int i = 0; i < bcu.bus->telegramLen; ++i)
		{
			serial.print(bcu.bus->telegram[i], HEX, 2);
		}
		serial.println();
		bcu.bus->discardReceivedTelegram();

        digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));
    }

    // handle the incoming data from the serial line
    while (serial.available())
    {
        if (receiveCount < 0)
        {
            receiveCount = serial.read();
            tLength = 0;
        }
        else
        {
            sendTelBuffer[tLength++] = serial.read();
            if (tLength == (receiveCount & 0x7F))
            {
                bcu.bus->sendTelegram(sendTelBuffer, tLength);
                serial.write (0xAD);
                receiveCount = -1;
            }
        }
    }

    // Sleep until the next 1 msec timer interrupt occurs (or shorter)
    __WFI();
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{

}
/** @}*/
