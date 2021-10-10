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
#include <sblib/interrupt.h>


#define SYSTICK_ENABLED             ((SysTick->CTRL &  SysTick_CTRL_ENABLE_Msk) == SysTick_CTRL_ENABLE_Msk)
#define SYSTICK_INTERRUPT_ENABLED   ((SysTick->CTRL &  SysTick_CTRL_TICKINT_Msk) == SysTick_CTRL_TICKINT_Msk)

// The number of milliseconds since processor start/reset
volatile unsigned int systemTime;

// The timers
static LPC_TMR_TypeDef* const timers[4] = { LPC_TMR16B0, LPC_TMR16B1, LPC_TMR32B0, LPC_TMR32B1 };

void delay(unsigned int msec)
{
#ifndef IAP_EMULATION
    // check that SysTick is enabled, this also reads and resets COUNTFLAG
    if ((!SYSTICK_ENABLED) && (!SYSTICK_INTERRUPT_ENABLED))
    {
        fatalError(); // no SysTick, no delay()
    }

    // if any interrupt is active, fall-back to delayMicroseconds() (waiting SysTick's)
    // otherwise "while" will end in an infinite loop
    if (isInsideInterrupt())
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
void delayMicroseconds(unsigned int usec)
{
    uint16_t lastSystemTickValue = SysTick->VAL; // get our start SysTickcount
    uint16_t sysTickValue;                       // use word access for SysTick register
    int elapsed;
    int ticksToWait = 1; // as fast as we can go

    if (usec > MIN_DELAY_MICROSECONDS)
    {
        if (usec > MAX_DELAY_MICROSECONDS)
        {
            fatalError(); //stop execution otherwise we can run into a overflow
        };
        ticksToWait = (usec - MIN_DELAY_MICROSECONDS) * (SystemCoreClock / 1000000);
        // ticksToWait = ((usec - MIN_DELAY_MICROSECONDS) * (SystemCoreClock / 1000)) / 1000; // this is more precise but also slower
    }

    while (ticksToWait > 0)
    {
        // don't use SysTick->CTRL COUNTFLAG, by reading and processing it
        // a undetected overflow can happen
        sysTickValue = SysTick->VAL;
        elapsed = lastSystemTickValue - sysTickValue;
        if (elapsed < 0)
        {
            elapsed += SysTick->LOAD;
        }
        ticksToWait -= elapsed;
        lastSystemTickValue = sysTickValue;
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
// TODO there is nothing like attachInterrupt() in the sblib, copy&paste error?
extern "C" void SysTick_Handler()
{
    ++systemTime;
}
