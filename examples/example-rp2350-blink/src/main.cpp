/*
 *  main.cpp - LED blink example for RP2350 using sblib GPIO functions.
 *
 *  Blinks the on-board LED (GP25 on Pico / Pico 2) at 250 ms intervals.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"
#include <sblib/digital_pin.h>
#include <sblib/timer.h>

// On-board LED pin (GP25 on Raspberry Pi Pico / Pico 2)
#define LED_PIN 25

int main()
{
    stdio_init_all();

    pinMode(LED_PIN, OUTPUT);

    while (true)
    {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(250);
    }
}
