package org.selfbus.updater.upd;

import org.selfbus.updater.Utils;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * Implementation of the UPD/UDP protocol result commands
 * see /Bus-Updater/inc/upd_protocol.h for details
 */
public enum UDPResult {
    // IAP_ are in decimal (IAP ist in application programming of the MCU's flash
    // these results are defined in iap.h Selfbus sblib

    /** Flash (IAP) Command is executed successfully */
    IAP_SUCCESS(0, "Flash (IAP) Command is executed successfully.", false),
    /** Flash (IAP) Invalid command */
    IAP_INVALID_COMMAND(1, "Flash (IAP) Invalid command.", true),
    /** Flash (IAP) Source address is not on a word boundary */
    IAP_SRC_ADDR_ERROR(2, "Flash (IAP) Source address is not on a word boundary.", true),
    /** Flash (IAP) Destination address is not on a correct boundary */
    IAP_DST_ADDR_ERROR(3, "Flash (IAP) Destination address is not on a correct boundary.", true),
    /** Flash (IAP) Source address is not mapped in the memory map. Count value is taken in to consideration where applicable */
    IAP_SRC_ADDR_NOT_MAPPED(4, "Flash (IAP) Source address is not mapped in the memory map. Count value is taken in to consideration where applicable.", true),
    /** Flash (IAP) Destination address is not mapped in the memory map. Count value is taken in to consideration where applicable. */
    IAP_DST_ADDR_NOT_MAPPED(5, "Flash (IAP) Destination address is not mapped in the memory map. Count value is taken in to consideration where applicable.", true),
    /** Flash (IAP) Byte count is not multiple of 4 or 256 | 512 | 1024 | 4096 */
    IAP_COUNT_ERROR(6, "Flash (IAP) Byte count is not multiple of 4 or 256 | 512 | 1024 | 4096", true),
    /** Flash (IAP) Sector number is invalid */
    IAP_INVALID_SECTOR(7, "Flash (IAP) Sector number is invalid.", true),
    /** Flash (IAP) Sector is not blank */
    IAP_SECTOR_NOT_BLANK(8, "Flash (IAP) Sector is not blank.", true),
    /** Flash (IAP) Command to prepare sector for write operation was not executed */
    IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION(9, "Flash (IAP) Command to prepare sector for write operation was not executed.", true),
    /** Flash (IAP) Source and destination data is not same */
    IAP_COMPARE_ERROR(10, "Flash (IAP) Source and destination data is not same.", true),
    /** Flash (IAP) Flash programming hardware interface is busy */
    IAP_BUSY(11, "Flash (IAP) Flash programming hardware interface is busy.", true),

    // following are in hexadecimal

    /** Command is not defined */
    UNKNOWN_COMMAND(0x100, "Command unknown", true),
    /** CRC calculated on the device and by the updater don't match */
    CRC_ERROR(0x101, "CRC error, try option -full for a clean and full flash", true),
    /** Specified address cannot be programmed */
    ADDRESS_NOT_ALLOWED_TO_FLASH(0x102, "Address not allowed to flash", true),
    /** The specified sector cannot be erased */
    SECTOR_NOT_ALLOWED_TO_ERASE(0x103, "Sector not allowed being erased", true),
    /** Internal buffer for storing the data would overflow */
    RAM_BUFFER_OVERFLOW(0x104, "RAM Buffer Overflow", true),
    /** The boot descriptor block does not exist */
    WRONG_DESCRIPTOR_BLOCK(0x105, "Boot Descriptor block wrong", true),
    /** The programmed application is not startable */
    APPLICATION_NOT_STARTABLE(0x106, "Application not startable", true),
    /** The device is locked */
    DEVICE_LOCKED(0x107, "Device locked. Programming mode on device must be active!", true),
    /** UID sent to unlock the device is invalid */
    UID_MISMATCH(0x108, "UID mismatch", true),
    /** Flash page erase failed */
    ERASE_FAILED(0x109, "Flash page erase failed", true),

    // counting in hex so here is space for 0x10A-0x10F

    /** Flash page could not be programmed */
    FLASH_ERROR(0x110, "Flash page could not be programmed", true),
    /** Flash page not allowed to erase */
    PAGE_NOT_ALLOWED_TO_ERASE(0x111, "Flash page not allowed to erase", true),
    /** Command not implemented */
    NOT_IMPLEMENTED(0xFFFF, "Command not implemented", true),
    /** Unknown error */
    INVALID(0xFFFFFFFF, "Unknown error", true);

    private static final Map<Long, UDPResult> BY_INDEX = new HashMap<>();
    static {
        for (UDPResult e: values()) {
            BY_INDEX.put(e.id, e);
        }
    }

    /** ID of the UDPResult */
    public final long id;
    /** Simple description of the UDPResult */
    private final String description;
    /** True if the UPDResult represents an error */
    private final boolean isError;

    /**
     * Create a UDPResult instance from given id, description and error state
     * @param id ID of the UDPResult
     * @param description Description of the UDPResult
     * @param isError Set to true, if the UPDResult represents a error, otherwise set to false
     */
    UDPResult(Integer id, String description, boolean isError) {
        this.id = id;
        this.description = description;
        this.isError = isError;
    }

    /**
     * Create a UPDResult instance from a given ID
     * @param index ID of the UPDResult
     * @return Instance of the UPDResult from the given ID
     */
    public static UDPResult valueOfIndex(long index) {
        UDPResult res = BY_INDEX.get(index);
        return Objects.requireNonNullElse(res, INVALID);
    }

    @Override
    public String toString() {
        return this.description;
    }

    /**
     * Returns whether the UPDResult represents an error or not
     * @return True if the UPDResult represents an error, otherwise false
     */
    public boolean isError() {
        return this.isError;
    }

    /**
     * Create a UPDResult instance from a byte array
     * @param toParse Byte array to parse and create the UPDResult from
     * @param offset Position in byte array to start parsing
     * @return Instance of UPDResult created from the given byte array
     */
    public static UDPResult fromByteArray(byte[] toParse, int offset)
    {
        long resultCode = Utils.streamToLong(toParse, offset);
        return UDPResult.valueOfIndex(resultCode);
    }
}

