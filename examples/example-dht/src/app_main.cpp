/*
 *  A simple application which will demonstrate how to read Temperature and
 *  Humidity from one or more DHT22 sensors a timer and the timer interrupt.
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#include <sblib/core.h>

#include <sblib/sensors/dht.h>

DHT dht;  // DHT 1st Sensor
//DHT dht1; // DHT 2nd Sensor

#define READ_TIMER 50         // Read values timer in Milliseconds
bool bReadTimer= false;       // Condition to read values if timer reached

/*
 * Handler for the timer interrupt.
 */
extern "C" void TIMER32_0_IRQHandler()
{
  // Toggle the Info and Prog LED
  digitalWrite(PIO2_6, !digitalRead(PIO2_6));   // Indicate timer is running
  digitalWrite(PIO2_0, !digitalRead(PIO2_0));   // Indicate timer is running


  // Clear the timer interrupt flags.
  timer32_0.resetFlags();                     // Otherwise the interrupt handler is called again immediately after returning.
  bReadTimer= true;
}

/*
 * Initialize the application.
 */
void setup()
{
  dht.DHTInit(PIO2_2, DHT22);       // Use the DHT22 sensor on PIN
  //dht1.DHTInit(PIO0_7, DHT22);      // Use the DHT22 sensor on PIN

  // LED Initialize
	pinMode(PIO2_6, OUTPUT);	// Info LED (yellow)
	pinMode(PIO3_3, OUTPUT);	// Run  LED (green)
	pinMode(PIO2_0, OUTPUT);  // Prog LED (red)

	// LED Set Initial Value (ON|OFF)
	digitalWrite(PIO3_3, 0);  // Info LED (yellow)
	digitalWrite(PIO2_6, 0);  // Run  LED (green)             // Will be toggled with dht function (blink on read success)
	digitalWrite(PIO2_0, 1);  // Prog LED (red)

  enableInterrupt(TIMER_32_0_IRQn);                         // Enable the timer interrupt
  timer32_0.begin();                                        // Begin using the timer
    timer32_0.prescaler((SystemCoreClock / 1000) - 1);      // Let the timer count milliseconds
    timer32_0.matchMode(MAT1, RESET | INTERRUPT);           // On match of MAT1, generate an interrupt and reset the timer
    timer32_0.match(MAT1, READ_TIMER);                      // Match MAT1 when the timer reaches this value (in milliseconds)
  timer32_0.start();                                        // Start now the timer
}

/*
 * Read Data from DHT. On success the green LED will blink.
 */
bool ReadDHT()
{
  if(dht.readData() /*&& dht1.readData() */ )
  {
    digitalWrite(PIO3_3, !digitalRead(PIO3_3));
    // dht._lastTemperature;
    // dht._lastHumidity;
    // float fDewPoint= dht.CalcdewPointFast( dht._lastTemperature, dht._lastHumidity);
    return true;
  } //else dht._lastError;
  return false;
}

/*
 * The main processing loop.
 */
void loop()
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

/******************************************************************************
**                            End Of File
******************************************************************************/
