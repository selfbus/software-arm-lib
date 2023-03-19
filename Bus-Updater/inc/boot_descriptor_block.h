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
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
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
#include <version.h>

#ifdef DEBUG
    #define BL_FEATURES      0x8100             //!< Feature list of bootloader in Debug version
#else
    #define BL_FEATURES      0x0100             //!< Feature list of bootloader in Release version
#endif

/**
 * @def ALTERNATIVE_PROGRAMMING_BUTTON
 *
 * define to use the alternative programming button
 */
//#define ALTERNATIVE_PROGRAMMING_BUTTON


#define BL_ID_STRING         "[SB KNX BL ]"     //!< boot loader identity string for getAppVersion()
#define BL_ID_STRING_LENGTH  13                 //!< length of boot loader identity string

#define BOOT_BLOCK_DESC_SIZE FLASH_PAGE_SIZE    //!< 1 flash page
#define BOOT_BLOCK_COUNT 1                      //!< Number of applications supported (application description blocks)
                                                //!< @warning @ref BOOT_BLOCK_COUNT other's than 1 are not tested!

extern char bl_id_string[BL_ID_STRING_LENGTH]; //!< default bootloader identity "string" used in @ref getAppVersion()

/**
 * @struct AppDescriptionBlock
 * Application Description Block struct
 *
 */
typedef struct AppDescriptionBlock
{
    uint8_t * startAddress;         ///< start address of the application
    uint8_t * endAddress;           ///< end address of the application
    uint32_t crc;                   //!< crc from startAddress to end endAddress
    char * appVersionAddress;       //!< address of the APP_VERSION[20] @note string MUST start with "!AVP!@:" e.g. "!AVP!@:SBuid   1.00"
}__attribute__ ((aligned (BOOT_BLOCK_DESC_SIZE))) AppDescriptionBlock;

/**
 * Checks the application description block for valid
 *        start and end addresses
 *
 * @param block Application description block to check start and end address
 * @return      1 if block is valid, otherwise 0
 */
unsigned int checkApplication(AppDescriptionBlock * block);

/**
 * Returns the address of the @ref APP_VERSION_STRING of the application starting after the magic identifier !AVP!@:
 *
 * @param block Application description block to get the address
 * @return      if valid, pointer to buffer of application version string (length @ref BL_ID_STRING_LENGTH)
 *              otherwise bl_id_string
 */
char * getAppVersion(AppDescriptionBlock * block);

/**
 * Return start address of application
 *
 * @param block Application description block to get the start address from
 * @return      Start address of application in case of valid descriptor block,
 *              otherwise base address of firmware area, directly behind bootloader
 */
uint8_t * getFirmwareStartAddress(AppDescriptionBlock * block);

/**
 * Returns the first address of the bootloader image (_image_start symbol included by the linker)
 *
 * @return first address of the bootloader image
 */
uint8_t * bootLoaderFirstAddress(void);

/**
 * Returns the last address of the bootloader image (__image_end symbol included by the linker)
 *
 * @return last address of the bootloader image
 */
uint8_t * bootLoaderLastAddress(void);

/**
 * Returns the size of the bootloader image in bytes (__image_end - _image_start - 1)
 *
 * @return size of the bootloader image in bytes
 */
unsigned int bootLoaderSize(void);

/**
 * Returns the first address of the default flash memory (__base_Flash symbol included by the linker)
 *
 * @return first address of the the default flash memory
 */
uint8_t * flashFirstAddress(void);

/**
 * Returns the last address of the default flash memory (__top_Flash symbol included by the linker)
 *
 * @return last address of the the default flash memory
 */
uint8_t * flashLastAddress(void);

/**
 * Returns the size of the default flash memory in bytes (__top_Flash - __base_Flash - 1)
 *
 * @return size of the default flash memory in bytes
 */
unsigned int flashSize(void);

/**
 * Returns the first address of the application's firmware
 *
 * @return first address of the application's firmware
 */
uint8_t * applicationFirstAddress(void);

/**
 * Returns the first address of the boot descriptor block
 *
 * @return first address of the boot descriptor block
 */
uint8_t * bootDescriptorBlockAddress(void);

/**
 * Returns the page number of the boot descriptor block
 *
 * @return page number of the boot descriptor block
 */
unsigned int bootDescriptorBlockPage(void);

#endif /* BOOT_DESCRIPTOR_BLOCK_H_ */

/** @}*/
