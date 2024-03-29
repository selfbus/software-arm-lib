/*
 *  print.cpp - Base class that provides print() and println()
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/print.h>
#include <sblib/math.h>

#include <string.h>

// The size of the internal buffer in print()
#define PRINTBUF_SIZE (8 * sizeof(int) + 1)


int Print::print(int value, Base base, int digits)
{
    int wlen = 0;
    if (value < 0)
    {
        wlen += write('-');
        value = -value;
        --digits;
    }

    return print((uintptr_t) value, base, digits) + wlen;
}

int Print::print(const char* str, int value, Base base, int digits)
{
    int wlen = print(str);
    wlen += print(value, base, digits);
    return wlen;
}

int Print::print(uintptr_t value, Base base, int digits)
{
    byte buf[PRINTBUF_SIZE]; // need the maximum size for binary printing
    byte ch;

    short b = (short) base;
    if (b < 2) b = 2;

    byte* pos = buf + PRINTBUF_SIZE;
    do
    {
        ch = value % b;
        *--pos = (ch < 10 ? '0' : 'A' - 10) + ch;

        value /= b;
    }
    while (--digits > 0 || value);

    return write((byte*) pos, buf + PRINTBUF_SIZE - pos);
}

int Print::print(const char* str, uintptr_t value, Base base, int digits)
{
    int wlen = print(str);
    wlen += print(value, base, digits);
    return wlen;
}

int Print::print(float value, int precision)
{
    int number = (int)value;
    float fraction = abs((float)(value - number));
    int wlen = print(number);

    if (precision < 1)
    {
        return (wlen);
    }

    precision = min(7, precision);

    wlen += print(".");
    for (uint8_t i = 0; i < precision; i++)
    {
        fraction *= 10.0f;
    }
    wlen += print((int)fraction, DEC, precision);
    return wlen;
}

int Print::print(const char* str, float value, int precision)
{
    int wlen = print(str);
    wlen += print(value, precision);
    return wlen;
}

int Print::println()
{
    return write('\r') + write('\n');
}

int Print::write(const byte* data, int count)
{
    int wlen = 0;
    while (count--)
    {
        wlen += write(*data++);
    }

    return wlen;
}

int Print::write(const char* str)
{
    if (str)
        return write((const byte*) str, strlen(str));
    return 0;
}

int Print::println(const char* str, uintptr_t value, Base base, int digits)
{
    int wlen = print(str, value, base, digits);
    wlen += println();
    return wlen;
}

int Print::println(const char* str, int value, Base base, int digits)
{
    int wlen = print(str, value, base, digits);
    wlen += println();
    return wlen;
}

int Print::println(float value, int precision)
{
    int wlen = print(value, precision);
    wlen += println();
    return wlen;
}

int Print::println(const char* str, float value, int precision)
{
    int wlen = print(str, value, precision);
    wlen += println();
    return wlen;
}
