/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_KNX_TELEGRAM_HANDLER_1 KNX-Telegramhandler
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Handles @ref APCI_USERMSG_MANUFACTURER_0 and @ref APCI_USERMSG_MANUFACTURER_6 for the update process
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

/**
 * Handles KNX @ref APCI_USERMSG_MANUFACTURER_0 which encapsulates our UPD/UDP protocol
 *
 * @param sendBuffer The buffer to write the response into
 * @param data       data buffer received from KNX bus
 * @param size       size of data buffer
 * @return           whether to return a telegram, the real return values are encapsulated in sendBuffer[9..]
 */
bool handleApciUsermsgManufacturer(uint8_t * sendBuffer, uint8_t * data, uint32_t size);

/**
 * @brief Resets the UPD/UDP protocol ramBuffer and global variables to default
 */
void resetUPDProtocol(void);

/**
 * Handles deprecated KNX memory requests by sending the old @ref UPD_SEND_LAST_ERROR with old value of @ref UDP_NOT_IMPLEMENTED
 *
 * @param sendBuffer The buffer to write the response into
 * @return always true
 */
bool handleDeprecatedApciMemoryWrite(uint8_t * sendBuffer);


///\todo remove on release
// void testUpdProgram(uint32_t address, uint16_t count);
#endif /* UPDATE_H_ */
/** @}*/
