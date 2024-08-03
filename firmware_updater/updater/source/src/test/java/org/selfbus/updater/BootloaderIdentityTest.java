package org.selfbus.updater;

import org.junit.jupiter.api.Test;
import org.selfbus.updater.bootloader.BootloaderIdentity;

import static org.junit.jupiter.api.Assertions.*;;

public class BootloaderIdentityTest {

    @Test
    public void testGetVersionSBLib() {
        BootloaderIdentity blIdentity = new BootloaderIdentity(0, 0,
                0, 0x00, 0x0000, 0x9000);
        assertEquals("0.00", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0x09, 0x0000, 0x9000);
        assertEquals("0.09", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0x10, 0x0000, 0x9000);
        assertEquals("0.10", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0x11, 0x0000, 0x9000);
        assertEquals("0.11", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0x20, 0x0000, 0x9000);
        assertEquals("0.20", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0x21, 0x0000, 0x9000);
        assertEquals("0.21", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0x90, 0x0000, 0x9000);
        assertEquals("0.90", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0x99, 0x0000, 0x9000);
        assertEquals("0.99", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                1, 0x20, 0x0000, 0x9000);
        assertEquals("1.20", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                2, 0x57, 0x0000, 0x9000);
        assertEquals("2.57", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                254, 0x99, 0x0000, 0x9000);
        assertEquals("254.99", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0x0a, 0x0000, 0x9000);
        assertEquals("0.0y", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0xa0, 0x0000, 0x9000);
        assertEquals("0.x0", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0xf0, 0x0000, 0x9000);
        assertEquals("0.x0", blIdentity.getVersionSBLib());

        blIdentity = new BootloaderIdentity(0, 0,
                0, 0xff, 0x0000, 0x9000);
        assertEquals("0.xy", blIdentity.getVersionSBLib());
    }
}
