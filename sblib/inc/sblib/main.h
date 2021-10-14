/**************************************************************************//**
 * @file   main.h
 * @brief  Functions for applications using the Selfbus library.
 *
 * @note   Your Application must at least implement following functions:
 *         - void setup()
 *         - void loop()
 * @par
 * @note   Optional your application can implement following functions:
 *         - void loop_noapp()
 *         - void loop_debug()
 *
 * @warning All functions must return after their work is done.
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

#ifndef sblib_main_h
#define sblib_main_h

#define WEAK __attribute__ ((weak))

/**
 * @brief This function is called by the Selfbus's library main
 *        when the processor is started or reset.
 *
 * @note  You must implement this function in your code.
 */
void setup();

/**
 * @brief This function is called in a loop by the Selfbus's library main.
 *        This is where your main work shall happen.
 *
 * @note  You must implement this function in your code.
 */
void loop();

/**
 * @brief While no KNX-application is loaded,
 *        this function is called in a loop by the Selfbus's library main.
 *        Implementation is optional.
 */
WEAK void loop_noapp();

#endif /*sblib_main_h*/
