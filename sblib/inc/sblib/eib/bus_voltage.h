/*
 *  bus_voltage.h - simple ADC monitoring of the bus voltage on the AD7 of the LPCxx
 *
 *  Copyright (c) 2020 Darthyson <darth@maptrack.de>
 *
 *  last changes: Nov. 2024  Horst Rauch
 *                Bus voltage measurement in polling mode, can be overridden by a user class usrADC
 *                and callback function
 *                check for possible HW change on the analog part by ID2 level
 *                Bus voltage monitoring function for BCU phase
 *                - power up
 *                - power down
 *                - normal operation added
 *
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 *
 *  Don't create instances of class BusVoltage, use variable busVoltageMonitor instead
 *
 *  Notes:
 *      in the KNX-specification a bus fail is defined as a bus voltage drop below 20V with a hysteresis of 1V.
 *      for 4TE-ARM & TS_ARM the voltage divider on the AD-Pin is R3 (91K0) and R12 (10K0).
 *      Use resistors with 1% tolerance or better for these.
 *
 *  Usage:
 *
 *      Try to not use functions from sblib/analog_pin.h, because they will not work well together without changes to analog_pin.h
 *      In case you need these functions, a restart of the monitoring with
 *      busVoltageMonitor.enable();
 *      is mandatory, or otherwise monitoring will stop.
 *
 *      implement a sub-class of BusVoltageCallback in your app_main.h or app_main.cpp to get notifications of bus voltage fail/return. e.g.:
 *      -------------------------------------------------------------------------------------------------------------------------------------

        include "bus_voltage.h"

        class MySubBusVoltageCallback public BusVoltageCallback {
        public:
            virtual void BusVoltageFail();
            virtual void BusVoltageReturn();
            virtual int convertADmV(int valueAD);
            virtual int convertmVAD(int valuemV);
        };

        void MySubBusVoltageCallback::BusVoltageFail()
        {
            // return as fast as possible from this method, because its called directly from the ADC interrupt service routine
            // and avoid calling other functions which depend on interrupts
            e.g.
            .... your actions on bus voltage fail ....
            bcu.end();
        }

        void MySubBusVoltageCallback::BusVoltageReturn()
        {
            // return as fast as possible from this method, because its called directly from the ADC interrupt service routine
            // and avoid calling other functions which depend on interrupts
            .... your actions on bus voltage return ....
            e.g.
            bcu.begin(...);
        }

        int MySubBusVoltageCallback::convertADmV(int valueAD)
        {
            // provide a function or polynomial 2th degree (or better) to convert AD values to millivoltages of the bus voltage
            return valueAD;
        }

        int MySubBusVoltageCallback::convertmVAD(int valuemV)
        {
            // provide a function or polynomial 2th degree (or better) to convert millivoltages of the bus voltage to AD values
            return valuemV;
        }

        MySubBusVoltageCallback callback; // instance of MySubBusVoltageCallback a sub-class of class BusVoltageCallback to receive notifications

 *
 *
 *      start bus voltage monitoring in your app_main.cpp, e.g.:
 *
 *
        void setup()
        {
            .... your code ....
            if (busVoltageMonitor.setup(VBUS_AD_PIN, VBUS_AD_CHANNEL, VBUS_ADC_SAMPLE_FREQ,
                                VBUS_THRESHOLD_FAILED, VBUS_THRESHOLD_RETURN,
                                VBUS_VOLTAGE_FAILTIME_MS, VBUS_VOLTAGE_RETURNTIME_MS,
                                &timer32_0, 0, &callback)
            // setup & enable bus voltage monitoring
            if (busVoltageMonitor.setup(PIN_VBUS,   // IO-Pin used for bus voltage monitoring
                                        AD7,        // AD-Channel correspondending to the IO-Pin
                                        10000,      // AD-conversion frequency in Hz
                                        20000,      // Threshold for bus failing in milli-voltage
                                        21000,      // Threshold for bus returning in milli-voltage
                                        20,         // time (milliseconds) bus voltage (milli-voltage) must stay below, to trigger bus failing notification
                                        1500,       // time (milliseconds) bus voltage (milli-voltage) must stay above, to trigger bus returning notification
                                        &timer32_0, // timer used for AD-conversion. Can be timer16_0 or timer32_0 from sblib/timer.h, timer can't be used otherwise!!
                                        0,          // timer match channel used for AD-conversion. Can be 0 or 1
                                        &callback)) // instance of a sub-class of class BusVoltageCallback
            {
                busVoltageMonitor.enable();
                while (busVoltageMonitor.busFailed())
                {
                  delay(1);
                }
            }
            .... your code ....
        }
 */

#ifndef BUS_VOLTAGE_H_
#define BUS_VOLTAGE_H_

#include <sblib/timer.h>
#include <sblib/ioports.h>
#include <sblib/io_pin_names.h>
#include <sblib/timer.h>
#include <sblib/analog_pin.h>

/*
 * MCU ADC and Vref accuracy is about +-4LSB, remaining accuracy is about 6bit
 * Based on Resistors with 1% accuracy Vref on BD9G101 is about +-2% -> Vddmax ~3,52V, Vddin ~3,278, Vddnom ~3,397V
 *  the Vref of the ADC is about 3,397V +-120mV, leads to LSB of 3,317mV +-0,117mV
 */

