/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_BOOTLOADER_DUMP Debugging over serial port stuff
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Some macros for debugging over serial port stuff
 * @details 
 *
 *
 * @{
 *
 * @file   dump.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef DUMP_H_
#define DUMP_H_

#if (defined(DUMP_TELEGRAMS_LVL1) || defined(DUMP_TELEGRAMS_LVL2))
#   include <sblib/serial.h>
#endif

#define LOG_SEP '\t'
#define knx_area(addr) ((addr >> 12) & 0x0F)
#define knx_line(addr) ((addr >> 8) & 0x0F)
#define knx_device(addr) (addr & 0xFF)
#define is_repeated(controlByte) (((bool)((controlByte & (1 << 5)) == 0)))

// for update.cpp, upd_protocol.cpp, flash.cpp and partly decompressor.cpp
#if defined(DUMP_TELEGRAMS_LVL1)
#   define d1(x) {serial.print(x);}
#   define dline(x) {serial.println(x);}
#   define d2(u,v,w) {serial.print(u,v,w);}
#   define d2ptr(u) {serial.print(u);}
#   define d3(x) {x;}
#else
#   define d1(x)
#   define d2(u,v,w)
#   define d2ptr(u)
#   define d3(x)
#   define dline(x)
#endif

// for bcu_updater.h
#if defined(DUMP_TELEGRAMS_LVL2)
#   define dump2(code) code
#else
#   define dump2(x)
#endif

#endif /* DUMP_H_ */
/** @}*/
