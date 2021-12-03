package org.hkfree.knxduino.updater.upd;

import java.util.HashMap;
import java.util.Map;

/**
 * Implementation of the UPD/UDP protocol control commands
 */
public enum UPDCommand {
    ERASE_SECTOR(0),
    SEND_DATA(1),
    PROGRAM(2),
    UPDATE_BOOT_DESC(3),
    SEND_DATA_TO_DECOMPRESS(4),
    PROGRAM_DECOMPRESSED_DATA(5),
    ERASE_ADDRESS_RANGE(8),
    REQ_DATA(10),
    GET_LAST_ERROR(20),
    SEND_LAST_ERROR(21),
    UNLOCK_DEVICE(30),
    REQUEST_UID(31),
    RESPONSE_UID(32),
    APP_VERSION_REQUEST(33),
    APP_VERSION_RESPONSE(34),
    RESET(35),
    REQUEST_BOOT_DESC(36),
    RESPONSE_BOOT_DESC(37),
    REQUEST_BL_IDENTITY(40),
    RESPONSE_BL_IDENTITY(41),
    SET_EMULATION(100);

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
