/*
 *  bus_voltage.cpp
 *
 *  Copyright (c) 2020 Darthyson <darth@maptrack.de>
 *
 *  last changes: Nov. 2024 Horst Rauch
 *                based on the BCU HW LPC1115 v1.02
 *                changes are made according to the remarks below
 *                bus voltage check and monitor moved to sblib as subclass of
 *                the BCU_default class
 *
 *
 *  **** General remarks, requirements and some generic functions for BCU startup and shutdown as well as Bus Voltage measurement ****
 *
 *  According to the KNX specification the BCU shall only draw leakage current in case Vbus is <=18v. On power up the BCU/SMPS should start
 *  operating at Vbus >18V, BCU (the LIB main function) should initialize all LIB functions and wait for Vbus >=21V before
 *  initializing the App/ trigger the UsrInitPtr and start normal operation (calling repeatedly the App/UsrPrgPtr.
 *  The BCU shall trigger the UsrSavePtr in case the Vbus is falling below 20V.
 *  Specifically for the BCU1 the ADC on the unit has typically several channels and a resolution of 8bit with 150mV LBS at the API/Bus interface.
 *
 *  For specific use in an App we could internally operate with different values but any Bus activity should be stopped below Vbus<=20V
 *
 *
 * *** Specifics based on the HW implementation ****
 *
 *  Based on the SMPS design on the 4TE v1.02 BCU board and the BD9G101, the accuracy of the ref voltages of the BD9G101, 1% resistors,
 *  the Ri ~130Ohm of L2 (~1,2V @8,5mA of idle current of the BCU board) and  Ven (BD9G101) of about 1.5V (resulting in a level of Vcc/+3V3 ~=1.6V,
 *  according to spec Ven should be ~1,3V) we have:
 *    - SMPS start of operation with  Vcc/+3v3 ~=1,6V @ EIB_DC ~>= 16,5V -> Vbus ~= 18,2V and stop operation if EIB_DC is below 16,3V -> Vbus~18V
 *      based on R18=390k, R14=43k. The nominal Vcc/+3V3 of ~=3,3V was delivered @ EIB_DC >=16,8V -> Vbus >=18,5V. Below that, the ADC measurement
 *      is invalid as the reference voltage (supply of the MCU) is changing in an undefined behavior as the start of the SMPS is related to
 *      the Ven Pin of the BD9g101 and the soft start up function (~2ms rise of Vout). Operation start at about Ven ~1,3V with Vout < Vnom and
 *      Vnom is reached at Ven~1,6V. In this window of Vcc rise during start up, no ADC measurement is of any value as the Vref of the ADC is undefined.
 *
 *    - VCC accuracy of +-3,5%, Vccmax~3,52V, Vccmin~3,278V, Vccnom~3,397V
 *
 *
 * *** Some ideas on relay boards ***
 *
 *  To ensure reliable operation of all relais in an application it is proposed to use only relais with Un <=18V and
 *  Relais PWM supply:
 *   Omron G5Q typ 30%Un (SPST, Ri=720ohm -> Un=12V,In=16mA) and 40%Un (SPDT, Ri=360ohm, Un=12V, In=32mA)
 *   holding voltage after 100ms activation puls with Un. With min Vbus=21V Dstart= 12/21=0,57, Dhold= 12* 0,3/21=0,17
 *   respectivly Dhold+ 12* 0,4/21=0,23. Switching frequency should be 10kHz. To reduce the power we could measure the Vbus
 *   and adapt D accordingly for activation/hold.
 *
 *   Hongfa HFE20 - to be calculated based on Spec, Ri and inductance, Spec: min50ms activation at >=Un.
 *   Usage of single coil relay reduces the power from 600mW to 400mW ( H-bridge driver needed).
 *   To reduce the capacitors/Energy storage for switching the relays during power down phase we could add on the energy board a boost SMPS
 *   working down to a few volts and providing 12V output for the relays.
 *
 *
 * *** some thoughts on possible implementation ***
 *
 *   In order to avoid any false measurement due to Vcc rise during start up, we delay MCU/BCU-setup at the main() by >100ms. In addition it looks
 *   like that after a MCU reset the ADC HW need some time to setup after being enabled first time. We give the ADC after enabling for the first
 *   measurement about >500ms setup time.
 *
 *   As we have an absolute error of the MCU-ADC of about +-4LSB according to spec and the +-3,5% error on the Ref voltage
 *   we have an accuracy of less than +-100mV or less than 6bit resolution.
 *
 *   In order to reduce cpu processing power and code space for the calculations we normalize all calculations to the LSB.
 *   All calculations will be done with integer values only (based on the ADC read out).
 *   For bus voltage monitoring/measurements,  we do not need the real voltage representation in the SW,
 *   we calculate based on the the LSB values and let the compiler calculate the number of LSB representing a dedicated Voltage level.

 *  The remaining processing time for the MCU when the VBus is below the specified Vbus=20V and the stop of operation of the SMPS below 18V
 *  is about ~110ms-80ms with the C6=100uF and Iload=10mA. In case long time before shut down is needed an increse of C6 and/or reduction of R-Divider
 *  for Ven could be implemented.
 *
 *  todo: In addition we could use the definition of the power supply slope (KNX spec) to calculate a bus failure :
 *    A normal Bus power supply should drop the bus voltage by less than 5% during a 100ms
 *    main power outage. So we could use that slope (<= 14V/s) as orientation and anything
 *    far above that is probably an fault.
 *
 *  ADC HW basics for (Bus) voltage measurement:
 *   The input pin should be defined as analog pin.
 *   Read the BUS Voltage at the Analog PIN => VADC= (Vbus - Uf(D1) ) * R12/(R9+R2+R3+R12) ~= (Vbus-0,6V) / 10,3. Due to the leakage current of the z-diode
 *   we have VADC = (Vbus-0,6V) / 11,4 (at Vbus 20V-21V).
 *
 *   The corner frequency of the RC element is ~ R12||(R9+R2+R3) * C20 = 175Hz, so we do not need to sample the voltage at a higher frequency.
 *
 *   For bus voltage measurements it is proposed to measure at an interval close to the time of the input filter (~5,6ms) e.g. 10ms.
 *   In addition a SW filter/averaging could be used e.g. FIR filter with n=4, resulting in about max measurement bandwidth of 25Hz (40ms).
 *
 *   To avoid this SW overhead for filtering a much simpler way would be to increase C20 to 470nF - 680nF and add this to HW modification V1.
 *
 *   In order to avoid major leakage current trough the zener diode ( ~30uA @2,5V)
 *   we could replace that with a shottky diode to +3V3 to cut high voltage peaks at the analog input or replace R3 with 130k as the TVS
 *   is clamping voltages above ~45V. An additional diode to GND is not needed as neg. pulse should not occur.
 *   In case we leave the Z-diode, we need to calculate in the observed Vbus range of 18V-21V with a resistor division ration of about 11,4.
 *
 *   To give the SW a possibility to find out about the installed HW (e.g. changes of the z-Diode etc),
 *   we could use
 *   - the App-Sel input (IO12-AD5 e.g. used on the relais boards) e.g.
 *    	- resistor to GND  -> version 1, no Z-Diode
 *    	- resistor to Vdd -> version 2, extVref
 *    	- open            -> verson 3
 *   	to identify an open pin:  configure the pin as input with pull-up/down and measure the logical level or the analog voltage, Ri-up > ~40k, Ri-down > ~22k).
 *
 *   - or the IDx pins on the board (even in parallel to a serial usage of these ports if we use a 680R resistor to GND to avoid any output shortage)
 *		- In debug ID2 might be used as TX output, we can identify if a TX receiver is connected -> high level when configured as input with PU
 *        or the pin is pulled low -> HWv1 or HWv2. In case ID2 is low, than no TX/RX is connected and we check ID1 as well:
 *		  	ID1			high		high	low
 * 			ID2			high		low 	low
 * 			HW version	default		v1		v2
 *
 * HW default: original z-diode D2 3v6 -> resistor division ratio :1/114
 * HWv1		 : replace D2 with shottky BAT86 (Ir ~0.1uA @Vr=5V) to Vcc -> resistor division ratio :103/1000, C20 increase to 470nf - 680nF
 * HWv2      : v1 and  ext. Vref of 2.5V at AD4/PIO1_3/SWDIO, AD5/APP_SEL or AD0/LT8
 *
 *
 *  A VBus failure could be triggered (call UsrSavePtr or respective App-callback):
 *    1)slope >= 25V/s (250mV/10ms is above the error level of +-100mV) together with mean value Vbus <=20V(last Vbus measurement is 20V or below)
 *    2)or we have a mean value Vbus < 20V
 *
 *   The measurement could be timer triggered or based on the system timer in the main BCU loop or in the App loop.
 *   An active data transmission on the Bus will probably change measurement value by ~-1,5% of the actual VBus as
 *   we have the RC filter which has about 30dB damping for the 9,6kHz of the data signal. So we can ignore the bus state.
 *
 *
 *  In case we need longer time to save some user data and switch some relays we could reduce R18 to 270k (or 240K)
 *  which gives us a start/Stop of the SMPS at ~12V. The BCU main function should stop any Bus activity at VBus < 20V and
 *  initialize the App at Vbus >= 21V. In addition, the ADC measurements would be valid  @ Vbus ~>=13V. As long as EIB_DC would be lager
 *  than the Vbus, this would be KNX spec conform as D3 would block any drain of bus current. -> Could be monitored in the SW of the energy storage
 *
 *
 *  Proposal for some HW modifications in order to have a more precise ADC read out and Power-up/power-down phase:
 *  replace R18 with 270k and D2 with shottky diode (e.g. BAT41) towards Vcc to clamp the max analog level to Vcc.
 *
 *  In addition we could add an external Vref source e.g. LM4040A2.5 to increase the accuracy of the ADC measurements e.g. on APP_Sel, LT8 or SWDIO which
 *  are not used in normal operation(todo !!!!!need to be checked). For the Vbus measurement the ext Vref is not necessary as an absolut measurments
 *  of +- few 100mV is non critical.
 *
 *  To let the SW find out about the HW modification, the ID1/ID2 PIO is used ( or we cold use the APP_SEL IO12 Pin):
 *  - open: use normal HW version
 *  - connected to GND: use modified HW version
 *  IN bcu_default.begin the PIO is configured as input with pull up and the level is measured.
 *  If connected to a RX input device we would measure a high level (need to be confirmed by check)
 *
 *
 *
 * *** implementation ***
 *
 * The function Bus voltage monitoring is moved to the sbLIB as function of the BCU in the bcu_default class and in the main() function.
 * During start up procedure in main() (after HW reset or Power-up) we wait until the Bus voltage has reached the Run level (>=21V) before
 * we initialize the BCU and APP.
 *
 * According to KNX spec:
 *  1. initialize basic system/HW : charge capacitors etc.  -- we do that in main() ( all  functions e.g. system tick, Libc ...) and wait for Run level
 *  2. if Bus voltage is >=18V initialize remaining BCU functions and KNX bus interface+++ is not possible as we have the setup in the APP so we do
 *     that in the last step and after that the BCU gets initialized
 *  3. if Bus voltage is >21V initialize  APP and BCU by call to setup in the App
 *
 *  -> In case of Bus failure:
 *  1. if bus voltage is < 20V send signal to App  (e.g. UsrPTRSave)in order to store volatile data
 *     and let the App switch some relays to respective state and stop bus activity/BCU and reset the MCU.
 *     If 18V < Vbus < 21V MCU will remain in a reset loop.
 *  2. If the bus Voltage is below the value of correct operation for the SMPS ( ~<18V) the BCU will draw only some leakage current
 *     put all App activities into reset/sleeping state
 *  3. If Vbus >21V the BCU will leave the reset loop and start operation, main() will initialize all SW
 *
 * -> during start up:
 *  1. If Vbus < 18V, SMPS is disabled and no Vcc is applied.
 *  2. If VBus >18V, SMPS operation starts, Vcc is rising and the MCU starts operation if Vcc >1,8V. We wait some time in main() to let Vcc rise and reach
 *     its normal level before we measure the Vbus level. The BCU lib SW ( main() ) is waiting for Vbus >21V before remaining SW
 *     of BCU and App is initialized and normal operation begin.
 *
 * Two callbacks provide an interface for the lib to the App:
 *  1. Notification on bus voltage failure (<21V). App SW should return to let the BCU/lib shut down and wait for Vbus runlevel again.
 *  2. ADC measurement as an App function in case the adc function of the lib would disturb any app related measurement. The App adc function should
 *     measure the Vbus level at an interval based on the corner frequency of the RC filter and return the last measurement in the callback.
 *
 *  For the HW modification identification we implement the ID1/2 pin as option input and let the lib-SW set the environment accordingly.
 *  The intervall for the Vbus measurement is set to 10ms at HW-DEFAULT, and 40ms for HWV1 or HWV2 (C20=680nF)
 *  Sw filtering/averaging can be used by respective Define.
 *
 ******************************************************************************************************
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/bus_voltage.h>
#include <sblib/eib/bus_voltage_level.h>
#include <sblib/core.h>
#include <sblib/main.h>
#include <sblib/interrupt.h>
#include <sblib/io_pin_names.h>
#include <sblib/digital_pin.h>
#include <sblib/analog_pin.h>

//#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
//#endif

BusVoltage::BusVoltage() :
		ptr_appadcreadCallback(nullptr) {
}

/**
 * Set callback for user application ADC read function
 */
