/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_BOOTLOADER_MAIN Bootloader
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Bootloader version configuration
 * @details 
 *
 *
 * @{
 *
 * @file   version.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef SB_BOOTLOADER_VERSION_H_
#define SB_BOOTLOADER_VERSION_H_

#include <stdint.h>

constexpr uint8_t BOOTLOADER_MAJOR_VERSION = 1;  //!< Bootloader major version @note change also in BootloaderLoader's app_main.cpp
constexpr uint8_t BOOTLOADER_MINOR_VERSION = 10; //!< Bootloader minor version @note change also in BootloaderLoader's app_main.cpp

constexpr uint8_t UPDATER_MIN_MAJOR_VERSION = 1; //!< minimum required major version of the Selfbus Updater we are talking to
constexpr uint8_t UPDATER_MIN_MINOR_VERSION = 0; //!< minimum required minor version of the Selfbus Updater we are talking to

#endif /* SB_BOOTLOADER_VERSION_H_ */
/** @}*/
