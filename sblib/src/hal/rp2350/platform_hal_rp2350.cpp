/*
 *  platform_hal_rp2350.cpp - PlatformHAL implementation for RP2350 (Pico SDK).
 *
 *  Uses the Pico SDK's hardware_timer, hardware_sync, and watchdog APIs.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#ifdef __SBLIB_TARGET_RP2350__

#include <sblib/hal/platform_hal.h>

// RP2350 Pico SDK headers
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/sync.h"
#include "hardware/watchdog.h"
#include "hardware/structs/scb.h"

static volatile uint32_t systemTimeMs = 0;
static uint32_t irqSaveState = 0;

// Repeating timer callback for 1ms system tick
static bool systemTickCallback(struct repeating_timer* t)
{
    (void)t;
    systemTimeMs++;
    return true;
}

static struct repeating_timer systemTimer;

namespace PlatformHAL
{

void init()
{
    stdio_init_all();
    // Start a 1ms repeating timer
    add_repeating_timer_ms(-1, systemTickCallback, nullptr, &systemTimer);
}

void disableInterrupts()
{
    irqSaveState = save_and_disable_interrupts();
}

void enableInterrupts()
{
    restore_interrupts(irqSaveState);
}

void waitForInterrupt()
{
    __wfi();
}

bool isInsideInterrupt()
{
    // On ARM Cortex-M33: check ICSR VECTACTIVE
    // On RISC-V Hazard3: check mcause or equivalent
#if defined(__ARM_ARCH)
    return (scb_hw->icsr & M0PLUS_ICSR_VECTACTIVE_BITS) != 0;
#else
    // RISC-V: check if mstatus.MIE is cleared (inside exception handler)
    uint32_t mstatus;
    __asm volatile("csrr %0, mstatus" : "=r"(mstatus));
    return (mstatus & 0x8) == 0; // MIE bit
#endif
}

uint32_t millis()
{
    return systemTimeMs;
}

uint32_t micros()
{
    return (uint32_t)time_us_64();
}

void delayMs(uint32_t ms)
{
    sleep_ms(ms);
}

void delayUs(uint32_t us)
{
    sleep_us(us);
}

[[noreturn]] void systemReset()
{
    watchdog_reboot(0, 0, 0);
    // Should not reach here, but just in case:
    while (true) { __wfi(); }
}

} // namespace PlatformHAL

#endif // __SBLIB_TARGET_RP2350__
