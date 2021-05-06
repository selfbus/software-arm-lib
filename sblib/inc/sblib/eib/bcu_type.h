/*
 *  bcu_type.h - BCU type definitions.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_bcu_type_h
#define sblib_bcu_type_h


//
// Set the BCU_TYPE from compiler defines
//

#if defined(BCU2) || BCU_TYPE == 0x20 || BCU_TYPE == 20
//  BCU 2, mask version 2.0
#   undef BCU_TYPE
#   define BCU_TYPE 0x20
#   define BCU_NAME BCU2
#   define MASK_VERSION 0x20

#elif defined(BIM112) || defined(BIM112_71) || BCU_TYPE == 0x701
//  BIM 112, mask version 7.1
#   undef BCU_TYPE
#   define BCU_TYPE 0x701
#   define BCU_NAME BIM112
#   define MASK_VERSION 0x701
#   define BIM112_TYPE 0x701 //!> The BIM-112 type, for comparison between a BIM 112 7.1 and other BCU's, do not change it

#elif defined(BIM112_75) || BCU_TYPE == 0x705
//  BIM 112, mask version 7.5
#   undef BCU_TYPE
#   define BCU_TYPE 0x705
#   define BCU_NAME BIM112_75
#   define MASK_VERSION 0x705
#   define BIM112_TYPE 0x705  //!> The BIM-112 type, for comparison between a BIM 112 7.5 and other BCU's, do not change it

#elif defined(SYSTEM_B) || BCU_TYPE == 0x7B0
//  SYSTEM B
#   undef BCU_TYPE
#   define BCU_TYPE 0x7B0
#   define BCU_NAME SYSTEM_B
#   define MASK_VERSION 0x7B0
#   define SYSTEM_B_TYPE 0x7B0  //!> For comparison between a SYSTEM_B and other BCU's, do not change it

#elif defined(BCU1) || defined(BCU1_12) || BCU_TYPE == 0x10 || BCU_TYPE == 10 || !defined(BCU_TYPE)
//  BCU 1, mask version 1.2
//  Also use BCU 1 if no BCU type is set
#   undef BCU_TYPE
#   define BCU_TYPE 0x10
#   define BCU_NAME BCU1
#   define MASK_VERSION 0x12

#else
//  Unknown BCU type
#   error "Unknown BCU type. Please add a compiler define: either BCU_TYPE with the correct value or one of the predefined BCU types - see sblib/eib/bcu_type.h for valid types"
#endif


#define BCU1_TYPE 0x10      //!> The BCU 1 type, for comparison between a BCU 1 or newer BCU's, do not change it
#define SYSTEMB_TYPE 0x7B0  //!> The SYSTEM_B type, for comparison for SYSTEM_B, do not change it


//
// Set BCU_TYPE specific parameters, like UserRam, UserEeprom, LoadControladdress, LoadStateaddress
//

#if BCU_TYPE == 0x10  /* BCU 1 */

    /** Start address of the user RAM when ETS talks with us. */
#   define USER_RAM_START_DEFAULT 0

    /** The size of the user RAM in bytes. */
#   define USER_RAM_SIZE 0x100
    /** How many bytes have to be allocated at the end of the RAM
        for shadowed values
    */
#   define USER_RAM_SHADOW_SIZE 3

    /** Start address of the user EEPROM when ETS talks with us. */
#   define USER_EEPROM_START 0x100

    /** The size of the user EEPROM in bytes. */
#   define USER_EEPROM_SIZE 256

#elif BCU_TYPE == 0x20  /* BCU 2 */

    /** Start address of the user RAM when ETS talks with us. */
#   define USER_RAM_START_DEFAULT 0

    /** The size of the user RAM in bytes. */
#   define USER_RAM_SIZE 0x100
    /** How many bytes have to be allocated at the end of the RAM
        for shadowed values
    */
#   define USER_RAM_SHADOW_SIZE 0

    /** Start address of the user EEPROM when ETS talks with us. */
#   define USER_EEPROM_START 0x100

    /** The size of the user EEPROM in bytes. */
#   define USER_EEPROM_SIZE 1024

    /** The start of the HighRam, will be ignored by us, its only needed for device programming */
#   define HIGH_RAM_START 0x0900

    /** The length of the HighRam */
#   define HIGH_RAM_LENGTH 0xBC

#elif BCU_TYPE == 0x701 || BCU_TYPE == 0x705 /* BIM 112, v7.1 and v7.5*/

    /** Address for load control */
#   define LOAD_CONTROL_ADDR 0x104

    /** Address for load state */
#   define LOAD_STATE_ADDR 0xb6e9

    /** Start address of the user RAM when ETS talks with us. */
#   define USER_RAM_START_DEFAULT 0x5FC

#   ifndef EXTRA_USER_RAM_SIZE
#       define EXTRA_USER_RAM_SIZE 0
#   endif

    /** The size of the user RAM in bytes. */
#   define USER_RAM_SIZE (0x304 + EXTRA_USER_RAM_SIZE)

    /** How many bytes have to be allocated at the end of the RAM for shadowed values */
#   define USER_RAM_SHADOW_SIZE 3

    /** Start address of the user EEPROM when ETS talks with us. */
#   define USER_EEPROM_START 0x3f00

    /** The size of the user EEPROM in bytes. */
#   ifndef __LPC11UXX__
#       define USER_EEPROM_SIZE       3072
#       define USER_EEPROM_FLASH_SIZE 4096
#   else
#       define USER_EEPROM_SIZE       3072  // was reduced from 4096 to 3072 to support LPC11Uxx. see commit 3194e058a850bdacb33cd068222647f1dbf19488
#       define USER_EEPROM_FLASH_SIZE 4096
#   endif

#elif BCU_TYPE == 0x7B0 /* SYSTEM_B */

    /** Address for load control */
#   define LOAD_CONTROL_ADDR 0x104

    /** Address for load state */
#   define LOAD_STATE_ADDR 0xb6e9

    /** Start address of the user RAM when ETS talks with us. */
#   define USER_RAM_START_DEFAULT 0x5FC

#   ifndef EXTRA_USER_RAM_SIZE
#       define EXTRA_USER_RAM_SIZE 0
#   endif

    /** The size of the user RAM in bytes. */
#   define USER_RAM_SIZE (0x304 + EXTRA_USER_RAM_SIZE)

    /** How many bytes have to be allocated at the end of the RAM for shadowed values */
#   define USER_RAM_SHADOW_SIZE 3

    /** Start address of the user EEPROM when ETS talks with us. */
#   define USER_EEPROM_START 0x3300

    /** The size of the user EEPROM in bytes. */
#   ifndef __LPC11UXX__
#       define USER_EEPROM_SIZE       3072
#       define USER_EEPROM_FLASH_SIZE 4096
#   else
#       define USER_EEPROM_SIZE       3072  // was reduced from 4096 to 3072 to support LPC11Uxx. see commit 3194e058a850bdacb33cd068222647f1dbf19488
#       define USER_EEPROM_FLASH_SIZE 4096
#   endif
#else


// BCU_TYPE contains an invalid value and no other BCU type define is set
#   error "Unsupported BCU type"
#endif

#ifndef USER_EEPROM_FLASH_SIZE
#   define USER_EEPROM_FLASH_SIZE USER_EEPROM_SIZE
#endif

/** End address of the user EEPROM +1, when ETS talks with us. */
#define USER_EEPROM_END (USER_EEPROM_START + USER_EEPROM_SIZE)


#endif /*sblib_bcu_type_h*/
