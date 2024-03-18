package org.selfbus.updater;

import java.time.Duration;

/**
 * Basic information about the bootloader's MCU
 */
public final class Mcu {
    /** Maximum length a asdu can be in a standard frame */
    public static final int MAX_ASDU_LENGTH = 14;
    /** Maximum data payload one APCI_USERMSG_MANUFACTURER_0/APCI_USERMSG_MANUFACTURER_6 can handle */
    public static final int MAX_PAYLOAD = MAX_ASDU_LENGTH - 1;
    /** Selfbus ARM controller flash page size */
    public static final int FLASH_PAGE_SIZE = 256;
    /** Size in bytes to flash with UPD_PROGRAM */
    public static final int UPD_PROGRAM_SIZE = 1024;
    /** Vector table end of the mcu */
    public static final int VECTOR_TABLE_END = 0xC0;
    /** Length of bootloader identity string */
    public static final int BL_ID_STRING_LENGTH = 12;
    /** App Pointer follows this MAGIC word */
    public static final byte[] APP_VER_PTR_MAGIC = {'!','A','V','P','!','@',':'};

    /** Default restart time of the MCU in seconds */
    public static final int DEFAULT_RESTART_TIME_SECONDS = 6;

    /**
     * Maximum execution time for an UPDCommand ERASE_ADDRESS_RANGE.
     * In the worst case the addressrange 0x0100-0xfeff will be erased,
     * resulting in 15 sector and 30 page erases taking ~4725ms
     * */
    public static final Duration MAX_FLASH_ERASE_TIMEOUT = Duration.ofSeconds(5);
}
