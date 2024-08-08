package org.selfbus.updater.upd;

import java.util.HashMap;
import java.util.Map;

/**
 * Implementation of the UPD/UDP protocol control commands
 * see /Bus-Updater/inc/upd_protocol.h for details
 */
public enum UPDCommand {
    // ERASE_SECTOR(0, "ERASE_SECTOR"),                            //!< Erase flash sector number (data[3]) @note device must be unlocked
                                                                //!< @note deprecated, use ERASE_ADDRESSRANGE instead
    SEND_DATA((byte)0xef, "SEND_DATA"),                                  //!< Copy ((data[0] & 0x0f)-1) bytes to ramBuffer starting from address data[3] @note device must be unlocked
    PROGRAM((byte)0xee, "PROGRAM"),                                      //!< Copy count (data[3-6]) bytes from ramBuffer to address (data[7-10]) in flash buffer, crc in data[11-14] @note device must be unlocked
    UPDATE_BOOT_DESC((byte)0xed, "UPDATE_BOOT_DESC"),                    //!< Flash an application boot descriptor block @note device must be unlocked
    SEND_DATA_TO_DECOMPRESS((byte)0xec, "SEND_DATA_TO_DECOMPRESS"),      //!< Copy bytes from telegram (data) to ramBuffer with differential method @note device must be unlocked
    PROGRAM_DECOMPRESSED_DATA((byte)0xeb, "PROGRAM_DECOMPRESSED_DATA"),  //!< Flash bytes from ramBuffer to flash with differential method @note device must be unlocked
    ERASE_COMPLETE_FLASH((byte)0xea, "ERASE_COMPLETE_FLASH"),            //!< Erase the entire flash area excluding the bootloader itself @note device must be unlocked
    ERASE_ADDRESS_RANGE((byte)0xe9, "ERASE_ADDRESS_RANGE"),              //!< Erase flash from given start address to end address (start: data[3-6] end: data[7-10]) @note device must be unlocked
    REQ_DATA((byte)0xe8, "REQ_DATA"),                                   //!< Return bytes from flash at given address?  @note device must be unlocked
                                                                //!<@warning Not implemented
    DUMP_FLASH((byte)0xe7, "DUMP_FLASH"),                               //!< DUMP the flash of a given address range (data[0-3] - data[4-7]) to serial port of the mcu, works only with DEBUG version of bootloader
    REQUEST_STATISTIC((byte)0xdf, "STATISTIC_REQUEST"),                 //!< Return some statistic data for the active connection
    RESPONSE_STATISTIC((byte)0xde, "STATISTIC_RESPONSE"),               //!< Response for @ref UPD_STATISTIC_RESPONSE containing the statistic data
    SEND_LAST_ERROR((byte)0xdc, "SEND_LAST_ERROR"),                     //!< Response containing the last error
    UNLOCK_DEVICE((byte)0xbf, "UNLOCK_DEVICE"),                         //!< Unlock the device for operations, which are only allowed on an unlocked device
    REQUEST_UID((byte)0xbe, "REQUEST_UID"),                             //!< Return the 12 byte shorten UID (GUID) of the mcu @note device must be unlocked
    RESPONSE_UID((byte)0xbd, "RESPONSE_UID"),                           //!< Response for @ref REQUEST_UID containing the first 12 bytes of the UID
    APP_VERSION_REQUEST((byte)0xbc, "APP_VERSION_REQUEST"),             //!< Return address of AppVersion string
    APP_VERSION_RESPONSE((byte)0xbb, "APP_VERSION_RESPONSE"),           //!< Response for @ref APP_VERSION_REQUEST containing the application version string
    // RESET(35, "RESET"),                                         //!< Reset the device @note device must be unlocked
    REQUEST_BOOT_DESC((byte)0xba, "REQUEST_BOOT_DESC"),                 //!< Return the application boot descriptor block @note device must be unlocked
    RESPONSE_BOOT_DESC((byte)0xb9, "RESPONSE_BOOT_DESC"),               //!< Response for @ref REQUEST_BOOT_DESC containing the application boot descriptor block
    REQUEST_BL_IDENTITY((byte)0xb8, "REQUEST_BL_IDENTITY"),             //!< Return the bootloader's identity @note device must be unlocked
    RESPONSE_BL_IDENTITY((byte)0xb7, "RESPONSE_BL_IDENTITY"),           //!< Response for @ref UPD_REQUEST_BL_IDENTITY containing the identity
    RESPONSE_BL_VERSION_MISMATCH((byte)0xb6, "RESPONSE_BL_VERSION_MISMATCH"), //!< Response for @ref UPD_REQUEST_BL_IDENTITY containing the minimum required major and minor version of Selfbus Updater
    SET_EMULATION((byte)0x01, "SET_EMULATION");                        //!<@warning Not implemented

    private static final Map<Byte, UPDCommand> BY_INDEX = new HashMap<>();
    static {
        for (UPDCommand e: values()) {
            BY_INDEX.put(e.id, e);
        }
    }

    public final byte id;
    private final String description;
    UPDCommand(byte id, String description) {
        this.id = id;
        this.description = description;
    }

    public static UPDCommand valueOfIndex(Integer index) {
        return BY_INDEX.get(index);
    }
    @Override
    public String toString() {
        return String.format("%s.%s", this.getClass().getSimpleName(), this.description);
    }
}
