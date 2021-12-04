package org.hkfree.knxduino.updater.upd;

import java.util.HashMap;
import java.util.Map;

/**
 * Implementation of the UPD/UDP protocol control commands
 * see /Bus-Updater/inc/upd_protocol.h for details
 */
public enum UPDCommand {
    ERASE_SECTOR(0),                    //!< Erase flash sector number (data[3]) @note device must be unlocked
                                            //!< @note deprecated, use ERASE_ADDRESSRANGE instead
    SEND_DATA(1),                       //!< Copy ((data[0] & 0x0f)-1) bytes to ramBuffer starting from address data[3] @note device must be unlocked
    PROGRAM(2),                         //!< Copy count (data[3-6]) bytes from ramBuffer to address (data[7-10]) in flash buffer, crc in data[11-14] @note device must be unlocked
    UPDATE_BOOT_DESC(3),                //!< Flash an application boot descriptor block @note device must be unlocked
    SEND_DATA_TO_DECOMPRESS(4),         //!< Copy bytes from telegram (data) to ramBuffer with differential method @note device must be unlocked
    PROGRAM_DECOMPRESSED_DATA(5),       //!< Flash bytes from ramBuffer to flash with differential method @note device must be unlocked
    ERASE_COMPLETE_FLASH(7),            //!< Erase the entire flash area excluding the bootloader itself @note device must be unlocked
    ERASE_ADDRESS_RANGE(8),             //!< Erase flash from given start address to end address (start: data[3-6] end: data[7-10]) @note device must be unlocked
    REQ_DATA(10),                       //!< Return bytes from flash at given address?  @note device must be unlocked
                                            //!<@warning Not implemented
    GET_LAST_ERROR(20),                 //!< Returns last error
    SEND_LAST_ERROR(21),                //!< Response for @ref GET_LAST_ERROR containing the last error
    UNLOCK_DEVICE(30),                  //!< Unlock the device for operations, which are only allowed on a unlocked device
    REQUEST_UID(31),                    //!< Return the 12 byte shorten UID (GUID) of the mcu @note device must be unlocked
    RESPONSE_UID(32),                   //!< Response for @ref REQUEST_UID containing the first 12 bytes of the UID
    APP_VERSION_REQUEST(33),            //!< Return address of AppVersion string
    APP_VERSION_RESPONSE(34),           //!< Response for @ref APP_VERSION_REQUEST containing the application version string
    RESET(35),                          //!< Reset the device @note device must be unlocked
    REQUEST_BOOT_DESC(36),              //!< Return the application boot descriptor block @note device must be unlocked
    RESPONSE_BOOT_DESC(37),             //!< Response for @ref REQUEST_BOOT_DESC containing the application boot descriptor block
    REQUEST_BL_IDENTITY(40),            //!< Return the bootloader's identity @note device must be unlocked
    RESPONSE_BL_IDENTITY(41),           //!< Response for @ref REQUEST_BL_IDENTITY containing the identity
    SET_EMULATION(100);                 //!<@warning Not implemented

    private static final Map<Integer, UPDCommand> BY_INDEX = new HashMap<>();
    static {
        for (UPDCommand e: values()) {
            BY_INDEX.put(e.id, e);
        }
    }

    public final Integer id;
    UPDCommand(Integer id) {
        this.id = id;
    }

    public static UPDCommand valueOfIndex(Integer index) {
        return BY_INDEX.get(index);
    }
}
