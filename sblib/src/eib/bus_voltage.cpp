/*
 *  bus_voltage.cpp
 *
 *  Copyright (c) 2020 Darthyson <darth@maptrack.de>
 *
 *  last changes: Nov. 2024 Horst Rauch
 *                based on the BCU HW LPC1115 v1.02
 *                changes are made according to the remarks below
 *
 *
 *  **** General remarks, requirements and some generic functions for BCU startup and shutdown as well as Bus Voltage measurement ****
 *
 *  According to the KNX specification the BCU shall only draw leakage current in case Vbus is <=18v. On power up the BCU/SMPS should start
 *  operating at Vbus >18V, BCU (the LIB main function) should initialize all LIB functions and wait for Vbus >=21V before
 *  initializing the App/ trigger the UsrInitPtr and start normal operation (calling repeatedly the App/UsrPrgPtr.
 *  The BCU shall trigger the UsrSavePtr in case the Vbus is below 20V.
 *  The ADC has typically several channels and on the BCU1 and a resolution of 150mV LBS at the API/Bus interface.
 *
 *  For specific use in an App we could internally operate with different values but any Bus activity should be stopp below Vbus<=20V
 *
 *
 *  Based on the SMPS design on the BCU board the BD9G101, the accuracy of the different voltages of the BD9G101 and 1% resistors we have:
 *    - SMPS start of operation if Vbus > 23,8V/21,5V and stop operation if VBus is below 12V/9V
 *    - VDD accuracy of +-3,5%, Vddmax~3,52V, VDDmin~3,278V, Vddnom~3,397V
 *
 *  The remaining processing time for the MCU when the VBus is below the specified Vbus=20V and the stop of operation of the SMPS
 *  is about ~110ms-80ms with the C6=100uF and Iload=10mA.
 *
 *  In addition we could use the definition of the power supply slope to calculate a bus failure :
 *    A normal Bus power supply should drop the bus voltage by less than 5% during a 100ms
 *    main power outage. So we use that slope (<= 14V/s) as orientation and anything
 *    far above that is probably an fault.
 *
 *  ADC HW basics for (Bus) voltage measurement:
 *   The input pin should be defined as analog pin  (
 *   Read the BUS Voltage at the Analog PIN => VADC= (Vbus - Uf(D1)) * R12/(R9+R2+R3+R12) ~= (Vbus-05V) / 10,3
 *   The corner frequency of the RC element is ~ R12||(R9+R2+R3) * C20 = 175Hz
 *   so we do not need to sample the voltage at higher frequency. In order to avoid major leakage current trough the zener diode
 *   we could repalce that with a shottky diode to +3V3 to cut high voltage peaks at the analog input or replace R3 with 130k as the TVS
 *   is clamping voltages above ~45V. An additional diode to GND is not needed as neg. pulse should not occur. In order to give the SW a possibility
 *   to find out about the installed HW, we could use the App-Sel input (IO12-AD5 e.g. used on the relais boards)
 *   e.g. resistor to GND  -> version 1, resistor to Vdd version2 (need to find a defined value for the open situation,
 *   e.g connfigure the pin as input with pull-up/down and measure the analog voltage, Ri-up > ~40k, Ri-down > ~22k).
 *
 *   As we have an absolute error of the MCU-ADC of about +-4LSB according to spec and the +-3,5% error on the Ref voltage
 *   we have an accuracy of less than +-100mV or less than 6bit resolution. In order to increase that by 1 additional bit we do an
 *   oversampling by 4: add four consecutive measurements and divide the result by 2 with a sliding window filter.
 *   All calculations will be done with integer values (based on the ADC read out) in order to limit the use of the MCU resources.
 *   As we do not need the real voltage representation in the SW, we calculate based on the the LSB values and let the compiler calculate the
 *   number of LSB representing a dedicated Voltage level.
 *
 *   A VBus failure should be triggered (call UsrSavePtr or respective App-callback):
 *    1)slope >= 25V/s (250mV/10ms is above the error level of +-100mV) together with mean value Vbus <=20,25V(last Vbus measurement is 20V or below)
 *    2)or we have a mean value Vbus < 20V
 *
 *   For bus voltage measurements it is proposed to measure every 10ms which results with the oversampling factor of four
 *   at a max measurement bandwidth of 25Hz.
 *   The measurement could be timer triggered or based on the system timer in the main BCU loop or in the App loop.
 *   An active data transmission on the Bus will probably change measurement value by ~-1,5% of the actual VBus as
 *   we have the RC filter which has about 30dB damping for the 9,6kHz of the data signal. So we can ignore the bus state.
 *
 *
 *  To achieve start-up/shut-down as per KNX spec 3.2.1, we could change value of R18 to 330k to stop the SMPS at 18V and below or stop any MCU activity to reduce
 *  the power usage by VBus measure. In case we need longer time to save some user data and switch some relais we could reduce
 *  R18 to 270k which gives us a start of the SMPS at ~18V and a stop at ~8,6V. The BCU main function should stop any Bus activity at VBus < 20V and
 *  initialize the App at Vbus > 21V.
 *
 *
 * The function Bus voltage monitoring is moved to the sbLIB as function of the BCU in the bcu_default class
 *  used during start up procedure: after HW reset or Power-up :
 *  1. initialize basic function and features of the BCU (e.g. system tick, GPIO,  basic Bus voltage monitoring,..)
 *  2. if Bus voltage is >=18V initialize remaining BCU functions and KNX bus interface
 *  3. if Bus voltage is >21V initialize APP
 *
 *  In case of Bus failure:
 *  1. if bus voltage is < 20V send signal to App  (e.g. UsrPTRSave)in order to store volatile data
 *     and let the App switch some relays to respective state
 *  2. if Bus voltaeg is < 18V stop all BCU and App activities. If the App needs more time and power to set some
 *     functions we could  let the App active until
 *  3. the bus Voltage is below the value of correct operation for the SMPS (  xmV above the switch off level of the BD9G101)
 *     put all App activities in to reset/sleeping state
 *
 *  Proposal for some HW modifications in order to have a more precise ADC read out and Power-up/power-down phase:
 *  replace R18 with 270k and D2 with shottky diode (e.g. BAT41) towards Vdd to clamp the max analog level to Vdd
 *
 *  To let the SW find out about the HW modification, the ID2 PIO is used:
 *  - open: use normal HW version
 *  - connected to GND: use modified HW version
 *  IN bcu_default.begin the PIO is configured as input with pull up and the level is measured.
 *  If connected to a RX input device we would measure a high level (need to be confirmed by check)
 *
 *
 *
 *  To ensure reliable operation of all relais in an application it is proposed to use only relais with Un <=18V and
 *  Relais PWM supply:
 *   Omron G5Q typ 30%Un (SPST, Ri=720ohm -> Un=12V,In=16mA) and 40%Un (SPDT, Ri=360ohm, Un=12V, In=32mA)
 *   holding voltage after 100ms activation puls with Un. With min Vbus=21V Dstart= 12/21=0,57, Dhold= 12* 0,3/21=0,17
 *   respectivly Dhold+ 12* 0,4/21=0,23. Switching frequency should be 10kHz. To reduce the power we could measure the Vbus
 *   and adapt D accordingly for activation/hold.
 *
 *   Hongfa HFE20 - to be calculated based on Spec, Ri and inductance, Spec: min50ms activation at >=Un
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/bus_voltage.h>

#include <sblib/interrupt.h>
#include <sblib/io_pin_names.h>
#include <sblib/digital_pin.h>
#include <sblib/analog_pin.h>

//#ifdef DEBUG_SERIAL
#include <sblib/serial.h>
//#endif

// from sblib/analog_pin.cpp
#define ADC_DONE 0x80000000      // ADC conversion complete
#define ADC_OVERRUN 0x40000000   // ADC overrun

/*
 * ADC clock will not influence the adc noise or accuracy, it has an impact on the ADC Ri :
 * Input resistance Ri depends on the sampling frequency fs: Ri = 1 / (fs  Cia).
 *
*/
//#define ADC_CLOCK 2400000        // Clock for AD conversion - max sample time: 4,5us (218kHz)
#define ADC_CLOCK 4400000        // max Clock for AD conversion -11 clock periods/conversion -> sample time: 2,5us (400ksamples/s)

