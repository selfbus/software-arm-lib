/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_SERIAL_BUS_2 Serial Bus Monitor 2 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Configuration file for the Serial Bus Monitor 2
 *
 * @{
 *
 * @file   config.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef CONFIG_H_
#define CONFIG_H_

#include <sblib/io_pin_names.h>

#define SERIAL_BAUD_RATE 576000

#define BLINK_TIME_MS 1000

#ifndef TS_ARM
#   define PIN_BLINK PIN_INFO
#   define PIN_SERIAL_TX PIO2_8 //!< serial Tx-Pin on a 4TE-Controller
#   define PIN_SERIAL_RX PIO2_7 //!< serial Rx-Pin on a 4TE-Controller
#else
#   define PIN_BLINK PIN_PROG
#   define PIN_SERIAL_TX PIO3_0 //!< serial Tx-Pin on a TS_ARM
#   define PIN_SERIAL_RX PIO3_1 //!< serial Rx-Pin on a TS_ARM
#endif

#endif /* CONFIG_H_ */



/** @}*/
