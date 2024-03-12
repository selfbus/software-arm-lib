package org.selfbus.updater.upd;

import java.util.HashMap;
import java.util.Map;

/**
 * Implementation of the UPD/UDP protocol result commands
 * see /Bus-Updater/inc/upd_protocol.h for details
 */
public enum UDPResult {
    /** Flash (IAP) Command is executed successfully */
    IAP_SUCCESS((byte)0x7f, "Flash (IAP) Command is executed successfully.", false),
    /** Flash (IAP) Invalid command */
    IAP_INVALID_COMMAND((byte)0x7e, "Flash (IAP) Invalid command.", true),
    /** Flash (IAP) Source address is not on a word boundary */
    IAP_SRC_ADDR_ERROR((byte)0x7d, "Flash (IAP) Source address is not on a word boundary.", true),
    /** Flash (IAP) Destination address is not on a correct boundary */
    IAP_DST_ADDR_ERROR((byte)0x7c, "Flash (IAP) Destination address is not on a correct boundary.", true),
    /** Flash (IAP) Source address is not mapped in the memory map. Count value is taken in to consideration where applicable */
    IAP_SRC_ADDR_NOT_MAPPED((byte)0x7b, "Flash (IAP) Source address is not mapped in the memory map. Count value is taken in to consideration where applicable.", true),
    /** Flash (IAP) Destination address is not mapped in the memory map. Count value is taken in to consideration where applicable. */
    IAP_DST_ADDR_NOT_MAPPED((byte)0x7a, "Flash (IAP) Destination address is not mapped in the memory map. Count value is taken in to consideration where applicable.", true),
    /** Flash (IAP) Byte count is not multiple of 4 or 256 | 512 | 1024 | 4096 */
    IAP_COUNT_ERROR((byte)0x79, "Flash (IAP) Byte count is not multiple of 4 or 256 | 512 | 1024 | 4096", true),
    /** Flash (IAP) Sector number is invalid */
    IAP_INVALID_SECTOR((byte)0x78, "Flash (IAP) Sector number is invalid.", true),
    /** Flash (IAP) Sector is not blank */
    IAP_SECTOR_NOT_BLANK((byte)0x77, "Flash (IAP) Sector is not blank.", true),
    /** Flash (IAP) Command to prepare sector for write operation was not executed */
    IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION((byte)0x76, "Flash (IAP) Command to prepare sector for write operation was not executed.", true),
    /** Flash (IAP) Source and destination data is not same */
    IAP_COMPARE_ERROR((byte)0x75, "Flash (IAP) Source and destination data is not same. Make sure that the affected flash sectors/pages are erased prior flashing.", true),
    /** Flash (IAP) Flash programming hardware interface is busy */
    IAP_BUSY((byte)0x74, "Flash (IAP) Flash programming hardware interface is busy.", true),

    /** Unknown IAP_Status */
    UDP_IAP_UNKNOWN((byte)0x73, "Unknown IAP_Status", true),

    /** Command is not defined */
    UNKNOWN_COMMAND((byte)0x5f, "Command unknown", true),
    /** CRC calculated on the device and by the updater don't match */
    CRC_ERROR((byte)0x5e, "CRC error, try option -full for a clean and full flash", true),
    /** Specified address cannot be programmed */
    ADDRESS_NOT_ALLOWED_TO_FLASH((byte)0x5d, "Address not allowed to flash", true),
    /** The specified sector cannot be erased */
    SECTOR_NOT_ALLOWED_TO_ERASE((byte)0x5c, "Sector not allowed being erased", true),
    /** Internal buffer for storing the data would overflow */
    RAM_BUFFER_OVERFLOW((byte)0x5b, "RAM Buffer Overflow", true),
    /** The boot descriptor block does not exist */
    WRONG_DESCRIPTOR_BLOCK((byte)0x5a, "Boot Descriptor block wrong", true),
    /** The programmed application is not startable */
    APPLICATION_NOT_STARTABLE((byte)0x59, "Application not startable", true),
    /** The device is locked */
    DEVICE_LOCKED((byte)0x58, "Device locked. Programming mode on device must be active!", true),
    /** UID sent to unlock the device is invalid */
    UID_MISMATCH((byte)0x57, "UID mismatch", true),
    /** Flash page erase failed */
    ERASE_FAILED((byte)0x56, "Flash page erase failed", true),

    /** Data received is invalid */
    INVALID_DATA((byte)0x55, "Data received is invalid", true),
    /** No data received in telegram */
    UDP_NO_DATA((byte)0x54, "No data received in telegram", true),

    /** Flash page could not be programmed */
    FLASH_ERROR((byte)0x53, "Flash page could not be programmed", true),
    /** Flash page not allowed to erase */
    PAGE_NOT_ALLOWED_TO_ERASE((byte)0x52, "Flash page not allowed to erase", true),

    /** Address range not allowed to erase */
    UDP_ADDRESS_RANGE_NOT_ALLOWED_TO_ERASE((byte)0x51, "Address range not allowed to erase", true),

    /** Command not implemented */
    NOT_IMPLEMENTED((byte)0x02, "Command not implemented", true),
    /** Unknown error */
    INVALID((byte)0x01, "Unknown error", true);

    private static final Map<Byte, UDPResult> BY_INDEX = new HashMap<>();
    static {
        for (UDPResult e: values()) {
            BY_INDEX.put(e.id, e);
        }
    }

    /** ID of the UDPResult */
    public final byte id;
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
    UDPResult(byte id, String description, boolean isError) {
        this.id = id;
        this.description = description;
        this.isError = isError;
    }

    /**
     * Create a UPDResult instance from a given ID
     * @param index ID of the UPDResult
     * @return Instance of the UPDResult from the given ID
     */
    public static UDPResult valueOfIndex(byte index) {
        for (UDPResult e: values()) {
             if (e.id == index) {
                 return e;
             }
        }
        return INVALID;
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

}

