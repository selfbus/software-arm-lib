/**************************************************************************//**
 * @file   main.cpp
 * @brief  The Selfbus library's main implementation.
 *
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2014
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 *
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/main.h>

#include <sblib/interrupt.h>
#include <sblib/timer.h>

#include <sblib/eib/bcu_base.h>

#if defined(__SBLIB_TARGET_RP2350__)
#include "pico/stdlib.h"
#endif

/**
 * @brief Initializes the library.
 *        - On LPC: Configures and starts the system timer to call SysTick_Handler once every 1 millisecond.
 *        - On RP2350: Initializes the Pico SDK (stdio, etc.). Time runs via the hardware 64-bit µs counter.
 */
static inline void lib_setup()
{
#if defined(__SBLIB_TARGET_RP2350__)
    stdio_init_all();
#else
	SysTick_Config(SystemCoreClock / 1000);
#endif
}

/**
 * @brief The main of the Selfbus library.
 *        Calls setup(), loop() and optional loop_noapp() from the application.
 *
 * @return will never return
 */
int main()
{
    lib_setup();
    BcuBase* bcu = setup();

    while (1)
    {
        bcu->loop();
        if (bcu->applicationRunning())
            loop();
        else
            loop_noapp();
    }
}
