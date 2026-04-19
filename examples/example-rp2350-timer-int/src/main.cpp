/*
 *  main.cpp - Timer interrupt blink example for RP2350.
 *
 *  Blinks the on-board LED (GP25) using the sblib Timer class emulation.
 *  The timer ISR toggles the LED every 500 ms — the same pattern as
 *  the LPC11xx example-int-blink.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"
#include <sblib/digital_pin.h>
#include <sblib/timer.h>
#include <sblib/types.h>

#define LED_PIN 25

/**
 * Handler for the timer interrupt.
 * Called automatically by the Timer emulation on RP2350.
 */
extern "C" void TIMER32_0_IRQHandler()
{
    // Toggle the LED
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));

    // Clear the timer interrupt flags
    timer32_0.resetFlags();
}

int main()
{
    stdio_init_all();

    pinMode(LED_PIN, OUTPUT);

    // Enable the timer interrupt
    timer32_0.interrupts();

    // Begin using the timer
    timer32_0.begin();

    // Let the timer count milliseconds
    timer32_0.prescaler((SystemCoreClock / 1000) - 1);

    // On match of MAT1, generate an interrupt and reset the timer
    timer32_0.matchMode(MAT1, RESET | INTERRUPT);

    // Match MAT1 when the timer reaches this value (in milliseconds)
    timer32_0.match(MAT1, 500);

    timer32_0.start();

    while (true)
    {
        // Sleep until the next interrupt
        __wfi();
    }
}
