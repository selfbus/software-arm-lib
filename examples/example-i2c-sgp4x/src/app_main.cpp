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

SGP4xClass SGP40;
SHT4xClass SHT4x;
BCU1 bcu = BCU1();
bool testDone = false;


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
    serial.println("Selfbus I2C SGP4x sensor example");



//    SGP40.getSerialnumber();


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

    int16_t bytesRead = SGP40.init();;
    switch (bytesRead)
    {
        case -1:
            serial.println("SGP40.init: sending FAILED!");
            break;

        case 0:
            serial.println("SGP40.init: reading FAILED!");
            break;

        default:
            serial.println("SGP40 initialized successfully! bytesRead=", bytesRead);
            break;
    }

    return (&bcu);
}

void readSGP4Serial()
{
	uint16_t temp = SGP40.getSerialnumber();

	serial.println();
//	serial.println("SerialNr[0]: ", temp);
}


void getVocValue()
{
	uint16_t srawVoc = 0;

	if (!SHT4x.measureHighPrecision())
	{
	    serial.println("SHT40.measureHighPrecision() failed.");
	    return;
	}
    uint16_t relativeHumidity = SHT4x.getHumidity() ;
	uint16_t temperature = SHT4x.getTemperature() ;
	serial.println("SGP temp: ", temperature);
	serial.println("SGP hum: ", relativeHumidity);


// use calculated hum / temp value
// needed when hum / temp is taken from another source than SHT4x
//	SGP40.measureRawSignal(relativeHumidity * 65535 / 175, temperature * 65535 / 100, srawVoc);

// directly use tTicks and hTicks from SHT4x
    SGP40.measureRawSignal(SHT4x.getHumTicks(), SHT4x.getTempTicks(), srawVoc);
    serial.println("SGP VOC Ticks: ", srawVoc);
}


/*
 * execute SGP4x's built-in self-test
 * if result is 0 everything is OK
 * otherwise consult the datasheet
 */
void doSelfTest()
{
	serial.println("SELFTEST Ergebnis: ", SGP40.executeSelfTest());
}



/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    if (bReadTimer)
    {

        getVocValue();


        if(!testDone)
        {
        	serial.println("Einmal SELFTEST und SERIAL bitte!");
        	doSelfTest();
        	readSGP4Serial();
        	testDone = true;
        }

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