/*
 * #define ADC_AD_SAMPLE_COUNT_MIN 100
 *  the impelmented SW is not an accurate average over 100 samples- the first value has a weighting of 1/(2exp 100), the last sample of 1/ (2exp 1)
 * we use a sliding window filter
 */
#define ADC_AD_SAMPLE_COUNT_MIN 4  // we use a sliding window average of 4


// not needed we do polling
// The interrupt handler for the ADC busVoltageMonitor object
//ADC_INTERRUPT_HANDLER(ADC_IRQHandler, busVoltageMonitor);




BusVoltage::BusVoltage()
: _ADChannel(AD7)
, _thresholdVoltageFailedmV(20000)
, _thresholdVoltageReturnmV(21000)
, _thresholdVoltageIdlemV (18000)
, _adcCallback(nullptr)
{
}

/**
 * setup Bus Voltage Monitoring.
 *
 * @param ADChannel AD-Channel correspondending to the ADC IO-Pin
 * @param thresholdVoltageFailed Threshold for bus failing in milli-voltage
 * @param thresholdVoltageReturn Threshold for bus returning in milli-voltage
 * @param callback instance of a sub-class of class BusVoltageCallback
 *
 * @return non-zero if successful.
 *
 */


unsigned int BusVoltage::setup( unsigned int ADChannel,
		unsigned int thresholdVoltageFailedmV, unsigned int thresholdVoltageReturnmV,
		unsigned int thresholdVoltageIdlemV, ADCCallback *callback)
{
	serial.println("busvoltage var setup");

	_ADChannel = ADChannel;
	_thresholdVoltageFailedmV = thresholdVoltageFailedmV;
	_thresholdVoltageReturnmV = thresholdVoltageReturnmV;
	_thresholdVoltageIdlemV   = thresholdVoltageIdlemV;

	_adcCallback = callback;


	if (_adcCallback != nullptr)
	{ //use user adc class
	}
	else // use default adc class
	{
	}
	return setup();
}