void BusVoltage::setAppADCReadCallback(AppADCReadCallback *callback) {
	ptr_appadcreadCallback = callback;
}

/**
 *  Setup Bus Voltage Monitoring.
 *
 * Check HW version based on ID1 and ID2 pins set in main()
 * and set the LSB value, thresholds and resistor divider ratio accordingly
 *
 * We use IO pins which are possibly used in debug sessions only
 * most normal operations should not used these IO pins
 *
 * In debug ID1 might be used as TX, we can identify if an RX receiver
 * is connected -> high level when configured as input with PU
 * or the pin is pulled low -> HWv1 or HWv2
 * In case ID1 is low, than no TX/RX is connected and we check ID2 as well
 *
 * HW default: original z-diode D2 3v6
 * HWv1		 : replace D2 with shottky BAT81 to Vcc
 * HWv2      : v1 plus Vref of 2.5V at AD4/PIO1_3/SWDIO (AD0 on LT8 or AD5, APP_SEL pin)
 *
 * ID1			high		high	low
 * ID2			high		low		low
 * HW version	default		v1		v2
 *
 * Based on HW we calculate ADC Count-value threshold below which a bus
 * failure/return should be reported, todo we could mask the low order bits based on the accuracy
 * We initially fill the filter buffer with real measurements to avoid false results within the first few
 * bus voltage checks
 *
 * @return non-zero if successful.
 *
 *
 */
