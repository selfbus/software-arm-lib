/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_BIM112_HELIOVENT_1 BCU BIM112 Maskversion 0x0705 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief
 * @details
 *
 * @{
 *
 * @file   app_heliovent.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef APP_HELIOVENT_H_
#define APP_HELIOVENT_H_

#include <sblib/types.h>
#include <sblib/eibMASK0705.h>
#include <sblib/eibSYSTEMB.h>



#ifdef VALLOX_07B0
    extern SYSTEMB bcu;
#else
    extern MASK0705 bcu;
#endif


/**
 * A communication object was updated from somebody via the EIB bus.
 *
 * @param objno - the ID of the communication object.
 */
void objectUpdated(int objno);

/**
 * The value of an input channel changed.
 *
 * @param channel - the input channel (0..)
 * @param value - the new value
 */
void inputChanged(int channel, int value);

/**
 * Handle the periodic functions.
 */
void checkPeriodic(void);

/**
 * Initialization of the application
 */
void initApplication(void);



#endif /* APP_HELIOVENT_H_ */
/** @}*/
