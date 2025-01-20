/**************************************************************************//**
 * @file   main.cpp
 * @brief  The Selfbus library's main implementation.
 *
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2014
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @author HoRa Copyright (c) 2025
 *
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/main.h>

#include <sblib/interrupt.h>
#include <sblib/timer.h>
#include <sblib/analog_pin.h>
#include <sblib/digital_pin.h>
#include <sblib/eib/bcu_base.h>
#include <sblib/io_pin_names.h>
#include <sblib/eib/bus_voltage_level.h>

//#if defined(INCLUDE_SERIAL)
#include <sblib/serial.h>
//#endif

//#define DEBUG_BUS_VOLTAGE
enum HWVersion HW_Version;

/**
 * @brief Initializes the library.
 *        - Configures and starts the system timer to call SysTick_Handler once every 1 millisecond.
 */
static inline void lib_setup() {
	SysTick_Config(SystemCoreClock / 1000);
}

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
 * In debug ID2 might be used as TX output, we can identify if a TX receiver
 * is connected -> high level when configured as input with PU
 * or the pin is pulled low -> HWv1 or HWv2
 * In case ID2 is low, than no TX/RX is connected and we check ID1 as well
 *
 * HW default: original z-diode D2 3v6 -> resistor division ratio :114/1000
 * HWv1		 : replace D2 with shottky BAT81 to Vcc -> resistor division ratio :103/1000
 * HWv2      : v1 plus Vref of 2.5V at AD4/PIO1_3/SWDIO, AD5/APP_SEL or AD0/LT8
 *
 * ID1			high		high	low
 * ID2			high		low 	low
 * HW version	default		v1		v2
 *
 * measurement with default resistor divider ratio and basic analog functions
 * Indication of Vbus status:
 * - blink of both RUN and INFO LED : analog read error
 * - blink of RUN LED we did not reached RUN voltage level yet
 *
 */
