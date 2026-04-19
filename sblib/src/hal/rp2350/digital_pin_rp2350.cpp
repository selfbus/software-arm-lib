/*
 *  digital_pin_rp2350.cpp - Digital I/O functions for RP2350 (Pico SDK).
 *
 *  Provides the same free-function API as the LPC11xx digital_pin.cpp
 *  (pinMode, digitalWrite, digitalRead) using Pico SDK GPIO calls.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#ifdef __SBLIB_TARGET_RP2350__

#include <sblib/digital_pin.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

void pinMode(int pin, int mode)
{
    gpio_init(pin);

    if (mode & OUTPUT)
    {
        gpio_set_dir(pin, GPIO_OUT);
        if (mode & OPEN_DRAIN)
        {
            // RP2350 doesn't have true open-drain; emulate with pulls disabled
        }
    }
    else
    {
        gpio_set_dir(pin, GPIO_IN);
        if (mode & PULL_UP)
            gpio_pull_up(pin);
        else if (mode & PULL_DOWN)
            gpio_pull_down(pin);
        else
            gpio_disable_pulls(pin);
    }
}

void pinDirection(int pin, int dir)
{
    gpio_set_dir(pin, (dir & OUTPUT) ? GPIO_OUT : GPIO_IN);
}

void digitalWrite(int pin, bool value)
{
    gpio_put(pin, value);
}

bool digitalRead(int pin)
{
    return gpio_get(pin);
}

void pinInterruptMode(int pin, int mode)
{
    (void)pin;
    (void)mode;
    // On RP2350, GPIO interrupts are managed via gpio_set_irq_enabled_with_callback.
    // This stub is provided for API compatibility.
}

void pinEnableInterrupt(int pin)
{
    (void)pin;
}

void pinDisableInterrupt(int pin)
{
    (void)pin;
}

#endif // __SBLIB_TARGET_RP2350__
