/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_I2C_SHT4X i2c SHT4x temperature and humidity sensor example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Read SHT4x temperature and humidity sensor
 *
 * @note    Debug build needs at least a 64KB LPC111x.
 *
 * @{
 *
 * @author Erkan Colak <erkanc@gmx.de> Copyright (c) 2015
 * @author Mario Theodoridis Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2023
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/types.h>
#include <sblib/io_pin_names.h>
#include <sblib/i2c.h>
#include <sblib/eibBCU1.h>
#include <sblib/serial.h>
#include <sblib/i2c/SHT4x.h>

#define READ_TIMER (2000) ///> Read values timer in Milliseconds
bool bReadTimer = false;  ///> Condition to read values if timer reached

SHT4xClass SHT40;
BCU1 bcu = BCU1();

/**
 * Handler for the timer interrupt.
 */
extern "C" void TIMER32_0_IRQHandler()
{
    // Clear the timer interrupt flags, otherwise the interrupt handler is called again immediately after returning.
    timer32_0.resetFlags();
    bReadTimer= true;
}

/**
 * Initialize the application.
 */
BcuBase* setup()
{
    serial.setRxPin(PIO1_6); // @ swd/jtag connector
    serial.setTxPin(PIO1_7); // @ swd/jtag connector

    // serial.setRxPin(PIO2_7); // @ 4TE-ARM Controller pin 1 on connector SV3 (ID_SEL)
    // serial.setTxPin(PIO2_8); // @ 4TE-ARM Controller pin 2 on connector SV3 (ID_SEL)

    serial.begin(115200);
    serial.println("Selfbus I2C SHT4x sensor example");
    SHT40.init();

/*
    if(I2C::Instance()->bI2CIsInitialized) {  // I2CScan
        I2C::Instance()->I2CScan();           // check .I2CScan_State ans .I2CScan_uAdress
    }
*/

    // LED Initialize
    pinMode(PIN_INFO, OUTPUT);	 // Info LED (yellow)
    pinMode(PIN_RUN, OUTPUT);	 // Run  LED (green)
    pinMode(PIN_PROG, OUTPUT);   // Prog LED (red)

    // LED Set Initial Value (ON|OFF)
    digitalWrite(PIN_RUN, 0);    // Info LED (yellow), will be toggled with dht function (blink on read success)
    digitalWrite(PIN_INFO, 0);   // Run  LED (green), will be toggled with LUX function (off if Lux == 0)
    digitalWrite(PIN_PROG, 1);   // Prog LED (red), will be toggled with rtc function

    enableInterrupt(TIMER_32_0_IRQn);                   // Enable the timer interrupt
    timer32_0.begin();                                  // Begin using the timer
    timer32_0.prescaler((SystemCoreClock / 1000) - 1);  // Let the timer count milliseconds
    timer32_0.matchMode(MAT1, RESET | INTERRUPT);       // On match of MAT1, generate an interrupt and reset the timer
    timer32_0.match(MAT1, READ_TIMER);                  // Match MAT1 when the timer reaches this value (in milliseconds)
    timer32_0.start();                                  // Start now the timer
    return (&bcu);
}

void readSHT4TempHum()
{
	if (!SHT40.measureHighPrecision())
	{
	    serial.println("SHT40.measureHighPrecision() failed.");
	    return;
	}

    serial.print("SHT4x temperature: ", SHT40.getTemperature(), 2);
    serial.print(" C");
    serial.print(" humidity: ", SHT40.getHumidity(), 2);
    //serial.print(" Dew point: ", SHT40.getDewPoint(), 2);
    //serial.print(" C");
    serial.println();
}

void readSHT4readSerial()
{
    uint32_t serialnumber = SHT40.getSerialnumber();
    if (serialnumber == 0)
    {
        serial.println("SHT40.getSerialnumber() failed.");
        return;
    }

    serial.println("SHT4x serial number: 0x", (unsigned int)serialnumber, HEX, 8);
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    if(bReadTimer)
    {
        readSHT4TempHum();
        readSHT4readSerial();
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
/** @}*/
