package org.selfbus.updater.tests.flashdiff;

import org.selfbus.updater.Flash;

import java.util.Arrays;

public class FlashPage {
    public static final int PAGE_SIZE = Flash.FLASH_PAGE_SIZE;
    private byte[] old; // old content before patching

    public FlashPage(byte[] fromByteArray, int begin) {
        old = Arrays.copyOfRange(fromByteArray, begin, begin + PAGE_SIZE);
    }

    public byte[] getOldBinData() {
        return old;
    }
}
