package org.selfbus.updater;

/**
 * Basic information about the bootloader's MCU
 */
public final class Mcu {
    /** Maximum payload one APCI_MEMORY_READ_PDU/APCI_MEMORY_WRITE_PDU can handle */
    public static final int MAX_PAYLOAD = 11;
    ///\todo get rid of this, the MCU should handle this by it self or report us the size
    /** Selfbus ARM controller flash page size */
    public static final int FLASH_PAGE_SIZE = 256;
    /** Vector table end of the mcu */
    public static final int VECTOR_TABLE_END = 0xD0;
    /** Length of bootloader identity string */
    public static final int BL_ID_STRING_LENGTH = 12;
    /** App Pointer follows this MAGIC word */
    public static final byte[] APP_VER_PTR_MAGIC = {'!','A','V','P','!','@',':'};

    /** Default restart time of the MCU in seconds */
    public static final int DEFAULT_RESTART_TIME_SECONDS = 6;
    /** KNX transport layer 4 connection timeout */
    public static final int TL4_CONNECTION_TIMEOUT_MS = 6300;
}
