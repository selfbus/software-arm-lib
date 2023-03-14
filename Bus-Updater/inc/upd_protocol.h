/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_UPD_UDP_PROTOCOL_1 UPD/UDP protocol
 * @ingroup SBLIB_BOOTLOADER
 *
 * @brief   Definition of the UPD/UDP bootloader protocol
 *
 * @details Updater protocol:
 *   We use the @ref APCI_USERMSG_MANUFACTURER_0 and @ref APCI_USERMSG_MANUFACTURER_6 KNX telegrams for the update process.
 *
 *    -BYTES of the KNX telegram:
 *       - 8    command Number (@ref UPD_Command)
 *       - 9-x  command dependent
 *      .

 *    -@ref UPD_SEND_DATA
 *      - 9-   the actual data which will be copied into a RAM buffer for later use.
 *              The address of the RAM buffer will be automatically incremented.
 *              After a @ref UPD_PROGRAM or @ref UPD_UPDATE_BOOT_DESC the RAM buffer address will be reseted.
 *      .

 *    -@ref UPD_PROGRAM
 *      - 9-12 How many bytes of the RAM Buffer should be programmed. Be aware that the value needs to be one of the following
 *             256, 512, 1024, 4096 (required by the IAP of the LPC11xx mcu)
 *      - 13-16 Flash address the data should be programmed to
 *      - 16-19 The CRC of the data downloaded via the UPD_SEND_DATA commands. If the CRC does not match the
 *              programming, error is returned
 *      .
 *
 *    -@ref UPD_UPDATE_BOOT_DESC
 *      - 9-12 The CRC of the data downloaded via the UPD_SEND_DATA commands. If the CRC does not match a
 *             programming error is returned
 *      - 13 Which boot descriptor should be used
 *      .
 *
 *    -@ref UPD_REQ_DATA (not implemented)
 *
 *    -Workflow:
 *      -# unlock the device with @ref UPD_UNLOCK_DEVICE
 *      -# erase the address range which needs to be programmed (@ref UPD_ERASE_ADDRESSRANGE)
 *      -# download the data via @ref UPD_SEND_DATA telegrams
 *      -# program the transmitted data into the FLASH  (@ref UPD_PROGRAM)
 *      -# repeat the above steps until the whole application has been downloaded
 *      -# download the boot descriptor block via @ref UPD_SEND_DATA telegrams
 *      -# update the boot descriptor block so that the bootloader is able to start the new
 *         application (@ref UPD_UPDATE_BOOT_DESC)
 *      -# restart the mcu (@ref UPD_RESTART)
 *
 *
 * @{
 *
 * @file   upd_protocol.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2023
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef UPD_PROTOCOL_H_
#define UPD_PROTOCOL_H_

#include <sblib/types.h>
#include <sblib/internal/iap.h>
#include "boot_descriptor_block.h"

#define UID_LENGTH_USED (12)            //!< Length of the mcu's UID (guid) used by the PC Updater Tool
const uint8_t idxInvalidUPDCommand = 0; //!< Array index of the @ref UPD_INVALID command in @ref updCommands
const uint8_t updCommandCount = 25;     //!< Total number of @ref UPD_Code commands (used by @ref UPD_Command), adjust when new commands are introduced.

/**
 * Control commands for flash process with @ref APCI_USERMSG_MANUFACTURER_0 and @ref APCI_USERMSG_MANUFACTURER_6
 */
enum UPD_Code : uint8_t
{
    UPD_INVALID = 0x00,                     //!< Marks a invalid code
    UPD_SEND_DATA = 0xef,                   //!< Copy ((data[0] & 0x0f)-1) bytes to ramBuffer starting from address data[3] @note device must be unlocked
    UPD_PROGRAM = 0xee,                     //!< Copy count (data[3-6]) bytes from ramBuffer to address (data[7-10]) in flash buffer, crc in data[11-14] @note device must be unlocked
    UPD_UPDATE_BOOT_DESC = 0xed,            //!< Flash an application boot descriptor block @note device must be unlocked
    UPD_SEND_DATA_TO_DECOMPRESS = 0xec,     //!< Copy bytes from telegram (data) to ramBuffer with differential method @note device must be unlocked
    UPD_PROGRAM_DECOMPRESSED_DATA = 0xeb,   //!< Flash bytes from ramBuffer to flash with differential method @note device must be unlocked
    UPD_ERASE_COMPLETE_FLASH = 0xea,        //!< Erase the entire flash area excluding the bootloader itself @note device must be unlocked
    UPD_ERASE_ADDRESSRANGE = 0xe9,          //!< Erase flash from given start address to end address (start: data[3-6] end: data[7-10]) @note device must be unlocked
    UPD_REQ_DATA = 0xe8,                    //!< Return bytes from flash at given address? @note device must be unlocked @warning Not implemented
    UPD_DUMP_FLASH = 0xe7,                  //!< DUMP the flash of a given address range (data[0-3] - data[4-7]) to serial port of the mcu,
                                            //!< works only with DEBUG version of bootloader @note device must be unlocked
    UPD_REQUEST_STATISTIC = 0xdf,           //!< Return some statistic data for the active connection
    UPD_RESPONSE_STATISTIC = 0xde,          //!< Response for @ref UPD_STATISTIC_RESPONSE containing the statistic data
    UPD_SEND_LAST_ERROR = 0xdc,             //!< Response containing the last error

    UPD_UNLOCK_DEVICE = 0xbf,               //!< Unlock the device for operations, which are only allowed on a unlocked device
    UPD_REQUEST_UID = 0xbe,                 //!< Return the 12 byte shorten UID (GUID) of the mcu @note device must be unlocked
    UPD_RESPONSE_UID = 0xbd,                //!< Response for @ref UPD_REQUEST_UID containing the first 12 bytes of the UID
    UPD_APP_VERSION_REQUEST = 0xbc,         //!< Return address of AppVersion string
    UPD_APP_VERSION_RESPONSE = 0xbb,        //!< Response for @ref UPD_APP_VERSION_REQUEST containing the application version string
    UPD_REQUEST_BOOT_DESC = 0xba,           //!< Return the application boot descriptor block @note device must be unlocked
    UPD_RESPONSE_BOOT_DESC = 0xb9,          //!< Response for @ref UPD_REQUEST_BOOT_DESC containing the application boot descriptor block

    UPD_REQUEST_BL_IDENTITY = 0xb8,         //!< Return the bootloader's identity @note device must be unlocked
    UPD_RESPONSE_BL_IDENTITY = 0xb7,        //!< Response for @ref UPD_REQUEST_BL_IDENTITY containing the identity
    UPD_RESPONSE_BL_VERSION_MISMATCH = 0xb6, //!< Response for @ref UPD_REQUEST_BL_IDENTITY containing the minimum required major and minor version of Selfbus Updater
    UPD_SET_EMULATION = 0x01                //!<@warning Not implemented
};

/**
 * Defines the minimum and maximum number of data bytes for each @ref UPD_Code, excluding the @ref UPD_Code itself.
 */
const struct UPD_Command {
    UPD_Code code;
    uint8_t minBytes;
    uint8_t maxBytes;
} updCommands[updCommandCount] =
{
    {UPD_INVALID, 0, 0},     // needs to be always at index 0 because it's used as a invalid return value of code2UPDCommand, see @ref idxInvalidUPDCommand
    {UPD_SEND_DATA, 2, 254}, // at least byte index and one byte, max. 254 for extended frames support
    {UPD_PROGRAM, 10, 10},
    {UPD_UPDATE_BOOT_DESC, 8, 8},
    {UPD_SEND_DATA_TO_DECOMPRESS, 1, 254}, // max. 254 for extended frames support
    {UPD_PROGRAM_DECOMPRESSED_DATA, 4, 4},
    {UPD_ERASE_COMPLETE_FLASH, 0, 0},
    {UPD_ERASE_ADDRESSRANGE, 8, 8},
    {UPD_REQ_DATA, 3, 3}, // not implemented, maybe flash address (2 bytes ) and count (1 byte)?
    {UPD_DUMP_FLASH, 8, 8},
    {UPD_REQUEST_STATISTIC, 0, 0},
    {UPD_RESPONSE_STATISTIC, 4, 4},
    {UPD_SEND_LAST_ERROR, 1, 1},
    {UPD_UNLOCK_DEVICE, UID_LENGTH_USED, UID_LENGTH_USED},
    {UPD_REQUEST_UID, 0, 0},
    {UPD_RESPONSE_UID, UID_LENGTH_USED, UID_LENGTH_USED},
    {UPD_APP_VERSION_REQUEST, 1, 1},
    {UPD_APP_VERSION_RESPONSE, BL_ID_STRING_LENGTH - 1, BL_ID_STRING_LENGTH - 1},
    {UPD_REQUEST_BOOT_DESC, 0, 1},
    {UPD_RESPONSE_BOOT_DESC, 12, 12},
    {UPD_REQUEST_BL_IDENTITY, 8, 8},
    {UPD_RESPONSE_BL_IDENTITY, 12, 12},
    {UPD_RESPONSE_BL_VERSION_MISMATCH, 8, 8},
    {UPD_SET_EMULATION, 0xff, 0xff} // not implemented
};

/**
 * Control responses to @ref UPD_Command for flash process with @ref APCI_USERMSG_MANUFACTURER_0 and @ref APCI_USERMSG_MANUFACTURER_6
 */
enum UDP_State : uint8_t
{
    // UDP_IAP_ (IAP is "In Application Programming" of the MCU's flash
    // these results are defined in iap.h Selfbus sblib
    UDP_IAP_SUCCESS = 0x7f,                                 //!< IAP Command is executed successfully.
    UDP_IAP_INVALID_COMMAND = 0x7e,                         //!< IAP Invalid command.
    UDP_IAP_SRC_ADDR_ERROR = 0x7d,                          //!< IAP Source address is not on a word boundary.
    UDP_IAP_DST_ADDR_ERROR = 0x7c,                          //!< IAP Destination address is not on a correct boundary.
    UDP_IAP_SRC_ADDR_NOT_MAPPED = 0x7b,                     //!< IAP Source address is not mapped in the memory map. Count value is taken in to consideration where applicable.
    UDP_IAP_DST_ADDR_NOT_MAPPED = 0x7a,                     //!< IAP Destination address is not mapped in the memory map. Count value is taken in to consideration where applicable.
    UDP_IAP_COUNT_ERROR = 0x79,                             //!< IAP Byte count is not multiple of 4 or 256 | 512 | 1024 | 4096
    UDP_IAP_INVALID_SECTOR = 0x78,                          //!< IAP Sector number is invalid.
    UDP_IAP_SECTOR_NOT_BLANK = 0x77,                        //!< IAP Sector is not blank.
    UDP_IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION = 0x76, //!< IAP Command to prepare sector for write operation was not executed.
    UDP_IAP_COMPARE_ERROR = 0x75,                           //!< IAP Source and destination data is not same.
    UDP_IAP_BUSY = 0x74,                                    //!< IAP Flash programming hardware interface is busy.
    UDP_IAP_UNKNOWN = 0x73,                                 //!< IAP unknown @ref IAP_Status.

    UDP_UNKNOWN_COMMAND = 0x5f,              //!< received command is not defined
    UDP_CRC_ERROR = 0x5e,                    //!< CRC calculated on the device and by the PC Updater tool don't match
    UDP_ADDRESS_NOT_ALLOWED_TO_FLASH = 0x5d, //!< specifed address cannot be programmed
    UDP_SECTOR_NOT_ALLOWED_TO_ERASE = 0x5c,  //!< the specified sector cannot be erased
    UDP_RAM_BUFFER_OVERFLOW = 0x5b,          //!< internal buffer for storing the data would overflow
    UDP_WRONG_DESCRIPTOR_BLOCK = 0x5a,       //!< the boot descriptor block does not exist
    UDP_APPLICATION_NOT_STARTABLE = 0x59,    //!< the programmed application is not startable
    UDP_DEVICE_LOCKED = 0x58,                //!< the device is still locked
    UDP_UID_MISMATCH = 0x57,                 //!< UID sent to unlock the device is invalid
    UDP_ERASE_FAILED = 0x56,                 //!< page erase failed
    UDP_INVALID_DATA = 0x55,                 //!< data received is invalid
    UDP_NO_DATA = 0x54,                      //!< no data received in telegram
    UDP_FLASH_ERROR = 0x53,                  //!< page program (flash) failed
    UDP_PAGE_NOT_ALLOWED_TO_ERASE = 0x52,    //!< page not allowed to erase
    UDP_ADDRESS_RANGE_NOT_ALLOWED_TO_ERASE = 0x51, //!< address range not allowed to erase

    UDP_NOT_IMPLEMENTED = 0x02,              //!< this command is not yet implemented
    UDP_INVALID = 0x01                       //!< Unknown error
};

/**
 * Returns the @ref UPD_Command according to data
 *
 * @param code The code to convert
 * @return The @ref UPD_Command according to data
 */
UPD_Command code2UPDCommand(uint8_t code);

/**
 * Converts a @ref IAP_Status into a @ref UDP_State
 * @param iapState @ref IAP_Status to convert
 * @return the corresponding @ref UDP_State
 */
UDP_State iapResult2UDPState(IAP_Status iapState);

/**
 * Converts a @ref UPD_Command into "string" and sends it to the serial port
 *
 * @note Needs preprocessor macro @ref DUMP_TELEGRAMS_LVL1
 * @warning This function is only for debugging purposes.
 * @param cmd Command to send to the serial port.
 */
void updCommand2Serial(UPD_Command cmd);

/**
 * Converts a @ref UDP_State into "string" and sends it to the serial port
 * @param result
 * @note Needs preprocessor macro @ref DUMP_TELEGRAMS_LVL1
 * @warning This function is only for debugging purposes.
 */
void updResult2Serial(UDP_State result);



#endif /* UPD_PROTOCOL_H_ */
/** @}*/
