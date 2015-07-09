/*
 *  app_main.cpp - The application's main.
 *
 *  Copyright (c) 2015 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib.h>
#include <sblib/serial.h>
#include <sblib/io_pin_names.h>
#include <sblib/timeout.h>

Timeout blinky;

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

    pinMode(PIN_INFO, OUTPUT);    // Info LED
    pinMode(PIN_RUN, OUTPUT);    // Run LED
    blinky.start(1);
    bcu.setOwnAddress(0x1140);
}

/*
 * Convert a character from the serial line into a number
 */
static unsigned char _byteToNumber(unsigned char data)
{
    unsigned char result;
    result = data - 48;
    if (result > 9)
    {
        result -= 7;
        if (result > 15)
            result -= 32;
    }
    return result;
}

/*
 * The main processing loop.
 */
int receiveCount = -1;
int tLength = 0;
int  byteCount = 0;
unsigned char sendTelBuffer[32];
unsigned char data;
unsigned char mode = 0;
Timeout waitForAck;

void loop()
{

    if (blinky.expired ())
    {
        blinky.start (500);
        digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));
    }

    if (waitForAck.started() && waitForAck.expired())
    {
        serial.write ((byte) 0x00);
    }

    if (bus.telegramReceived())
    {
        if (! mode)
        {
            for (int i = 0; i < bus.telegramLen; ++i)
            {
                if (i) serial.print(" ");
                serial.print(bus.telegram[i], HEX, 2);
            }
            serial.println();
        }
        else
        {
            if (waitForAck.started ())
            {
                //B0 11 03 11 40 60 C2 AE
                if (   (bus.telegramLen == 8)
                   &&  (bus.telegram [4] == 0x40)
                   &&  (bus.telegram [5] == 0x60)
                   &&  (bus.telegram [6] == 0xC2)
                   )
                {
                    waitForAck.stop();
                    serial.write(0xAD);
                }
            }
#if 0
            serial.write(bus.telegramLen);
            for (int i = 0; i < bus.telegramLen; ++i)
            {
                serial.write(bus.telegram[i]);
            }
#endif
        }
        bus.discardReceivedTelegram();

        digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));
    }

    if (mode)
    {
        while (serial.available())
        {
            if (receiveCount < 0)
            {
                receiveCount = serial.read();
                tLength    = 0;
            }
            else
            {
                sendTelBuffer[tLength++] = serial.read();
                if (tLength == (receiveCount & 0x7F))
                {
                    bus.sendTelegram(sendTelBuffer, tLength);
                    if (receiveCount & 0x80)
                        waitForAck.start (1000);
                    else
                    {
                        while (bus.sendingTelegram());
                        serial.write (0xAD);
                    }
                    receiveCount = -1;
                }
            }
        }
    }
    else
    {
        // handle the incoming data form the serial line
        // the format should be:
        // CC aa bb cc dd ee
        // where CC is the number of bytes of the the telegram to send
        //       aa,bb,cc,dd,ee   the telegram data (without the checksum)
        if (serial.available())
        {
            int byte = serial.read();
            byteCount++;
            data = (data << 4) + _byteToNumber(byte);
            if (byteCount == 2)
            {   // a new data byte has been received
                byteCount = 0;
                serial.print(data, HEX, 2);
                serial.print(" ");
                if (receiveCount < 0)
                {   // the first byte is the number of bytes
                    if (data == 0xFF)
                    {
                        mode = 1;
                        serial.println("Switch to binary mode");
                    }
                    else
                        receiveCount = data;
                }
                else
                {
                    sendTelBuffer[tLength++] = data;
                    if (tLength == receiveCount)
                    {
                        bus.sendTelegram(sendTelBuffer, tLength);
                        tLength = 0;
                        receiveCount = -1;
                        serial.println();
                    }
                }
            }
        }
    }
    // Sleep until the next 1 msec timer interrupt occurs (or shorter)
    __WFI();
}
