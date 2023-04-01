/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_KNX_TELEGRAM_HANDLER_1 KNX-Telegramhandler
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Handles @ref APCI_USERMSG_MANUFACTURER_0 and @ref APCI_USERMSG_MANUFACTURER_6 for the update process
 * @details
 *
 * @{
 *
 * @file   update.cpp
 * @author Martin Glueck <martin@mangari.org> Copyright (c) 2015
 * @author Stefan Haller Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2023
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#include <cstring>
#include <sblib/eib.h>
#include <sblib/eib/bus.h>
#include <sblib/eib/knx_tpdu.h>
#include <sblib/eib/apci.h>
#include <sblib/internal/iap.h>
#include <sblib/io_pin_names.h>

#include "upd_protocol.h"
#include "flash.h"
#include "bcu_updater.h"
#include "crc.h"
#include "update.h"
#include "dump.h"

#if defined(DEBUG)
#   include <sblib/serial.h>
#   include "intelhex.h"
#endif

#ifdef DECOMPRESSOR
#   include "decompressor.h"
#endif

#ifdef DECOMPRESSOR
    static Decompressor decompressor((AppDescriptionBlock*) bootDescriptorBlockAddress()); //!< get application base address from boot descriptor
#endif


#define DEVICE_LOCKED   ((unsigned int ) 0x5AA55AA5)     //!< magic number for device is locked and can't be flashed
#define DEVICE_UNLOCKED ((unsigned int ) ~DEVICE_LOCKED) //!< magic number for device is unlocked and flashing is allowed

static uint8_t __attribute__ ((aligned (FLASH_RAM_BUFFER_ALIGNMENT))) ramBuffer[RAM_BUFFER_SIZE]; //!< RAM buffer used for flash operations

// Try to avoid direct access to these global variables.
// It's better to use their get, set and reset functions
static unsigned int deviceLocked = DEVICE_LOCKED;   //!< current device locking state @note Better use GetDeviceUnlocked() & setDeviceLockState()
static unsigned int ramLocation = 0;                //!< current location of the ramBuffer processed
static unsigned int bytesReceived = 0;              //!< number of bytes received by UPD_SEND_DATA since last reset()
static unsigned int bytesFlashed = 0;               //!< number of bytes flashed by UPD_PROGRAM since last reset()

extern BcuUpdate bcu;

/**
 * Converts a unsigned int into a 4 byte long provided buffer
 * @details A direct cast does not work due to possible miss aligned addresses.
 *          therefore a good old conversion has to be performed
 *
 * @param buffer in the 4 first bytes of buffer the result will be stored
 * @param val    the unsigned int to be converted
 * @warning function doesn't perform any sanity-checks on the provided buffer
 */
void uInt32ToStream(uint8_t * buffer, unsigned int val);

/**
 * Send the flash content from startAddress to endAddress
 *        in Intel(R) hex file format over serial port
 *
 * @param startAddress
 * @param endAddress
 */
#if defined(DEBUG)
void dumpFlashContent(uint8_t * startAddress, uint8_t * endAddress)
{
    if (startAddress > endAddress)
    {
        uint8_t * temp = startAddress;
        startAddress = endAddress;
        endAddress = temp;
    }

    if (startAddress < flashFirstAddress())
    {
        startAddress = flashFirstAddress();
    }

    if (endAddress > flashLastAddress())
    {
        endAddress = flashLastAddress();
    }

    dumpToSerialinIntelHex(&serial, startAddress, endAddress - startAddress + 1);
}
#endif

/**
 * Converts a 4 byte long provided buffer into a unsigned integer
 * @details A direct cast does not work due to possible miss aligned addresses.
 *          therefore a good old conversion has to be performed
 *
 * @param buffer data to convert
 * @return to unsigned int converted value of the 4 first bytes of buffer
 * @warning function doesn't perform any sanity-checks on the provided buffer
 */
unsigned int streamToUIn32(uint8_t * buffer)
{
    return ((unsigned int)(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]));
}

/**
 * @brief Converts a 4 byte long provided buffer into a pointer
 *
 * @param buffer data to convert
 * @return first 4 bytes of buffer converted to a pointer
 * @warning function doesn't perform any sanity-checks on the provided buffer
 */
inline uint8_t * streamToPtr(uint8_t * buffer)
{
    return FLASH_BASE_ADDRESS + streamToUIn32(buffer);
}

///\todo implement universal numberToStream by providing the size as parameter
///      and delete uInt32ToStream + uShort16ToStream
void uInt32ToStream(uint8_t * buffer, unsigned int val)
{
    buffer[3] = (byte)(val >> 24);
    buffer[2] = (byte)(val >> 16);
    buffer[1] = (byte)(val >> 8);
    buffer[0] = (byte)(val & 0xff);
}

