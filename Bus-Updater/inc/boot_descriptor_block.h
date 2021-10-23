/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_BOOT_BLOCK_DESCRIPTOR Application Boot Block Descriptor
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Application Boot Block Descriptor
 * @details
 *
 * @{
 *
 * @file   boot_descriptor_block.h
 * @author Martin Glueck <martin@mangari.org> Copyright (c) 2015
 * @author Stefan Haller Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#ifndef BOOT_DESCRIPTOR_BLOCK_H_
#define BOOT_DESCRIPTOR_BLOCK_H_

// #include <sblib/platform.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef DEBUG
    #define BL_FEATURES      0x8100                 //!< Feature list of bootloader in Debug version
    #define APPLICATION_FIRST_SECTOR    0x7000      //!< where the application starts (BL size)  in Debug version
#else
    #define BL_FEATURES      0x0100                 //!< Feature list of bootloader in Release version
    #define APPLICATION_FIRST_SECTOR     0x3000     //!< where the application starts (BL size) in Release version
#endif

///\todo BL_IDENTITY move to bootloader.cpp or a new config.h file
#define BL_IDENTITY			 0x1056		        //!< boot loader Version 0.56
#define BL_ID_STRING         "[SB KNX BL ]"     //!< boot loader identity string for getAppVersion()
#define BL_ID_STRING_LENGTH  13                 //!< length of boot loader identity string


#define BOOT_BLOCK_DESC_SIZE FLASH_PAGE_SIZE    //!< 1 flash page
#define BOOT_BLOCK_COUNT 1                      //!< Number of applications supported (application description blocks)
#define BOOT_DSCR_ADDRESS  (APPLICATION_FIRST_SECTOR - (BOOT_BLOCK_DESC_SIZE * BOOT_BLOCK_COUNT)) //!< Descriptor uses last page of bootloader
#define BOOT_BLOCK_PAGE   ((APPLICATION_FIRST_SECTOR / BOOT_BLOCK_DESC_SIZE) - 1)


/**
 * @struct AppDescriptionBlock
 * @brief Application Description Block struct
 *
 */
typedef struct
{
    unsigned int startAddress;          ///< start address of the application
    unsigned int endAddress;            ///< end address of the application
    unsigned int crc;                   //!< crc from startAddress to end endAddress
    unsigned int appVersionAddress;     //!< address of the APP_VERSION[20] string MUST start with "!AVP!@:" e.g. "!AVP!@:SBuid   1.00";
}__attribute__ ((aligned (256))) AppDescriptionBlock; // \TODO replace 256 with BOOT_BLOCK_DESC_SIZE, including platform.h fails, not sure why



__attribute__((unused)) static unsigned char bl_id_string[BL_ID_STRING_LENGTH] = BL_ID_STRING; // actually it's used in getAppVersion,
                                                                                               // this is just to suppress compiler warning

/**
 * @brief Checks the application description block for valid
 *        start and end addresses
 *
 * @param block Application description block to check start and end address
 * @return      1 if block is valid, otherwise 0
 */
unsigned int checkApplication(AppDescriptionBlock * block);

/**
 * @brief Returns the address of function getAppversion() from the application
 *
 * @param block Application description block to get the address
 * @return      if valid, pointer to buffer of application version string (length BL_ID_STRING_LENGTH)
 *              otherwise bl_id_string
 */
unsigned char * getAppVersion(AppDescriptionBlock * block);

/**
 * @brief Return start address of application
 *
 * @param block Application description block to get the start address from
 * @return      Start address of application in case of valid descriptor block,
 *              otherwise base address of firmware area, directly behind bootloader
 */
unsigned char * getFirmwareStartAddress(AppDescriptionBlock * block);

unsigned int bootLoaderFirstAddress(void);
unsigned int bootLoaderLastAddress(void);
unsigned int bootLoaderSize(void);

unsigned int flashFirstAddress(void);
unsigned int flashLastAddress(void);
unsigned int flashSize(void);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_DESCRIPTOR_BLOCK_H_ */

/** @}*/
