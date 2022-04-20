/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_BIM112_HELIOVENT_1 BCU BIM112 Maskversion 0x0705 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief
 * @details
 *
 * @{
 *
 * @file   config.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/**
 * @def HELIOS_0705
 * @brief Mask 0x0705  Helios KWL-EB
 */
// #define HELIOS_0705

/**
 * @def MDT_TSS_0705
 * @brief Mask 0x0705  MDT BE-06001.02 Universal Interface 6-fold, flush mounted, Contact inputs, ApplicationProgram: M-0083_A-00B4-32-3919
 */
//#define MDT_TSS_0705

/**
 * @def MDT_LED_0705
 * @brief Mask 0x0705  MDT AKD-0424R.02 LED Controller 4 Ch/RGBW, MDRC, ApplicationProgram: M-0083_A-0040-24-1835
 */
//#define MDT_LED_0705

/**
 * @def MDT_GLASS_0705
 * @brief Mask 0x0705  MDT BE-GT2Tx.01 Glass Push Button II Smart with Temperature sensor
 */
#define MDT_GLASS_0705

/**
 * @def VALLOX_07B0
 * @brief Mask 0x07B0  Vallox MV KNX-Bus-Unit
 */
//#define VALLOX_07B0

#endif /* CONFIG_H_ */
/** @}*/
