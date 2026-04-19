/*
 *  main.cpp - I2C BH1750 light sensor example for RP2350.
 *
 *  Reads the BH1750 ambient light sensor every 2 seconds via I2C and
 *  prints the lux value over the sblib Serial port.
 *
 *  I2C pins: GP4 (SDA), GP5 (SCL)
 *  Serial pins: GP0 (TX), GP1 (RX)
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"
#include <sblib/i2c.h>
#include <sblib/i2c/bh1750.h>
#include <sblib/serial.h>
#include <sblib/timer.h>
#include <sblib/digital_pin.h>

#define LED_PIN 25

BH1750 lightSensor;

int main()
{
    stdio_init_all();

    pinMode(LED_PIN, OUTPUT);

    serial.begin(115200);
    serial.println("Selfbus RP2350 I2C BH1750 light sensor example");

    i2c_lpcopen_init();
    lightSensor.begin();

    while (true)
    {
        if (lightSensor.measurementReady(true))
        {
            float lux = lightSensor.readLightLevel();
            serial.print("Light: ");
            serial.print(lux, 1);
            serial.println(" lx");

            // LED on if light level > 0
            digitalWrite(LED_PIN, lux > 0.0f);
        }

        delay(2000);
    }
}
