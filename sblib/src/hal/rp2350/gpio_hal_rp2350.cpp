/*
 *  gpio_hal_rp2350.cpp - GpioHAL implementation for RP2350 (Pico SDK).
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#ifdef __SBLIB_TARGET_RP2350__

#include <sblib/hal/gpio_hal.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

namespace GpioHAL
{

void pinMode(int pin, uint16_t mode)
{
    gpio_init(pin);

    if (mode & GPIO_OUTPUT)
    {
        gpio_set_dir(pin, GPIO_OUT);
    }
    else
    {
        gpio_set_dir(pin, GPIO_IN);
    }

    if (mode & GPIO_PULL_UP)
        gpio_pull_up(pin);
    else if (mode & GPIO_PULL_DOWN)
        gpio_pull_down(pin);
    else
        gpio_disable_pulls(pin);

    if (mode & GPIO_HYSTERESIS)
        gpio_set_input_hysteresis_enabled(pin, true);
}

void digitalWrite(int pin, bool value)
{
    gpio_put(pin, value);
}

bool digitalRead(int pin)
{
    return gpio_get(pin);
}

} // namespace GpioHAL

#endif // __SBLIB_TARGET_RP2350__
