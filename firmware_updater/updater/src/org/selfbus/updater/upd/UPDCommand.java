package org.selfbus.updater.upd;

import java.util.HashMap;
import java.util.Map;

/**
 * Implementation of the UPD/UDP protocol control commands
 * <p>
 * See {@code /firmware_updater/bootloader/inc/upd_protocol.h} for details
 * </p>
 */
public enum UPDCommand {
    /**
     * Marks an invalid {@link UPDCommand} code
     */
    INVALID((byte) 0, "INVALID"),
    /**
     * Copy ((data[0] & 0x0f)-1) bytes to ramBuffer starting from address data[3].
     * <p>Device must be unlocked.</p>
     */
    SEND_DATA((byte)0xef, "SEND_DATA"),
    /**
     * Copy count (data[3-6]) bytes from ramBuffer to address (data[7-10]) in flash buffer, crc in data[11-14].
     * <p>Device must be unlocked.</p>
     */
    PROGRAM((byte)0xee, "PROGRAM"),
    /**
     * Flash an application boot descriptor block.
     * <p>Device must be unlocked.</p>
     */
    UPDATE_BOOT_DESC((byte)0xed, "UPDATE_BOOT_DESC"),
    /**
     * Copy bytes from telegram (data) to ramBuffer with differential method.
     * <p>Device must be unlocked.</p>
     */
    SEND_DATA_TO_DECOMPRESS((byte)0xec, "SEND_DATA_TO_DECOMPRESS"),
    /**
     * Flash bytes from ramBuffer to flash with differential method.
     * <p>Device must be unlocked.</p>
     */
    PROGRAM_DECOMPRESSED_DATA((byte)0xeb, "PROGRAM_DECOMPRESSED_DATA"),
    /**
     * Erase the entire flash area excluding the bootloader itself.
     * <p>Device must be unlocked.</p>
     */
    ERASE_COMPLETE_FLASH((byte)0xea, "ERASE_COMPLETE_FLASH"),
    /**
     * Erase flash from given start address to end address (start: data[3-6] end: data[7-10]).
     * <p>Device must be unlocked.</p>
     */
    ERASE_ADDRESS_RANGE((byte)0xe9, "ERASE_ADDRESS_RANGE"),
    /**
     * <p><strong>Warning:</strong> Not implemented.</p>
     * Return bytes from flash at given address.
     */
    REQ_DATA((byte)0xe8, "REQ_DATA"),
    /**
     * DUMP the flash of a given address range (data[0-3] - data[4-7]) to the serial port of the mcu.
     * <p><strong>Note:</strong> Works only with DEBUG version of the bootloader.</p>
     */
    DUMP_FLASH((byte)0xe7, "DUMP_FLASH"),
    /**
     * Request some statistic data for the active connection.
     */
    REQUEST_STATISTIC((byte)0xdf, "STATISTIC_REQUEST"),
    /**
     * Response for {@link #REQUEST_STATISTIC} containing the statistic data.
     */
    RESPONSE_STATISTIC((byte)0xde, "STATISTIC_RESPONSE"),
    /**
     * Response containing the last error.
     */
    SEND_LAST_ERROR((byte)0xdc, "SEND_LAST_ERROR"),
    /**
     * Unlock the device for operations, which are only allowed on an unlocked device.
     */
    UNLOCK_DEVICE((byte)0xbf, "UNLOCK_DEVICE"),
    /**
     * Request the 12 byte shorten UID (GUID) of the mcu.
     * <p>Device must be unlocked.</p>
     */
    REQUEST_UID((byte)0xbe, "REQUEST_UID"),
    /**
     * Response for {@link #REQUEST_UID} containing the first 12 bytes of the UID.
     */
    RESPONSE_UID((byte)0xbd, "RESPONSE_UID"),
    /**
     * Request address of AppVersion string.
     */
    APP_VERSION_REQUEST((byte)0xbc, "APP_VERSION_REQUEST"),
    /**
     * Response for {@link #APP_VERSION_REQUEST} containing the application version string.
     */
    APP_VERSION_RESPONSE((byte)0xbb, "APP_VERSION_RESPONSE"),
    /*
     * Reset the device.
     * <p>Device must be unlocked.</p>
     */
    //RESET((byte) 35, "RESET"),
    /**
     * Request the application boot descriptor block.
     * <p>Device must be unlocked.</p>
     */
    REQUEST_BOOT_DESC((byte)0xba, "REQUEST_BOOT_DESC"),
    /**
     * Response for {@link #REQUEST_BOOT_DESC} containing the application boot descriptor block.
     */
    RESPONSE_BOOT_DESC((byte)0xb9, "RESPONSE_BOOT_DESC"),
    /**
     * Request the bootloader's identity.
     * <p>Device must be unlocked.</p>
     */
    REQUEST_BL_IDENTITY((byte)0xb8, "REQUEST_BL_IDENTITY"),
    /**
     * Response for {@link #REQUEST_BL_IDENTITY} containing the identity.
     */
    RESPONSE_BL_IDENTITY((byte)0xb7, "RESPONSE_BL_IDENTITY"),
    /**
     * Negative response for {@link #REQUEST_BL_IDENTITY}
     * containing the minimum required major and minor version of the Selfbus Updater.
     */
    RESPONSE_BL_VERSION_MISMATCH((byte)0xb6, "RESPONSE_BL_VERSION_MISMATCH"),
    /**
     * <p><strong>Warning:</strong> Not implemented.</p>
     */
    SET_EMULATION((byte)0x01, "SET_EMULATION");

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