unsigned int BusVoltage::setup()
{
	filter_init();
	// ADC Count-value threshold below which a bus failure/return should be reported
	// we could mask the low order bits based on the accuracy
	serial.println("VoltagethresholdFailed :", _thresholdVoltageFailedmV);
	serial.println("VoltagethresholdReturn :", _thresholdVoltageReturnmV);
	serial.println("VoltagethresholdIdle :", _thresholdVoltageIdlemV);

	ADCCountthresholdFailed  = (((_thresholdVoltageFailedmV -500) *10000) / 103/ ADC_LSB_UV ); //& ADC_ACCURACY_MASK;(valuemV -500) *10000 )/103
	ADCCountthresholdReturn  = (((_thresholdVoltageReturnmV -500) *10000) / 103 / ADC_LSB_UV) ; // & ADC_ACCURACY_MASK;
	ADCCountthresholdIdle    = (((_thresholdVoltageIdlemV -500) *10000) / 103 / ADC_LSB_UV) ; // & ADC_ACCURACY_MASK;

	serial.println("iADCCountthresholdFailed :", ADCCountthresholdFailed);
	serial.println("ADCCountthresholdReturn :", ADCCountthresholdReturn);
	serial.println("ADCCountthresholdIdle :", ADCCountthresholdIdle);

	return 1;
}

/*
 *  sliding window filter
 *  original adapted to int values only
 *
 *  *** copy from https://atwillys.de/content/cc/digital-filters-in-c ****
 *
 * @file fir_filter_moving_avg8_uint.h
 * @author stfwi
 *
 * FIR FILTER EXAMPLE, MOVING AVERAGE IMPLEMENTATION
 *
 *
 *
 **/


/**
 * Here we initialise our filter. In this case we only set the
 * initial position and set all buffer values to zero.
 */
void BusVoltage::filter_init() {
	register int i;
	filter_sum = 0;
	filter_position = filter_buffer;
	// alternatively memset(filter_buffer, 0, sizeof(filter_buffer) * sizeof(unsigned int))
	for(i=0; i<FILTER_SIZE; i++) filter_buffer[i] = 0;
}

/**
 * This function is called every cycle. It adds substracts the oldest value from
 * the sum, adds the new value to the sum, overwrites the oldest value with the
 * new value and increments the ring buffer pointer (with rewind on overflow).
 *
 * @param unsigned int new _value
 * @return unsigned int
 */
unsigned int BusVoltage::filter( unsigned int new_value) {
	// Substract oldest value from the sum
	filter_sum -= *filter_position;

	// Update ring buffer (overwrite oldest value with new one)
	*filter_position =  new_value;

	// Add new value to the sum
	filter_sum +=  new_value;

	// Advance the buffer write position, rewind to the beginning if needed.
	if(++filter_position >= filter_buffer + FILTER_SIZE) {
		filter_position = filter_buffer;
	}

	// Return sum divided by FILTER_SIZE, which is faster done by right shifting
	// The size of the ring buffer in bits. ( filter_sum / 2^bits ).
	//
	return (filter_sum >> (FILTER_SIZE_IN_BITS ) );
}

