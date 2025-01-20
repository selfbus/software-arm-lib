/*
 *  Copyright (c) 2015 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef SBLIB_IO_PIN_NAMES_H_
#define SBLIB_IO_PIN_NAMES_H_

#if defined (__LPC11XX_GNAXBOARD__)
#   include <sblib/hardware/gpio_lpc11xx_gnax.h>
#elif defined(__LPC11XX_FUSED_IO__)
#   include <sblib/hardware/gpio_lpc11xx_fused_io.h>
#elif defined (__LPC11XX__)
#   include <sblib/hardware/gpio_lpc11xx.h>
#elif defined (__LPC11XX_OM13087__)
#   include <sblib/hardware/gpio_lpc11xx_OM13087.h>
#endif

#endif /* SBLIB_IO_PIN_NAMES_H_ */
