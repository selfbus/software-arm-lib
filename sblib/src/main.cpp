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

#include <sblib/internal/variables.h>
#include <sblib/eib/bcu_base.h>

/**
 * @brief Initializes the library.
 *        - Configures and starts the system timer to call SysTick_Handler once every 1 millisecond.
 */
static inline void lib_setup()
{
	SysTick_Config(SystemCoreClock / 1000);
	systemTime = 0;
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

void loop_noapp()
{
    waitForInterrupt();
};
