package org.selfbus.updater.bootloader;

public final class BootloaderUpdater {
    /** Bootloader Updater appVersion identity string from app_main.cpp of bootloaderupdater */
    @SuppressWarnings("SpellCheckingInspection")
    public static final String BOOTLOADER_UPDATER_ID_STRING = "SBblu";

    /** Maximum restart and processing time in milliseconds to
     * erase and program the new Bootloader in addressrange 0x0-0x2fff (3 sectors, 48 pages).
     * According to NXP UM10398 Erase time for one sector is 100 ms ± 5%.
     * Programming time for one block of 256 bytes is 1 ms ± 5%. (p.417).
     * In total ~365.4 milliseconds. Set to 1000ms. Better safe than sorry.
     * */
    public static final long BOOTLOADER_UPDATER_MAX_RESTART_TIME_MS = 1000;
}
