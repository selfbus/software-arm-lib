/*
 *  serial.cpp - Serial port access. This file defines the default serial port.
 *
 *  Copyright (c) 2015 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/serial.h>
#include <sblib/digital_pin.h>
#include <sblib/core.h>
#include <sblib/config.h>

// serial Tx and Rx-pin definitions moved to config.h
#if !defined(SERIAL_TX_PIN) || !defined(SERIAL_RX_PIN)
#   if defined (__LPC11XX__)
#       define SERIAL_TX_PIN PIO1_7     //!> default serial Tx-Pin PIO1.7 (LPC11xx)
#       define SERIAL_RX_PIN PIO1_6     //!> default serial Tx-Pin PIO1.6 (LPC11xx)
#   elif defined (__LPC11UXX__)
#       define SERIAL_TX_PIN PIO0_19    //!> default serial Tx-Pin PIO0.19 (LPC11Uxx)
#       define SERIAL_RX_PIN PIO0_18    //!> default serial Tx-Pin PIO0.18 (LPC11Uxx)
#   endif
#endif

Serial serial(SERIAL_RX_PIN, SERIAL_TX_PIN);

extern "C" void UART_IRQHandler()
{
    serial.interruptHandler();
}
