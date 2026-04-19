/*
 *  main.cpp - I2C SHT4x temperature/humidity sensor example for RP2350.
 *
 *  Reads the SHT4x sensor every 2 seconds via I2C and prints temperature
 *  and humidity over the sblib Serial port.
 *
 *  I2C pins: GP4 (SDA), GP5 (SCL) — defaults for sblib RP2350 I2C HAL.
 *  Serial pins: GP0 (TX), GP1 (RX) — defaults for sblib RP2350 Serial HAL.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"
#include <sblib/i2c.h>
#include <sblib/i2c/SHT4x.h>
#include <sblib/serial.h>
#include <sblib/timer.h>
#include <sblib/digital_pin.h>

#define LED_PIN  25
#define READ_INTERVAL_MS 2000

SHT4xClass sht40;

int main()
{
    stdio_init_all();

    pinMode(LED_PIN, OUTPUT);

    serial.begin(115200);
    serial.println("Selfbus RP2350 I2C SHT4x sensor example");

    sht40.init();

    while (true)
    {
        if (sht40.measureHighPrecision())
        {
            serial.print("Temp: ");
            serial.print(sht40.getTemperature(), 2);
            serial.print(" C  Hum: ");
            serial.print(sht40.getHumidity(), 2);
            serial.println(" %");

            // Toggle LED on successful read
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        }
        else
        {
            serial.println("SHT4x read failed.");
        }

        delay(READ_INTERVAL_MS);
    }
}
