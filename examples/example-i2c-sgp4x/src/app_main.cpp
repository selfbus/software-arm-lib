/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_I2C_SGP4X i2c SGP4x VOC/NOx air quality sensor example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Read SGP4x VOC/NOx air quality
 *
 * @note    Debug build needs at least a 64KB LPC111x.
 *
 * @{
 *
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2023
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
#include <sblib/i2c/SGP4x.h>
#include <sblib/i2c/SHT4x.h>

#define READ_TIMER (2000) ///> Read values timer in Milliseconds
bool bReadTimer = false;  ///> Condition to read values if timer reached

SGP4xClass SGP41;
SHT4xClass SHT4x;
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
 * Print an @ref SGP4xResult to the serial port with optional new line
 *
 * @param result    The SGP4xResult to print
 * @param newLine   Set True to print a new line character at the end.
 */
void printSGP4xResult(SGP4xResult result, bool newLine = true)
{
    serial.print(": ");
    switch (result)
    {
        case SGP4xResult::invalidByteCount:
            serial.print("invalid byte count!");
            break;

        case SGP4xResult::invalidCommandBuffer:
            serial.print("invalid command buffer!");
            break;

        case SGP4xResult::crc8Mismatch:
            serial.print("crc8 mismatch!");
            break;

        case SGP4xResult::sendError:
            serial.print("sending FAILED!");
            break;

        case SGP4xResult::readError:
            serial.print("reading FAILED!");
            break;

        case SGP4xResult::vocPixelError:
            serial.print("VOC pixel ERROR!");
            break;

        case SGP4xResult::noxPixelError:
            serial.print("NOx pixel ERROR!");
            break;

        case SGP4xResult::success:
            serial.print("succeeded!");
            break;

        default:
            serial.print("UNKNOWN result!");
            break;
    }
    if (newLine)
    {
        serial.println();
    }
}

/**
 * Executes SGP4x's built-in self-test
 * if result is @ref SGP4xResult::success everything is OK
 * otherwise consult the datasheet
 */
void doSelfTest()
{
    SGP4xResult result = SGP41.executeSelfTest();
    serial.print("SGP41.executeSelfTest");
    printSGP4xResult(result);
}

/**
 * Reads SGP4x's serial number
 */
void readSGP4Serial()
{
    uint8_t serialNumber[SGP4xClass::maxSerialNumberLength];
    const uint8_t length = sizeof(serialNumber)/sizeof(*serialNumber);
    SGP4xResult result = SGP41.getSerialnumber(&serialNumber[0], length);
    if (result != SGP4xResult::success)
    {
        serial.print("SGP41.getSerialnumber");
        printSGP4xResult(result);
        return;
    }

    serial.print("SGP41 SerialNr (hex) : ");
    for (uint8_t i = 0; i < length; i++)
    {
        serial.print(" ", serialNumber[i], HEX, 2);
    }
    serial.println();
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
    serial.println("Selfbus I2C SGP4x sensor example");

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

    if (!SHT4x.init())
    {
        serial.println("SHT4x.init: FAILED!");
    }
    else
    {
        serial.println("SHT4x initialized successfully!");
    }

    SGP4xResult result = SGP41.init(READ_TIMER);
    if (result != SGP4xResult::success)
    {
        serial.print("SGP41.init");
        printSGP4xResult(result);
    }

    result = SGP41.readFeatureSet();
    if (result != SGP4xResult::success)
    {
        serial.print("SGP41.readFeatureSet");
        printSGP4xResult(result);
    }
    serial.println("SGP41.getFeatureSet: 0x", SGP41.getFeatureSet(), HEX, 4);

    doSelfTest();
    readSGP4Serial();

    return (&bcu);
}

void printCompensationValues(float relHumity, float temperature, bool compensate)
{
    if (compensate)
    {
        serial.print("rel. humidity ", relHumity, 2);
        serial.print(", temperature ", temperature, 2);
    }
    else
    {
        serial.print("compensation off");
    }
    serial.print(":");
}

void measureAndPrintResults(float relHumity, float temperature, bool compensate)
{
    SGP4xResult result = SGP41.measureRawSignal(relHumity, temperature, compensate);
    if (result != SGP4xResult::success)
    {
        serial.print("SGP41.measureRawSignal");
        printSGP4xResult(result, true);
        return;
    }

    printCompensationValues(relHumity, temperature, compensate);
    serial.println();
    serial.print("  Ticks: VOC: ", (int)SGP41.getRawVocValue());
    serial.println(" NOx: ", (int)SGP41.getRawNoxValue());
    serial.print("  Index: VOC: ", (int)SGP41.getVocIndexValue());
    serial.println(" NOx: ", (int)SGP41.getNoxIndexValue());
}

void readSGP41Sensor()
{
    bool useCompensation = false;
    float relativeHumidity = 0.f;
    float temperature = 0.f;
	if (!SHT4x.measureHighPrecision())
	{
	    serial.println("SHT40.measureHighPrecision() failed.");
	}
	else
	{
        relativeHumidity = SHT4x.getHumidity() ;
        temperature = SHT4x.getTemperature() ;
        // serial.print("SHT4x rel. humidity: ", relativeHumidity);
        // serial.println(" temperature: ", temperature);
        useCompensation = true;
	}

	measureAndPrintResults(relativeHumidity, temperature, useCompensation);

	// uncomment to test default and min/max values for compensation
	// measureAndPrintResults(50.f, 25.f, false);
	// measureAndPrintResults(50.f, 25.f, true);   // should give the same result as with compensation disabled
	// measureAndPrintResults(0.f, -45.f, true);   // test supported minimum values
	// measureAndPrintResults(100.f, 130.f, true); // test supported maximum values
	serial.println();
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    if (bReadTimer)
    {
        readSGP41Sensor();
        bReadTimer = false;
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
