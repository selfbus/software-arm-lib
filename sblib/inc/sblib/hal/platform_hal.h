/*
 *  platform_hal.h - Hardware Abstraction Layer for platform-specific functions.
 *
 *  Provides a platform-independent interface for basic MCU functions:
 *  interrupts, system timer, GPIO, and flash access.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */
#ifndef SBLIB_HAL_PLATFORM_HAL_H
#define SBLIB_HAL_PLATFORM_HAL_H

#include <stdint.h>

/**
 * Hardware Abstraction Layer — platform-independent interface.
 *
 * Each target MCU (LPC11xx, RP2350, ...) provides an implementation.
 * Only one implementation is linked into a given build.
 */
namespace PlatformHAL
{
    /** Initialize the platform (clocks, GPIO, system timer). */
    void init();

    /** Disable all interrupts globally (must be nestable or paired with enableInterrupts). */
    void disableInterrupts();

    /** Re-enable interrupts after disableInterrupts(). */
    void enableInterrupts();

    /** Put the CPU to sleep until the next interrupt. */
    void waitForInterrupt();

    /** Return true if executing inside an ISR context. */
    bool isInsideInterrupt();

    /** Milliseconds since system start (wraps). */
    uint32_t millis();

    /** Microseconds since system start (wraps). */
    uint32_t micros();

    /** Busy-wait for the given number of milliseconds. */
    void delayMs(uint32_t ms);

    /** Busy-wait for the given number of microseconds. */
    void delayUs(uint32_t us);

    /** Perform a software system reset. Does not return. */
    [[noreturn]] void systemReset();
}

#endif /* SBLIB_HAL_PLATFORM_HAL_H */
