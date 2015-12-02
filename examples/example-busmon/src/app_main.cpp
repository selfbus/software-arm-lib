/*
 *  app_main.cpp - The application's main.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib.h>
#include <sblib/serial.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/io_pin_names.h>
/*
 * Initialize the application.
 */
void setup()
{
    bcu.begin(2, 1, 1); // ABB, dummy something device

    // Disable telegram processing by the lib
    if (userRam.status & BCU_STATUS_TL)
        userRam.status ^= BCU_STATUS_TL | BCU_STATUS_PARITY;

    serial.begin(115200);
    serial.println("Selfbus Bus Monitor");

    pinMode(PIN_INFO, OUTPUT);	// Info LED
    pinMode(PIN_RUN, OUTPUT);	// Run LED
}

/*
 * The main processing loop.
 */
void loop()
{
	// static int receiveCount = -1;
  // static int telLength = 0;
  // static int  byteCount = 0;
  // static unsigned char telegram[32];
  // static unsigned char data;

	digitalWrite(PIN_RUN, 1);

	if (bus.telegramReceived())
    {
        for (int i = 0; i < bus.telegramLen; ++i)
        {
            if (i) serial.print(" ");
            serial.print(bus.telegram[i], HEX, 2);
        }
        serial.println();

        bus.discardReceivedTelegram();

        digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));
    }

    // Sleep until the next 1 msec timer interrupt occurs (or shorter)
    __WFI();
}
