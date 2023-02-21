/**************************************************************************//**
 * @file   main.h
 * @brief  Functions for applications using the Selfbus library.
 *
 * @note   Your Application must implement following functions:
 *         - void setup()
 *         - void loop()
 *         - void loop_noapp()
 *
 * @warning All functions must return after their work is done.
 *
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2014
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
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

#include <sblib/eib/bcu_base.h>

/**
 * @brief This function is called by the Selfbus's library main
 *        when the processor is started or reset.
 *
 * @note  You must implement this function in your code.
 */
BcuBase* setup();

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
 *
 * @note  You must implement this function in your code.
 */
void loop_noapp();

#endif /*sblib_main_h*/
