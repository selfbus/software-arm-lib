package org.selfbus.updater.bootloader;

import org.selfbus.updater.Utils;

/**
 * Holds Bootloader identity information
 * <p>
 * see /firmware_updater/bootloader/src/update.cpp (method updRequestBootloaderIdentity) for more information.
 */
public record BootloaderIdentity(long versionMajor,
                                 long versionMinor,
                                 long versionSBLibMajor,
                                 long versionSBLibMinor,
                                 long features,
                                 long applicationFirstAddress) {
    private String hexVersionToString(long versionMajor, long versionMinor) {
        byte high = (byte) (versionMinor >> 4);
        byte low = (byte) (versionMinor & 0x0f);

        String highPart;
        String lowPart;

        if (high < 10) {
            highPart = String.format("%d", high);
        }
        else {
            highPart = "x";
        }

        if (low < 10) {
            lowPart = String.format("%d", low);
        }
        else {
            lowPart = "y";
        }

        return String.format("%d.%s%s", versionMajor, highPart, lowPart);
    }

    public static BootloaderIdentity fromArray(byte[] parse) {
        long vMajor = parse[0] & 0xff;
        long vMinor = parse[1] & 0xff;
        long features = Utils.streamToShort(parse, 2);
        long versionSBLibMajor = parse[4] & 0xff;
        long versionSBLibMinor = parse[5] & 0xff;
        long applicationFirstAddress = Utils.streamToLong(parse, 6);
        return new BootloaderIdentity(vMajor, vMinor, versionSBLibMajor, versionSBLibMinor, features, applicationFirstAddress);
    }

    public String toString() {
        return String.format("Version: %s, sbLib Version: %s, Features: 0x%04X, App-start: 0x%04X",
                getVersion(), getVersionSBLib(), features(), applicationFirstAddress());
    }

    public String getVersion() {
        return String.format("%d.%02d", versionMajor(), versionMinor());
    }

    public String getVersionSBLib() {
        return hexVersionToString(versionSBLibMajor(), versionSBLibMinor());
    }
}
