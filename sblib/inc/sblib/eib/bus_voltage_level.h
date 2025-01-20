/*
 *  bus_voltage_level.h -
 *
 */

#ifndef BUS_VOLTAGE_LEVEL_H_
#define BUS_VOLTAGE_LEVEL_H_


/*
#define ADC_RESOLUTION 1024  //10bit -> LSB = Vref / 2^N
#define ADC_ACCURACY_BITS 6
#define ADC_ACCURACY_MASK 0xfffffff0
#define ADC_REF_VOLTAGE 3397   // Vdd nom mV
#define ADC_MAX_VOLTAGE  (ADC_REF_VOLTAGE * (ADC_RESOLUTION - 1) / ADC_RESOLUTION)  // = LSB *(2^N -1)
#define ADC_LSB_MV  (ADC_REF_VOLTAGE / ADC_RESOLUTION)      // 3,3174mV
#define ADC_LSB_UV  ADC_REF_VOLTAGE *1000 / ADC_RESOLUTION  // 3317uV - could be used for integer based calculations
*/

// voltage level for the bus voltage monitoring according to KNX spec, we subtract the D1 diode Vf of 600mV
#define VoltageFailedmV 19400  // Vbus 20V
#define VoltageRunmV 20400     // Vbus 21V
#define VoltageIdlemV 17400    // Vbus 18V

// default values for HW version default
#define VoltageFailedcnt  (( VoltageFailedmV * 10000) /(ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_DEFAULT))
#define VoltageRuncnt     (( VoltageRunmV * 10000) /( ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_DEFAULT))
#define VoltageIdlecnt    (( VoltageIdlemV 10000) /(ADC_LSB_UV * VBUS_VOLTAGEDIVIDER_DEFAULT))


#endif /* BUS_VOLTAGE_H_ */
