package org.hkfree.knxduino.updater;

import java.util.HashMap;
import java.util.Map;

public enum UDPResult {
    OK(0x000),
    UNKNOWN_COMMAND(0x100),             //!< received command is not defined
    CRC_ERROR(0x101),                   //!< CRC calculated on the device and by the updater don't match
    ADDRESS_NOT_ALLOWED_TO_FLASH(0x102),//!< specified address cannot be programmed
    SECTOR_NOT_ALLOWED_TO_ERASE(0x103), //!< the specified sector cannot be erased
    RAM_BUFFER_OVERFLOW(0x104),         //!< internal buffer for storing the data would overflow
    WRONG_DESCRIPTOR_BLOCK(0x105),      //!< the boot descriptor block does not exist
    APPLICATION_NOT_STARTABLE(0x106),   //!< the programmed application is not startable
    DEVICE_LOCKED(0x107),               //!< the device is still locked
    UID_MISMATCH(0x108),                //!< UID sent to unlock the device is invalid
    ERASE_FAILED(0x109),                //!< page erase failed
    // counting in hex so here is space for 0x10A-0x10F
    FLASH_ERROR(0x110),                 //!< page program (flash) failed
    PAGE_NOT_ALLOWED_TO_ERASE(0x111),   //!< page not allowed to erase
    NOT_IMPLEMENTED(0xFFFF);            //!< this command is not yet implemented

    private static final Map<Integer, UDPResult> BY_INDEX = new HashMap<>();
    static {
        for (UDPResult e: values()) {
            BY_INDEX.put(e.id, e);
        }
    }

    public final Integer id;

    private UDPResult(Integer id) {
        this.id = id;
    }

    public static UDPResult valueOfIndex(Integer index) {
        return BY_INDEX.get(index);
    }
}

