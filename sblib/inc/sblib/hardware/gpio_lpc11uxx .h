/*
 *  Copyright (c) 2015 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef SBLIB_GPIO_LPC11UXX_H_
#define SBLIB_GPIO_LPC11UXX_H_

#include <sblib/ioports.h>

#define PIN_EIB_TX  PIO0_21
#define PIN_EIB_RX  PIO0_20
#define PIN_PROG    PIO1_19
#define PIN_VBUS    PIO0_23
#define PIN_INFO    PIO1_25
#define PIN_RUN     PIO1_16

#define PIN_PWM     PIO1_15
#define PIN_APRG    PIO0_6
#define PIN_IO1     PIO1_21
#define PIN_IO2     PIO0_7
#define PIN_IO3     PIO1_31
#define PIN_IO4     PIO1_28
#define PIO_SCL     PIO0_4
#define PIO_SDA     PIO0_5
#define PIN_IO5     PIO0_2
#define PIN_IO6     PIO0_8
#define PIN_IO7     PIO0_9
#define PIN_IO8     PIO1_29
#define PIN_TX      PIO1_13
#define PIN_RX      PIO1_14
#define PIN_IO9     PIO0_12
#define PIN_IO10    PIO0_13
#define PIN_IO11    PIO0_14
#define PIN_IO12    PIO0_16
#define PIN_IO13    PIO0_17
#define PIN_IO14    PIO0_19
#define PIN_IO15    PIO0_18

#define PIN_SSEL0   () ///\todo set correct gpio
#define PIN_SCK0    ()
#define PIN_MISO0   ()
#define PIN_MOSI0   ()

#define PIN_SSEL1   ()
#define PIN_SCK1    ()
#define PIN_MISO1   ()
#define PIN_MOSI1   ()

#define PIN_LT1     PIO1_20
#define PIN_LT2     PIO0_3
#define PIN_LT3     PIO1_27
#define PIN_LT4     PIO1_26
#define PIN_LT5     PIO1_24
#define PIN_LT6     PIO1_23
#define PIN_LT7     PIO0_22
#define PIN_LT8     PIO0_11
#define PIN_LT9     PIO1_22

#endif /* SBLIB_GPIO_LPC11UXX_H_ */
