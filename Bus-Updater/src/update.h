/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_UPD_UDP_PROTOCOL_1 Selfbus UPD/UDP Bootloader protocol
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Definition of the Selfbus UPD/UDP protocol
 * @details Updater protocol:
 *   We miss-use the memory write EIB frames. Miss-use because we do not transmit the address in each request
 *   to have more frame left for the actual data transmission:
 *     BYTES of the EIB telegram:
 *       8    CMD Number (see enum below), this is data[2]
 *       9-x  CMD dependent
 *
 *    UPD_ERASE_SECTOR
 *      9    Number of the sector which should be erased
 *           if the erasing was successful a T_ACK_PDU will be returned, otherwise a T_NACK_PDU
 *    UPD_SEND_DATA
 *      9-   the actual data which will be copied into a RAM buffer for later use
 *           If the RAM buffer is not yet full a T_ACK_PDU will be returned, otherwise a T_NACK_PDU
 *           The address of the RAM buffer will be automatically incremented.
 *           After a Program or Boot Desc. update, the RAM buffer address will be reseted.
 *    UPD_PROGRAM
 *      9-12 How many bytes of the RMA Buffer should be programmed. Be aware that the value needs to be one of the following
 *           256, 512, 1024, 4096 (required by the IAP of the LPC11xx devices)
 *     13-16 Flash address the data should be programmed to
 *     16-19 The CRC of the data downloaded via the UPD_SEND_DATA commands. If the CRC does not match the
 *           programming, error is returned
 *    UPD_UPDATE_BOOT_DESC
 *    UPD_PROGRAM
 *      9-12 The CRC of the data downloaded via the UPD_SEND_DATA commands. If the CRC does not match the
 *           programming, error is returned
 *        13 Which boot block should be used
 *    UPD_REQ_DATA
 *      ???
 *    UPD_GET_LAST_ERROR
 *      Returns the reason why the last memory write PDU had a T_NACK_PDU
 *
 *    Workflow:
 *      - erase the sector which needs to be programmed (UPD_ERASE_SECTOR)
 *      - download the data via UPD_SEND_DATA telegrams
 *      - program the transmitted data to into the FLASH  (UPD_PROGRAM)
 *      - repeat the above steps until the whole application has been downloaded
 *      - download the boot descriptor block via UPD_SEND_DATA telegrams
 *      - update the boot descriptor block so that the bootloader is able to start the new
 *        application (UPD_UPDATE_BOOT_DESC)
 *      - restart the board (UPD_RESTART)
 *
 * @{
 *
 * @file   update.h
 * @author Stefan Haller Copyright (c) May 24, 2020
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef UPDATE_H_
#define UPDATE_H_

#include "boot_descriptor_block.h"

// #define RAM_BUFFER_SIZE FLASH_PAGE_SIZE    //!< Size in byte for the ram buffer
#define RAM_BUFFER_SIZE FLASH_SECTOR_SIZE  //!< \todo Why should we use 4096 byte buffer Size in byte for the ram buffer

#define RESET_DELAY_MS 500                 //!< Time in milliseconds a System reset should be delayed to
                                           //!< give the bcu enough time to send it's T_ACK_PDU
#define UID_LENGTH_USED 12                 //!< length of the mcu UID (guid) used by the PC Updater Tool

/**
 * @brief Request a reset of the mcu in x milliseconds
 * @param msec time in millisecond to wait before resetting the mcu.
 */
void restartRequest (unsigned int msec);

/**
 * @brief Returns the state of the Timeout timer to reset the mcu
 * @return true if restart request is expired and mcu should be reset
 */
bool restartRequestExpired(void);

/**
 * @brief Handles KNX memory requests which encapsulate our UPD/UDP protocol
 *
 * @param apciCmd   not used
 * @param sendTel   buffer for answer telegram
 * @param data      telegram buffer received from KNX bus
 * @return          always T_ACK_PDU, the real return value is encapsulated in bcu.sendTelegram[10-13]
 */
unsigned char handleMemoryRequests(int apciCmd, bool * sendTel, unsigned char * data);

void dumpFlashContent(AppDescriptionBlock * buffer);

/** @}*/
#endif /* UPDATE_H_ */
