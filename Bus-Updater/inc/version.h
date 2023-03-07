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

#ifndef VERSION_H_
#define VERSION_H_

#define BL_IDENTITY (0x1090) //!< boot loader Version 0.90, (change also APP_VERSION in BootloaderLoader's app_main.cpp)
#define UPDATER_MIN_MAJOR_VERSION (0) //!< minimum required major version of the Selfbus Updater we are talking to
#define UPDATER_MIN_MINOR_VERSION (90) //!< minimum required minor version of the Selfbus Updater we are talking to


#endif /* VERSION_H_ */
/** @}*/
