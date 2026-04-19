/*
 *  analog_pin_rp2350.cpp - ADC functions for RP2350 using Pico SDK.
 *
 *  The RP2350 has a 12-bit ADC with 5 inputs (GPIO26-29 + internal temp).
 *  Results are scaled to 10-bit (0-1023) for API compatibility with
 *  the LPC11xx version.
 *
 *  Channel mapping:
 *    Channel 0 → GPIO26 (ADC0)
 *    Channel 1 → GPIO27 (ADC1)
 *    Channel 2 → GPIO28 (ADC2)
 *    Channel 3 → GPIO29 (ADC3)
 *    Channel 4 → Internal temperature sensor
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#ifdef __SBLIB_TARGET_RP2350__

#include <sblib/analog_pin.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"

static bool adcInitialized = false;
static bool channelInitialized[5] = {false};

void analogBegin()
{
    if (!adcInitialized)
    {
        adc_init();
        adcInitialized = true;
    }
}

void analogEnd()
{
    // Pico SDK ADC has no explicit power-down; reset state
    adcInitialized = false;
    for (int i = 0; i < 5; i++)
        channelInitialized[i] = false;
}

static int analogPoll(int channel)
{
    if (channel < 0 || channel > 4)
        return -1;

    // Initialize GPIO for ADC if not done yet (channels 0-3 = GPIO26-29)
    if (channel < 4 && !channelInitialized[channel])
    {
        adc_gpio_init(26 + channel);
        channelInitialized[channel] = true;
    }
    else if (channel == 4 && !channelInitialized[4])
    {
        adc_set_temp_sensor_enabled(true);
        channelInitialized[4] = true;
    }

    adc_select_input(channel);
    uint16_t raw = adc_read(); // 12-bit result (0-4095)

    // Scale 12-bit to 10-bit for LPC11xx API compatibility
    return raw >> 2;
}

int analogRead(int channel)
{
    int val = analogPoll(channel);
    return (val < 0) ? 0 : val;
}

int analogValidRead(int channel)
{
    return analogPoll(channel);
}

#endif // __SBLIB_TARGET_RP2350__
