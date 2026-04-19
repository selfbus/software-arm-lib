/*
 *  gpio_hal.h - Hardware Abstraction Layer for GPIO operations.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */
#ifndef SBLIB_HAL_GPIO_HAL_H
#define SBLIB_HAL_GPIO_HAL_H

#include <stdint.h>

/**
 * Pin direction / mode constants (combinable via OR).
 */
enum GpioPinMode : uint16_t
{
    GPIO_INPUT        = 0x0000,
    GPIO_OUTPUT       = 0x0001,
    GPIO_PULL_UP      = 0x0010,
    GPIO_PULL_DOWN    = 0x0020,
    GPIO_OPEN_DRAIN   = 0x0040,
    GPIO_HYSTERESIS   = 0x0080,
};

/**
 * Hardware Abstraction for GPIO pins.
 *
 * Pin numbers are abstract identifiers defined per platform.
 * On LPC11xx, these match PIOx_y encoding.
 * On RP2350, these are 0-29 GPIO numbers.
 */
namespace GpioHAL
{
    /** Configure a GPIO pin mode (direction + pulls, etc.). */
    void pinMode(int pin, uint16_t mode);

    /** Write a digital output (true = HIGH). */
    void digitalWrite(int pin, bool value);

    /** Read a digital input. */
    bool digitalRead(int pin);
}

#endif /* SBLIB_HAL_GPIO_HAL_H */
