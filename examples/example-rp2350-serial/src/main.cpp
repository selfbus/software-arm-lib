/*
 *  main.cpp - Serial port example for RP2350 using sblib Serial class.
 *
 *  Demonstrates the sblib Serial class on RP2350.
 *  Uses UART0 on GP0 (TX) / GP1 (RX), 115200 baud, 8N1.
 *  Connects a terminal to the UART pins or use USB CDC for printf output.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"
#include <sblib/serial.h>
#include <sblib/timer.h>

int main()
{
    stdio_init_all();

    serial.begin(115200);

    serial.println("Selfbus RP2350 serial port example");
    serial.println();

    int counter = 0;

    while (true)
    {
        serial.print("Counter: ");
        serial.println(++counter);

        // Echo received characters back
        while (serial.available())
        {
            unsigned char c = serial.read();
            serial.write(c);
        }

        delay(1000);
    }
}
