/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_UPD_UDP_PROTOCOL_1 UPD/UDP protocol
 * @ingroup SBLIB_BOOTLOADER
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

#include <sblib/eib.h>
#include <sblib/eib/bus.h>
#include <sblib/eib/apci.h>
#include <sblib/timeout.h>
#include <sblib/internal/iap.h>
#include <sblib/io_pin_names.h>
#include "bcu_updater.h"
#include "crc.h"
#include "update.h"
#if defined(DEBUG)
#   include "intelhex.h"
#endif

#ifdef DECOMPRESSOR
#   include "decompressor.h"
#endif

#if defined(DUMP_TELEGRAMS_LVL1) && defined(DEBUG)
#   include <sblib/serial.h>

#   define d1(x) {serial.print(x);}
#   define dline(x) {serial.println(x);}
#   define d2(u,v,w) {serial.print(u,v,w);}
#   define d3(x) {x;}
#else
#   define d1(x)
#   define d2(u,v,w)
#   define d3(x)
#   define dline(x)
#endif

#ifdef DECOMPRESSOR
    static Decompressor decompressor((AppDescriptionBlock*) BOOT_DSCR_ADDRESS); //!< get application base address from boot descriptor
#endif


#define DEVICE_LOCKED   ((unsigned int ) 0x5AA55AA5)     //!< magic number for device is locked and can't be flashed
#define DEVICE_UNLOCKED ((unsigned int ) ~DEVICE_LOCKED) //!< magic number for device is unlocked and flashing is allowed
#define ADDRESS_TO_SECTOR(a) ((a + FLASH_SECTOR_SIZE) / FLASH_SECTOR_SIZE) //!< address to sector conversion based on flash sector size
#define ADDRESS_TO_PAGE(a) ((a + FLASH_PAGE_SIZE) / FLASH_PAGE_SIZE)       //!< address to page conversion based on flash page size

#define PAGE_ALIGNMENT 0xff               //!< page alignment which is allowed to flash
static unsigned char ramBuffer[RAM_BUFFER_SIZE]; //!< RAM buffer used for flash operations

/**
 * @brief Control commands for flash process with APCI_MEMORY_READ_PDU and APCI_MEMORY_WRITE_PDU
 *
 */
enum UPD_Command
{
    UPD_ERASE_SECTOR = 0,               //!< Erase flash sector number (data[3]) @note device must be unlocked
    UPD_SEND_DATA = 1,                  //!< Copy ((data[0] & 0x0f)-1) bytes to ramBuffer starting from address data[3] @note device must be unlocked
    UPD_PROGRAM = 2,                    //!< Copy count (data[3-6]) bytes from ramBuffer to address (data[7-10]) in flash buffer, crc in data[11-14] @note device must be unlocked
    UPD_UPDATE_BOOT_DESC = 3,           //!< Flash a application boot descriptor block @note device must be unlocked
    UPD_SEND_DATA_TO_DECOMPRESS = 4,    //!< Copy bytes from telegram (data) to ramBuffer with differential method @note device must be unlocked
    UPD_PROGRAM_DECOMPRESSED_DATA = 5,  //!< Flash byte from rmBuffer to flash with differential method @note device must be unlocked

    UPD_REQ_DATA = 10,                  //!< Return bytes from flash at given address?  @note device must be unlocked
                                        //!<@warning Not implemented
    UPD_GET_LAST_ERROR = 20,            //!< Return last error
    UPD_SEND_LAST_ERROR = 21,           //!< Response for @ref UPD_GET_LAST_ERROR containing the last error

    UPD_UNLOCK_DEVICE = 30,             //!< Unlock the device for operations, which are only allowed on a unlocked device
    UPD_REQUEST_UID = 31,               //!< Return the 12 byte shorten UID (GUID) of the mcu @note device must be unlocked
    UPD_RESPONSE_UID = 32,              //!< Response for @ref UPD_REQUEST_UID containing the 12 first bytes of the UID
    UPD_APP_VERSION_REQUEST = 33,       //!< Return address of AppVersion string
    UPD_APP_VERSION_RESPONSE = 34,      //!< Response for @ref UPD_APP_VERSION_REQUEST containing the application version string
    UPD_RESET = 35,                     //!< Reset the device @note device must be unlocked
    UPD_REQUEST_BOOT_DESC = 36,         //!< Return the application boot descriptor block @note device must be unlocked
    UPD_RESPONSE_BOOT_DESC = 37,        //!< Response for @ref UPD_REQUEST_BOOT_DESC containing the application boot descriptor block

