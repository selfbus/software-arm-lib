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
#include <sblib/libconfig.h>

Serial serial(SERIAL_RX_PIN, SERIAL_TX_PIN);

extern "C" void UART_IRQHandler()
{
    serial.interruptHandler();
}
