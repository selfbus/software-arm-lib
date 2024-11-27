/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_ANALOG_READ_1 Analog read example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Read the analog channel AD0 (PIO0.11) and print the read value to the serial port.
 * @details A simple application that reads the analog channel AD0 (PIO0.11) and
 *          prints the read value to the serial port.<br/>
 *          The serial port is used with 115200 baud, 8 data bits, no parity, 1 stop bit.
 *          Tx-pin is PIO1.7, Rx-pin is PIO1.6<br />
 *
 * @{
 *
 * @file   app_main.cpp
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2014
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/eibBCU1.h>
#include <sblib/analog_pin.h>
#include <sblib/serial.h>

BCU1 bcu = BCU1();

/**
 * Initialize the application.
 */
BcuBase* setup()
{
    analogBegin();
    pinMode(PIO0_11, INPUT_ANALOG); // AD0 @ PIO0.11

    // Enable the serial port with 115200 baud, no parity, 1 stop bit
    // 4TE Tx: PIO1.7, Rx: PIO1.6
    //serial.setRxPin(PIO1_6);
    //serial.setTxPin(PIO1_7);
// OM1387 board
    serial.setRxPin(PIO2_7);
    serial.setTxPin(PIO2_8);
    // TS_ARM Tx: PIO3.0, Rx: PIO3.1
    // serial.setRxPin(PIO3_1);
    // serial.setTxPin(PIO3_0);
    serial.begin(115200);
    serial.println("Analog read example");

    return (&bcu);
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    int value = analogRead(AD0);

    serial.println( " adc PIO0_11 cnt:",value);
    serial.println( " adc PIO0_11 mV :",(value * ADC_LSB_MV));

    // check bus voltage
	enum BusVoltage::OPERATION_STATE OpState;
	int BVmV;
		OpState =  busVoltageMonitor.CheckBusVoltage();  // read Bus voltage and check level
		value = busVoltageMonitor.valueBusVoltagemV();

		serial.println( " analog value PIO1_11 mV:",value);


    delay(500);
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
/** @}*/