#define ADC_RESOLUTION 1024  //10bit -> LSB = Vref / 2^N
#define ADC_ACCURACY_BITS 6
#define ADC_ACCURACY_MASK 0xfffffff0
#define ADC_REF_VOLTAGE 3397   // Vdd nom mV
#define ADC_MAX_VOLTAGE  (ADC_REF_VOLTAGE * (ADC_RESOLUTION - 1) / ADC_RESOLUTION)  // = LSB *(2^N -1)
#define ADC_LSB_MV  (ADC_REF_VOLTAGE / ADC_RESOLUTION)      // 3,3174mV
#define ADC_LSB_UV  ADC_REF_VOLTAGE *1000 / ADC_RESOLUTION  // 3317uV - could be used for integer based calculations


/*
 * implement a subclass of ADCCallback in your application
 */
class ADCCallback
{
public:
    virtual int ReadADCCount(unsigned int ADCChannel) = 0;   // interface to read the adc in a user function, for bus voltage we need a samling rate of >=100
};

class BusVoltage
{
public:

	enum STATE  : unsigned int {FAILED = 0x00, OK = 0x01, FALLING = 0x10, RISING = 0x11,UNKNOWN = 0xFF
	};
	enum STATE State;

	enum OPERATION_STATE  : unsigned int {POWER_UP_IDLE     = 0x0100,   // Vbus <18V
		POWER_UP_START_UP    = 0x0101,  // 18V < Vbus < 21V
		POWER_UP_OPERATION   = 0x0110,  // Vbus >=21V
		//POWER_DOWN_OPERATION = 0x1010,  // Vbus >=20V
		POWER_DOWN_HOLD_UP   = 0x1001,  // Vbus < 20V
		POWER_DOWN_IDLE      = 0x1000	// Vbus < SMPS above min. operation voltage
	};
	enum OPERATION_STATE operationState;


	BusVoltage();

    /**
     * setup Bus Voltage Monitoring.
     *
     * @param ADChannel AD-Channel correspondending to the IO-Pin
     * @param thresholdVoltageFailed Threshold for bus failing in milli-voltage
     * @param thresholdVoltageReturn Threshold for bus returning in milli-voltage
     * @param callback instance of a sub-class of class BusVoltageCallback
     *
     * @return non-zero if successful.
     *
     * or with overload function without parameter as we know all these in gerneral
     */
    unsigned int setup( unsigned int ADChannel,
                       unsigned int thresholdVoltageFailedmV, unsigned int thresholdVoltageReturnmV,
                       unsigned int thresholdVoltageIdlemV, ADCCallback *adcCallback);

    unsigned int setup(void);


    bool busFailed();
  //  enum STATE ValidBusVoltage(unsigned int);
    int valueBusVoltagemV(); // returns measured bus voltage in mV (-1 if measurement is invalid)
    int valueBusVoltageADCCount(); // returns measured bus voltage in AD (-1 if measurement is invalid)
    int lastADCCount(); // returns only onces the measured bus voltage in ADC counts ( (-1 if measurement is invalid or already read)
    int filteredADCCount(); // returns measured bus voltage in ADC counts ( (-1 if measurement is invalid)
//    enum OPERATION_STATE CheckBusVoltage(void);
    enum OPERATION_STATE ValidBusVoltage(unsigned int );
    int measureBusVoltage(unsigned int ADCchannel);
    enum OPERATION_STATE CheckBusVoltage(void);

protected:

private:

    /**
      * The size of our filter. We specify it in bits
      */
     #define FILTER_SIZE_IN_BITS (2)
     #define FILTER_SIZE ( 1 << (FILTER_SIZE_IN_BITS) )

     /**
      *
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
    unsigned int filter (unsigned int );

    unsigned int _ADChannel;
    unsigned int _thresholdVoltageFailedmV;
    unsigned int _thresholdVoltageReturnmV;
    unsigned int _thresholdVoltageIdlemV;
    unsigned int _thresholdADCCountFailed;
    unsigned int _thresholdADCCountReturn;
    unsigned int _thresholdADCCountIdle;

    ADCCallback*_adcCallback =(nullptr);


    // careful, variables can be changed any time by the Isr ADC_IRQHandler
    volatile int VoltagethresholdFailedmV;      // AD-value threshold below which a bus failure should be reported
    volatile int VoltagethresholdReturnmV;      // AD-value threshold above which a bus return should be reported
    volatile unsigned int ADCCountthresholdFailed;      // AD-value threshold below which a bus failure should be reported
    volatile unsigned int ADCCountthresholdReturn;      // AD-value threshold above which a bus return should be reported
    volatile unsigned int ADCCountthresholdIdle;      // AD-value threshold above which the BCU should start init process
    volatile enum STATE isrBusVoltageState;
    volatile int adccount;
};
extern BusVoltage busVoltageMonitor; // declared in bus_voltage.cpp, use only this instance for access

#endif /* BUS_VOLTAGE_H_ */
