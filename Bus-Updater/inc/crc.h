/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_CRC Crc calculation
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Crc calculation
 * @details
 *
 * @{
 *
 * @file   crc.h
 * @author Deti Fliegl <deti@fliegl.de> Copyright (c) 2015
 * @author Martin Glueck <martin@mangari.org> Copyright (c) 2015
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#ifndef _CRC32_H_
#define _CRC32_H_

#ifdef __cplusplus
extern "C" {
#endif

unsigned int crc32 (unsigned int start, unsigned char * data, unsigned int count);

#ifdef __cplusplus
}
#endif

#endif

/** @}*/
