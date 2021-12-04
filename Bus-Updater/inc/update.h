/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_KNX_TELEGRAM_HANDLER_1 KNX-Telegramhandler
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Handles @ref APCI_MEMORY_READ_PDU and @ref APCI_MEMORY_WRITE_PDU for the update process
 * @details
 *
 * @{
 *
 * @file   update.h
 * @author Stefan Haller Copyright (c) May 24, 2020
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
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

#define RAM_BUFFER_SIZE FLASH_PAGE_SIZE    //!< Size in byte for the ram buffer
// #define RAM_BUFFER_SIZE FLASH_SECTOR_SIZE  //!< \done Why should we use 4096 byte buffer Size in byte for the ram buffer

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

/**
 * @brief Resets the UPD/UDP protocol ramBuffer and global variables to default
 */
void resetProtocol(void);

#endif /* UPDATE_H_ */
/** @}*/
