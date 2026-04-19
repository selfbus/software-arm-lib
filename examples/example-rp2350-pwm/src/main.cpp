/*
 *  main.cpp - PWM blink (breathing LED) example for RP2350.
 *
 *  Fades the on-board LED (GP25) up and down using hardware PWM.
 *  On the RP2350, PWM is accessed via the Pico SDK hardware_pwm module
 *  rather than the sblib Timer class (which emulates LPC timer match mode).
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <sblib/timer.h>

#define LED_PIN 25

int main()
{
    stdio_init_all();

    // Configure GP25 for PWM
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(LED_PIN);
    uint channel = pwm_gpio_to_channel(LED_PIN);

    pwm_set_wrap(slice, 1000);         // 0-1000 duty cycle range
    pwm_set_chan_level(slice, channel, 0);
    pwm_set_enabled(slice, true);

    int duty = 0;
    int step = 5;

    while (true)
    {
        duty += step;
        if (duty >= 1000 || duty <= 0)
            step = -step;

        pwm_set_chan_level(slice, channel, duty);
        delay(5);
    }
}
