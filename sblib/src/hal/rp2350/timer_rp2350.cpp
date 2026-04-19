/*
 *  timer_rp2350.cpp - Timer and time functions for RP2350 (Pico SDK).
 *
 *  Provides the same API as the LPC11xx timer.cpp (millis(), delay(),
 *  elapsed(), delayMicroseconds()) using Pico SDK time functions.
 *
 *  Also provides a Timer class emulation using Pico SDK repeating timers
 *  to support the same interrupt-driven periodic timer pattern used by
 *  most sblib examples.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#ifdef __SBLIB_TARGET_RP2350__

#include <sblib/timer.h>
#include <sblib/types.h>
#include "pico/stdlib.h"
#include "pico/time.h"

// ---- Free time functions ----

static volatile unsigned int systemTime = 0;

void delay(unsigned int msec)
{
    sleep_ms(msec);
}

void delayMicroseconds(unsigned int usec)
{
    busy_wait_us_32(usec);
}

unsigned int millis()
{
    return (unsigned int)(time_us_64() / 1000);
}

void setMillis(unsigned int newSystemTime)
{
    systemTime = newSystemTime;
}

unsigned int elapsed(unsigned int since)
{
    return millis() - since;
}

// ---- Weak default ISR handlers (user code overrides these) ----

extern "C" {
    __attribute__((weak)) void TIMER16_0_IRQHandler() {}
    __attribute__((weak)) void TIMER16_1_IRQHandler() {}
    __attribute__((weak)) void TIMER32_0_IRQHandler() {}
    __attribute__((weak)) void TIMER32_1_IRQHandler() {}
}

// ---- Repeating timer storage (one per timer instance) ----

static repeating_timer_t rpTimers[4];

// ---- Pico SDK repeating timer callback ----

static bool timerCallback(repeating_timer_t* rt)
{
    Timer* t = static_cast<Timer*>(rt->user_data);
    t->_timerFired();
    return true; // keep repeating
}

// ---- Timer class implementation ----

Timer timer16_0(TIMER16_0);
Timer timer16_1(TIMER16_1);
Timer timer32_0(TIMER32_0);
Timer timer32_1(TIMER32_1);

Timer::Timer(byte aTimerNum)
    : timerNum(aTimerNum)
    , prescalerVal(0)
    , matchValues{0, 0, 0, 0}
    , matchModes{0, 0, 0, 0}
    , irqFlags(0)
    , counterVal(0)
    , running(false)
    , interruptEnabled(false)
    , rpTimerPtr(&rpTimers[aTimerNum])
{
}

void Timer::begin()
{
    // No hardware initialization needed on RP2350 —
    // the Pico SDK timer is always available.
    prescalerVal = 0;
    for (int i = 0; i < 4; i++)
    {
        matchValues[i] = 0;
        matchModes[i] = 0;
    }
    irqFlags = 0;
    counterVal = 0;
    running = false;
}

void Timer::end()
{
    stop();
}

void Timer::prescaler(unsigned int factor)
{
    prescalerVal = factor;
}

unsigned int Timer::prescaler() const
{
    return prescalerVal;
}

void Timer::start()
{
    if (running) return;
    running = true;
    startRepeatingTimer();
}

void Timer::stop()
{
    if (!running) return;
    running = false;
    stopRepeatingTimer();
}

void Timer::restart()
{
    stop();
    counterVal = 0;
    start();
}

void Timer::reset()
{
    counterVal = 0;
}

unsigned int Timer::value() const
{
    return counterVal;
}

void Timer::value(unsigned int val)
{
    counterVal = val;
}

void Timer::interrupts()
{
    interruptEnabled = true;
}

void Timer::noInterrupts()
{
    interruptEnabled = false;
}

int Timer::flags() const
{
    return irqFlags;
}

void Timer::resetFlags()
{
    irqFlags = 0;
}

void Timer::resetFlag(TimerMatch match)
{
    irqFlags &= ~(1 << match);
}

void Timer::resetFlag(TimerCapture capture)
{
    irqFlags &= ~(16 << capture);
}

bool Timer::flag(TimerMatch match) const
{
    return irqFlags & (1 << match);
}

bool Timer::flag(TimerCapture capture) const
{
    return irqFlags & (16 << capture);
}

int Timer::flagMask(TimerMatch match)
{
    return 1 << match;
}

int Timer::flagMask(TimerCapture cap)
{
    return 16 << cap;
}

void Timer::matchMode(int channel, int mode)
{
    if (channel >= 0 && channel < 4)
        matchModes[channel] = mode;
}

int Timer::matchMode(int channel) const
{
    if (channel >= 0 && channel < 4)
        return matchModes[channel];
    return 0;
}

void Timer::match(int channel, unsigned int matchVal)
{
    if (channel >= 0 && channel < 4)
        matchValues[channel] = matchVal;
}

unsigned int Timer::match(int channel) const
{
    if (channel >= 0 && channel < 4)
        return matchValues[channel];
    return 0;
}

void Timer::captureMode(int channel, int mode)
{
    (void)channel;
    (void)mode;
    // Capture not emulated on RP2350
}

int Timer::captureMode(int channel) const
{
    (void)channel;
    return 0;
}

unsigned int Timer::capture(int channel) const
{
    (void)channel;
    return 0;
}

void Timer::pwmEnable(int channel)
{
    (void)channel;
    // PWM not emulated — use hardware_pwm directly on RP2350
}

void Timer::pwmDisable(int channel)
{
    (void)channel;
}

void Timer::counterMode(int mode, int clearMode)
{
    (void)mode;
    (void)clearMode;
    // Counter mode not emulated on RP2350
}

void Timer::matchModePinConfig(int channel, int mode)
{
    (void)channel;
    (void)mode;
}

bool Timer::is32bitTimer(void)
{
    return timerNum > 1;
}

bool Timer::getMatchChannelLevel(int channel)
{
    (void)channel;
    return false;
}

void Timer::setIRQPriority(uint32_t newPriority)
{
    (void)newPriority;
}

void Timer::_timerFired()
{
    // Find which match channels have RESET|INTERRUPT and set their flags
    for (int ch = 0; ch < 4; ch++)
    {
        if (matchModes[ch] & INTERRUPT)
        {
            irqFlags |= (1 << ch);
        }
    }

    if (!interruptEnabled)
        return;

    // Call the appropriate ISR handler
    switch (timerNum)
    {
        case TIMER16_0: TIMER16_0_IRQHandler(); break;
        case TIMER16_1: TIMER16_1_IRQHandler(); break;
        case TIMER32_0: TIMER32_0_IRQHandler(); break;
        case TIMER32_1: TIMER32_1_IRQHandler(); break;
    }
}

void Timer::startRepeatingTimer()
{
    // Find the first match channel with RESET mode to determine the period.
    // This is the standard usage pattern: one channel resets the timer and
    // generates an interrupt at a fixed interval.
    int resetChannel = -1;
    for (int ch = 0; ch < 4; ch++)
    {
        if ((matchModes[ch] & RESET) && matchValues[ch] > 0)
        {
            resetChannel = ch;
            break;
        }
    }

    if (resetChannel < 0)
    {
        // No RESET channel found — try any channel with INTERRUPT
        for (int ch = 0; ch < 4; ch++)
        {
            if ((matchModes[ch] & INTERRUPT) && matchValues[ch] > 0)
            {
                resetChannel = ch;
                break;
            }
        }
    }

    if (resetChannel < 0)
        return; // Nothing configured — no timer to start

    // Compute period in microseconds:
    // tick_period = (prescalerVal + 1) / SystemCoreClock seconds
    // match_period = matchValues[ch] * tick_period
    // match_period_us = matchValues[ch] * (prescalerVal + 1) * 1000000 / SystemCoreClock

    uint64_t periodUs = (uint64_t)matchValues[resetChannel]
                      * (uint64_t)(prescalerVal + 1)
                      * 1000000ULL
                      / (uint64_t)SystemCoreClock;

    if (periodUs == 0)
        periodUs = 1; // minimum 1 µs

    repeating_timer_t* rt = static_cast<repeating_timer_t*>(rpTimerPtr);
    rt->user_data = this;

    // Negative delay = period from end of callback to start of next (more reliable)
    add_repeating_timer_us(-(int64_t)periodUs, timerCallback, this, rt);
}

void Timer::stopRepeatingTimer()
{
    repeating_timer_t* rt = static_cast<repeating_timer_t*>(rpTimerPtr);
    cancel_repeating_timer(rt);
}

#endif // __SBLIB_TARGET_RP2350__
