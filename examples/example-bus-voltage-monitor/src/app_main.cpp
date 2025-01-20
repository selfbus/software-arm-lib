/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_bus_voltage-monitor
 * @ingroup SBLIB_EXAMPLES
 * @brief   Read the analog channel AD7 and print the read value to the serial port.
 * @details A simple application that reads the analog channel of vBus and
 *          prints the read value to the serial port. In addition it uses the callback
 *          for lib and provides functions for the Vbus failure notification
 *          and analog read on app level<br/>
 *          The serial port is used with 115200 baud, 8 data bits, no parity, 1 stop bit.
 *          Tx-pin is PIO1.7, Rx-pin is PIO1.6<br />
 *
 * @
 *
 * @file   app_main.cpp
 * @author Horst Rauch Copyright (c) 2025
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

//#include <sblib/eibBCU1.h>
#include <sblib/eibBCU2.h>
#include <sblib/analog_pin.h>
#include <sblib/serial.h>
#include <sblib/eib/bus_voltage.h>



//BCU1 bcu = BCU1();
BCU2 bcu = BCU2();
#define DEBUG_SERIAL

class AppADCReadCallback appadcreadcallback;
class AppUserSaveCallback appusersavecallback;
class Timeout Printtimeout;

int vADC_LSB_UV = ADC_LSB_UV;



/**
 * App adc read callback function
 * @param : ADC channel
 * @return: ADC read count
 *
 */
int AppADCReadCallback::ReadADCCount(unsigned int ADCchannel)
{
	DB_BUSVOLTAGE(
			serial.println ("user adc read channel :", ADCchannel););

	analogBegin();
	int adccount = analogValidRead(ADCchannel);
	analogEnd();
	return adccount;
}

void AppUserSaveCallback::Notify(UsrCallbackType type)
{
	serial.println( "user save notify: ", (int)type);
	return;
}


void initSerial()
{
	// Enable the serial port with 115200 baud, no parity, 1 stop bit
	// serial pins are defined in libconfig.h based of HW and config
	// e.g. SERIAL_ON_SWD

#ifdef DEBUG_SERIAL
	serial.setRxPin(SERIAL_RX_PIN);
	serial.setTxPin(SERIAL_TX_PIN);
	serial.begin(115200, SERIAL_8N1);
	serial.println("Analog Test Debug Started");
#endif

}

/**
 * Initialize the application.
 */
BcuBase* setup()
{
	pinMode(PIN_EXT_VREF_AD, INPUT_ANALOG); //  analog IO pin or ext Vref to measure AD0 @ PIO0.11

#ifdef __LPC11XX_OM13087__
	pinMode(PIO1_0, INPUT_ANALOG); // AD1 @ PIO1.0
#else
	//todo check HW version
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          	pinMode(PIO1_11, INPUT_ANALOG); // Vbus: AD7 @ PIO1.11
#endif

	bcu.begin(2, 1, 1); // ABB, dummy something device
	//setup usersave callback
	//setup adcread callback
	bcu.setAppUserSaveCallback(( AppUserSaveCallback *) &appusersavecallback);

	bcu.setAppADCReadCallback((AppADCReadCallback*) &appadcreadcallback);

	initSerial();
	serial.println("Analog read example2:  Vbus measurement ");

	//set timeout for noloop action
	Printtimeout.stop();
	Printtimeout.start(10000);

	return (&bcu);
}


/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
	int value, cnt;
	if (Printtimeout.expired())
	{
		digitalWrite(PIN_RUN, 0);  // switch off the LEDs
		digitalWrite(PIN_INFO, 0);

		serial.println();
		serial.println(" *** App no loop *** ");
		analogBegin();

// check HW for Vbus measurement and ext Vref
// for the dev board we use ADo for ext Vref
#ifdef __LPC11XX_OM13087__
		value = analogRead(AD0);
		serial.print( " adc AD0 cnt:");
		serial.print (value);
		serial.println();
		serial.print( " adc AD0 mV :");
		value = (value * vADC_LSB_UV * 106)/10000;
		serial.print (value);
		serial.println();

		cnt = analogRead(AD1);
		value = cnt;
		serial.print( " adc AD1 cnt:");
		serial.print (value);
		serial.println();
		serial.print( " adc AD1 mV :");
		value = (value * vADC_LSB_UV )/1000;
		serial.print (value);
		serial.println();
		serial.print( " VCC mV :");
		value = (2500 * 1024)/ cnt;
		serial.print (value);
		serial.println();
		vADC_LSB_UV = (value *1000 )/1024;
#else
		pinMode(PIN_VBUS_AD , INPUT_ANALOG); // Vbus typ at AD7 @ PIO1.11
		value = analogValidRead(VBUS_ADC);
		serial.print( " adc AD7 cnt: ");
		serial.print (value);
		vADC_LSB_UV = bcu.valueLSBuV();
		serial.print( " vADC-LSB_uV: ");
		serial.print(vADC_LSB_UV);
		//      serial.println();
		serial.print( ", adc VBus mV: ");
		value = bcu.valueBusVoltagemV();
		serial.println (value);

		if (HW_Version == HW_V2)
		{
		pinMode(PIN_EXT_VREF_AD, INPUT_ANALOG); //ext Vref at  AD0 @ PIO0_11
		cnt = analogValidRead(EXTVREF_ADC);
		value = cnt;
		serial.print( " ***HW MOD***  ext Vref adc AD0 cnt: ");
		serial.print (value);
		vADC_LSB_UV = EXT_VREF_UV / cnt;
			serial.print(", calc ADC-LSB_uV : ");
		serial.print (vADC_LSB_UV);

		serial.print( ", VCC mV: ");
			value = (vADC_LSB_UV * 1024) / 1000;
		serial.print (value);
		//serial.println();
//		serial.println();
		} else if (HW_Version == HW_V1) {
			serial.print(" ***HW MOD***  ext Vref adc AD0 cnt: ");
			serial.print(", VCC mV: ");
			value = (vADC_LSB_UV * 1024) / 1000;
			serial.print(value);
		}
		
		analogEnd();
		serial.println(" *** App end *** ");
#endif
		Printtimeout.start (10000);
	}


}

/**
 * The main processing loop.
 */
void loop()
{
	// will never be called in this example
}





/** @}*/
