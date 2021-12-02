package org.hkfree.knxduino.updater;

/**
 * Basic information about the bootloader's (MCU's) flash parameters
 */
public final class Flash {
    public static final int MAX_PAYLOAD = 11;         //!< maximum payload one APCI_MEMORY_READ_PDU/APCI_MEMORY_WRITE_PDU can handle
    public static final int FLASH_SECTOR_SIZE = 4096; //!< Selfbus ARM controller flash sector size
    public static final int FLASH_PAGE_SIZE = 256;    //!< Selfbus ARM controller flash page size

}
