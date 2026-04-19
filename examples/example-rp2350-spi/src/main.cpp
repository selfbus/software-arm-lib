/*
 *  main.cpp - SPI output example for RP2350 using sblib SPI class.
 *
 *  Sends an incrementing byte value over SPI every second, toggling
 *  the on-board LED with each transfer.
 *
 *  SPI0 pins: GP2 (SCK), GP3 (MOSI), GP4 (MISO)
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"
#include <sblib/digital_pin.h>
#include <sblib/spi.h>
#include <sblib/timer.h>

#define LED_PIN 25

SPI spi(SPI_PORT_0);

int main()
{
    stdio_init_all();

    pinMode(LED_PIN, OUTPUT);

    spi.setClockDivider(128);
    spi.begin();

    int val = 0;

    while (true)
    {
        val = (val + 1) & 0xFF;

        digitalWrite(LED_PIN, true);
        spi.transfer(val);
        delay(200);

        digitalWrite(LED_PIN, false);
        delay(800);
    }
}
