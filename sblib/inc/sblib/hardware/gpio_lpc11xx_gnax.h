/*
 *  Copyright (c) 2015 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef SBLIB_GPIO_LPC11XX_GNAX_H_
#define SBLIB_GPIO_LPC11XX_GNAX_H_

#include <sblib/ioports.h>

#define PIN_EIB_TX  PIO1_9
#define PIN_EIB_RX  PIO1_8
#define PIN_PROG    PIO2_11
#define PIN_VBUS    ()      ///\todo was set to PIO0_23, but doesn't exist on a LPC1115 :(
#define PIN_INFO    PIO1_0
#define PIN_RUN     PIO2_10

#define PIN_PWM     PIO3_2
#define PIN_APRG    PIO1_7
#define PIN_IO1     PIO2_7
#define PIN_IO2     PIO0_1
#define PIN_IO3     PIO0_3
#define PIN_IO4     PIO2_8
#define PIO_SCL     PIO0_4
#define PIO_SDA     PIO0_5
#define PIN_IO5     PIO0_6
#define PIN_IO6     PIO0_2
#define PIN_IO7     PIO0_9
#define PIN_IO8     PIO0_8
#define PIN_TX      PIO3_0
#define PIN_RX      PIO3_1
#define PIN_IO9     PIO2_4
#define PIN_IO10    PIO3_4
#define PIN_IO11    PIO1_10
#define PIN_IO12    PIO2_11
#define PIN_IO13    PIO0_11
#define PIN_IO14    PIO1_0
#define PIN_IO15    PIO2_10

#define PIN_SSEL0   PIO0_2
#define PIN_SCK0    PIO0_6
#define PIN_MISO0   PIO0_8
#define PIN_MOSI0   PIO0_9

#define PIN_SSEL1   PIO2_0
#define PIN_SCK1    PIO2_1
#define PIN_MISO1   PIO2_2
#define PIN_MOSI1   PIO2_3

#define PIN_LT1     () ///\todo define remaining gpio
#define PIN_LT2     ()
#define PIN_LT3     ()
#define PIN_LT4     ()
#define PIN_LT5     ()
#define PIN_LT6     ()
#define PIN_LT7     ()
#define PIN_LT8     ()
#define PIN_LT9     ()

#endif /* SBLIB_GPIO_LPC11XX_GNAX_H_ */
