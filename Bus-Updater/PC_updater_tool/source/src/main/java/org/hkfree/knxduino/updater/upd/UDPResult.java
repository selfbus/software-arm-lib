package org.hkfree.knxduino.updater.upd;

import org.hkfree.knxduino.updater.Utils;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * Implementation of the UPD/UDP protocol result commands
 */
public enum UDPResult {
    // IAP_ are in decimal (IAP ist in application programming of the MCU's flash
    // these results are defined in iap.h Selfbus sblib
    IAP_SUCCESS( 0, "OK", false),
    IAP_INVALID_COMMAND( 1, "Flash (IAP) invalid command", true),
    IAP_SRC_ADDR_ERROR( 2, "Flash (IAP) source address error", true),
    IAP_DST_ADDR_ERROR( 3, "Flash (IAP) destination address error", true),
    IAP_SRC_ADDR_NOT_MAPPED( 4, "Flash (IAP) source address not mapped", true),
    IAP_DST_ADDR_NOT_MAPPED( 5, "Flash (IAP) destination address not mapped", true),
    IAP_COUNT_ERROR( 6, "Flash (IAP) count error", true),
    IAP_INVALID_SECTOR( 7, "Flash (IAP) invalid sector error", true),
    IAP_SECTOR_NOT_BLANK( 8, "Flash (IAP) sector not blank", true),
    IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION( 9, "Flash (IAP) sector not prepared for write operation", true),
    IAP_COMPARE_ERROR( 10, "Flash (IAP) compare error", true),
    IAP_BUSY( 11, "Flash (IAP) busy", true),

    // following are in hexadecimal
    UNKNOWN_COMMAND(0x100, "Command unknown", true), //!< received command is not defined
    CRC_ERROR(0x101, "CRC error, try option -full for a clean and full flash", true), //!< CRC calculated on the device and by the updater don't match
    ADDRESS_NOT_ALLOWED_TO_FLASH(0x102, "Address not allowed to flash", true), //!< specified address cannot be programmed
    SECTOR_NOT_ALLOWED_TO_ERASE(0x103, "Sector not allowed being erased", true), //!< the specified sector cannot be erased
    RAM_BUFFER_OVERFLOW(0x104, "RAM Buffer Overflow", true), //!< internal buffer for storing the data would overflow
    WRONG_DESCRIPTOR_BLOCK(0x105, "Boot Descriptor block wrong", true), //!< the boot descriptor block does not exist
    APPLICATION_NOT_STARTABLE(0x106, "Application not startable", true), //!< the programmed application is not startable
    DEVICE_LOCKED(0x107, "Device locked. Programming mode on device must be active!", true), //!< the device is still locked
    UID_MISMATCH(0x108, "UID mismatch", true), //!< UID sent to unlock the device is invalid
    ERASE_FAILED(0x109, "Flash page erase failed", true), //!< page erase failed
    // counting in hex so here is space for 0x10A-0x10F
    FLASH_ERROR(0x110, "Flash page could not be programmed", true), //!< page program (flash) failed
    PAGE_NOT_ALLOWED_TO_ERASE(0x111, "Flash page not allowed to erase", true), //!< page not allowed to erase
    NOT_IMPLEMENTED(0xFFFF, "Command not implemented", true), //!< this command is not yet implemented
    INVALID(0xFFFFFFFF, "Unknown error", true);  //!< not a UPDResult

    private static final Map<Long, UDPResult> BY_INDEX = new HashMap<>();
    static {
        for (UDPResult e: values()) {
            BY_INDEX.put(e.id, e);
        }
    }

    public final long id;
    private final String description;
    private final boolean isError;

    UDPResult(Integer id, String description, boolean isError) {
        this.id = id;
        this.description = description;
        this.isError = isError;
    }

    public static UDPResult valueOfIndex(long index) {
        UDPResult res = BY_INDEX.get(index);
        return Objects.requireNonNullElse(res, INVALID);
    }

    @Override
    public String toString() {
        return this.description;
    }

    public boolean isError() {
        return this.isError;
    }


    public static UDPResult fromByteArray(byte[] toParse, int offset)
    {
        long resultCode = Utils.streamToLong(toParse, offset);
        return UDPResult.valueOfIndex(resultCode);
    }
}