    UPD_REQUEST_BL_IDENTITY = 40,       //!< Return the boot loader identity @note device must be unlocked
    UPD_RESPONSE_BL_IDENTITY = 41,      //!< Response for @ref UPD_REQUEST_BL_IDENTITY containing the identity
    UPD_SET_EMULATION = 100             //!<@warning Not implemented
};

/**
 * @brief UDP_State
 *
 */
enum UDP_State
{
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

    // counting in hex so here is space for A-F
    UDP_FLASH_ERROR = 0x110,                  //!< page program (flash) failed
    UDP_PAGE_NOT_ALLOWED_TO_ERASE = 0x111,    //!< page not allowed to erase
    UDP_NOT_IMPLEMENTED = 0xFFFF              //!< this command is not yet implemented
};

Timeout mcuRestartRequestTimeout; //!< Timeout used to trigger a MCU Reset by NVIC_SystemReset()

// Try to avoid direct access to these global variables.
// It's better to use their get, set and reset functions
static unsigned int deviceLocked = DEVICE_LOCKED;   //!< current device locking state,
                                                    //!<@note It's better to use GetDeviceUnlocked() & setDeviceLockState()
static unsigned int lastError = 0;                  //!< last error while processing a UDP command
static unsigned int ramLocation = 0;                //!< current location of the ramBuffer processed
static unsigned int bytesReceived = 0;              //!< number of bytes received by UPD_SEND_DATA since last reset()
static unsigned int bytesFlashed = 0;               //!< number of bytes flashed by UPD_PROGRAM since last reset()

ALWAYS_INLINE void uInt32ToStream(unsigned char * buffer, unsigned int val);

void dumpFlashContent(AppDescriptionBlock * buffer)
{
    d3(
        serial.println();
        dumpToSerialinIntelHex(&serial, (unsigned char *) buffer->startAddress, buffer->endAddress - buffer->startAddress);
        serial.println();
    );
}

#ifdef DEBUG
/**
 * @brief Converts the name of a UPD_Command to "string" and
 *        sends it over the serial port
 *
 * @note Needs preprocessor macro DUMP_TELEGRAMS_LVL1
 * @warning This function is only for debugging purposes.
 * @param cmd Command to send to the serial port.
 */
static void updCommand2Serial(byte cmd)
{
    d3(
        switch (cmd)
        {
            case UPD_ERASE_SECTOR: d1("UPD_ERASE_SECTOR "); break;
            case UPD_SEND_DATA: d1("UPD_SEND_DATA "); break;
            case UPD_PROGRAM: d1("UPD_PROGRAM "); break;
            case UPD_UPDATE_BOOT_DESC: d1("UPD_UPDATE_BOOT_DESC "); break;
            case UPD_SEND_DATA_TO_DECOMPRESS: d1("UPD_SEND_DATA_TO_DECOMPRESS "); break;
            case UPD_PROGRAM_DECOMPRESSED_DATA: d1("UPD_PROGRAM_DECOMPRESSED_DATA "); break;
            case UPD_REQ_DATA: d1("UPD_REQ_DATA "); break;
            case UPD_GET_LAST_ERROR: d1("UPD_GET_LAST_ERROR "); break;
            case UPD_SEND_LAST_ERROR: d1("UPD_SEND_LAST_ERROR "); break;
            case UPD_UNLOCK_DEVICE: d1("UPD_UNLOCK_DEVICE "); break;
            case UPD_REQUEST_UID: d1("UPD_REQUEST_UID "); break;
            case UPD_RESPONSE_UID: d1("UPD_RESPONSE_UID "); break;
            case UPD_APP_VERSION_REQUEST: d1("UPD_APP_VERSION_REQUEST "); break;
            case UPD_APP_VERSION_RESPONSE: d1("UPD_APP_VERSION_RESPONSE "); break;
            case UPD_RESET: d1("UPD_RESET "); break;
            case UPD_REQUEST_BOOT_DESC: d1("UPD_REQUEST_BOOT_DESC "); break;
            case UPD_RESPONSE_BOOT_DESC: d1("UPD_RESPONSE_BOOT_DESC "); break;
            case UPD_REQUEST_BL_IDENTITY: d1("UPD_REQUEST_BL_IDENTITY "); break;
            case UPD_RESPONSE_BL_IDENTITY: d1("UPD_RESPONSE_BL_IDENTITY "); break;
            case UPD_SET_EMULATION: d1("UPD_SET_EMULATION "); break;
            default:  d3(serial.print("UPD_unkown command ", cmd)); break;
        }
    );
}
#endif

