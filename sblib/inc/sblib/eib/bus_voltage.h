/*
 *  bus_voltage.h - simple ADC monitoring of the bus voltage on the AD7 of the LPCxx
 *
 *  Copyright (c) 2020 Darthyson <darth@maptrack.de>
 *
 *  last changes: Nov. 2024  Horst Rauch
 *
 *  @brief:  Bus voltage measurement and level check
 *           Is implemented as subclass of class BcuDefault
 *           Bus voltage measurement in polling mode, can be overridden by a user class AppADCReadCallback
 *           and callback function.
 *           Check for possible HW change on the analog part by ID1/2 level should be implemented in main()
 *           and set the LSB, threshold and resistor division ratio.
 *           Bus voltage monitoring function for BCU phase
 *                - power up
 *                - power down
 *                - normal operation added
 *
 *
 *      In case your application is using the MCU ADC you need to implement a class of AppADCReadCallback in you App.
 *      The BCU will call that function with an interval of 20ms to check the bus voltage. Your function should return
 *      as fast as possible the raw ADCcount value. In case of failure return a negative value.
 *
 *      In case you application requires a notification on a bus voltage failure you need to implement a class of AppUserSaveCallback.
 *
 *      Set the callback pointers in the BCU in the application setup function
 *
 *      e.g.:
 *      -------------------------------------------------------------------------------------------------------------------------------------

 #include <sblib/eib/bus_voltage.h>
 class AppADCReadCallback appadcreadcallback;
 class AppUserSaveCallback appusersavecallback;

 // read the adc value at ADCchannel
 int AppADCReadCallback::ReadADCCount(unsigned int ADCchannel)
 {
 analogBegin();
 int adccount = analogValidRead(ADCchannel);
 analogEnd();
 return adccount;
 }

 void AppUserSaveCallback::Notify(UsrCallbackType type)
 {
 // do wahever your app need to do in case of bus voltage failure and return as fast as possible

 return;
 }


 //Initialize the application.

 BcuBase* setup()
 {
 ... your code ....

 bcu.setAppUserSaveCallback(( AppUserSaveCallback *) &appusersavecallback);
 bcu.setAppADCReadCallback(( AppADCReadCallback *) &appadcreadcallback);
 }
 */

#ifndef BUS_VOLTAGE_H_
#define BUS_VOLTAGE_H_

#include <sblib/timer.h>
#include <sblib/ioports.h>
#include <sblib/io_pin_names.h>
#include <sblib/timer.h>
#include <sblib/analog_pin.h>
#include <sblib/usr_callback.h>
#include <sblib/eib/bus_voltage_level.h>

/*
 * MCU ADC and Vref accuracy is about +-4LSB, remaining accuracy is about 6bit
 * Based on Resistors with 1% accuracy Vref on BD9G101 is about +-2% -> Vddmax ~3,52V, Vddin ~3,278, Vddnom ~3,397V
 * the Vref of the ADC is about 3,397V +-120mV, leads to LSB of 3,317mV +-0,117mV
 * Bus voltage measurement: Voltage divider on the AD7 channel for Bus voltage measurement: 10/103 = 0,097
 * For the default HW with the nonlinear leakage current of the Z-diode ( ~50uA @20VVbus/18000mV Vz) we need to  adapt the value based on measurements
 * from Darthyson which resume in a ratio of 11,9 (at Vbus ~17V to 11,54 (at Vbus ~22v) _> we use 11,9 as mean value
 * plus D1 forward voltage of 600mV and loss of 1,1V of L2
 */

/*
 * Implement a subclass of ADCCallback in your application wich will be used by the bus voltage monitoring function
 * The ADC value should be update at an interval shorter than 20ms
 *
 */
class AppADCReadCallback {
public:
	virtual int ReadADCCount(unsigned int ADCChannel); // interface to read the adc in a user function, for bus voltage we need a sampling rate of >=100
};

class AppUserSaveCallback: public UsrCallback {
public:
	virtual void Notify(UsrCallbackType type);
};

class BusVoltage {
public:

	enum STATE : unsigned int {
		FAILED = 0x00, OK = 0x01, FALLING = 0x10, RISING = 0x11, UNKNOWN = 0xFF
	};
	enum STATE State;

	enum OPERATION_STATE : unsigned int {
		POWER_UP_IDLE = 0x0100,   // Vbus <18V
		POWER_UP_START_UP = 0x0101,  // 18V < Vbus < 21V
		POWER_UP_OPERATION = 0x0110,  // Vbus >=21V
		POWER_DOWN_OPERATION = 0x1010,  // 21V > Vbus >=20V
		POWER_DOWN_HOLD_UP = 0x1001,  // Vbus < 20V
		POWER_DOWN_IDLE = 0x1000, // Vbus < SMPS above min. operation voltage-> No Vcc, useless as MCU stopped operation, so we can not detect
		ADC_ERROR = 0xffff
	};
	enum OPERATION_STATE operationState;

	int bus_measurement_interval; // could be used on App level for bus measurements

	BusVoltage();

	/**
	 * setup Bus Voltage Monitoring.
	 *
	 *
	 * @return non-zero if successful.
	 *
	 * or with overload function without parameter as we know all these in gerneral
	 */
	unsigned int setup();

	/**
	 * ADC LBS value in uV
	 *
	 * @return ADC LSB value in uV
	 */
	int valueLSBuV();

	/**
	 * Bus Voltage in mV
	 *
	 * @return Bus Voltage, -1 on error
	 */
	int valueBusVoltagemV(); // returns measured bus voltage in mV (-1 if measurement is invalid)

	int measureBusVoltage(unsigned int ADCchannel);

	int lastADCCount(); // returns last the measured bus voltage in ADC counts ( (-1 if measurement is invalid or already read)

	bool busFailed();

	enum OPERATION_STATE CheckBusVoltage(void);
	enum OPERATION_STATE ValidBusVoltage(unsigned int);

	AppADCReadCallback *ptr_appadcreadCallback = nullptr;

	/**
	 * Set a callback class to read the adc channel in the app SW
	 */
	void setAppADCReadCallback(AppADCReadCallback *callback);

#if defined(DEBUG_BUS_VOLTAGE)
#	define BUS_VOLTAGE_MEASUREMENT_INTERVAL 3000
#	define BUS_VOLTAGE_MEASUREMENT_INTERVAL_HWV1 3000
#else
#	define BUS_VOLTAGE_MEASUREMENT_INTERVAL 10
#	define BUS_VOLTAGE_MEASUREMENT_INTERVAL_HWV1 40
#endif

protected:

private:

#if defined (FILTER)
	/**
	 * The size of our filter. We specify it in bits
	 */
#define FILTER_SIZE_IN_BITS (2)
#define FILTER_SIZE ( 1 << (FILTER_SIZE_IN_BITS) )

	/**
	 * The filter buffer for old values
	 */
	unsigned int filter_buffer[FILTER_SIZE];

	/**
	 * Here we store the sum of all history values
	 */
	unsigned int filter_sum;

	/**
	 * This is the actual position of the ring buffer.
	 */
	unsigned int *filter_position;

	void filter_init(void);
	unsigned int filter(unsigned int);
#endif

	int _ADChannel;
	int _thresholdVoltageFailedmV;
	int _thresholdVoltageReturnmV;
	int _thresholdVoltageIdlemV;
	int _thresholdADCCountFailed;
	int _thresholdADCCountReturn;
	int _thresholdADCCountIdle;
	volatile int adccount;

};

#endif /* BUS_VOLTAGE_H_ */
