/*
 *  main.cpp - OneWire DS18x20 temperature sensor example for RP2350.
 *
 *  Searches for DS18B20/DS18S20 sensors on the OneWire bus and reads
 *  temperature every 2 seconds.
 *
 *  OneWire data pin: GP16 (with 4.7k pull-up to 3.3V)
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"
#include <sblib/sensors/ds18x20.h>
#include <sblib/serial.h>
#include <sblib/timer.h>
#include <sblib/digital_pin.h>

#define ONEWIRE_PIN  16
#define LED_PIN      25

DS18x20 ds(ONEWIRE_PIN);

int main()
{
    stdio_init_all();

    pinMode(LED_PIN, OUTPUT);

    serial.begin(115200);
    serial.println("Selfbus RP2350 OneWire DS18x20 example");

    uint8_t numDevices = ds.Search();
    serial.print("Found ");
    serial.print((int)numDevices);
    serial.println(" sensor(s)");

    while (true)
    {
        ds.startConversionAll();
        delay(750); // DS18B20 conversion time at 12-bit resolution

        ds.readResultAll();

        for (uint8_t i = 0; i < numDevices; i++)
        {
            if (ds.lastReadOk(i))
            {
                float temp = ds.temperature(i);
                serial.print("Sensor ");
                serial.print((int)i);
                serial.print(": ");
                serial.print(temp, 2);
                serial.println(" C");
            }
        }

        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(1250); // Total cycle ~2s
    }
}