unsigned int BusVoltage::setup() {

	int error = 1;
#if defined (FILTER)
	BusVoltage::filter_init();
#endif
	
	adc_LSB_uV = ADC_LSB_UV; // set default LSB value in uV

	if (HW_Version == HW_V2) {
		if ((error = BusVoltage::valueLSBuV()) >= 0) {
			adc_LSB_uV = error;
		}
	}

	if (HW_Version >= HW_V1) {
		// we calculate  the voltage levels without leakage current of D2, adc_LSB_uV has default value
		_thresholdADCCountFailed = (( VoltageFailedmV * 10000)
				/ (adc_LSB_uV * VBUS_VOLTAGEDIVIDER_HWV1));
		_thresholdADCCountReturn = (( VoltageRunmV * 10000)
				/ (adc_LSB_uV * VBUS_VOLTAGEDIVIDER_HWV1));
		_thresholdADCCountIdle = (( VoltageIdlemV * 10000)
				/ (adc_LSB_uV * VBUS_VOLTAGEDIVIDER_HWV1));
		bus_measurement_interval = BUS_VOLTAGE_MEASUREMENT_INTERVAL_HWV1;

		DB_BUSVOLTAGE(serial.println(" *** HWV1/2 ***  ")
		; );
	} else // default
	{
		_thresholdADCCountFailed = (( VoltageFailedmV * 10000)
				/ (ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_DEFAULT));
		_thresholdADCCountReturn = (( VoltageRunmV * 10000)
				/ (ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_DEFAULT));
		_thresholdADCCountIdle = (( VoltageIdlemV * 10000)
				/ (ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_DEFAULT));
		bus_measurement_interval = BUS_VOLTAGE_MEASUREMENT_INTERVAL;

		//		#define ADC_TO_VBUS_MV(adccount )  (adccount * (adc_LSB_uV * VBUS_VOLTAGEDIVIDER_DEFAULT))/10000
	}

	DB_BUSVOLTAGE(
			serial.print("ADC cnt Failed :", _thresholdADCCountFailed); serial.print(", ADC cnt Run :", _thresholdADCCountReturn); serial.print(", ADC cnt Idle :", _thresholdADCCountIdle); serial.println(", ADC LSBuV :", adc_LSB_uV););

#if defined (FILTER)
	// fill the filter buffer with current Vbus values;
	for (int i = 0; i < FILTER_SIZE; i++)
		BusVoltage::measureBusVoltage(VBUS_ADC);
#endif

	return error;
}

