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
#define PIN_VBUS    () // PIO0_23  ///\todo 0_23 doesn't exist on a LPC1115 :(
#define PIN_INFO    PIO1_0
#define PIN_RUN     PIO2_10

#define PIN_PROG_INVERTED  (false)

#define PIN_PWM     () ///\todo define remaining gpio
#define PIN_APRG    ()
#define PIN_IO1     ()
#define PIN_IO2     ()
#define PIN_IO3     ()
#define PIN_IO4     ()
#define PIO_SCL     ()
#define PIO_SDA     ()
#define PIN_IO5     ()
#define PIN_IO6     ()
#define PIN_IO7     ()
#define PIN_IO8     ()
#define PIN_TX      ()
#define PIN_RX      ()
#define PIN_IO9     ()
#define PIN_IO10    ()
#define PIN_IO11    ()
#define PIN_IO12    ()
#define PIN_IO13    ()
#define PIN_IO14    ()
#define PIN_IO15    ()

#define PIN_SSEL0   PIO2_0
#define PIN_SCK0    PIO2_1
#define PIN_MISO0   PIO2_2
#define PIN_MOSI0   PIO2_3

#define PIN_SSEL1   PIN_SSEL0
#define PIN_SCK1    PIN_SCK0
#define PIN_MISO1   PIN_MISO0
#define PIN_MOSI1   PIN_MOSI0

#define PIN_LT1     ()
#define PIN_LT2     ()
#define PIN_LT3     ()
#define PIN_LT4     ()
#define PIN_LT5     ()
#define PIN_LT6     ()
#define PIN_LT7     ()
#define PIN_LT8     ()
#define PIN_LT9     ()

#endif /* SBLIB_GPIO_LPC11XX_GNAX_H_ */
