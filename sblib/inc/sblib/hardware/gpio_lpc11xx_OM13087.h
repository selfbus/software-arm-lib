/*
 *  Copyright (c) 2015 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef SBLIB_GPIO_LPC11XX_H_
#define SBLIB_GPIO_LPC11XX_H_

#include <sblib/ioports.h>

#define PIN_EIB_TX  PIO1_9
#define PIN_EIB_RX  PIO1_8
#define PIN_PROG    PIO2_0
#define PIN_VBUS    PIO0_11
#define PIN_INFO    PIO2_6
#define PIN_RUN     PIO3_3

#define PIN_PWM     PIO3_2
#define PIN_APRG    PIO0_6
#define PIN_IO1     PIO2_2
#define PIN_IO2     PIO0_7
#define PIN_IO3     PIO2_10
#define PIN_IO4     PIO2_9
#define PIO_SCL     PIO0_4
#define PIO_SDA     PIO0_5
#define PIN_IO5     PIO0_2
#define PIN_IO6     PIO0_8
#define PIN_IO7     PIO0_9
#define PIN_IO8     PIO2_11
//#define PIN_TX      PIO3_0
//#define PIN_RX      PIO3_1
#define PIN_IO9     PIO1_0
#define PIN_IO10    PIO1_1
#define PIN_IO11    PIO1_2
#define PIN_IO12    PIO1_4
#define PIN_IO13    PIO1_5
#define PIN_IO14    PIO1_7
#define PIN_IO15    PIO1_6

#define PIN_SSEL0   PIO0_2
#define PIN_SCK0    PIO0_6 // alternative PIO0_10, PIO2_11 configured in IOCON_SCK_LOC
#define PIN_MISO0   PIO0_8
#define PIN_MOSI0   PIO0_9

#define PIN_SSEL1   PIO2_0 // alternative PIO2_4 configured in IOCON_SSEL1_LOC
#define PIN_SCK1    PIO2_1 // alternative PIO3_2 configured in IOCON_SCK1_LOC
#define PIN_MISO1   PIO2_2 // alternative PIO1_10 configured in IOCON_MISO1_LOC
#define PIN_MOSI1   PIO2_3 // alternative PIO1_9 configured in IOCON_MOSI1_LOC

#define PIN_LT1     PIO2_1
#define PIN_LT2     PIO0_3
#define PIN_LT3     PIO2_4
#define PIN_LT4     PIO2_5
#define PIN_LT5     PIO3_5
#define PIN_LT6     PIO3_4
#define PIN_LT7     PIO1_10
#define PIN_LT8     PIO0_11
#define PIN_LT9     PIO2_3

//definitions of 8 ADC ports
#define PIN_AD0	    PIO0_11
#define PIN_AD1	    PIO1_0
#define PIN_AD2	    PIO1_1
#define PIN_AD3	    PIO1_2
#define PIN_AD4	    PIO1_3
#define PIN_AD5	    PIO1_4
#define PIN_AD6	    PIO1_11
#define PIN_AD7	    PIO1_11

//special use for HW version detection and external Vref of 2.5v 0.1% eg LM4040A 2.5
#define PIN_ID1		PIO2_7
#define PIN_ID2		PIO2_8
#define PIN_VREF	PIO1_3
#define EXT_VREF_UV 2500000
#define PIN_VBUS_AD 	PIN_AD0
#define PIN_EXT_VREF_AD PIN_AD1
#define EXTVREF_ADC	AD1
#define VBUS_ADC 	AD0


#define VBUS_VOLTAGEDIVIDER_HWV1 103 //  R9+R2+R3 / R12 !!!with factor 10 to allow for integer calculations
#define VBUS_VOLTAGEDIVIDER_DEFAULT 114 // R9+R2+R3 / R12  plus leakage current zender diode D3

// definitions for serial ports
#define PIN_TX      PIO3_0  // at header SV2
#define PIN_RX      PIO3_1  // at header SV2
#define PIN_TX_JTAG PIO1_7  // at header JTAG/SV2
#define PIN_RX_JTAG PIO1_6  // at header JTAG/SV2
#define PIN_TX_ID   PIO2_8  // at header ID SEL
#define PIN_RX_ID   PIO2_7  // at header ID SEL


#endif /* SBLIB_GPIO_LPC11XX_H_ */