#if defined (FILTER)
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
 **/
/**
 * Here we initialise our filter. In this case we only set the
 * initial position and set all buffer values to zero.
 */
void BusVoltage::filter_init() {
	register int i;
	DB_BUSVOLTAGE(serial.println("filter init")
	; );

	filter_sum = 0;
	filter_position = &filter_buffer[0];
	// alternatively memset(filter_buffer, 0, sizeof(filter_buffer) * sizeof(unsigned int))
	for (i = 0; i < FILTER_SIZE; i++)
		filter_buffer[i] = 0;
}

/**
 * This function is called every cycle. It substracts the oldest value from
 * the sum, adds the new value to the sum, overwrites the oldest value with the
 * new value and increments the ring buffer pointer (with rewind on overflow).
 *
 * @param unsigned int new _value
 * @return unsigned int
 */
unsigned int BusVoltage::filter(unsigned int new_value) {
	/* DB_BUSVOLTAGE(
	 serial.print("filter position: ", filter_position);
	 serial.print(" value filter position: ", *filter_position);
	 serial.print(",  filter sum: ", filter_sum);
	 serial.print(", new value: ", new_value);
	 serial.print (", filter values 0: ",filter_buffer[0]);
	 serial.print (", 2: ",filter_buffer[2]);
	 serial.println (", 3: ",filter_buffer[3]);
	 );
	 */
	// Substract oldest value from the sum
	filter_sum -= *filter_position;
	// Update ring buffer (overwrite oldest value with new one)
	*filter_position = new_value;

	// Add new value to the sum
	filter_sum += new_value;

	// Advance the buffer write position, rewind to the beginning if needed.
	if (++filter_position >= filter_buffer + FILTER_SIZE) {
		filter_position = filter_buffer;
	}

	// Return sum divided by FILTER_SIZE, which is faster done by right shifting
	// The size of the ring buffer in bits. ( filter_sum / 2^bits ).
	//
	/*
	 DB_BUSVOLTAGE(
	 serial.print("filter sum+: ", filter_sum);
	 serial.println(",  filter sum/4: ", filter_sum >> 2);
	 );
	 */

	return (filter_sum >> (FILTER_SIZE_IN_BITS));
}
#endif

