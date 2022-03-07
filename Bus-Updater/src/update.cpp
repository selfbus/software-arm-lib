/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_KNX_TELEGRAM_HANDLER_1 KNX-Telegramhandler
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Handles @ref APCI_MEMORY_READ_PDU and @ref APCI_MEMORY_WRITE_PDU for the update process
 * @details
 *
 * @{
 *
 * @file   update.cpp
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

#include <string.h>
#include <sblib/eib.h>
#include <sblib/eib/bus.h>
#include <sblib/eib/knx_tpdu.h>
#include <sblib/eib/apci.h>
#include <sblib/timeout.h>
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

static unsigned char __attribute__ ((aligned (FLASH_RAM_BUFFER_ALIGNMENT))) ramBuffer[RAM_BUFFER_SIZE]; //!< RAM buffer used for flash operations


Timeout mcuRestartRequestTimeout; //!< Timeout used to trigger a MCU Reset by NVIC_SystemReset()

// Try to avoid direct access to these global variables.
// It's better to use their get, set and reset functions
static unsigned int deviceLocked = DEVICE_LOCKED;   //!< current device locking state @note Better use GetDeviceUnlocked() & setDeviceLockState()
static UDP_State lastError = UDP_IAP_SUCCESS;       //!< last error while processing a UDP command
static unsigned int ramLocation = 0;                //!< current location of the ramBuffer processed
static unsigned int bytesReceived = 0;              //!< number of bytes received by UPD_SEND_DATA since last reset()
static unsigned int bytesFlashed = 0;               //!< number of bytes flashed by UPD_PROGRAM since last reset()

/**
 * @brief Converts a unsigned int into a 4 byte long provided buffer
 * @details A direct cast does not work due to possible miss aligned addresses.
 *          therefore a good old conversion has to be performed
 *
 * @param buffer in the 4 first bytes of buffer the result will be stored
 * @param val    the unsigned int to be converted
 * @warning function doesn't perform any sanity-checks on the provided buffer
 */
ALWAYS_INLINE void uInt32ToStream(unsigned char * buffer, unsigned int val);

/**
 * @brief Send the flash content from startAddress to endAddress
 *        in Intel(R) hex file format over serial port
 *
 * @param startAddress
 * @param endAddress
 */
#if defined(DEBUG)
void dumpFlashContent(unsigned int startAddress, unsigned int endAddress)
{
    if (startAddress > endAddress)
    {
        unsigned int temp = startAddress;
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

    dumpToSerialinIntelHex(&serial, (unsigned char *) startAddress, endAddress - startAddress + 1);
}
#endif

void restartRequest (unsigned int msec)
{
    d3(
        serial.println("Systime: ", systemTime, DEC);
    );
	mcuRestartRequestTimeout.start(msec);
}

bool restartRequestExpired(void)
{
	return (mcuRestartRequestTimeout.expired());
}

/**
 * @brief Converts a 4 byte long provided buffer into a unsigned integer
 * @details A direct cast does not work due to possible miss aligned addresses.
 *          therefore a good old conversion has to be performed
 *
 * @param buffer data to convert
 * @return to unsigned int converted value of the 4 first bytes of buffer
 * @warning function doesn't perform any sanity-checks on the provided buffer
 */
ALWAYS_INLINE unsigned int streamToUIn32(unsigned char * buffer)
{
    return ((unsigned int)(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]));
}

///\todo implement universal numberToStream by providing the size as parameter
///      and delete uInt32ToStream + uShort16ToStream
ALWAYS_INLINE void uInt32ToStream(unsigned char * buffer, unsigned int val)
{
    buffer[3] = (byte)(val >> 24);
    buffer[2] = (byte)(val >> 16);
    buffer[1] = (byte)(val >> 8);
    buffer[0] = (byte)(val & 0xff);
}

ALWAYS_INLINE void uShort16ToStream(unsigned char * buffer, unsigned int val)
{
    buffer[1] = (byte)(val >> 8);
    buffer[0] = (byte)(val & 0xff);
}

/**
 * @brief Prepares a UPD/UDP telegram with command/response and number of bytes
 *        the return telegram will have.
 *
 * @param count Number of bytes the return telegram shall have
 * @param cmd   UPD/UDP command/response to set the return telegram
 * @return always true
 */
static bool prepareReturnTelegram(unsigned int count, unsigned char cmd)
{
    bcu.sendTelegram[5] = 0x63 + (byte)count;
    bcu.sendTelegram[6] = 0x42;
    bcu.sendTelegram[7] = 0x40 | (byte)count;
    bcu.sendTelegram[8] = 0;
    bcu.sendTelegram[9] = cmd;
    return (true);
}


/**
 * @brief Returns the unlocked status of the device
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
 * @brief Sets the device lock state.
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
 * @brief Returns the state of the programming button
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
 * @brief Sets lastError and prepares the @ref UPD_SEND_LAST_ERROR response telegram
 *
 * @param errorToSet The error to set
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 */
static void setLastError(UDP_State errorToSet, bool * sendTel)
{
    lastError = errorToSet;
    *sendTel = prepareReturnTelegram(4, UPD_SEND_LAST_ERROR);
    uInt32ToStream(bcu.sendTelegram + 10, lastError);
}

void resetUPDProtocol(void)
{
    lastError = UDP_IAP_SUCCESS;
    ramLocation = 0;
    bytesReceived = 0;
    bytesFlashed = 0;
    dump2(serial.println("resetUPDProtocol"));
}

/**
 * @brief Handles the @ref UPD_UNLOCK_DEVICE command.
 *        The device is unlocked if the programming mode is active or the provided UID (guid) is valid.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    Buffer for the UID
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if device is unlocked, otherwise @ref UDP_UID_MISMATCH or a @ref IAP_Status.
 * @return        always T_ACK_PDU
 */
static unsigned char updUnlockDevice(bool * sendTel, unsigned char * data)
{
    if (getProgButtonState())
    { // the operator has physical access to the device -> we unlock it
        setDeviceLockState(DEVICE_UNLOCKED);
        setLastError(UDP_IAP_SUCCESS, sendTel);
        dline(" by Button");
    }
    else
    {   // we need to ensure that only authorized operators can
        // update the application
        // as a simple method we use the unique ID of the CPU itself
        // only if this UID (GUID) is known, the device will be unlocked
        byte uid[IAP_UID_LENGTH];
        lastError = (UDP_State)iapReadUID(uid);
        if (lastError != UDP_IAP_SUCCESS)
        {
            // could not read UID of mcu
            setLastError((UDP_State)lastError, sendTel);
            return (T_ACK_PDU);
        }

        for (unsigned int i = 0; i < UID_LENGTH_USED; i++)
        {
            if (data[i + 3] != uid[i])
            {
                // uid is not correct, decline access
                setLastError(UDP_UID_MISMATCH, sendTel);
                return (T_ACK_PDU);
            }
        }

        // we can now unlock the device
        setDeviceLockState(DEVICE_UNLOCKED);
        setLastError(UDP_IAP_SUCCESS, sendTel);
        dline(" by UID");
        resetUPDProtocol();
    }
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_RESET command and calls @ref restartRequest to prepare a reset of the device.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @post          calls setLastErrror always with UDP_IAP_SUCCESS
 * @return        always T_ACK_PDU
 */
static unsigned char updResetDevice(bool * sendTel)
{
    restartRequest(RESET_DELAY_MS); // request restart in RESET_DELAY_MS ms to allow transmission of ACK before
    setLastError(UDP_IAP_SUCCESS, sendTel);
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_APP_VERSION_REQUEST command and sends a @ref UPD_APP_VERSION_RESPONSE
 *        The response contains the address of the AppVersion string
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    Buffer to store the AppVersion string address
 * @post          sets lastError always to UDP_IAP_SUCCESS
 * @return        always T_ACK_PDU
 */
static unsigned char updAppVersionRequest(bool * sendTel, unsigned char * data)
{
    unsigned char* appversion;
    appversion = getAppVersion((AppDescriptionBlock *) (applicationFirstAddress() - (1 + data[3]) * BOOT_BLOCK_DESC_SIZE));
    lastError = UDP_IAP_SUCCESS;
    *sendTel = prepareReturnTelegram(BL_ID_STRING_LENGTH - 1, UPD_APP_VERSION_RESPONSE);
    memcpy(bcu.sendTelegram + 10, appversion, BL_ID_STRING_LENGTH - 1);
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
 * @brief Handles the @ref UPD_ERASE_SECTOR command, erases if allowed the requested sector and resets @ref ramLocation
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    data[3] should contain the sector number to be erased
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status.
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
/*
static unsigned char updEraseSector(bool * sendTel, unsigned char * data)
{
    resetProtocol();
    setLastError(eraseSector(data[3]), sendTel);
    return (T_ACK_PDU);
}
*/

/**
 * @brief Handles the @ref UPD_DUMP_FLASH command and dumps the given flash address range to the serial port in intel(R) hex
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    data[3-6] contains startAddress, data[7-10] contains endAddress
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status.
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updDumpFlashRange(bool * sendTel, unsigned char * data)
{
#ifdef DEBUG
    unsigned int startAddress = streamToUIn32(&data[3]);
    unsigned int endAddress = streamToUIn32(&data[7]);
    setLastError(UDP_IAP_SUCCESS, sendTel);
    *sendTel = true;
    dumpFlashContent(startAddress, endAddress);
#else
    setLastError(UDP_NOT_IMPLEMENTED, sendTel);
#endif
    return (T_ACK_PDU);
}


/**
 * @brief Handles the @ref UPD_ERASE_ADDRESSRANGE command and erases the requested flash address range
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    data[3-6] contains startAddress, data[7-10] contains endAddress
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status.
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updEraseAddressRange(bool * sendTel, unsigned char * data)
{
    unsigned int startAddress = streamToUIn32(&data[3]);
    unsigned int endAddress = streamToUIn32(&data[7]);
    setLastError(eraseAddressRange(startAddress, endAddress), sendTel);
    resetUPDProtocol();
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_ERASE_COMPLETE_FLASH command and erases the entire flash except from the bootloader itself
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status.
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updEraseFullFlash(bool * sendTel)
{
    setLastError(eraseFullFlash(), sendTel);
    resetUPDProtocol();
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_SEND_DATA command and copies the received bytes from data to @ref ramBuffer
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    data[3] must contain the ramBuffer location and data[4...] the bytes to copy to the ramBuffer
 * @param nCount  Number of bytes to copy
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_RAM_BUFFER_OVERFLOW or a @ref IAP_Status
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updSendData(bool * sendTel, unsigned char * data, unsigned int nCount)
{
    ramLocation = data[3];// * 11;  // Current Byte position as message number with 11 Bytes payload each
    nCount --;                      // 1 Bytes abziehen, da diese für die Nachrichtennummer verwendet wird

    if ((ramLocation + nCount) > sizeof(ramBuffer)/sizeof(ramBuffer[0])) // enough space left?
    {
        setLastError(UDP_RAM_BUFFER_OVERFLOW, sendTel);
        dline("ramBuffer Full");
        return (T_ACK_PDU);
    }

    bytesReceived += nCount;

    memcpy((void *) &ramBuffer[ramLocation], data + 4, nCount); //ab dem 4. Byte sind die Daten verfügbar
    setLastError(UDP_IAP_SUCCESS, sendTel);
    for(unsigned int i=0; i<nCount; i++)
    {
        d2(data[i+4],HEX,2);
        d1(" ");
    }
    d3(serial.print("at: ", ramLocation, DEC, 3));
    d3(serial.println(" #", nCount, DEC, 2));
    // *sendTel = false; ///\todo delete on release
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_PROGRAM command and copies the bytes from ramBuffer to flash
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    the number of bytes to flash is in data[3-6] the flash address to program in data[7-10]
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise a @ref UDP_State or a @ref IAP_Status
 * @return        T_ACK_PDU
 * @note          device must be unlocked
 * @warning       The function calls @ref executeProgramFlash which calls @ref iap_Program which by itself calls @ref no_interrupts().
 */
static unsigned char updProgram(bool * sendTel, unsigned char * data)
{
    unsigned int crcRamBuffer;
    unsigned int flash_count = streamToUIn32(data + 3);
    unsigned int address = streamToUIn32(data + 3 + 4);

    if (!addressAllowedToProgram(address, flash_count))
    {
        // invalid address
        setLastError(UDP_ADDRESS_NOT_ALLOWED_TO_FLASH, sendTel);
        return (T_ACK_PDU);
    }


    if (flash_count > (sizeof(ramBuffer)/sizeof(ramBuffer[0])))
    {
        setLastError(UDP_RAM_BUFFER_OVERFLOW, sendTel);
        return (T_ACK_PDU);
    }

    crcRamBuffer = crc32(0xFFFFFFFF, ramBuffer, flash_count);
    if (crcRamBuffer != streamToUIn32(data + 3 + 4 + 4))
    {
        // invalid crcRamBuffer
        setLastError(UDP_CRC_ERROR, sendTel);
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
    d3(serial.print(" bytes @ 0x", address, HEX, 8));
    d3(serial.println(" crc 0x", crcRamBuffer, HEX, 8));

    bytesFlashed += flash_count;
    UDP_State error = executeProgramFlash(address, ramBuffer, flash_count);
    setLastError(error, sendTel);
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_REQUEST_BL_IDENTITY command. Copies bootloader version (@ref BL_IDENTITY),
 *        bootloader features (@ref BL_FEATURES) and applications first possible start address (@ref applicationFirstAddress())
 *        to the return telegram.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    the major version of Selfbus Updater in data[3-6],  the minor version of Selfbus Updater in data[7-10]
 *
 * @return always T_ACK_PDU
 */
static unsigned char updRequestBootloaderIdentity(bool * sendTel, unsigned char * data)
{
    unsigned int majorVersionUpdater = streamToUIn32(data + 3);
    unsigned int minorVersionUpdater = streamToUIn32(data + 3 + 4);
    bool versionMatch = (majorVersionUpdater >= UPDATER_MIN_MAJOR_VERSION) && (minorVersionUpdater >= UPDATER_MIN_MINOR_VERSION);

    if (!versionMatch)
    {
        // version mismatch send the minimum requested major and minor version
        unsigned int minMajorVersionUpdater = UPDATER_MIN_MAJOR_VERSION;
        unsigned int minMinorVersionUpdater = UPDATER_MIN_MINOR_VERSION;
        *sendTel = prepareReturnTelegram(sizeof(minMajorVersionUpdater) + sizeof(minMinorVersionUpdater), UPD_RESPONSE_BL_VERSION_MISMATCH);
        uInt32ToStream(bcu.sendTelegram + 10, minMajorVersionUpdater);
        uInt32ToStream(bcu.sendTelegram + 10 + sizeof(minMajorVersionUpdater), minMinorVersionUpdater);

        d3(serial.print("Updater version mismatch! Required ", minMajorVersionUpdater));
        d3(serial.print(".", minMinorVersionUpdater));
        d3(serial.print(" received: ", majorVersionUpdater));
        d3(serial.println(".", minorVersionUpdater));
        return (T_ACK_PDU);
    }

    unsigned int bootloaderIdentity = BL_IDENTITY;
    unsigned int bootloaderFeatures = BL_FEATURES;
    unsigned int appFirstAddress = applicationFirstAddress();
    *sendTel = prepareReturnTelegram(12, UPD_RESPONSE_BL_IDENTITY);
    uInt32ToStream(bcu.sendTelegram + 10, bootloaderIdentity);  // Bootloader identity
    uInt32ToStream(bcu.sendTelegram + 14, bootloaderFeatures);  // Bootloader features
    uInt32ToStream(bcu.sendTelegram + 18, appFirstAddress);     // application first possible start address
    d3(serial.print("BL ID 0x", bootloaderIdentity, HEX, 8));
    d3(serial.print("    BL feature 0x", bootloaderFeatures, HEX, 8));
    d3(serial.println("    FW start   0x", appFirstAddress, HEX, 8));

    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_REQUEST_STATISTIC command.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @return always T_ACK_PDU
 */
static unsigned char updRequestStatistic(bool * sendTel)
{
    unsigned short dummy = 0;
    byte *startPos = bcu.sendTelegram + 10;
    byte sizeA = sizeof(telegramCount);
    byte sizeB = sizeof(disconnectCount);
    byte sizeC = sizeof(repeatedTelegramCount);
    byte sizeD = sizeof(repeatedIgnoredTelegramCount);
    byte sizeE = sizeof(dummy);

    unsigned int sizeTotal = sizeA + sizeB + sizeC + sizeD + sizeE;

    *sendTel = prepareReturnTelegram(sizeTotal, UPD_RESPONSE_STATISTIC);
    uShort16ToStream(startPos, telegramCount);
    startPos += sizeA;
    uShort16ToStream(startPos, disconnectCount);
    startPos += sizeB;
    uShort16ToStream(startPos, repeatedTelegramCount);
    startPos += sizeC;
    uShort16ToStream(startPos, repeatedIgnoredTelegramCount);
    startPos += sizeD;
    uShort16ToStream(startPos, dummy);

    d3(serial.print("#tel ", telegramCount));
    d3(serial.print(" #DC ", disconnectCount));
    d3(serial.print(" #rep. ", repeatedTelegramCount));
    d3(serial.println(" #ignor ", repeatedIgnoredTelegramCount));
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_REQUEST_BOOT_DESC command. Copies the application description block
 *        (@ref AppDescriptionBlock) to the return telegram.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @return always T_ACK_PDU
 */
static unsigned char udpRequestBootDescriptionBlock(bool * sendTel)
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
        bootDescr->startAddress = 0xFFFFFFFF;
        bootDescr->endAddress = bootDescr->startAddress;
        bootDescr->appVersionAddress = bootDescr->startAddress;
        bootDescr->crc = bootDescr->startAddress;
    }

    *sendTel = prepareReturnTelegram(12, UPD_RESPONSE_BOOT_DESC);
    memcpy(bcu.sendTelegram + 10, bootDescr, 12); // startAddress, endAddress, crc

    d3(serial.print("FW start@ 0x", bootDescr->startAddress , HEX, 8));   // Firmware start address
    d3(serial.print(" end@ 0x", bootDescr->endAddress, HEX, 8));        // Firmware end address
    d3(serial.print(" Desc.@ 0x", bootDescr->appVersionAddress, HEX, 8)); // Firmware App descriptor address (for getAppVersion())
    d3(serial.println(" CRC : 0x", bootDescr->crc, HEX, 8));                // Firmware CRC
    return (T_ACK_PDU);
}

/**
 * @brief Function not implemented.
 * \todo Function not implemented
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @post          calls setLastErrror with @ref UDP_NOT_IMPLEMENTED
 * @return        always T_ACK_PDU
 * @warning function is not implemented, missing parameters address and count
 */
static unsigned char updRequestData(bool * sendTel)
{
    //XXX check and implement this
    /*
     memcpy(bcu.sendTelegram + 9, (void *)address, count);
     bcu.sendTelegram[5] = 0x63 + count;
     bcu.sendTelegram[6] = 0x42;
     bcu.sendTelegram[7] = 0x40 | count;
     bcu.sendTelegram[8] = UPD_SEND_DATA;
     sendTel = true;
     */
    setLastError(UDP_NOT_IMPLEMENTED, sendTel);
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_REQUEST_UID command. Copies @ref UID_LENGTH_USED bytes
 *        to the return telegram.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, if device is locked @ref UDP_DEVICE_LOCKED
 *                otherwise a @ref IAP_Status
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updRequestUID(bool * sendTel)
{
    if (getProgButtonState() != true)
    {
        setLastError(UDP_DEVICE_LOCKED, sendTel);
        return (T_ACK_PDU);
    }

    // the operator has physical access to the device -> we unlock it
    byte uid[4 * 4];
    UDP_State result = (UDP_State)iapReadUID(uid);
    if (result != UDP_IAP_SUCCESS)
    {
        dline("iapReadUID error");
        setLastError(result, sendTel);
        return (T_ACK_PDU);
    }
    *sendTel = prepareReturnTelegram(UID_LENGTH_USED, UPD_RESPONSE_UID);
    memcpy(bcu.sendTelegram + 10, uid, UID_LENGTH_USED);
    dline(" OK");
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_GET_LAST_ERROR command. Copies the last error to the return telegram.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @return        always T_ACK_PDU
 */
static unsigned updGetLastError(bool * sendTel)
{
    setLastError((UDP_State)lastError, sendTel);
    return (T_ACK_PDU);
}

/**
 * @brief Handles all unknown UPD/UDP commands.
 *
 * @post          calls setLastErrror with @ref UDP_UNKNOWN_COMMAND
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @return        always T_ACK_PDU
 */
static unsigned char updUnkownCommand(bool * sendTel)
{
    setLastError(UDP_UNKNOWN_COMMAND, sendTel); // set to any error
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_UPDATE_BOOT_DESC command.
 *        - checks the received application boot descriptor block for a possible buffer overflow
 *        - checks the crc32 of the received application boot descriptor block
 *        - if the received application boot descriptor block differs from the one already in Flash (@ref BOOT_DSCR_ADDRESS),
 *          checks that the address is allowed to program, erases the flash page and flashes the new one.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    - data[7..4] contains the length of the application boot descriptor block received
 *                - data[11..8] contains the crc32 of the received bytes
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise a @ref UDP_State or @ref IAP_Status
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 * @warning       The function calls @ref executeProgramFlash which calls @ref iap_Program which by itself calls @ref no_interrupts().
 */
static unsigned char updUpdateBootDescriptorBlock(bool * sendTel, unsigned char * data)
{
    unsigned int count = streamToUIn32(data + 3); // length of the descriptor
    unsigned int address;
    unsigned int crc;
    UDP_State result = UDP_NOT_IMPLEMENTED;

    // check for a possible ramBuffer overflow
    if (count > sizeof(ramBuffer)/sizeof(ramBuffer[0]))
    {
        setLastError(UDP_RAM_BUFFER_OVERFLOW, sendTel);
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


    //address = FIRST_SECTOR - (1 + data[7]) * BOOT_BLOCK_SIZE; // start address of the descriptor block
    address = bootDescriptorBlockAddress();                // start address of boot block descriptor
    crc = crc32(0xFFFFFFFF, ramBuffer, count);  // checksum on used length only

    d3(serial.println("Desc.      @ 0x", address, HEX, 4));
    d3(serial.println("Desc.    CRC 0x", crc, HEX, 8));
    d3(serial.println("Received CRC 0x", streamToUIn32(data + 7), HEX, 8));
    // compare calculated crc with the one we received for this packet
    if (crc != streamToUIn32(data + 7))
    {
        d3(serial.print("-->UDP_CRC_ERROR "));
        d3(serial.print(" data[3-0]:", streamToUIn32(data), HEX, 8));
        d3(serial.print(" data[7-4]:", streamToUIn32(data+4), HEX, 8));
        d3(serial.print(" data[11-8]:", streamToUIn32(data+8), HEX, 8));
        d3(serial.print(" data[15-12]:", streamToUIn32(data+12), HEX, 8));
        setLastError(UDP_CRC_ERROR, sendTel);
        return (T_ACK_PDU);
    }

    d3(serial.println("CRC MATCH, comparing MCUs BootDescriptor: count: ", count));
    //If received descriptor is not equal to current one, flash it
    if(memcmp((byte *) address, ramBuffer, count) == 0)
    {
        d3(serial.println("is equal, skipping"));
        result = UDP_IAP_SUCCESS;
        // dont return here, let's also check the AppDescriptionBlock
    }
    else
    {
        d3(serial.print("it's different, Erase Page: "));
        result = erasePageRange(bootDescriptorBlockPage(), bootDescriptorBlockPage()); // - data[7]);
        if (result != UDP_IAP_SUCCESS)
        {
            setLastError(result, sendTel);
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
            setLastError(result, sendTel);
            if (result == UDP_ADDRESS_NOT_ALLOWED_TO_FLASH)
            {
                return (T_ACK_PDU);
            }
        }
    }
    // dumpFlashContent(((AppDescriptionBlock *) ramBuffer)->startAddress, ((AppDescriptionBlock *) ramBuffer)->endAddress); // untested
    if (!checkApplication((AppDescriptionBlock *) ramBuffer))
    {
        d3(
            serial.println("-->UDP_APPLICATION_NOT_STARTABLE");
            //DONE turn back on and remove above
            // dumpFlashContent(((AppDescriptionBlock *) ramBuffer)->startAddress, ((AppDescriptionBlock *) ramBuffer)->endAddress); // untested
        );
        setLastError(UDP_APPLICATION_NOT_STARTABLE, sendTel);
        return (T_ACK_PDU);
    }

    setLastError((UDP_State)result, sendTel);
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_SEND_DATA_TO_DECOMPRESS command. "Copies" the bytes from data to the @ref Decompressor.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    data[3..3+nCount] buffer containing the bytes to "copy" to the @ref Decompressor
 * @param nCount  Number of bytes to read from data
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise @ref UDP_RAM_BUFFER_OVERFLOW or @ref UDP_NOT_IMPLEMENTED
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 * @warning       The function calls @ref Decompressor.pageCompletedDoFlash which calls @ref iap_Program which by itself calls @ref no_interrupts().
 */
static unsigned char updSendDataToDecompress(bool * sendTel, unsigned char * data, unsigned int nCount)
{
#ifndef DECOMPRESSOR
    dline("-->not implemented")
    setLastError(UDP_NOT_IMPLEMENTED, sendTel);
#else
    if ((ramLocation + nCount) > (sizeof(ramBuffer)/sizeof(ramBuffer[0]))) // Check if this can be removed. Overflow protection should be in decompressor class instead!
    {
        setLastError(UDP_RAM_BUFFER_OVERFLOW, sendTel);
        return (T_ACK_PDU);
    }
    dline("-->decompressor");
    for (unsigned int i = 0; i < nCount; i++)
    {
        decompressor.putByte(data[i + 3]);
    }
    setLastError(UDP_IAP_SUCCESS, sendTel);
#endif
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_PROGRAM_DECOMPRESSED_DATA command.
 *        - checks that the flash address is allowed to be programmed
 *        - checks the crc32 received in data[11-14] with the one from the @ref Decompressor
 *        - calls @ref Decompressor.pageCompletedDoFlash to flash
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    data[11-14] contains the crc32 for the \todo check that this is correct
 * @post          calls setLastErrror with UDP_IAP_SUCCESS if successful, otherwise a @ref UDP_State or a @ref IAP_Status
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 * @warning       The function calls @ref Decompressor.pageCompletedDoFlash which calls @ref iap_Program which by itself calls @ref no_interrupts().
 */
static unsigned char updProgramDecompressedDataToFlash(bool * sendTel, unsigned char * data)
{
#ifndef DECOMPRESSOR
    setLastError(UDP_NOT_IMPLEMENTED, sendTel);
#else
    unsigned int count = decompressor.getBytesCountToBeFlashed();
    unsigned int address = decompressor.getStartAddrOfPageToBeFlashed();
    unsigned int crc;

    d1("\n\rFlash Diff address 0x");
    d2(address,HEX,4);
    d1(" length: ");
    d2(count,DEC,3);

    if (!addressAllowedToProgram(address, count))
    {
        dline(" Address protected!");
        setLastError(UDP_ADDRESS_NOT_ALLOWED_TO_FLASH, sendTel);
        return (T_ACK_PDU);
    }

    d1(" Address valid, ");
    crc = decompressor.getCrc32();
    if (crc != streamToUIn32(data + 3 + 4 + 4))
    {
        dline("CRC Error!");
        setLastError(UDP_CRC_ERROR, sendTel);
        return (T_ACK_PDU);
    }

    dline("CRC OK");
    if (decompressor.pageCompletedDoFlash())
    {
        setLastError(UDP_FLASH_ERROR, sendTel);
    }
    else
    {
        setLastError(UDP_IAP_SUCCESS, sendTel);
    }

    resetUPDProtocol(); // we need this, otherwise updSendDataToDecompress will run into a buffer overflow
#endif
     return (T_ACK_PDU);
}

unsigned char handleMemoryRequests(int apciCmd, bool * sendTel, unsigned char * data)
{
    // only allowed for APCI_MEMORY_WRITE_PDU
    if (apciCmd != APCI_MEMORY_WRITE_PDU)
    {
        dline("no memory command!");
        return (updUnkownCommand(sendTel));
    }

    unsigned int count = data[0] & 0x0f;
    byte updCommand = data[2];

#if defined(DEBUG) && (!(defined(TS_ARM)))
    digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));
#endif

    // simple command debugging message
    updCommand2Serial(updCommand);

    // check for commands allowed on a locked device
    if (!getDeviceUnlocked())
    {
        switch (updCommand)
        {
            // list of commands allowed on a locked device
            case UPD_UNLOCK_DEVICE:
            case UPD_REQUEST_UID:
            case UPD_APP_VERSION_REQUEST:
            case UPD_GET_LAST_ERROR:
                break;

            default:
                // device is locked -> command not allowed, send lastError and return
                if (!getDeviceUnlocked())
                {
                    setLastError(UDP_DEVICE_LOCKED, sendTel);
                    return (T_ACK_PDU);
                }
        }
    }

    // now comes the real work on the unlocked device
    switch (updCommand)
    {
        case UPD_UNLOCK_DEVICE:
            return (updUnlockDevice(sendTel, data));

        case UPD_REQUEST_UID:
            return (updRequestUID(sendTel));

        case UPD_APP_VERSION_REQUEST:
            return (updAppVersionRequest(sendTel, data));

        case UPD_GET_LAST_ERROR:
            return (updGetLastError(sendTel));

        case UPD_RESET:
            return (updResetDevice(sendTel));

        case UPD_ERASE_SECTOR:
            return (updUnkownCommand(sendTel)); // return (updEraseSector(sendTel, data));

        case UPD_SEND_DATA:
            return (updSendData(sendTel, data, count));

        case UPD_PROGRAM:
            return (updProgram(sendTel, data));

        case UPD_SEND_DATA_TO_DECOMPRESS:
            return (updSendDataToDecompress(sendTel, data, count));

        case UPD_PROGRAM_DECOMPRESSED_DATA:
            return (updProgramDecompressedDataToFlash(sendTel, data));

        case UPD_ERASE_COMPLETE_FLASH:
            return (updEraseFullFlash(sendTel));

        case UPD_ERASE_ADDRESSRANGE:
            return (updEraseAddressRange(sendTel, data));

        case UPD_DUMP_FLASH:
            return (updDumpFlashRange(sendTel, data));

        case UPD_REQUEST_STATISTIC:
            return (updRequestStatistic(sendTel));

        case UPD_UPDATE_BOOT_DESC:
            return (updUpdateBootDescriptorBlock(sendTel, data));

		case UPD_REQUEST_BOOT_DESC:
            return (udpRequestBootDescriptionBlock(sendTel));

		case UPD_REQUEST_BL_IDENTITY:
		    return (updRequestBootloaderIdentity(sendTel, data));

		case UPD_REQ_DATA:
		    return (updRequestData(sendTel));

        default:
            return (updUnkownCommand(sendTel));
    }
    return (T_NACK_PDU); //we should never land here
}


/** @}*/
