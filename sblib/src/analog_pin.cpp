/*
 *  analog_pin.cpp - Functions for analog I/O
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 * last change: Horst Rauch
 *  update ADC clock to max clock to speed up the conversion
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/analog_pin.h>

#include <sblib/platform.h>

// ADC conversion complete
#define ADC_DONE  0x80000000

// ADC overrun
#define ADC_OVERRUN  0x40000000

// Start ADC now
#define ADC_START_NOW  (1 << 24)

// Clock for AD conversion, max sample rate of 400ksample/s,
// adc uses 11 clock cycles per conversion
#define ADC_CLOCK  4400000

// for any ADC read from bus or app the BCU has only the bus voltage channel implemented
int KNXChannelToADCChannel [] = { -1, AD7, -1,-1,-1, -1, -1,-1};
int ADCChannelToPIO [] = { PIN_AD0, PIN_AD1, PIN_AD2, PIN_AD3, PIN_AD4, PIN_AD5, PIN_AD6, PIN_AD7 };

int ADCtoPIO (int channel)
{
	return ADCChannelToPIO[channel];
}

int KNXtoADC (int channel)
{
	return KNXChannelToADCChannel[channel];
}



void analogBegin()
{
    // Disable power down bit to the ADC block.
    LPC_SYSCON->PDRUNCFG &= ~(1<<4);

    // Enable AHB clock to the ADC.
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<13);
#ifndef IAP_EMULATION
    LPC_ADC->CR = ((SystemCoreClock / LPC_SYSCON->SYSAHBCLKDIV) / ADC_CLOCK - 1) << 8;
#endif
}


void analogEnd()
{
    // Enable power down bit to the ADC block.
    LPC_SYSCON->PDRUNCFG |= 1<<4;

    // Disable AHB clock to the ADC.
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<13);
}

static unsigned int analogPoll(int channel)
{
#ifdef IAP_EMULATION
    return 0;
#endif
    LPC_ADC->CR &= 0xffffff00;
    LPC_ADC->DR[channel]; // read the channel to clear the "done" flag

    LPC_ADC->CR |= (1 << channel) | ADC_START_NOW; // start the ADC reading

    unsigned int regVal;
    do
    {
        regVal = LPC_ADC->DR[channel];
    }
    while (!(regVal & ADC_DONE));

    LPC_ADC->CR &= 0xf8ffffff;  // Stop ADC
    return regVal;
}

int analogRead(int channel)
{
    unsigned int regVal = analogPoll(channel);
    // This bit is 1 if the result of one or more conversions was lost and
    // overwritten before the conversion that produced the result.
    if (regVal & ADC_OVERRUN)
        return 0;

    return (int)(regVal >> 6) & 0x3ff;
}

int analogValidRead(int channel)
{
    unsigned int regVal = analogPoll(channel);
    // This bit is 1 if the result of one or more conversions was lost and
    // overwritten before the conversion that produced the result.
    if (regVal & ADC_OVERRUN)
        return -1;

    return (int)(regVal >> 6) & 0x3ff;
}