void wait_for_run_level_voltage(void) {

	// we monitor the bus voltage in DEBUG mode only if we are explicitly debug the Bus Voltage Monitoring
#ifdef DEBUG
#ifndef DEBUG_BUS_VOLTAGE
	//	return;
#endif
#endif
	//stop any serial output as it might use the the ID1/ID2 pin
	HW_Version = HW_DEFAULT;
	// HW versioning is only available on the controller board TE4 v1.02
#if defined (__LPC11XX__)
#if defined(INCLUDE_SERIAL)
	serial.end();
#endif
	delay(1000);
	// we check the ID1 andID2 for HW version setting
	// check ID2 first-  in case a TX receiver is connected or open: pin is high and no HW modifications are present, if low, HW mod present

	pinMode(PIN_ID2, INPUT | PULL_UP);
	if (!digitalRead(PIN_ID2)) // low?
			{ // check ID2
		pinMode(PIN_ID1, INPUT | PULL_UP);
		if (!digitalRead(PIN_ID1)) // low?
				{			//HW V2
			HW_Version = HW_V2;
			//pinMode(PIN_EXT_VREF_AD, INPUT); // ext Vref at PIO0.11
			pinMode(PIN_EXT_VREF_AD, INPUT_ANALOG);
		} else
			//HWv1
			HW_Version = HW_V1;
	}

#ifdef DEBUG
#if defined(INCLUDE_SERIAL)
	// re-enable the serial port as we might have changed the config by reading ID1/2
	// the with 115200 baud, no parity, 1 stop bit
	//serial pins are defined in libconfig.h based on HW and config
	serial.setRxPin(SERIAL_RX_PIN);
	serial.setTxPin(SERIAL_TX_PIN);
	serial.begin(115200, SERIAL_8N1);
	serial.println(" Debug Started");
	serial.println(" main HW version: ", HW_Version);
#endif
#endif

#endif

	int pin_ad;
	int v_run_cnt, v_reset_cnt;
	if (HW_Version >= HW_V1)
	{
		v_run_cnt = (( VoltageRunmV * 10000)
				/ (ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_HWV1));
		v_reset_cnt = (( VoltageIdlemV * 10000)
				/ (ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_HWV1));
	} else {
		v_run_cnt = (( VoltageRunmV * 10000)
				/ (ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_DEFAULT));
		v_reset_cnt = (( VoltageIdlemV * 10000)
				/ (ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_DEFAULT));
	}
	DB_BUSVOLTAGE(
			serial.print( "*** Vbus run cnt:"); serial.print (v_run_cnt); serial.print( ", Vbus reset/idle cnt:"); serial.println (v_reset_cnt););


// wait until the bus voltage has reached the normal operation level
#ifdef __LPC11XX_OM13087__
	pinMode(PIO0_11, INPUT_ANALOG); // AD0 @ PIO0.11
	pin_ad =AD0;
#else
	pinMode(PIN_VBUS_AD, INPUT_ANALOG); // AD7 @ PIO1.11
	pin_ad = VBUS_ADC;
#endif

	analogBegin();
	int vb, vbc = 0;
	pinMode(PIN_RUN, OUTPUT);
	pinMode(PIN_INFO, OUTPUT);
	digitalWrite(PIN_RUN, 1);
	digitalWrite(PIN_INFO, 0);

	do { /// read the adc few time to remove any previous noise
		vbc = analogValidRead(pin_ad);
		DB_BUSVOLTAGE(serial.print( "*** adc vb cnt0:"); serial.print (vbc););
		vbc = analogValidRead(pin_ad);
		DB_BUSVOLTAGE(serial.print( ", adc vb cnt00:"); serial.print (vbc););
		delay(1000); // wait for MCU supply voltage setup to about nominal and adc analog part setup

		vbc = analogValidRead(pin_ad);
		DB_BUSVOLTAGE(serial.print( ",  adc vb cnt01:"); serial.println (vbc););
		if (vbc < 0) {
			//analog read failure. Stop BCU and blink the RUN and Info LED
			do {
				DB_BUSVOLTAGE(
						serial.print( " adc Vbus failure cnt:"); serial.print (vbc););
				delay(200);
				digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));
				digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));
				vbc = analogRead(pin_ad);
				DB_BUSVOLTAGE(
						serial.print( " adc Vbus failure cnt:"); serial.print (vbc););

			} while (vbc < 0);
		} else { // bus voltage not at run level-> wait and blink run led only
			delay(200);
			digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));
			//digitalWrite(PIN_iNFO, !digitalRead(PIN_INFO));
		}
		DB_BUSVOLTAGE(
				if (HW_Version != HW_DEFAULT) {vb = (vbc * ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_HWV1) /10000;
				} else
				{ // we use the default ratio for the resistor divider on the analog input with 50uA leakage current of D2
					vb = (vbc * ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_DEFAULT) /10000;}
				serial.print( " wait for Vbus cnt:"); serial.print(vbc); serial.print( ", Vbus mV:");
				serial.print (vb););

		// analog measurement at Vcc<Vnom (Vbus<18V and unmodified Ven of BD9G101) is not valid as LSB value is undefined
		// reset works correct only if R18 is e.g. <=330k
		DB_BUSVOLTAGE(
				serial.println( " adc rest cnt:"); serial.print (v_reset_cnt););
		if (vbc <= v_reset_cnt) // we do a reset in case Vbus is below idle level (18V)
				{
			DB_BUSVOLTAGE(

			serial.println( " adc rest cnt:"); serial.print (v_reset_cnt););
			delay(1000); // print buffer before we reset
			NVIC_SystemReset(); //should never return
		}
	} while (vbc <= v_run_cnt);
	digitalWrite(PIN_RUN, 1);  // indicate run state
	digitalWrite(PIN_INFO, 1);

	analogEnd();
}

/**
 * @brief The main of the Selfbus library.
 *        Calls setup(), loop() and optional loop_noapp() from the application.
 *
 * @return will never return
 */
int main() {
	lib_setup();
#ifdef DEBUG
#if defined(INCLUDE_SERIAL)
	serial.setRxPin(SERIAL_RX_PIN);
	serial.setTxPin(SERIAL_TX_PIN);
	serial.begin(115200, SERIAL_8N1);
	serial.println("Main setup");
#endif
#endif

	wait_for_run_level_voltage();
	BcuBase *bcu = setup();

	while (1) {
		bcu->loop();
		if (bcu->applicationRunning())
			loop();
		else
			loop_noapp();
	}
}
