/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_UPD_UDP_PROTOCOL_1 UPD/UDP protocol
 * @ingroup SBLIB_BOOTLOADER
 *
 * @brief   Definition of the UPD/UDP bootloader protocol
 *
 * @details Updater protocol:
 *   We miss-use the memory write EIB frames (@ref APCI_MEMORY_READ_PDU and @ref APCI_MEMORY_WRITE_PDU). Miss-use because we do not transmit the address in each request
 *   to have more frame left for the actual data transmission:
 *
 *    -BYTES of the KNX telegram:
 *       - 8    CMD Number (@ref UPD_Command), this is data[2]
 *       - 9-x  CMD dependent
 *       .
 *
 *    -@ref UPD_ERASE_SECTOR (deprecated, since v0.62 replaced by @ref UPD_ERASE_ADDRESSRANGE
 *      - 9    Number of the sector which should be erased
 *              if the erasing was successful a T_ACK_PDU will be returned, otherwise a T_NACK_PDU
 *      .
 *    -@ref UPD_SEND_DATA
 *      - 9-   the actual data which will be copied into a RAM buffer for later use
 *              If the RAM buffer is not yet full a T_ACK_PDU will be returned, otherwise a T_NACK_PDU
 *              The address of the RAM buffer will be automatically incremented.
 *              After a Program or Boot Desc. update, the RAM buffer address will be reseted.
 *      .
 *    -@ref UPD_PROGRAM
 *      - 9-12 How many bytes of the RMA Buffer should be programmed. Be aware that the value needs to be one of the following
 *             256, 512, 1024, 4096 (required by the IAP of the LPC11xx devices)
 *      - 13-16 Flash address the data should be programmed to
 *      - 16-19 The CRC of the data downloaded via the UPD_SEND_DATA commands. If the CRC does not match the
 *              programming, error is returned
 *      .
 *
 *    -@ref UPD_UPDATE_BOOT_DESC
 *
 *    -@ref UPD_PROGRAM
 *      - 9-12 The CRC of the data downloaded via the UPD_SEND_DATA commands. If the CRC does not match the
 *             programming, error is returned
 *      - 13 Which boot block should be used
 *      .
 *
 *    -@ref UPD_REQ_DATA (not implemented)
 *
 *    -@ref UPD_GET_LAST_ERROR
 *      - Returns the reason why the last memory write PDU had a T_NACK_PDU
 *      .
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
 *      -# restart the board (@ref UPD_RESTART)
 *
 *
 * @{
 *
 * @file   upd_protocol.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
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

/**
 * @brief Control commands for flash process with @ref APCI_MEMORY_READ_PDU and @ref APCI_MEMORY_WRITE_PDU
 *
 */
enum UPD_Command
{
    UPD_ERASE_SECTOR = 0,               //!< Erase flash sector number (data[3]) @note device must be unlocked
                                        //!< @note deprecated, use UPD_ERASE_ADDRESSRANGE instead
    UPD_SEND_DATA = 1,                  //!< Copy ((data[0] & 0x0f)-1) bytes to ramBuffer starting from address data[3] @note device must be unlocked
    UPD_PROGRAM = 2,                    //!< Copy count (data[3-6]) bytes from ramBuffer to address (data[7-10]) in flash buffer, crc in data[11-14] @note device must be unlocked
    UPD_UPDATE_BOOT_DESC = 3,           //!< Flash an application boot descriptor block @note device must be unlocked
    UPD_SEND_DATA_TO_DECOMPRESS = 4,    //!< Copy bytes from telegram (data) to ramBuffer with differential method @note device must be unlocked
    UPD_PROGRAM_DECOMPRESSED_DATA = 5,  //!< Flash bytes from ramBuffer to flash with differential method @note device must be unlocked

    UPD_ERASE_COMPLETE_FLASH = 7,       //!< Erase the entire flash area excluding the bootloader itself @note device must be unlocked
    UPD_ERASE_ADDRESSRANGE = 8,         //!< Erase flash from given start address to end address (start: data[3-6] end: data[7-10]) @note device must be unlocked

    UPD_REQ_DATA = 10,                  //!< Return bytes from flash at given address?  @note device must be unlocked
                                        //!<@warning Not implemented
    UPD_GET_LAST_ERROR = 20,            //!< Returns last error
    UPD_SEND_LAST_ERROR = 21,           //!< Response for @ref UPD_GET_LAST_ERROR containing the last error

    UPD_UNLOCK_DEVICE = 30,             //!< Unlock the device for operations, which are only allowed on a unlocked device
    UPD_REQUEST_UID = 31,               //!< Return the 12 byte shorten UID (GUID) of the mcu @note device must be unlocked
    UPD_RESPONSE_UID = 32,              //!< Response for @ref UPD_REQUEST_UID containing the first 12 bytes of the UID
    UPD_APP_VERSION_REQUEST = 33,       //!< Return address of AppVersion string
    UPD_APP_VERSION_RESPONSE = 34,      //!< Response for @ref UPD_APP_VERSION_REQUEST containing the application version string
    UPD_RESET = 35,                     //!< Reset the device @note device must be unlocked
    UPD_REQUEST_BOOT_DESC = 36,         //!< Return the application boot descriptor block @note device must be unlocked
    UPD_RESPONSE_BOOT_DESC = 37,        //!< Response for @ref UPD_REQUEST_BOOT_DESC containing the application boot descriptor block

    UPD_REQUEST_BL_IDENTITY = 40,       //!< Return the bootloader's identity @note device must be unlocked
    UPD_RESPONSE_BL_IDENTITY = 41,      //!< Response for @ref UPD_REQUEST_BL_IDENTITY containing the identity
    UPD_SET_EMULATION = 100             //!<@warning Not implemented
};

/**
 * @brief Control responses to @ref UPD_Command for flash process with @ref APCI_MEMORY_READ_PDU and @ref APCI_MEMORY_WRITE_PDU
 *
 */
enum UDP_State
{
    // UDP_IAP_ are in decimal (IAP is "In Application Programming" of the MCU's flash
    // these results are defined in iap.h Selfbus sblib
    UDP_IAP_SUCCESS = 0,                                 //!< IAP OK
    UDP_IAP_INVALID_COMMAND = 1,                         //!< IAP invalid command
    UDP_IAP_SRC_ADDR_ERROR = 2,                          //!< IAP source address error
    UDP_IAP_DST_ADDR_ERROR = 3,                          //!< IAP destination address error
    UDP_IAP_SRC_ADDR_NOT_MAPPED = 4,                     //!< IAP source address not mapped
    UDP_IAP_DST_ADDR_NOT_MAPPED = 5,                     //!< IAP destination address not mapped
    UDP_IAP_COUNT_ERROR = 6,                             //!< IAP count error
    UDP_IAP_INVALID_SECTOR = 7,                          //!< IAP invalid sector error
    UDP_IAP_SECTOR_NOT_BLANK = 8,                        //!< IAP sector not blank"
    UDP_IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION = 9, //!< IAP sector not prepared for write operation
    UDP_IAP_COMPARE_ERROR = 10,                          //!< IAP compare error
    UDP_IAP_BUSY = 11,                                   //!< IAP busy

    // following are in hexadecimal
    UDP_UNKNOWN_COMMAND = 0x100,              //!< received command is not defined
    UDP_CRC_ERROR = 0x101,                    //!< CRC calculated on the device and by the PC Updater tool don't match
    UDP_ADDRESS_NOT_ALLOWED_TO_FLASH = 0x102, //!< specifed address cannot be programmed
    UDP_SECTOR_NOT_ALLOWED_TO_ERASE = 0x103,  //!< the specified sector cannot be erased
    UDP_RAM_BUFFER_OVERFLOW = 0x104,          //!< internal buffer for storing the data would overflow
    UDP_WRONG_DESCRIPTOR_BLOCK = 0x105,       //!< the boot descriptor block does not exist
    UDP_APPLICATION_NOT_STARTABLE = 0x106,    //!< the programmed application is not startable
    UDP_DEVICE_LOCKED = 0x107,                //!< the device is still locked
    UDP_UID_MISMATCH = 0x108,                 //!< UID sent to unlock the device is invalid
    UDP_ERASE_FAILED = 0x109,                 //!< page erase failed

    // counting in hex so here is space for 0x10A-0x10F
    UDP_FLASH_ERROR = 0x110,                  //!< page program (flash) failed
    UDP_PAGE_NOT_ALLOWED_TO_ERASE = 0x111,    //!< page not allowed to erase
    UDP_ADDRESS_RANGE_NOT_ALLOWED_TO_ERASE = 0x112, //!< address range not allowed to erase
    UDP_NOT_IMPLEMENTED = 0xFFFF              //!< this command is not yet implemented
};

/**
 * @brief Converts the name of a UPD_Command to "string" and
 *        sends it over the serial port
 *
 * @note Needs preprocessor macro DUMP_TELEGRAMS_LVL1
 * @warning This function is only for debugging purposes.
 * @param cmd Command to send to the serial port.
 */
void updCommand2Serial(byte cmd);

void updResult2Serial(UDP_State result);



#endif /* UPD_PROTOCOL_H_ */
/** @}*/
