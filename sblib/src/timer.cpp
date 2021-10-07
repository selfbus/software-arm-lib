/*
 *  timer.cpp - Timer manipulation and time functions.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/timer.h>

#include <sblib/internal/variables.h>


// The number of milliseconds since processor start/reset
volatile unsigned int systemTime;

// The timers
static LPC_TMR_TypeDef* const timers[4] = { LPC_TMR16B0, LPC_TMR16B1, LPC_TMR32B0, LPC_TMR32B1 };

void delay(unsigned int msec)
{
#ifndef IAP_EMULATION
    // if any interrupt is active, fall-back to delayMicroseconds() (waiting SysTick's)
    // otherwise "while" will end in an infinite loop
    if (__get_IPSR() != 0x0)
    {
        unsigned int maxDelayMs = MAX_DELAY_MILLISECONDS;
        while (msec > maxDelayMs)
        {
            delayMicroseconds(maxDelayMs * 1000);
            msec -= (maxDelayMs);
        }

        delayMicroseconds(msec * 1000);

        return;
    }
#endif

    unsigned int lastSystemTime = systemTime;
    while (msec)
    {
        if (lastSystemTime == systemTime)
        {
            __WFI();
        }
        else
        {
            lastSystemTime = systemTime;
            --msec;
        }
    }
}

#ifndef IAP_EMULATION
/**
 * @fn      unsigned int getSysTickValue()
 * @brief   Returns the current value of the System tick timer.
 *
 * @return  current value of System tick timer
 */
ALWAYS_INLINE unsigned int getSysTickValue()
{
    return SysTick->VAL & 0xFFFFFF; // result in bits 0...23
}

/**
 * @fn              unsigned int getSysTicksElapsed(unsigned int&)
 * @brief           Returns the number of system ticks since lastSystemTickValue
 *
 * @post            lastSystemTickValue is updated to current system timer
 * @param[in,out]   lastSystemTickValue
 * @return          count of system ticks since lastSystemTickValue
 */
ALWAYS_INLINE unsigned int getSysTicksElapsed(unsigned int& lastSystemTickValue)
{
    // SysTick is counting down and is reset to SysTick->LOAD when
    // it reaches zero.
    int elapsed;
    unsigned int sysTickValue = getSysTickValue();
    elapsed = lastSystemTickValue - sysTickValue;
    lastSystemTickValue = sysTickValue;
    if (elapsed < 0)
        elapsed += SysTick->LOAD;
    return elapsed;
}

void delayMicroseconds(unsigned int usec)
{
    unsigned int lastSysTickValue = getSysTickValue();
    int ticksToWait = 1;  // as fast as we can go

    if (usec > MIN_DELAY_MICROSECONDS)
    {
        usec -= MIN_DELAY_MICROSECONDS; // usec's we lost till now @48MHz
        if (usec <= PROCESS_FAST_DELAY_MILLISECONDS)
        {
            // fast calculation of ticksToWait
            usec -= 1; // ~1usec to calculate the next step @48MHz
            ticksToWait = (usec * SystemCoreClock) / 1000000;
        }
        else
        {
            // precise calculation of ticksToWait
            ticksToWait = (double)usec * ((double)SystemCoreClock / 1000000);
            if (ticksToWait < 0)
            {
                // we run into a overflow => set ticksToWait to maximum
                ticksToWait = MAX_DELAY_MICROSECONDS * (SystemCoreClock / 1000000);
            }
            // subtract the system ticks needed for above calculation
            ticksToWait -= getSysTicksElapsed(lastSysTickValue);
        }
    }

    while (ticksToWait > 0)
    {
        ticksToWait -= getSysTicksElapsed(lastSysTickValue);
    }
}
#endif

//----- Class Timer -----------------------------------------------------------

Timer timer16_0(TIMER16_0);
Timer timer16_1(TIMER16_1);
Timer timer32_0(TIMER32_0);
Timer timer32_1(TIMER32_1);


Timer::Timer(byte aTimerNum)
{
    timerNum = aTimerNum;
    timer = timers[aTimerNum];
}

void Timer::begin()
{
    LPC_SYSCON->SYSAHBCLKCTRL |= 1 << (7 + timerNum);

    timer->EMR = 0;
    timer->MCR = 0;
    timer->CCR = 0;
}


void Timer::matchMode(int channel, int mode)
{
    int offset;

    // Configure the match control channel

    offset = channel * 3;
    timer->MCR &= ~(7 << offset);
    timer->MCR |= (mode & 7) << offset;

    // Configure the external match channel
    matchModePinConfig(channel, mode);
}

int Timer::matchMode(int channel) const
{
    int mode, offset;

    // Query the match control channel

    offset = channel * 3;
    mode = (timer->MCR >> offset) & 7;

    // Query the external match channel

    offset = channel << 1;
    mode |= (timer->EMR >> offset) & 0x30;

    return mode;
}

void Timer::captureMode(int channel, int mode)
{
    short offset = channel * 3;

    short val = (mode >> 6) & 3;
    if (mode & INTERRUPT)
        val |= 4;

    timer->CCR &= ~(7 << offset);
    timer->CCR |= val << offset;
}

int Timer::captureMode(int channel) const
{
    int mode = ((timer->CCR >> (channel * 3)) & 7) << 6;

    if (mode & 0x100)
    {
        mode &= 0xc0;
        mode |= INTERRUPT;
    }

    return mode;
}

void Timer::counterMode(int mode, int clearMode)
{
    int config = 0;

    if (mode & RISING_EDGE)
        config |= 0x1;
    if (mode & FALLING_EDGE)
        config |= 0x2;
    if (mode & CAP1)
        config |= 0x4;

    if (clearMode)
    {
        config |= 0x10;

        if (clearMode & (CAP1 | FALLING_EDGE))
            config |= 0x60;
        else if (clearMode & (CAP1 | RISING_EDGE))
            config |= 0x40;
        else if (clearMode & (CAP0 | FALLING_EDGE))
            config |= 0x20;
        // CAP0 | RISING_EDGE is 0x00
    }

    timer->CTCR = config;
}

//
// The original timer handler is used for performance reasons.
// Use attachInterrupt() to override this handler.
//
extern "C" void SysTick_Handler()
{
    ++systemTime;
}
