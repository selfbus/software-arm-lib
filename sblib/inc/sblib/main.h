/**************************************************************************//**
 * @file   main.h
 * @brief  Functions for applications using the Selfbus library.
 *
 * @note   Your Application must implement following functions:
 *         - void setup()
 *         - void loop()
 *         - void loop_noapp()
 *
 *         In case your application uses the MCU ADC you might implement a callback
 *         for the BCU bus voltage measurement analog read function.
 *
 *         If your application need to be notified in case of bus voltage failure
 *         you need to implement a callback for that as well. Details can be found
 *         in the class BusVoltage
 *
 *
 * @warning All functions must return after their work is done as fast as possible.
 *
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2014
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @author HoRa Copyright (c) 2025
 *
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef sblib_main_h
#define sblib_main_h

#include <sblib/eib/bcu_base.h>

enum HWVersion
	{
    	HW_DEFAULT =0,
		HW_V1,
		HW_V2
	} ;

extern enum HWVersion HW_Version;

/**
 * @brief Check bus voltage and wait for run level
 *
 * check HW version based on ID1 and ID2 pins
 * as we are in early initialization phase of main(), HW IO is not yet
 * configured and we can use it without disturbance of an APP or the BCU
 *
 * We use IO pins which are possibly used in debug sessions only
 * most normal operations should not used these IO pins
 *
 * In debug ID1 might be used as TX, we can identify if an TX receiver
 * is connected -> high level when configured as input with PU
 * or the pin is pulled low -> HWv1 or HWv2
 * In case ID1 is low, than no TX/RX is connected and we check ID2 as well
 *
 * HW default: original z-diode D2 3v6 -> resistor division ratio :119/1000
 * HWv1		 : replace D2 with shottky BAT81 to Vcc -> resistor division ratio :103/1000
 * HWv2      : v1 plus Vref of 2.5V at AD4/PIO1_3/SWDIO, AD5/APP_SEL or AD0/LT8
 *
 * ID1			high		low		low
 * ID2			high		high	low
 * HW version	default		v1		v2
 *
 * measurement with default resistor divider ratio and basic analog functions
 * Indication of Vbus status:
 * - blink of both RUN and INFO LED : analog read error
 * - blink of RUN LED we did not reached RUN voltage level yet
 *
 */
void wait_for_run_level_voltage(void);


/**
 * @brief This function is called by the Selfbus's library main
 *        when the processor is started or reset.
 *
 * @note  You must implement this function in your code.
 */
BcuBase* setup();

/**
 * @brief This function is called in a loop by the Selfbus's library main.
 *        This is where your main work shall happen.
 *
 * @note  You must implement this function in your code.
 */
void loop();

/**
 * @brief While no KNX-application is loaded,
 *        this function is called in a loop by the Selfbus's library main.
 *
 * @note  You must implement this function in your code.
 */
void loop_noapp();

#endif /*sblib_main_h*/