/*
 * measure the current bus voltage and check against the different levels
 * check the measured bus voltage save and return the current level indicator
 *
 * For simple level indication of >= run level (>21V) or < failure level (20V)
 * we use simple comparison, no edge (rising/falling)... indication
 *
 * all calculations are based on the LSB
 *
 * @return enum BusVoltage
 *
 */
enum BusVoltage::OPERATION_STATE BusVoltage::CheckBusVoltage(void) {
	// read the bus voltage by bcu function or a user adc function
	static int lastADCCount;
	enum OPERATION_STATE _operationState = POWER_DOWN_IDLE;

	int adcCount = BusVoltage::measureBusVoltage(VBUS_ADC);

	//check status
	if (adcCount < 0) {
		_operationState = ADC_ERROR;
		State = UNKNOWN;
	} else {
#if defined (OP_STATES)
		// check for bus voltage failure situation
		//Voltage falling or rising?
		if (adcCount < lastADCCount) {
			State = FALLING;
			if (adcCount < _thresholdADCCountFailed) {
				State = FAILED;
				if (adcCount < _thresholdADCCountIdle) {
					// send a reset to App and BCU - should not happen ad the SMPS/MCU  stopped working
					_operationState = POWER_DOWN_IDLE;
				} else {
					// we are in power down/hold-up phase, send usr-Save to the App in order to save volatile data and switch relays accordingly
					_operationState = POWER_DOWN_HOLD_UP;
				}
			} else // above failing -  but still on run level
			{
				_operationState = POWER_UP_OPERATION;
				State = OK;
			}
		} else if (adcCount > lastADCCount) {
			State = RISING;
			if (adcCount >= _thresholdADCCountIdle) {
				if (adcCount >= _thresholdADCCountReturn) {
					// init all remaining SW and App for normal operation
					_operationState = POWER_UP_OPERATION;
					State = OK;
				} else {
					// we are in power up/start-up phase,  int all BCU SW  and wait for normal operation
					_operationState = POWER_UP_START_UP;
				}
			} else {
				// below idle
				_operationState = POWER_DOWN_IDLE;

			}
		} else  // no change in voltage
		{
			if (adcCount > _thresholdADCCountReturn) {
				_operationState = POWER_UP_OPERATION;
				State = OK;
			} else {
				_operationState = POWER_DOWN_HOLD_UP;
				State = FALLING;

			}
		}
#else

		// simple check for bus voltage failure situation
		//Voltage falling or rising?
		if (adcCount < lastADCCount) {
			State = FALLING;
			if (adcCount < _thresholdADCCountFailed) { // Vbus <20v
				State = FAILED;
				// we are in power down/hold-up phase
				_operationState = POWER_DOWN_HOLD_UP;

			} else if (adcCount >= _thresholdADCCountReturn) { //Vbus >21V
				// init all remaining SW and App for normal operation
				_operationState = POWER_UP_OPERATION;
				State = OK;
			} else {
				//  20V < Vbus < 21V,  we are in power down operation phase
				_operationState = POWER_DOWN_OPERATION;
			}
		} else {
			if (adcCount >= _thresholdADCCountReturn) {
				_operationState = POWER_UP_OPERATION;
				State = OK;
			} else {
				_operationState = POWER_DOWN_OPERATION;
				//_operationState = POWER_DOWN_HOLD_UP;
				State = RISING;

			}
		}
		lastADCCount = adcCount;
		operationState = _operationState;
		DB_BUSVOLTAGE(serial.println("Power operation state: ", _operationState)
		; );

	}
#endif
	return _operationState;
}

