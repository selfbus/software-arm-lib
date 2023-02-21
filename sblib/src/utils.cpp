/*
 *  utils.cpp - Utility functions.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/utils.h>

#include <sblib/digital_pin.h>
#include <sblib/platform.h>
#include <sblib/io_pin_names.h>
#include <string.h>

static int fatalErrorPin = PIN_PROG;
static int eibTxPin = PIN_EIB_TX; ///\todo make it universal

void reverseCopy(byte* dest, const byte* src, int len)
{
    src += len - 1;
    while (len > 0)
    {
        *dest++ = *src--;
        --len;
    }
}

void setPinsInSecureModes()
{
    pinMode(fatalErrorPin, OUTPUT);
    pinMode(eibTxPin, INPUT);
}

void fatalError()
{
    // We use only low level functions here as a fatalError() could happen
    // anywhere in the lib and we want to ensure that the function works
    SysTick_Config(0x1000000);
    setPinsInSecureModes();
    while (1)
    {
        // Blink the fatalErrorLED
        digitalWrite(fatalErrorPin, (SysTick->VAL & 0x400000) == 0 ? 1 : 0);
    }
}

void HardFault_Handler()
{
    // We use only low level functions here as a HardFault could happen
    // anywhere and we want to ensure that the function works
    SysTick_Config(0x1000000);
    setPinsInSecureModes();
    while (1)
    {
        // Blink the fatalErrorLED
        digitalWrite(fatalErrorPin, (SysTick->VAL & 0x200000) == 0 ? 1 : 0);
    }
}

void setFatalErrorPin(int newPin)
{
    fatalErrorPin = newPin;
}

void setKNX_TX_Pin(int newTxPin)
{
    eibTxPin = newTxPin;
}

int hashUID(byte* uid, const int len_uid, byte* hash, const int len_hash)
{
    const int MAX_HASH_WIDE = 16;
    uint64_t BigPrime48 = 281474976710597u;  // FF FF FF FF FF C5
    uint64_t a, b;
    unsigned int mid;

    if ((len_uid <= 0) || (len_uid > MAX_HASH_WIDE))  // maximum of 16 bytes can be hashed by this function
        return 0;
    if ((len_hash <= 0) || (len_hash > len_uid))
        return 0;

    mid = len_uid/2;
    memcpy (&a, &uid[0], mid);          // copy first half of uid-bytes to a
    memcpy (&b, &uid[mid], len_uid-mid); // copy second half of uid-bytes to b

    // do some modulo a big primenumber
    a = a % BigPrime48;
    b = b % BigPrime48;
    a = a^b;
    // copy the generated hash to provided buffer
    for (int i = 0; i<len_hash; i++)
        hash[i] = uint64_t(a >> (8*i)) & 0xFF;
    return 1;
}
