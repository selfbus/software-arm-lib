package org.selfbus.updater;

/**
 * Basic information about the bootloader's (MCU's) flash parameters
 */
public final class Flash {
    public static final int MAX_PAYLOAD = 11;         //!< maximum payload one APCI_MEMORY_READ_PDU/APCI_MEMORY_WRITE_PDU can handle
    ///\todo get rid of this, the MCU should handle this by it self or report us the size
    public static final int FLASH_PAGE_SIZE = 256;    //!< Selfbus ARM controller flash page size
}
