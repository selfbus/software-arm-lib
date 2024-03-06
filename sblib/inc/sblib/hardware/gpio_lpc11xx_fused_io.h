/*
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef SBLIB_GPIO_LPC11XX_FUSED_IO_H_
#define SBLIB_GPIO_LPC11XX_FUSED_IO_H_

#include <sblib/ioports.h>

#define PIN_EIB_TX  PIO1_9
#define PIN_EIB_RX  PIO1_8
#define PIN_PROG    PIO2_0
#define PIN_VBUS    PIO1_11
#define PIN_INFO    PIO3_3
#define PIN_RUN     PIN_INFO // we have only one led, so it's the same as PIN_INFO

#define PIN_PWM     () ///\todo define
#define PIN_APRG    PIN_PROG // we have only one programming button, so it's the same as PIN_PROG
#define PIN_IO1     PIO3_4
#define PIN_IO2     PIO0_3
#define PIN_IO3     PIO2_5
#define PIN_IO4     PIO0_7
#define PIO_SCL     PIO0_4
#define PIO_SDA     PIO0_5
#define PIN_IO5     PIO3_5
#define PIN_IO6     PIO2_1
#define PIN_IO7     PIO0_6
#define PIN_IO8     PIO2_9
#define PIN_TX      PIO3_0
#define PIN_RX      PIO3_1
#define PIN_IO9     PIO0_8
#define PIN_IO10    PIO2_10
#define PIN_IO11    PIO1_10
#define PIN_IO12    PIO0_9
#define PIN_IO13    PIO1_4
#define PIN_IO14    PIO0_2
#define PIN_IO15    PIO2_7

#define PIN_SSEL0   PIO2_4 // actually this is port 1, see below
#define PIN_SCK0    PIO3_2 // actually this is port 1, see below
#define PIN_MISO0   PIO2_2 // actually this is port 1, see below
#define PIN_MOSI0   PIO2_3 // actually this is port 1, see below

#define PIN_SSEL1   PIN_SSEL0
#define PIN_SCK1    PIN_SCK0
#define PIN_MISO1   PIN_MISO0
#define PIN_MOSI1   PIN_MOSI0

#define PIN_LT1     () ///\todo define remaining gpio
#define PIN_LT2     ()
#define PIN_LT3     ()
#define PIN_LT4     ()
#define PIN_LT5     ()
#define PIN_LT6     ()
#define PIN_LT7     ()
#define PIN_LT8     ()
#define PIN_LT9     ()

#endif /* SBLIB_GPIO_LPC11XX_FUSED_IO_H_ */