/*
 * Measure the bus voltage
 *
 * Enable the ADC, start measurement, filter the value and stop ADC
 * In case we provided an ext Vref in the HW, we could
 * measure that and correct the LSB value as the VCC will probably change
 * after start up and during normal operation. But this is not really need
 * for bus voltage measurment.
 *
 * @param unsigned int  ADC channel number AD0 - AD7
 * @return int filtered ADC count value, -1 on error, -2 on wrong AD channel
 *
 */
int BusVoltage::measureBusVoltage(unsigned int ADCChannel) {
	// get IO Pin from channel and set to analog mode
	// enable analog measurement and do analog measurement poll
	//disable analog measurement
#if defined (FILTER)
		int filtered_adccount;
#endif

	if (ADCChannel > AD7 || ADCChannel < AD0)
		return -2;
	pinMode(ADCtoPIO(ADCChannel), INPUT_ANALOG); // Vbus typ at AD7 @ PIO1.11
	DB_BUSVOLTAGE(serial.println(); serial.print("BCU measure busvoltage: "););

	if (ptr_appadcreadCallback != nullptr) { //use user adc class
		DB_BUSVOLTAGE(serial.println(" use app ADC function"););
		adccount = ptr_appadcreadCallback->ReadADCCount(ADCChannel);
	} else // use default adc  functions
	{
		DB_BUSVOLTAGE(serial.println(" use BCU ADC function")
		;);
		analogBegin();
		adccount = analogValidRead(ADCChannel);
		analogEnd();
	}
	DB_BUSVOLTAGE(
			serial.print("ADC channel: ", ADCChannel); serial.print (", adccount :", adccount);)
#if defined (FILTER)
		if (adccount >= 0)
		filtered_adccount = filter(adccount); //filter only valid measurements
		DB_BUSVOLTAGE(
				serial.print(", filter ADC count: ", filtered_adccount);
		)
#endif

	DB_BUSVOLTAGE(
			serial.print(",  ADC LSBuV: ", adc_LSB_uV); serial.println(", last ADC measurement -> Vbus mV :", BusVoltage::valueBusVoltagemV()););

#if defined (FILTER)
		return filtered_adccount;
#else
	return adccount;
#endif
}

