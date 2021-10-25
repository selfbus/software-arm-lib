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

#include <sblib/platform.h>
/// \todo get rid of APPLICATION_FIRST_SECTOR, maybe somehow using bootLoaderLastAddress()
#ifdef DEBUG
    #define BL_FEATURES      0x8100             //!< Feature list of bootloader in Debug version
    #define APPLICATION_FIRST_SECTOR    0x7000  //!< where the application starts (BL size)  in Debug version
#else
    #define BL_FEATURES      0x0100             //!< Feature list of bootloader in Release version
    #define APPLICATION_FIRST_SECTOR    0x3000  //!< where the application starts (BL size) in Release version
#endif

///\todo move BL_IDENTITY to bootloader.cpp or a new config.h file
#define BL_IDENTITY			 0x1056		        //!< boot loader Version 0.56
#define BL_ID_STRING         "[SB KNX BL ]"     //!< boot loader identity string for getAppVersion()
#define BL_ID_STRING_LENGTH  13                 //!< length of boot loader identity string


#define BOOT_BLOCK_DESC_SIZE FLASH_PAGE_SIZE    //!< 1 flash page
#define BOOT_BLOCK_COUNT 1                      //!< Number of applications supported (application description blocks)
#define BOOT_DSCR_ADDRESS  (APPLICATION_FIRST_SECTOR - (BOOT_BLOCK_DESC_SIZE * BOOT_BLOCK_COUNT)) //!< Descriptor uses last page of bootloader
#define BOOT_BLOCK_PAGE   ((APPLICATION_FIRST_SECTOR / BOOT_BLOCK_DESC_SIZE) - 1) //!< flash page number of the application description block


extern unsigned char bl_id_string[BL_ID_STRING_LENGTH]; //!< default bootloader identity "string" used in @ref getAppVersion()

/**
 * @struct AppDescriptionBlock
 * @brief Application Description Block struct
 *
 */
typedef struct AppDescriptionBlock
{
    unsigned int startAddress;          ///< start address of the application
    unsigned int endAddress;            ///< end address of the application
    unsigned int crc;                   //!< crc from startAddress to end endAddress
    unsigned int appVersionAddress;     //!< address of the APP_VERSION[20] @note string MUST start with "!AVP!@:" e.g. "!AVP!@:SBuid   1.00"
}__attribute__ ((aligned (BOOT_BLOCK_DESC_SIZE))) AppDescriptionBlock;

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

/**
 * @brief returns the first address of the bootloader image (_image_start symbol included by the linker)
 *
 * @return first address of the bootloader image
 */
unsigned int bootLoaderFirstAddress(void);

/**
 * @brief returns the last address of the bootloader image (__image_end symbol included by the linker)
 *
 * @return last address of the bootloader image
 */
unsigned int bootLoaderLastAddress(void);

/**
 * @brief returns the size of the bootloader image in bytes (__image_end - _image_start - 1)
 *
 * @return size of the bootloader image in bytes
 */
unsigned int bootLoaderSize(void);

/**
 * @brief returns the first address of the default flash memory (__base_Flash symbol included by the linker)
 *
 * @return first address of the the default flash memory
 */
unsigned int flashFirstAddress(void);

/**
 * @brief returns the last address of the default flash memory (__top_Flash symbol included by the linker)
 *
 * @return last address of the the default flash memory
 */
unsigned int flashLastAddress(void);

/**
 * @brief returns the size of the default flash memory in bytes (__top_Flash - __base_Flash - 1)
 *
 * @return size of the default flash memory in bytes
 */
unsigned int flashSize(void);


#endif /* BOOT_DESCRIPTOR_BLOCK_H_ */

/** @}*/
