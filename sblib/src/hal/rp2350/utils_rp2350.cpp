/*
 *  utils_rp2350.cpp - Utility functions for RP2350.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#ifdef __SBLIB_TARGET_RP2350__

#include <sblib/utils.h>
#include <sblib/io_pin_names.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/watchdog.h"

static int fatalErrorPin = PIN_INFO;  // Pico 2 onboard LED

void reverseCopy(byte* dest, const byte* src, int len)
{
    src += len - 1;
    while (len > 0)
    {
        *dest++ = *src--;
        --len;
    }
}

void fatalError()
{
    gpio_init(fatalErrorPin);
    gpio_set_dir(fatalErrorPin, GPIO_OUT);

    // Blink the error LED rapidly
    while (1)
    {
        gpio_put(fatalErrorPin, 1);
        busy_wait_us_32(200000);
        gpio_put(fatalErrorPin, 0);
        busy_wait_us_32(200000);
    }
}

void setFatalErrorPin(int newPin)
{
    fatalErrorPin = newPin;
}

void setKNX_TX_Pin(int newTxPin)
{
    (void)newTxPin; // Not applicable on RP2350 — TX is managed by PIO
}

int hashUID(byte* uid, const int len_uid, byte* hash, const int len_hash)
{
    const int MAX_HASH_WIDE = 16;
    uint64_t BigPrime48 = 281474976710597u;
    uint64_t a, b;
    unsigned int mid;

    if ((len_uid <= 0) || (len_uid > MAX_HASH_WIDE))
        return 0;
    if ((len_hash <= 0) || (len_hash > len_uid))
        return 0;

    mid = len_uid / 2;
    memcpy(&a, &uid[0], mid);
    memcpy(&b, &uid[mid], len_uid - mid);

    a = a % BigPrime48;
    b = b % BigPrime48;
    a = a ^ b;
    for (int i = 0; i < len_hash; i++)
        hash[i] = uint64_t(a >> (8 * i)) & 0xFF;
    return 1;
}

#endif // __SBLIB_TARGET_RP2350__