/*
 *  measure the current bus voltage and check the against the different levels
 *
 * @param unsigned int  measured value based on LSB (ADC count value)
 * @return enum BUS_Voltage_STATE
 *
 */
enum BusVoltage::OPERATION_STATE BusVoltage::CheckBusVoltage(void)
{
	// read the bus voltage by bcu function or a user adc function
	int adcCount = BusVoltage::measureBusVoltage(PIN_VBUS_AD);


	//filter user supplied value

	//check status
	enum OPERATION_STATE operationState = BusVoltage::ValidBusVoltage(adcCount);

	return operationState;
}


/*
 * check the measured bus voltage and return the current level indicator
 *
 * @param unsigned int  measured value based on LSB (ADC count value)
 * @return enum BUS_Voltage_STATE
 *
 */
enum BusVoltage::OPERATION_STATE BusVoltage::ValidBusVoltage(unsigned int ADC_Count)
{
	serial.println("ADC count thrsfailed: ", ADCCountthresholdFailed);
	serial.println("ADC count thrsreturn: ", ADCCountthresholdReturn);

	static unsigned int lastADCCount;

	// check for bus voltage failure situation
	//Voltage falling or rising?
	if (ADC_Count < lastADCCount)
	{
		State = FALLING;
		if (ADC_Count < ADCCountthresholdFailed )
		{
			State = FAILED;
			if (ADC_Count < ADCCountthresholdIdle )
			{
				// send a reset to App and BCU
				operationState = POWER_DOWN_IDLE;
			}
			else
			{
				// we are in power down/hold-up phase, send usr-Save to the App in order to save volatile data and switch relays accordingly
				operationState = POWER_DOWN_HOLD_UP;
			}
		}
	} else if (ADC_Count > lastADCCount)
	{
		State = RISING;
		if (ADC_Count >= ADCCountthresholdIdle)
		{
			if (ADC_Count >= ADCCountthresholdReturn )
			{
				// init all remaining SW and App for normal operation
				operationState = POWER_UP_OPERATION;
				State = OK;
			}
			else
			{
				// we are in power up/start-up phase,  int all BCU SW  and wait for normal operation
				operationState = POWER_UP_START_UP;
			}
		}
	} else  // no change in voltage
	{
		if  (ADC_Count > ADCCountthresholdReturn)
		{
			operationState = POWER_UP_OPERATION;
			State = OK;
		}
	}

	serial.println("ADC failed0");
	serial.println("isr ADC end");

#ifdef DEBUG
	//	digitalWrite(PIN_INFO, 0);
#endif
	return operationState;
		}


/*
 * Measure the bus voltage
 * enable the ADC, start measurement, filter the value and stop ADC
 *
 * @param unsigned int  ADC channel number AD0 - AD7
 * @return int filtered ADC count value, -1 on error
 *
 */
int BusVoltage::measureBusVoltage(unsigned int ADCChannel)
{

	// get IO Pin from channel and set to analog mode
	// enable analog meas
	// do analog meas poll
	//disable analog meas

	pinMode(ADCtoPIO(ADCChannel), INPUT_ANALOG); // Vbus typ at AD7 @ PIO1.11
	serial.println("adcInt");

	analogBegin();
	adccount = analogValidRead(ADCChannel);
	analogEnd();

	serial.println("ADC count: ", adccount);
	adccount = filter(adccount);
	serial.println("filter ADC count: ", adccount);

	return adccount;
}


/*
 *  returns measured bus voltage in mV, returns -1 in case of invalid measurement
 */
int BusVoltage::valueBusVoltagemV()
{
	if (adccount < 1)
		return -1;
	else
	{
		return  (adccount * ADC_LSB_UV) / 1000;  // return value in mV
	}

}

int BusVoltage::valueBusVoltageADCCount()
{
	return adccount;
}

bool BusVoltage::busFailed()
{
	return (State != BusVoltage::OK);
}


int BusVoltage::filteredADCCount() // returns measured bus voltage in ADC counts ( (-1 if measurement is invalid)
{
	if (adccount < 1)
		return -1;
	else
	{
		return  (adccount);  // return value in mV
	}

}

BusVoltage busVoltageMonitor = BusVoltage();
