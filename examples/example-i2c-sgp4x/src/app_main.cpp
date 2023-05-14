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
 * Executes SGP4x's built-in self-test
 * if result is @ref SGP4xResult::success everything is OK
 * otherwise consult the datasheet
 */
void doSelfTest()
{
    SGP4xResult result = SGP41.executeSelfTest();
    switch (result)
    {
        case SGP4xResult::crc8Mismatch:
            serial.println("SGP41.executeSelfTest: crc8 mismatch!");
            break;

        case SGP4xResult::sendError:
            serial.println("SGP41.executeSelfTest: sending FAILED!");
            break;

        case SGP4xResult::readError:
            serial.println("SGP41.executeSelfTest: reading FAILED!");
            break;

        case SGP4xResult::vocPixelError:
            serial.println("SGP41.executeSelfTest: VOC pixel ERROR!");
            break;

        case SGP4xResult::noxPixelError:
            serial.println("SGP41.executeSelfTest: NOx pixel ERROR!");
            break;

        case SGP4xResult::success:
            serial.println("SGP41 self test succeeded!");
            break;

        default:
            serial.println("SGP41.executeSelfTest: UNKNOWN result!");
            break;
    }
}

/**
 * Reads SGP4x's serial number
 */
void readSGP4Serial()
{
    uint64_t serialNumber = 0;
    SGP4xResult result = SGP41.getSerialnumber(&serialNumber);
    switch (result)
    {
        case SGP4xResult::crc8Mismatch:
            serial.println("SGP41.getSerialnumber: crc8 mismatch!");
            break;

        case SGP4xResult::sendError:
            serial.println("SGP41.getSerialnumber: sending FAILED!");
            break;

        case SGP4xResult::readError:
            serial.println("SGP41.getSerialnumber: reading FAILED!");
            break;

        case SGP4xResult::success:
            serial.println("SGP41.getSerialnumber succeeded!");
            serial.print("SerialNr (hex) :");
            serial.print(" ", (uint8_t)((serialNumber >> 40) & 0xff), HEX, 2);
            serial.print(" ", (uint8_t)((serialNumber >> 32) & 0xff), HEX, 2);
            serial.print(" ", (uint8_t)((serialNumber >> 24) & 0xff), HEX, 2);
            serial.print(" ", (uint8_t)((serialNumber >> 16) & 0xff), HEX, 2);
            serial.print(" ", (uint8_t)((serialNumber >> 8) & 0xff), HEX, 2);
            serial.print(" ", (uint8_t)(serialNumber & 0xff), HEX, 2);
            break;

        default:
            serial.println("SGP41.getSerialnumber: UNKNOWN result!");
            break;
    }
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

    if (!SHT4x.init())
    {
        serial.println("SHT4x.init: FAILED!");
    }
    else
    {
        serial.println("SHT4x initialized successfully!");
    }

    SGP4xResult result = SGP41.init(READ_TIMER);
    switch (result)
    {
        case SGP4xResult::crc8Mismatch:
            serial.println("SGP41.init: crc8 mismatch!");
            break;

        case SGP4xResult::sendError:
            serial.println("SGP41.init: sending FAILED!");
            break;

        case SGP4xResult::readError:
            serial.println("SGP41.init: reading FAILED!");
            break;

        case SGP4xResult::success:
            serial.println("SGP41 initialized successfully!");
            break;

        default:
            serial.println("SGP41.init: UNKNOWN result!");
            break;
    }

    doSelfTest();
    readSGP4Serial();

    return (&bcu);
}

void readSGP41Sensor()
{
	if (!SHT4x.measureHighPrecision())
	{
	    serial.println("SHT40.measureHighPrecision() failed.");
	    return;
	}
    float relativeHumidity = SHT4x.getHumidity() ;
    float temperature = SHT4x.getTemperature() ;
	serial.print("SHT4x temp: ", temperature);
	serial.println(" hum: ", relativeHumidity);

	// use calculated hum / temp value
    // needed when hum / temp is taken from another source than SHT4x
    // SGP41.measureRawSignal(relativeHumidity * 65535 / 175, temperature * 65535 / 100, srawVoc);

	// directly use tTicks and hTicks from SHT4x
	switch (SGP41.measureRawSignal(SHT4x.getHumTicks(), SHT4x.getTempTicks()))
	{
        case SGP4xResult::crc8Mismatch:
            serial.println("SGP41.measureRawSignal: crc8 mismatch!");
            break;

        case SGP4xResult::sendError:
            serial.println("SGP41.measureRawSignal: sending FAILED!");
            break;

        case SGP4xResult::readError:
            serial.println("SGP41.measureRawSignal: reading FAILED!");
            break;

        case SGP4xResult::success:
            serial.println("SGP41 measureRawSignal succeeded!");
            serial.print("VOC Ticks: ", (int)SGP41.getRawVocValue());
            serial.println(" Index: ", (int)SGP41.getVocIndexValue());
            serial.print("NOx Ticks: ", (int)SGP41.getRawNoxValue());
            serial.println(" Index: ", (int)SGP41.getNoxIndexValue());
            break;

        default:
            serial.println("SGP41.measureRawSignal: UNKNOWN result!");
            break;
	}
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
