/*
 *  main.cpp - Analog read example for RP2350 using sblib ADC functions.
 *
 *  Reads ADC channel 0 (GP26) every 500 ms and prints the value (0-1023)
 *  over the sblib Serial port (UART0 on GP0/GP1).
 *
 *  RP2350 ADC channel mapping:
 *    Channel 0 → GP26
 *    Channel 1 → GP27
 *    Channel 2 → GP28
 *    Channel 3 → GP29
 *    Channel 4 → Internal temperature sensor
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"
#include <sblib/analog_pin.h>
#include <sblib/serial.h>
#include <sblib/timer.h>

// ADC channel 0 = GP26
#define ADC_CHANNEL 0

int main()
{
    stdio_init_all();

    analogBegin();

    serial.begin(115200);
    serial.println("Selfbus RP2350 analog read example");
    serial.println("Reading ADC channel 0 (GP26)");

    while (true)
    {
        int value = analogRead(ADC_CHANNEL);

        serial.print("ADC: ");
        serial.println(value);

        delay(500);
    }
}