/*
 *  returns the last non filtered measurement of bus voltage in mV, returns -1 in case of invalid measurement
 */
int BusVoltage::valueBusVoltagemV() {
	if (adccount < 1)
		return -1;
	else {
		if (HW_Version == HW_DEFAULT)
			return ((adccount * adc_LSB_uV) * VBUS_VOLTAGEDIVIDER_DEFAULT
					/ 10000) + 600;  // return value in mV
		else
			return ((adccount * adc_LSB_uV) * VBUS_VOLTAGEDIVIDER_HWV1 / 10000)
					+ 600;  // return value in mV
	}
}

/**
 * Return the current LSB value in uV
 *
 * If we have an ext Vref, we recalculate the LSB and store it in the appropriate
 * parameter
 *
 * @return: value LSB in uV or neg if measurement failure
 *
 */
int BusVoltage::valueLSBuV() {
	if (HW_Version == HW_V2) {
		// we need to calculate the VCC and LSB value based on the external Vref at ADx:
		// Vcc_uV = 2500000 *1024 / ADC-cnt in uV
		// ADC_LSB_UV : Vcc_uV/1024 = 2500000/ ADC-cnt
		pinMode(PIN_EXT_VREF_AD, INPUT_ANALOG); // Vbus typ at AD7 @ PIO1.11

		analogBegin();
		pinMode(PIO0_11, INPUT_ANALOG); // ext Vref:  todo AD1 @ PIO1.0, pin IO9 at SV2 or AD0 @ PIO0_11, pin LT8 at SV1
		int vbc = analogValidRead(EXTVREF_ADC);
		analogEnd();
		if (vbc < 0)
			return (-1);
				
		adc_LSB_uV = EXT_VREF_UV / vbc;
		DB_BUSVOLTAGE(
				serial.print(" HWV2- adc setup for vref measurement: "); serial.print(" adc cnt:", vbc); serial.println(", ADC LSBuV:", adc_LSB_uV););
	}
	else
		adc_LSB_uV = ADC_LSB_UV;
	
	return (adc_LSB_uV);
}

bool BusVoltage::busFailed() {
	return (State != BusVoltage::OK);
}

//BusVoltage busVoltageMonitor = BusVoltage();