void restartRequest (unsigned int msec)
{
#ifdef DUMP_TELEGRAMS_LVL1
    serial.println("Systime: ", systemTime, DEC);
#endif
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
    return (buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
}

/**
 * @brief Converts a unsigned int into a 4 byte long provided buffer
 * @details A direct cast does not work due to possible miss aligned addresses.
 *          therefore a good old conversion has to be performed
 *
 * @param buffer in the 4 first bytes of buffer the result will be stored
 * @param val    the unsigned int to be converted
 * @warning function doesn't perform any sanity-checks on the provided buffer
 */
ALWAYS_INLINE void uInt32ToStream(unsigned char * buffer, unsigned int val)
{
    buffer[3] = (byte)(val >> 24);
    buffer[2] = (byte)(val >> 16);
    buffer[1] = (byte)(val >> 8);
    buffer[0] = (byte)(val & 0xff);
}

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
 * @brief Checks if the requested page is allowed to be erased.
 *
 * @param  pageNumber Page number to check erase is allowed
 * @return            true if page is allowed to erase, otherwise false
 */
static bool pageAllowedToErase(unsigned int pageNumber)
{
    return ((pageNumber >= ADDRESS_TO_PAGE(bootLoaderLastAddress() + 1)) &&
            ( pageNumber < ADDRESS_TO_PAGE(flashLastAddress())));
}

/**
 * @brief Checks if the requested sector is allowed to be erased.
 *
 * @param  sectorNumber Sector number to check erase is allowed
 * @return              true if sector is allowed to erase, otherwise false
 */
static bool sectorAllowedToErase(unsigned int sectorNumber)
{
    return ((sectorNumber >= ADDRESS_TO_SECTOR(bootLoaderLastAddress() + 1)) &&
            ( sectorNumber < ADDRESS_TO_SECTOR(flashLastAddress())));
}

/**
 * @brief Checks if the address range is allowed to be programmed
 *
 * @param start            start of the address range to check
 * @param length           length of the address range
 * @param isBootDescriptor set true to check range of the application boot descriptor
 *                         or false to check range of the application itself
 * @return                 true if programming is allowed, otherwise false
 */
static bool addressAllowedToProgram(unsigned int start, unsigned int length, bool isBootDescriptor = false)
{
    if ((start & 0xff) || !length) // not aligned to page or 0 length
    {
        return (0);
    }
    unsigned int end = start + length - 1;
    // check in range > bootloader and < flash end
    //FIXME also secure the application boot blocks
    if (isBootDescriptor)
    {
        return ((start > bootLoaderLastAddress()) && (end < APPLICATION_FIRST_SECTOR));
    }
    else
    {
        return (((start >= bootLoaderLastAddress()) && (end <= flashLastAddress())) &&
               (start >= APPLICATION_FIRST_SECTOR));
    }
}

/**
 * @brief Returns the unlocked status of the device
 *
 * @return true if device is unlocked otherwise false
 */
static bool getDeviceUnlocked()
{
#ifdef DUMP_TELEGRAMS_LVL1
    if (deviceLocked != DEVICE_UNLOCKED)
    {
        d3(serial.print("-->DEVICE_LOCKED"));
    }
#endif
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
#ifdef DUMP_TELEGRAMS_LVL1
    {
        if (deviceLocked == DEVICE_UNLOCKED)
        {
            d3(serial.print("-->DEVICE_UNLOCKED"));
        }
        else
        {
            d3(serial.print("-->DEVICE_LOCKED"));
        }
    }
#endif
}

/**
 * @brief Returns the state of the programming button
 *
 * @return true if programming mode ist active, otherwise false
 */
static bool getProgButtonState()
{
    bool state = (((BcuUpdate &) bcu).programmingMode());
#ifdef DUMP_TELEGRAMS_LVL1
    if (state)
    {
        d3(serial.print("-->progButton pressed"));
    }
#endif
    return (state);
}

/**
 * @brief Erases if allowed the requested sector.
 * @param sector  Sector number to be erased
 * @return        IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status
 */
static UDP_State eraseSector(unsigned int sector)
{
    UDP_State result = UDP_SECTOR_NOT_ALLOWED_TO_ERASE;
    d3(serial.print("Sector ", sector, DEC, 1));
    if (!sectorAllowedToErase(sector))
    {
        dline(" not allowed!");
        return (UDP_SECTOR_NOT_ALLOWED_TO_ERASE);
    }

    result = (UDP_State)iapEraseSector(sector);
    d3(
        if (result != (UDP_State)IAP_SUCCESS)
        {
            dline(" iapEraseSector failed!");
        }
        else
        {
            dline(" OK");
        }
    );
    return (result);
}

/**
 * @brief Erases if allowed the requested page.
 * @param page  Page number to be erased
 * @return      IAP_SUCCESS if successful, otherwise @ref UDP_PAGE_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status
 */
static UDP_State erasePage(unsigned int page)
{
    UDP_State result = UDP_PAGE_NOT_ALLOWED_TO_ERASE;
    d3(serial.print("Page ", page, DEC, 1));
    if (!pageAllowedToErase(page))
    {
        dline(" not allowed!");
        return (UDP_PAGE_NOT_ALLOWED_TO_ERASE);
    }

    result = (UDP_State)iapErasePage(page);
    d3(
        if (result != (UDP_State)IAP_SUCCESS)
        {
            dline(" iapErasePage failed!");
        }
        else
        {
            dline(" OK");
        }
    );
    return (result);
}

/**
 * @brief Programs the specified number of bytes from the RAM to the specified location
 *        inside the FLASH.
 * @param address start address of inside the FLASH
 * @param ram     start address of the buffer containing the data
 * @param size    number of bytes to program
 * @param isBootDescriptor  Set true to program an @ref AppDescriptionBlock, default false
 *
 * @return        IAP_SUCCESS if successful, otherwise UDP_ADDRESS_NOT_ALLOWED_TO_FLASH
 *                or a @ref IAP_Status
 * @warning       The function calls iap_Program which by itself calls no_interrupts().
 */
static UDP_State programFlash(unsigned int address, const byte* ram, unsigned int size, bool isBootDescriptor = false)
{
    // lastError = iapProgram((byte *) address, ramBuffer, FLASH_PAGE_SIZE);
    UDP_State result = UDP_ADDRESS_NOT_ALLOWED_TO_FLASH;
    if (!addressAllowedToProgram(address, size, isBootDescriptor))
    {
        return (UDP_ADDRESS_NOT_ALLOWED_TO_FLASH);
    }

    result = (UDP_State)iapProgram((byte *) address, ram, size);
    return (result);
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

void resetProtocol(void)
{
    lastError = 0;
    ramLocation = 0;
    bytesReceived = 0;
    bytesFlashed = 0;
}

/**
 * @brief Handles the @ref UPD_UNLOCK_DEVICE command.
 *        The device is unlocked if the programming mode is active or the provided UID (guid) is valid.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    Buffer for the UID
 * @post          calls setLastErrror with IAP_SUCCESS if device is unlocked, otherwise @ref UDP_UID_MISMATCH or a @ref IAP_Status.
 * @return        always T_ACK_PDU
 */
static unsigned char updUnlockDevice(bool * sendTel, unsigned char * data)
{
    if (getProgButtonState())
    { // the operator has physical access to the device -> we unlock it
        setDeviceLockState(DEVICE_UNLOCKED);
        setLastError((UDP_State)IAP_SUCCESS, sendTel);
        dline(" by Button");
    }
    else
    {   // we need to ensure that only authorized operators can
        // update the application
        // as a simple method we use the unique ID of the CPU itself
        // only if this UID (GUID) is known, the device will be unlocked
        byte uid[IAP_UID_LENGTH];
        lastError = iapReadUID(uid);
        if (lastError != IAP_SUCCESS)
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
        setLastError((UDP_State)IAP_SUCCESS, sendTel);
        dline(" by UID");
        resetProtocol();
    }
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_RESET command and calls @ref restartRequest to prepare a reset of the device.
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @post          calls setLastErrror always with IAP_SUCCESS
 * @return        always T_ACK_PDU
 */
static unsigned char updResetDevice(bool * sendTel)
{
    restartRequest(RESET_DELAY_MS); // request restart in RESET_DELAY_MS ms to allow transmission of ACK before
    setLastError((UDP_State)IAP_SUCCESS, sendTel);
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_APP_VERSION_REQUEST command and sends a @ref UPD_APP_VERSION_RESPONSE
 *        The response contains the address of the AppVersion string
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    Buffer to store the AppVersion string address
 * @post          sets lastError always to IAP_SUCCESS
 * @return        always T_ACK_PDU
 */
static unsigned char updAppVersionRequest(bool * sendTel, unsigned char * data)
{
    unsigned char* appversion;
    appversion = getAppVersion((AppDescriptionBlock *) (APPLICATION_FIRST_SECTOR - (1 + data[3]) * BOOT_BLOCK_DESC_SIZE));
    lastError = IAP_SUCCESS;
    *sendTel = prepareReturnTelegram(BL_ID_STRING_LENGTH - 1, UPD_APP_VERSION_RESPONSE);
    memcpy(bcu.sendTelegram + 10, appversion, BL_ID_STRING_LENGTH - 1);
#ifdef DUMP_TELEGRAMS_LVL1
    if (appversion != bl_id_string)
    {
        d3(serial.print("AppVersionRequest OK: "));
    }
    else
    {
        d3(serial.print("AppVersionRequest outside range (default): "));
    }
    for (int i = 0; i < BL_ID_STRING_LENGTH - 1; i++)
    {
        serial.print((char)appversion[i]);
    }
    d3(serial.println());
#endif
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_ERASE_SECTOR command, erases if allowed the requested sector and resets @ref ramLocation
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    data[3] should contain the sector number to be erased
 * @post          calls setLastErrror with IAP_SUCCESS if successful, otherwise @ref UDP_SECTOR_NOT_ALLOWED_TO_ERASE or a @ref IAP_Status.
 * @return        always T_ACK_PDU
 * @note          device must be unlocked
 */
static unsigned char updEraseSector(bool * sendTel, unsigned char * data)
{
    resetProtocol();
    setLastError(eraseSector(data[3]), sendTel);
    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_SEND_DATA command and copies the received bytes from data into @ref ramBuffer
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    data[3] must contain the ramBuffer location and data[4...] the bytes to copy to the ramBuffer
 * @param nCount  Number of bytes to copy
 * @post          calls setLastErrror with IAP_SUCCESS if successful, otherwise @ref UDP_RAM_BUFFER_OVERFLOW or a @ref IAP_Status
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
    lastError = IAP_SUCCESS;
    setLastError((UDP_State)IAP_SUCCESS, sendTel);
    for(unsigned int i=0; i<nCount; i++)
    {
        d2(data[i+4],HEX,2);
        d1(" ");
    }
    d3(serial.print("at: ", ramLocation, DEC, 3));
    d3(serial.println(" #", nCount, DEC, 2));

    return (T_ACK_PDU);
}

/**
 * @brief Handles the @ref UPD_PROGRAM command and copies the bytes from ramBuffer to flash
 *
 * @param sendTel true if a @ref UPD_SEND_LAST_ERROR response telegram should be send, otherwise false
 * @param data    the number of bytes to flash is in data[3-6] the flash address to program in data[7-10]
 * @post          calls setLastErrror with IAP_SUCCESS if successful, otherwise a @ref UDP_State or a @ref IAP_Status
 * @return        T_ACK_PDU
 * @note          device must be unlocked
 * @warning       The function calls iap_Program which by itself calls no_interrupts().
 */
static unsigned char updProgramFlash(bool * sendTel, unsigned char * data)
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

    d3(serial.print(" flashWrite writing ", flash_count));
    d3(serial.print(" bytes @ 0x", address, HEX, 8));
    d3(serial.println(" crc 0x", crcRamBuffer, HEX, 8));

    // Everything is setup, wait for a free bus, so no interrupt will kill us
    /*
    while (!bus.idle())
            ;
            */
    bytesFlashed += flash_count;
    UDP_State error = (UDP_State)iapProgram((byte *) address, ramBuffer, flash_count);
    if (error != (UDP_State)IAP_SUCCESS)
    {
        error = UDP_CRC_ERROR;
    }

    setLastError(error, sendTel);
    return (T_ACK_PDU);
}

static unsigned char updRequestBootloaderIdentity(bool * sendTel)
{
    unsigned int bl_identity = BL_IDENTITY;
    unsigned int bl_features = BL_FEATURES;
    unsigned int bl_lastAddress = bootLoaderLastAddress();
    *sendTel = prepareReturnTelegram(12, UPD_RESPONSE_BL_IDENTITY);
    uInt32ToStream(bcu.sendTelegram + 10, bl_identity);  // Bootloader identity
    uInt32ToStream(bcu.sendTelegram + 14, bl_features);  // Bootloader features
    uInt32ToStream(bcu.sendTelegram + 18, bl_lastAddress); // Bootloader size
    d3(serial.print("BL ID 0x", bl_identity, HEX, 8));
    d3(serial.print("    BL feature 0x", bl_features, HEX, 8));
    d3(serial.println("    BL size    0x", bl_lastAddress, HEX, 8));
    return (T_ACK_PDU);
}

static unsigned char udpRequestBootDescriptionBlock(bool * sendTel)
{
    //TODO maybe check if the block is valid?
    AppDescriptionBlock* bootDescr = (AppDescriptionBlock *) BOOT_DSCR_ADDRESS; // Address of boot block descriptor
    *sendTel = prepareReturnTelegram(12, UPD_RESPONSE_BOOT_DESC);
    memcpy(bcu.sendTelegram + 10, bootDescr, 12); // startAddress, endAddress, crc

    d3(serial.print("FW start@ 0x", bootDescr->startAddress , HEX, 8));   // Firmware start address
    d3(serial.print(" end@ 0x", bootDescr->endAddress, HEX, 8));        // Firmware end address
    d3(serial.print(" Desc.@ 0x", bootDescr->appVersionAddress, HEX, 8)); // Firmware App descriptor address (for getAppVersion())
    d3(serial.println(" CRC : 0x", bootDescr->crc, HEX, 8));                // Firmware CRC
    return (T_ACK_PDU);
}

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
    if (result != (UDP_State)IAP_SUCCESS)
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

static unsigned updGetLastError(bool * sendTel)
{
    setLastError((UDP_State)lastError, sendTel);
    return (T_ACK_PDU);
}

static unsigned char updUnkownCommand(bool * sendTel)
{
    setLastError(UDP_UNKNOWN_COMMAND, sendTel); // set to any error
    return (T_ACK_PDU);
}

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
        serial.println("Bytes Rx    ", bytesReceived - count);
        serial.println("Bytes Flash ", bytesFlashed); //
        serial.println("Diff        ", (int)bytesReceived - (int)bytesFlashed); // difference here is normal, because flashing is always in multiple of FLASH_PAGE_SIZE
        serial.println();
        serial.println("FW start@ 0x", streamToUIn32(ramBuffer), HEX, 4);    // Firmware start address
        serial.println("FW end  @ 0x", streamToUIn32(ramBuffer+4), HEX, 4);  // Firmware end address
        serial.println("FW Desc.@ 0x", streamToUIn32(ramBuffer+12), HEX, 4); // Firmware App descriptor address (for getAppVersion())
        serial.println("FWs CRC : 0x", streamToUIn32(ramBuffer+8), HEX, 8);  // Firmware CRC
        // serial.println("RamBuffer[16-20] 0x", streamToUIn32(ramBuffer+16), HEX, 8);// This is beyond the descriptor block
    )


    //address = FIRST_SECTOR - (1 + data[7]) * BOOT_BLOCK_SIZE; // start address of the descriptor block
    address = BOOT_DSCR_ADDRESS;                // start address of boot block descriptor
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
    //If send descriptor is not equal to current one, flash it
    if(memcmp((byte *) address, ramBuffer, count) == 0)
    {
        d3(serial.println("is equal, skipping"));
        result = (UDP_State)IAP_SUCCESS;
        // dont return here, let's also check the AppDescriptionBlock
    }
    else
    {
        d3(serial.print("it's different, Erase Page:"));
        result = erasePage(BOOT_BLOCK_PAGE); // - data[7]);
        if (result != ((UDP_State)IAP_SUCCESS))
        {
            setLastError(result, sendTel);
            return (T_ACK_PDU);
        }

        d3(serial.print(" OK, Flash Page:"));

        result = programFlash(address, ramBuffer, FLASH_PAGE_SIZE, true); // no less than 256byte can be flashed
        if (result == ((UDP_State)IAP_SUCCESS))
        {
            d3(serial.println(" OK"));
        }
        else if (result == UDP_ADDRESS_NOT_ALLOWED_TO_FLASH)
        {
            d3(serial.println(" -->programFlash: UDP_ADDRESS_NOT_ALLOWED_TO_FLASH"));
            setLastError(UDP_ADDRESS_NOT_ALLOWED_TO_FLASH, sendTel);
            return (T_ACK_PDU);
        }
        else
        {
            d3(serial.println(" -->programFlash: ", result, DEC, 2));
            setLastError((UDP_State)result, sendTel);
        }
    }
    // dumpFlashContent((AppDescriptionBlock *) ramBuffer);
    if (!checkApplication((AppDescriptionBlock *) ramBuffer))
    {
        d3(
            serial.println("-->UDP_APPLICATION_NOT_STARTABLE");
            //DONE turn back on and remove above
            dumpFlashContent((AppDescriptionBlock *) ramBuffer);
        );
        setLastError(UDP_APPLICATION_NOT_STARTABLE, sendTel);
        return (T_ACK_PDU);
    }

    setLastError((UDP_State)result, sendTel);
    return (T_ACK_PDU);
}

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
    setLastError((UDP_State)IAP_SUCCESS, sendTel);
#endif
    return (T_ACK_PDU);
}

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
        setLastError((UDP_State)IAP_SUCCESS, sendTel);
    }

    resetProtocol(); //TODO check this rly is right?