/**
 * @brief Converts a pointer to a 4 byte long and writes it to buffer
 *
 * @param buffer memory area to receive converted value
 * @param val pointer to convert and write
 * @warning function doesn't perform any sanity-checks on the provided buffer
 */
inline void ptrToStream(uint8_t * buffer, uint8_t * val)
{
    uInt32ToStream(buffer, val - FLASH_BASE_ADDRESS);
}

void uShort16ToStream(uint8_t * buffer, unsigned int val)
{
    buffer[1] = (byte)(val >> 8);
    buffer[0] = (byte)(val & 0xff);
}

uint16_t streamToUShort16(uint8_t * buffer)
{
    return ((uint16_t)(buffer[1] << 8 | buffer[0]));
}

/**
 * Prepares a UPD/UDP telegram with command/response and number of bytes
 *        the return telegram will have.
 *
 * @param count Number of bytes the return telegram shall have
 * @param cmd   UPD/UDP command/response to set the return telegram
 */
static void prepareReturnTelegram(uint8_t * retTelegram, unsigned int count, unsigned char cmd)
{
    count += 2; // +1 byte because counting starts including retTelegram[7] (KNX Spec 2.1 3/3/3 2.1 NPDU p.6)
                // +1 byte for cmd (retTelegram[8])
    if (count > 0x0f) ///\todo maybe won't work with extended frames
    {
#ifdef DEBUG
        fatalError();
#else
        count &= 0x0f; // make sure not to mess up with routing count in release-build
#endif
    }
    retTelegram[5] = 0x60 + count; // routing count in high nibble + response length in low nibble
    setApciCommand(retTelegram, APCI_USERMSG_MANUFACTURER_6, 0);
    retTelegram[8] = cmd;
}

/**
 * Returns the unlocked status of the device
 *
 * @return true if device is unlocked otherwise false
 */
static bool getDeviceUnlocked()
{
    d3(
        if (deviceLocked != DEVICE_UNLOCKED)
        {
            d3(serial.print("-->DEVICE_LOCKED"));
        }
    );
    return (deviceLocked == DEVICE_UNLOCKED);
}

/**
 * Sets the device lock state.
 *
 * @param newDeviceLockState The new device lock state
 */
static void setDeviceLockState(unsigned int newDeviceLockState)
{
    deviceLocked = newDeviceLockState;
    d3(
        if (deviceLocked == DEVICE_UNLOCKED)
        {
            d3(serial.print("-->DEVICE_UNLOCKED"));
        }
        else
        {
            d3(serial.print("-->DEVICE_LOCKED"));
        }
    );
}

/**
 * Returns the state of the programming button
 *
 * @return true if programming mode is active, otherwise false
 */
static bool getProgButtonState()
{
    bool state = (((BcuUpdate &) bcu).programmingMode());
    d3(
        if (state)
        {
            serial.print("-->progButton pressed");
        }
        else
        {
            serial.print("-->progButton NOT pressed");
        }
    );
    return (state);
}

/**
 * Sets lastError and prepares the @ref UPD_SEND_LAST_ERROR response telegram
 *
 * @param errorToSet The error to set
 */
static void setLastError(UDP_State errorToSet)
{
    prepareReturnTelegram(bcu.sendTelegram, 1, UPD_SEND_LAST_ERROR);
    bcu.sendTelegram[9] = errorToSet;
}

void resetUPDProtocol(void)
{
    ramLocation = 0;
    bytesReceived = 0;
    bytesFlashed = 0;
    dump2(serial.println("resetUPDProtocol"));
}

/**
 * Handles the @ref UPD_UNLOCK_DEVICE command.
 * The device is unlocked if the provided UID (guid) is valid.
 *
 * @param data    buffer for the UID
 * @param size    size of the buffer
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if device is unlocked, otherwise @ref UDP_UID_MISMATCH or a @ref UDP_State.
 * @return        always T_ACK_PDU
 */
