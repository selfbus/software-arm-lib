package org.selfbus.updater.upd;

import java.util.HashMap;
import java.util.Map;

/**
 * Implementation of the UPD/UDP protocol control commands
 * see /Bus-Updater/inc/upd_protocol.h for details
 */
public enum UPDCommand {
    ERASE_SECTOR(0, "ERASE_SECTOR"),                            //!< Erase flash sector number (data[3]) @note device must be unlocked
                                                                //!< @note deprecated, use ERASE_ADDRESSRANGE instead
    SEND_DATA(1, "SEND_DATA"),                                  //!< Copy ((data[0] & 0x0f)-1) bytes to ramBuffer starting from address data[3] @note device must be unlocked
    PROGRAM(2, "PROGRAM"),                                      //!< Copy count (data[3-6]) bytes from ramBuffer to address (data[7-10]) in flash buffer, crc in data[11-14] @note device must be unlocked
    UPDATE_BOOT_DESC(3, "UPDATE_BOOT_DESC"),                    //!< Flash an application boot descriptor block @note device must be unlocked
    SEND_DATA_TO_DECOMPRESS(4, "SEND_DATA_TO_DECOMPRESS"),      //!< Copy bytes from telegram (data) to ramBuffer with differential method @note device must be unlocked
    PROGRAM_DECOMPRESSED_DATA(5, "PROGRAM_DECOMPRESSED_DATA"),  //!< Flash bytes from ramBuffer to flash with differential method @note device must be unlocked
    ERASE_COMPLETE_FLASH(7, "ERASE_COMPLETE_FLASH"),            //!< Erase the entire flash area excluding the bootloader itself @note device must be unlocked
    ERASE_ADDRESS_RANGE(8, "ERASE_ADDRESS_RANGE"),              //!< Erase flash from given start address to end address (start: data[3-6] end: data[7-10]) @note device must be unlocked
    REQ_DATA(10, "REQ_DATA"),                                   //!< Return bytes from flash at given address?  @note device must be unlocked
                                                                //!<@warning Not implemented
    DUMP_FLASH(11, "DUMP_FLASH"),                               //!< DUMP the flash of a given address range (data[0-3] - data[4-7]) to serial port of the mcu, works only with DEBUG version of bootloader
    REQUEST_STATISTIC(12, "STATISTIC_REQUEST"),                 //!< Return some statistic data for the active connection
    RESPONSE_STATISTIC(13, "STATISTIC_RESPONSE"),               //!< Response for @ref UPD_STATISTIC_RESPONSE containing the statistic data
    GET_LAST_ERROR(20, "GET_LAST_ERROR"),                       //!< Returns last error
    SEND_LAST_ERROR(21, "SEND_LAST_ERROR"),                     //!< Response for @ref GET_LAST_ERROR containing the last error
    UNLOCK_DEVICE(30, "UNLOCK_DEVICE"),                         //!< Unlock the device for operations, which are only allowed on a unlocked device
    REQUEST_UID(31, "REQUEST_UID"),                             //!< Return the 12 byte shorten UID (GUID) of the mcu @note device must be unlocked
    RESPONSE_UID(32, "RESPONSE_UID"),                           //!< Response for @ref REQUEST_UID containing the first 12 bytes of the UID
    APP_VERSION_REQUEST(33, "APP_VERSION_REQUEST"),             //!< Return address of AppVersion string
    APP_VERSION_RESPONSE(34, "APP_VERSION_RESPONSE"),           //!< Response for @ref APP_VERSION_REQUEST containing the application version string
    RESET(35, "RESET"),                                         //!< Reset the device @note device must be unlocked
    REQUEST_BOOT_DESC(36, "REQUEST_BOOT_DESC"),                 //!< Return the application boot descriptor block @note device must be unlocked
    RESPONSE_BOOT_DESC(37, "RESPONSE_BOOT_DESC"),               //!< Response for @ref REQUEST_BOOT_DESC containing the application boot descriptor block
    REQUEST_BL_IDENTITY(40, "REQUEST_BL_IDENTITY"),             //!< Return the bootloader's identity @note device must be unlocked
    RESPONSE_BL_IDENTITY(41, "RESPONSE_BL_IDENTITY"),           //!< Response for @ref REQUEST_BL_IDENTITY containing the identity
    SET_EMULATION(100, "SET_EMULATION");                        //!<@warning Not implemented

    private static final Map<Integer, UPDCommand> BY_INDEX = new HashMap<>();
    static {
        for (UPDCommand e: values()) {
            BY_INDEX.put(e.id, e);
        }
    }

    public final Integer id;
    private final String description;
    UPDCommand(Integer id, String description) {
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