#endif
     return (T_ACK_PDU);
}

unsigned char handleMemoryRequests(int apciCmd, bool * sendTel, unsigned char * data)
{
    // only allowed for APCI_MEMORY_READ_PDU or APCI_MEMORY_WRITE_PDU
    if ((apciCmd != APCI_MEMORY_READ_PDU) && (apciCmd != APCI_MEMORY_WRITE_PDU))
    {
        return (updUnkownCommand(sendTel));
    }

    unsigned int count = data[0] & 0x0f;
    byte updCommand = data[2];

#ifdef DEBUG
    digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));
#endif

    // simple command debugging message
    d3(updCommand2Serial(updCommand));

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
            return (updEraseSector(sendTel, data));

        case UPD_SEND_DATA:
            return (updSendData(sendTel, data, count));

        case UPD_PROGRAM:
            return (updProgramFlash(sendTel, data));

        case UPD_SEND_DATA_TO_DECOMPRESS:
            return (updSendDataToDecompress(sendTel, data, count));

        case UPD_PROGRAM_DECOMPRESSED_DATA:
            return (updProgramDecompressedDataToFlash(sendTel, data));

        case UPD_UPDATE_BOOT_DESC:
            return (updUpdateBootDescriptorBlock(sendTel, data));

		case UPD_REQUEST_BOOT_DESC:
            return (udpRequestBootDescriptionBlock(sendTel));

		case UPD_REQUEST_BL_IDENTITY:
		    return (updRequestBootloaderIdentity(sendTel));

		case UPD_REQ_DATA:
		    return (updRequestData(sendTel));

        default:
            return (updUnkownCommand(sendTel));
    }
    return (T_NACK_PDU); //we should never land here
}

//DEBUG delete and use code in other file if done here
//unsigned int checkVectorTableDBG(unsigned int start)
//{
//    unsigned int i;
//    unsigned int * address;
//    unsigned int cs = 0;
//    address = (unsigned int *) start; // Vector table start always at base address, each entry is 4 byte
//
//    for (i = 0; i < 7; i++)               // Checksum is 2's complement of entries 0 through 6
//        cs += address[i];
//    //if (address[7])
//
//    d1("\n\rVT Check 0 ");
//    d2(address[0],HEX,8);
//    d1("VT Check 1 ");
//  d2(address[1],HEX,8);
//  d1("VT Check 2 ");
//  d2(address[2],HEX,8);
//  d1("VT Check 3 ");
//  d2(address[3],HEX,8);
//  d1("VT Check 4 ");
//  d2(address[4],HEX,8);
//  d1("VT Check 5 ");
//  d2(address[5],HEX,8);
//  d1("VT Check 6 ");
//  d2(address[6],HEX,8);
//  d1("VT Check Res ");
//  d2(address[7],HEX,8);
//  d1("Should be == ");
//  d2(~cs+1,HEX,8);
//
//    return (~cs+1);
//}

/** @}*/
