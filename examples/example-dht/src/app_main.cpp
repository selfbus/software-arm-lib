/**************************************************************************//**
 * @file    app_main.cpp
 * @brief   A simple application which will demonstrate how to read Temperature and
 *          Humidity from one or more DHT22 sensors a timer and the timer interrupt.
 *
 *          needs BCU1 version of the sblib library
 *
 * @author Erkan Colak <erkanc@gmx.de> Copyright (c) 2015
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/core.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/io_pin_names.h>
#include <sblib/sensors/dht.h>


DHT dht;  ///< DHT 1st sensor
// DHT dht1; ///< DHT 2nd sensor


#define DHT_PIN PIO2_2      ///< IO-pin the 1st sensor is connected to
//#define DHT1_PIN PIO0_7     ///< IO-pin the 2nd sensor is connected to

#define READ_TIMER 50       ///< Read values timer in Milliseconds
bool bReadTimer = false;    ///< Condition to read values if timer reached

/**
 * Handler for the timer interrupt.
 */
extern "C" void TIMER32_0_IRQHandler()
{
    // Toggle the Info and Prog LED
    digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));   // Indicate timer is running
    digitalWrite(PIN_PROG, !digitalRead(PIN_PROG));   // Indicate timer is running

    // Clear the timer interrupt flags, otherwise the interrupt handler is called again immediately after returning.
    timer32_0.resetFlags();
    bReadTimer= true;
}

/**
 * Initialize the application.
 */
void setup()
{
    // initialize the DHT22 sensors
    dht.DHTInit(DHT_PIN, DHT22);
    // dht1.DHTInit(DHT1_PIN, DHT22);

    // LED Initialize
    pinMode(PIN_INFO, OUTPUT); // Info LED (yellow)
    pinMode(PIN_RUN, OUTPUT);  // Run  LED (green)
    pinMode(PIN_PROG, OUTPUT); // Prog LED (red)

    // LED Set Initial Value (ON|OFF)
    digitalWrite(PIN_INFO, 0); // Info LED (yellow)
    digitalWrite(PIN_RUN, 0);  // Run  LED (green) , will be toggled with dht function (blink on read success)
    digitalWrite(PIN_PROG, 1); // Prog LED (red)

    enableInterrupt(TIMER_32_0_IRQn);                   // Enable the timer interrupt
    timer32_0.begin();                                  // Begin using the timer
    timer32_0.prescaler((SystemCoreClock / 1000) - 1);  // Let the timer count milliseconds
    timer32_0.matchMode(MAT1, RESET | INTERRUPT);       // On match of MAT1, generate an interrupt and reset the timer
    timer32_0.match(MAT1, READ_TIMER);                  // Match MAT1 when the timer reaches this value (in milliseconds)
    timer32_0.start();                                  // Start now the timer
}

/**
 * Read Data from DHT. On success the green LED will blink.
 */
bool ReadDHT()
{
    if(dht.readData() /*&& dht1.readData() */ )
    {
        digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));
        // dht._lastTemperature;
        // dht._lastHumidity;
        // float fDewPoint= dht.CalcdewPointFast( dht._lastTemperature, dht._lastHumidity);
        return true;
    };
    return false;
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    if(bReadTimer)
    {
        delay(2500);
        ReadDHT();
        bReadTimer=false;
    }
    // Sleep until the next interrupt happens
    __WFI();
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