static unsigned char updUnlockDevice(uint8_t * data, uint32_t size)
{
    // we need to ensure that only authorized operators can
    // update the application
    // as a simple method we use the unique ID of the CPU itself
    // only if this UID (GUID) is known, the device will be unlocked
    if ((UID_LENGTH_USED != size))
    {
        setLastError(UDP_UID_MISMATCH);
        dline(" size mismatch");
        return (T_ACK_PDU);
    }

    byte uid[IAP_UID_LENGTH];
    UDP_State error = iapResult2UDPState(iapReadUID(uid));
    if (error != UDP_IAP_SUCCESS)
    {
        // could not read UID of mcu
        setLastError(error);
        dline(" iapReadUID failed");
        return (T_ACK_PDU);
    }

    if (memcmp(uid, data, size) != 0)
    {
        // uid is not correct, as a last chance we convert the uid from big-endian to little-endian as displayed by Flashmagic
        uint32_t reversed;
        for (uint8_t i = 0; i < UID_LENGTH_USED; i=i+sizeof(uint32_t))
        { ///\todo this should be reworked, maybe somehow with __REV
            reversed = streamToUIn32(&data[i]);
            data[i] = reversed >> 24;
            data[i+1] = reversed >> 16;
            data[i+2] = reversed >> 8;
            data[i+3] = reversed & 0xff;
        }

        if (memcmp(uid, data, size) != 0)
        {
            // uid is still not correct, finally decline access
            setLastError(UDP_UID_MISMATCH);
            dline(" uid mismatch");
            return (T_ACK_PDU);
        }
    }

    // we can now unlock the device
    setDeviceLockState(DEVICE_UNLOCKED);
    setLastError(UDP_IAP_SUCCESS);
    resetUPDProtocol();
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_APP_VERSION_REQUEST command and sends a @ref UPD_APP_VERSION_RESPONSE
 *        The response contains the address of the AppVersion string
 *
 * @post          sets lastError to @ref UDP_IAP_SUCCESS if successful, otherwise to @ref UDP_WRONG_DESCRIPTOR_BLOCK
 * @return        always @ref T_ACK_PDU
 */
static unsigned char updAppVersionRequest()
{
    char* appversion;
    appversion = getAppVersion((AppDescriptionBlock *) (applicationFirstAddress() - BOOT_BLOCK_DESC_SIZE));
    prepareReturnTelegram(bcu.sendTelegram, BL_ID_STRING_LENGTH - 1, UPD_APP_VERSION_RESPONSE);
    memcpy(bcu.sendTelegram + 9, appversion, BL_ID_STRING_LENGTH - 1);
    d3(
        if (appversion != bl_id_string)
        {
            serial.print("AppVersionRequest OK: ");
        }
        else
        {
            serial.print("AppVersionRequest outside range (default): ");
        }
        for (int i = 0; i < BL_ID_STRING_LENGTH - 1; i++)
        {
            serial.print((char)appversion[i]);
        }
        serial.println();
    );
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_DUMP_FLASH command and dumps the given flash address range to the serial port in intel(R) hex
 *
 * @param data    data[0-3] contains startAddress, data[4-7] contains endAddress
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref UDP_State.
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updDumpFlashRange(uint8_t * data)
{
#ifdef DEBUG
    uint8_t * startAddress = streamToPtr(&data[0]);
    uint8_t * endAddress = streamToPtr(&data[4]);
    setLastError(UDP_IAP_SUCCESS);
    dumpFlashContent(startAddress, endAddress);
#else
    setLastError(UDP_NOT_IMPLEMENTED);
#endif
    return (T_ACK_PDU);
}


/**
 * Handles the @ref UPD_ERASE_ADDRESSRANGE command and erases the requested flash address range
 *
 * @param data    data[0-3] contains startAddress, data[4-7] contains endAddress
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_ADDRESS_RANGE_NOT_ALLOWED_TO_ERASE or a @ref UDP_State.
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updEraseAddressRange(uint8_t * data)
{
    uint8_t * startAddress = streamToPtr(&data[0]);
    uint8_t * endAddress = streamToPtr(&data[4]);
    setLastError(eraseAddressRange(startAddress, endAddress));
    resetUPDProtocol();
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_ERASE_COMPLETE_FLASH command and erases the entire flash except from the bootloader itself
 *
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref UDP_State.
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updEraseFullFlash()
{
    setLastError(eraseFullFlash());
    resetUPDProtocol();
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_SEND_DATA command and copies the received bytes from data to @ref ramBuffer
 *
 * @param data    data[0] must contain the ramBuffer location and data[1...] the bytes to copy to the ramBuffer
 * @param nCount  Number of bytes to copy
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_RAM_BUFFER_OVERFLOW or a @ref UDP_State
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updSendData(uint8_t * data, uint32_t nCount)
{
    ramLocation = data[0]; // Current Byte position as message number with 12 Bytes payload each
    nCount --;             // 1 Bytes abziehen, da diese für die Nachrichtennummer verwendet wird

    if ((ramLocation + nCount) > sizeof(ramBuffer)/sizeof(ramBuffer[0])) // enough space left?
    {
        setLastError(UDP_RAM_BUFFER_OVERFLOW);
        dline("ramBuffer Full");
        return (T_ACK_PDU);
    }

    bytesReceived += nCount;

    memcpy(&ramBuffer[ramLocation], data + 1, nCount); //ab dem 1. Byte sind die Daten verfügbar
    setLastError(UDP_IAP_SUCCESS);
    for(unsigned int i=0; i<nCount; i++)
    {
        d2(data[i+1],HEX,2);
        d1(" ");
    }
    d3(serial.print("at: ", ramLocation, DEC, 3));
    d3(serial.println(" #", (unsigned int) nCount, DEC, 2));
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_PROGRAM command and copies the bytes from ramBuffer to flash
 *
 * @param data    the number of bytes to flash is in data[0-1], the flash address to program in data[2-5], and the crc32 in data[6-9]
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise a @ref UDP_State or a @ref UDP_State
 * @return        T_ACK_PDU
 * @note          device must be unlocked
 * @warning       The function calls @ref executeProgramFlash which calls @ref iap_Program which by itself calls @ref no_interrupts().
 */
static unsigned char updProgram(uint8_t * data)
{
    uint16_t flash_count = streamToUShort16(data);
    uint8_t * address = streamToPtr(data + 2);
    uint32_t crc32toCompare = streamToUIn32(data + 2 + 4);

    if (!addressAllowedToProgram(address, flash_count))
    {
        // invalid address
        setLastError(UDP_ADDRESS_NOT_ALLOWED_TO_FLASH);
        return (T_ACK_PDU);
    }

    if (flash_count > (sizeof(ramBuffer)/sizeof(ramBuffer[0])))
    {
        setLastError(UDP_RAM_BUFFER_OVERFLOW);
        return (T_ACK_PDU);
    }

    uint32_t crcRamBuffer = crc32(0xFFFFFFFF, ramBuffer, flash_count);
    if (crcRamBuffer != crc32toCompare)
    {
        // invalid crcRamBuffer
        setLastError(UDP_CRC_ERROR);
        return (T_ACK_PDU);
    }

    // Select smallest possible sector size
    if (flash_count > 4*FLASH_PAGE_SIZE)
        flash_count = 16*FLASH_PAGE_SIZE;
    else if (flash_count > 2*FLASH_PAGE_SIZE)
        flash_count = 4*FLASH_PAGE_SIZE;
    else if (flash_count > FLASH_PAGE_SIZE)
        flash_count = 2*FLASH_PAGE_SIZE;
    else
        flash_count = FLASH_PAGE_SIZE;

    d3(serial.print("writing ", flash_count));
    d3(serial.print(" bytes @ 0x", address));
    d3(serial.println(" crc 0x", (uintptr_t)crcRamBuffer, HEX, 8));

    bytesFlashed += flash_count;
    UDP_State error = executeProgramFlash(address, ramBuffer, flash_count);
    setLastError(error);
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_REQUEST_BL_IDENTITY command. Copies bootloader version (@ref BOOTLOADER_MAJOR_VERSION, @ref BOOTLOADER_MINOR_VERSION),
 *        bootloader features (@ref BL_FEATURES) and applications first possible start address (@ref applicationFirstAddress())
 *        to the return telegram.
 *
 * @param data    the major version of Selfbus Updater in data[0-3],  the minor version of Selfbus Updater in data[4-7] *
 * @return always T_ACK_PDU
 */
static unsigned char updRequestBootloaderIdentity(uint8_t * data)
{
    uint8_t majorVersionUpdater = data[0];
    uint8_t minorVersionUpdater = data[1];
    bool versionMatch = (majorVersionUpdater > UPDATER_MIN_MAJOR_VERSION) ||
              ((majorVersionUpdater == UPDATER_MIN_MAJOR_VERSION) && (minorVersionUpdater >= UPDATER_MIN_MINOR_VERSION));

    uint8_t offset = 9;
    if (!versionMatch)
    {
        // version mismatch send the minimum requested major and minor version
        prepareReturnTelegram(bcu.sendTelegram, sizeof(UPDATER_MIN_MAJOR_VERSION) + sizeof(UPDATER_MIN_MINOR_VERSION), UPD_RESPONSE_BL_VERSION_MISMATCH);
        bcu.sendTelegram[offset] = UPDATER_MIN_MAJOR_VERSION;
        bcu.sendTelegram[offset + sizeof(UPDATER_MIN_MAJOR_VERSION)] = UPDATER_MIN_MINOR_VERSION;
        d3(serial.print("Updater version mismatch! Required ", UPDATER_MIN_MAJOR_VERSION));
        d3(serial.print(".", UPDATER_MIN_MINOR_VERSION));
        d3(serial.print(" received: ", majorVersionUpdater));
        d3(serial.println(".", minorVersionUpdater));
        return (T_ACK_PDU);
    }

    uint32_t bootloaderFeatures = BL_FEATURES;
    uint8_t * appFirstAddress = applicationFirstAddress();
    const uint32_t dataSize = sizeof(BOOTLOADER_MAJOR_VERSION) +
                              sizeof(BOOTLOADER_MINOR_VERSION) +
                              sizeof(bootloaderFeatures) +
                              sizeof(appFirstAddress);

    prepareReturnTelegram(bcu.sendTelegram, dataSize, UPD_RESPONSE_BL_IDENTITY);
    bcu.sendTelegram[offset] = BOOTLOADER_MAJOR_VERSION;
    offset += sizeof(BOOTLOADER_MAJOR_VERSION);
    bcu.sendTelegram[offset] = BOOTLOADER_MINOR_VERSION;
    offset += sizeof(BOOTLOADER_MINOR_VERSION);
    uInt32ToStream(bcu.sendTelegram + offset, bootloaderFeatures);
    offset += sizeof(bootloaderFeatures);
    ptrToStream(bcu.sendTelegram + offset, appFirstAddress);
    d3(serial.print("BL v", BOOTLOADER_MAJOR_VERSION, DEC));
    d3(serial.print(".", BOOTLOADER_MINOR_VERSION, DEC, 2));
    d3(serial.print("    BL feature 0x", (unsigned int)bootloaderFeatures, HEX, 8));
    d3(serial.println("    FW start   0x", (uintptr_t)appFirstAddress, HEX, 8));
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_REQUEST_STATISTIC command.
 *
 * @return always T_ACK_PDU
 */
static unsigned char updRequestStatistic()
{
    uint16_t reserved = 0;
    uint32_t sizeTotal = sizeof(disconnectCount) + sizeof(reserved);

    prepareReturnTelegram(bcu.sendTelegram, sizeTotal, UPD_RESPONSE_STATISTIC);
    uShort16ToStream(bcu.sendTelegram + 9, disconnectCount);
    uShort16ToStream(bcu.sendTelegram + 9 + sizeof(disconnectCount), reserved);
    d3(serial.print(" #DC ", disconnectCount));
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_REQUEST_BOOT_DESC command. Copies the application description block
 *        (@ref AppDescriptionBlock) to the return telegram.
 * @return always T_ACK_PDU
 */
static unsigned char udpRequestBootDescriptionBlock()
{
    AppDescriptionBlock* bootDescr = (AppDescriptionBlock *) bootDescriptorBlockAddress(); // Address of boot block descriptor

    bool valid;
    // check that the start address is not beyond end address
    valid = bootDescr->startAddress <= bootDescr->endAddress;
    // addresses not outside the flash
    valid &= (bootDescr->startAddress <= flashLastAddress()) && (bootDescr->endAddress <= flashLastAddress());
    // addresses not smaller then allowed applications first address
    valid &= (bootDescr->startAddress >= applicationFirstAddress()) && (bootDescr->endAddress >= applicationFirstAddress());

    if (!valid)
    {
        bootDescr->startAddress = (uint8_t *)-1;
        bootDescr->endAddress = (uint8_t *)-1;
        bootDescr->appVersionAddress = (char *)-1;
        bootDescr->crc = -1;
    }

    prepareReturnTelegram(bcu.sendTelegram, 12, UPD_RESPONSE_BOOT_DESC);
    memcpy(bcu.sendTelegram + 9, bootDescr, 12); // startAddress, endAddress, crc

    d3(serial.print("FW start@ 0x", bootDescr->startAddress));   // Firmware start address
    d3(serial.print(" end@ 0x", bootDescr->endAddress));        // Firmware end address
    d3(serial.print(" Desc.@ 0x", bootDescr->appVersionAddress)); // Firmware App descriptor address (for getAppVersion())
    d3(serial.println(" CRC : 0x", (uintptr_t)bootDescr->crc, HEX, 8));                // Firmware CRC
    return (T_ACK_PDU);
}

/**
 * Function not implemented.
 * \todo Function not implemented
 *
 * @post          calls setLastErrror with @ref UDP_NOT_IMPLEMENTED
 * @return        always T_ACK_PDU
 * @warning function is not implemented, missing parameters address and count
 */
static unsigned char updRequestData()
{
     /*
     memcpy(bcu.sendTelegram + 9, address, count);
     bcu.sendTelegram[5] = 0x63 + count;
     bcu.sendTelegram[6] = 0x42;
     bcu.sendTelegram[7] = 0x40 | count;
     bcu.sendTelegram[8] = UPD_SEND_DATA;
     */
    setLastError(UDP_NOT_IMPLEMENTED);
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_REQUEST_UID command. Copies @ref UID_LENGTH_USED bytes
 *        to the return telegram.
 *
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, if device is locked @ref UDP_DEVICE_LOCKED
 *                otherwise a @ref IAP_Status
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updRequestUID()
{
    if (getProgButtonState() != true)
    {
        setLastError(UDP_DEVICE_LOCKED);
        return (T_ACK_PDU);
    }

    // the operator has physical access to the device -> we unlock it
    byte uid[4 * 4];
    UDP_State result = iapResult2UDPState(iapReadUID(uid));
    if (result != UDP_IAP_SUCCESS)
    {
        dline("iapReadUID error");
        setLastError(result);
        return (T_ACK_PDU);
    }
    prepareReturnTelegram(bcu.sendTelegram, UID_LENGTH_USED, UPD_RESPONSE_UID);
    memcpy(bcu.sendTelegram + 9, uid, UID_LENGTH_USED);
    dline(" OK");
    return (T_ACK_PDU);
}

/**
 * Handles all unknown UPD/UDP commands.
 *
 * @post          calls setLastErrror with @ref UDP_UNKNOWN_COMMAND
 * @return        always T_ACK_PDU
 */
static unsigned char updUnkownCommand()
{
    setLastError(UDP_UNKNOWN_COMMAND); // set to unknown error
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_UPDATE_BOOT_DESC command.
 *        - checks the received application boot descriptor block for a possible buffer overflow
 *        - checks the crc32 of the received application boot descriptor block
 *        - if the received application boot descriptor block differs from the one already in Flash (@ref BOOT_DSCR_ADDRESS),
 *          checks that the address is allowed to program, erases the flash page and flashes the new one.
 *
 * @param data    - data[0..3] contains the length of the application boot descriptor block received
 *                - data[4..7] contains the crc32 of the received bytes
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise a @ref UDP_State or @ref IAP_Status
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 * @warning       The function calls @ref executeProgramFlash which calls @ref iap_Program which by itself calls @ref no_interrupts().
 */
static unsigned char updUpdateBootDescriptorBlock(uint8_t * data)
{
    uint32_t count = streamToUIn32(data); // length of the descriptor
    uint32_t crcReceived = streamToUIn32(data + sizeof(count));
    uint8_t * address;
    uint32_t crc;

    UDP_State result = UDP_NOT_IMPLEMENTED;

    // check for a possible ramBuffer overflow
    if (count > sizeof(ramBuffer)/sizeof(ramBuffer[0]))
    {
        setLastError(UDP_RAM_BUFFER_OVERFLOW);
        dline("ramBuffer Full");
        return (T_ACK_PDU);
    }
    d3(
        bytesReceived -= count; // subtract bytes received for boot descriptor
        serial.println();
        serial.println("Bytes Rx    ", bytesReceived);
        serial.println("Bytes Flash ", bytesFlashed); //
        serial.println("Diff        ", (int)bytesFlashed - (int)bytesReceived); // difference here is normal, because flashing is always in multiple of FLASH_PAGE_SIZE
        serial.println();
        serial.println("FW start@ 0x", streamToUIn32(ramBuffer), HEX, 4);    // Firmware start address
        serial.println("FW end  @ 0x", streamToUIn32(ramBuffer+4), HEX, 4);  // Firmware end address
        serial.println("FW Desc.@ 0x", streamToUIn32(ramBuffer+12), HEX, 4); // Firmware App descriptor address (for getAppVersion())
        serial.println("FWs CRC : 0x", streamToUIn32(ramBuffer+8), HEX, 8);  // Firmware CRC
        // serial.println("RamBuffer[16-20] 0x", streamToUIn32(ramBuffer+16), HEX, 8);// This is beyond the descriptor block
    )

    address = bootDescriptorBlockAddress();     // start address of boot block descriptor
    crc = crc32(0xFFFFFFFF, ramBuffer, count);  // checksum on used length only

    d3(serial.println("Desc.      @ 0x", address));
    d3(serial.println("Desc.    CRC 0x", (uintptr_t)crc, HEX, 8));
    d3(serial.println("Received CRC 0x", (uintptr_t)crcReceived, HEX, 8));
    // compare calculated crc with the one we received for this packet
    if (crc != crcReceived)
    {
        d3(serial.print("-->UDP_CRC_ERROR "));
        d3(serial.print(" data[3-0]:", streamToUIn32(data), HEX, 8));
        d3(serial.print(" data[7-4]:", streamToUIn32(data+4), HEX, 8));
        setLastError(UDP_CRC_ERROR);
        return (T_ACK_PDU);
    }

    d3(serial.println("CRC MATCH, comparing MCUs BootDescriptor: count: ", (unsigned int)count));
    //If received descriptor is not equal to current one, flash it
    if(memcmp(address, ramBuffer, count) == 0)
    {
        d3(serial.println("is equal, skipping"));
        result = UDP_IAP_SUCCESS;
        // dont return here, let's also check the AppDescriptionBlock
    }
    else
    {
        d3(serial.print("it's different, Erase Page: "));
        result = erasePageRange(bootDescriptorBlockPage(), bootDescriptorBlockPage());
        if (result != UDP_IAP_SUCCESS)
        {
            setLastError(result);
            return (T_ACK_PDU);
        }

        d3(serial.print("Flash Page:"));

        result = executeProgramFlash(address, ramBuffer, FLASH_PAGE_SIZE, true); // no less than 256byte can be flashed
        d3(
           updResult2Serial(result);
           serial.println();
        );

        if (result != UDP_IAP_SUCCESS)
        {
            setLastError(result);
            if (result == UDP_ADDRESS_NOT_ALLOWED_TO_FLASH)
            {
                return (T_ACK_PDU);
            }
        }
    }

    if (!checkApplication((AppDescriptionBlock *) ramBuffer))
    {
        d3(
            serial.println("-->UDP_APPLICATION_NOT_STARTABLE");
        );
        setLastError(UDP_APPLICATION_NOT_STARTABLE);
        return (T_ACK_PDU);
    }

    setLastError(result);
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_SEND_DATA_TO_DECOMPRESS command. "Copies" the bytes from data to the @ref Decompressor.
 *
 * @param data    data[0..nCount-1] buffer containing the bytes to "copy" to the @ref Decompressor
 * @param nCount  Number of bytes to read from data
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_RAM_BUFFER_OVERFLOW or @ref UDP_NOT_IMPLEMENTED
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 * @warning       The function calls @ref Decompressor.pageCompletedDoFlash which calls @ref iap_Program which by itself calls @ref no_interrupts().
 */
static unsigned char updSendDataToDecompress(uint8_t * data, uint32_t nCount)
{
#ifndef DECOMPRESSOR
    dline("-->not implemented")
    setLastError(UDP_NOT_IMPLEMENTED);
#else
    if ((ramLocation + nCount) > (sizeof(ramBuffer)/sizeof(ramBuffer[0]))) // Check if this can be removed. Overflow protection should be in decompressor class instead!
    {
        setLastError(UDP_RAM_BUFFER_OVERFLOW);
        return (T_ACK_PDU);
    }
    dline("-->decompressor");
    for (unsigned int i = 0; i < nCount; i++)
    {
        decompressor.putByte(data[i]);
    }
    setLastError(UDP_IAP_SUCCESS);
#endif
    return (T_ACK_PDU);
}

/**
 * Handles the @ref UPD_PROGRAM_DECOMPRESSED_DATA command.
 *        - checks that the flash address is allowed to be programmed
 *        - checks the crc32 received in data with the one from the @ref Decompressor
 *        - calls @ref Decompressor.pageCompletedDoFlash to flash
 *
 * @param data    data[0-3] contains the crc32 for the received bytes
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise a @ref UDP_State
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 * @warning       The function calls @ref Decompressor.pageCompletedDoFlash which calls @ref iap_Program which by itself calls @ref no_interrupts().
 */
static unsigned char updProgramDecompressedDataToFlash(uint8_t * data)
{
#ifndef DECOMPRESSOR
    setLastError(UDP_NOT_IMPLEMENTED);
#else
    uint32_t crcReceived = streamToUIn32(data);
    uint32_t count = decompressor.getBytesCountToBeFlashed();
    uint8_t * address = decompressor.getStartAddrOfPageToBeFlashed();
    uint32_t crc;

    d1("\n\rFlash Diff address 0x");
    d2ptr(address);
    d1(" length: ");
    d2((unsigned int) count,DEC,3);

    if (!addressAllowedToProgram(address, count))
    {
        dline(" Address protected!");
        setLastError(UDP_ADDRESS_NOT_ALLOWED_TO_FLASH);
        return (T_ACK_PDU);
    }

    d1(" Address valid, ");
    crc = decompressor.getCrc32();
    if (crc != crcReceived)
    {
        dline("CRC Error!");
        setLastError(UDP_CRC_ERROR);
        return (T_ACK_PDU);
    }

    dline("CRC OK");
    setLastError(decompressor.pageCompletedDoFlash());
    resetUPDProtocol(); // we need this, otherwise updSendDataToDecompress will run into a buffer overflow
#endif
    return (T_ACK_PDU);
}

unsigned char handleDeprecatedApciMemoryWrite()
{
    bcu.sendTelegram[5] = 0x63 + 4; // routing count in high nibble + response length in low nibble
    bcu.sendTelegram[6] = 0x42;     // APCI_MEMORY_RESPONSE_PDU
    bcu.sendTelegram[7] = 0x40 | 4; // APCI_MEMORY_RESPONSE_PDU
    bcu.sendTelegram[8] = 0;        // [8-9] old value of UPD_SEND_LAST_ERROR = 0x0015
    bcu.sendTelegram[9] = 0x15;
    bcu.sendTelegram[10] = 0xff;    // [10-13] old value of UDP_NOT_IMPLEMENTED = 0x0000FFFF
    bcu.sendTelegram[11] = 0xff;
    bcu.sendTelegram[12] = 0x00;
    bcu.sendTelegram[13] = 0x00;
    return (T_ACK_PDU);
}

unsigned char handleApciUsermsgManufacturer(uint8_t * data, uint32_t size)
{
    if (size < sizeof(updCommands[idxInvalidUPDCommand].code))
    {
        d3(serial.println("UDP_NO_DATA"));
        setLastError(UDP_NO_DATA);
        return (T_ACK_PDU);
    }

    UPD_Command updCommand = code2UPDCommand(data[0]);
    if (updCommand.code == UPD_INVALID)
    {
        d3(serial.println("updCommand.code invalid"));
        setLastError(UDP_INVALID);
        return (T_ACK_PDU);
    }

    data++;
    size--;
    updCommand2Serial(updCommand); // simple command debugging message
    if ((size < updCommand.minBytes) || (size > updCommand.maxBytes))
    {
        d3(serial.print(" UDP_INVALID_DATA minCount=", updCommand.minBytes));
        d3(serial.print(" maxCount=", updCommand.maxBytes));
        d3(serial.println(" length=", (unsigned int)size));
        setLastError(UDP_INVALID_DATA);
        return (T_ACK_PDU);
    }

#if defined(DEBUG) && (!(defined(TS_ARM)))
    digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));
#endif
    // check for commands allowed on a locked device
    if (!getDeviceUnlocked())
    {
        switch (updCommand.code)
        {
            // list of commands allowed on a locked device
            case UPD_UNLOCK_DEVICE:
            case UPD_REQUEST_UID:
            case UPD_APP_VERSION_REQUEST:
                break;

            default:
                // device is locked -> command not allowed, send lastError and return
                setLastError(UDP_DEVICE_LOCKED);
                return (T_ACK_PDU);
        }
    }

    // now comes the real work on the unlocked device
    switch (updCommand.code)
    {
        case UPD_UNLOCK_DEVICE:
            return (updUnlockDevice(data, size));

        case UPD_REQUEST_UID:
            return (updRequestUID());

        case UPD_APP_VERSION_REQUEST:
            return (updAppVersionRequest());

        case UPD_SEND_DATA:
            return (updSendData(data, size));

        case UPD_PROGRAM:
            return (updProgram(data));

        case UPD_SEND_DATA_TO_DECOMPRESS:
            return (updSendDataToDecompress(data, size));

        case UPD_PROGRAM_DECOMPRESSED_DATA:
            return (updProgramDecompressedDataToFlash(data));

        case UPD_ERASE_COMPLETE_FLASH:
            return (updEraseFullFlash());

        case UPD_ERASE_ADDRESSRANGE:
            return (updEraseAddressRange(data));

        case UPD_DUMP_FLASH:
            return (updDumpFlashRange(data));

        case UPD_REQUEST_STATISTIC:
            return (updRequestStatistic());

        case UPD_UPDATE_BOOT_DESC:
            return (updUpdateBootDescriptorBlock(data));

		case UPD_REQUEST_BOOT_DESC:
            return (udpRequestBootDescriptionBlock());

		case UPD_REQUEST_BL_IDENTITY:
		    return (updRequestBootloaderIdentity(data));

		case UPD_REQ_DATA:
		    return (updRequestData());

        default:
            return (updUnkownCommand());
    }
    return (T_NACK_PDU); //we should never land here
}


/** @}*/
