/*
 *  bits.h - Bit and byte manipulation functions.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_bits_h
#define sblib_bits_h

#include <sblib/platform.h>
#include <sblib/types.h>
#include <stdint.h>


/**
 * Extract the lowest (rightmost) byte of a number. The number can be
 * any integer (byte, short, int, long).
 *
 * @param val - the value to extract the lowest byte.
 * @return The extracted byte (0..255)
 */
#define lowByte(val) ((val) & 255)

/**
 * Extract the highest (leftmost) byte of a number. The number can be
 * any integer (byte, short, int, long).
 *
 * @param val - the value to extract the highest byte.
 * @return The extracted byte (0..255)
 */
#define highByte(val) (((val) >> ((sizeof(val) - 1) << 3)) & 255)

/**
 * Combine two bytes to a 16 bit word.
 *
 * @param high - the high byte.
 * @param low - the low byte.
 * @return The bytes combined as word.
 */
uint16_t makeWord(byte high, byte low);

/**
 * Reverse the byte order of an integer.
 *
 * @param val - the value to reverse.
 * @return The value with reversed byte order.
 */
unsigned int reverseByteOrder(unsigned int val);

/**
 * Reverse the byte order of a short integer.
 *
 * @param val - the value to reverse.
 * @return The value with reversed byte order.
 */
unsigned short reverseByteOrder(unsigned short val);


//
//  Inline functions
//

inline uint16_t makeWord(byte high, byte low)
{
    return ((uint16_t)((high << 8) | low));
}

inline unsigned int reverseByteOrder(unsigned int val)
{
    return __REV(val);
}

inline unsigned short reverseByteOrder(unsigned short val)
{
    return __REV16(val);
}

#endif /*sblib_bits_h*/
