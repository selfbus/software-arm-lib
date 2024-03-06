package org.selfbus.updater.bootloader;

import org.selfbus.updater.Utils;

/**
 *  Holds Bootloader identity information
 *  see software-arm-lib/Bus-Updater/src/update.cpp (method updRequestBootloaderIdentity) for more information.
 */
public class BootloaderIdentity {
    private final long versionMajor;
    private final long versionMinor;
    private final long versionSBLibMajor;
    private final long versionSBLibMinor;
    private final long features;
    private final long applicationFirstAddress;

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

    public BootloaderIdentity(long versionMajor, long versionMinor, long versionSBLibMajor, long versionSBLibMinor, long features, long applicationFirstAddress) {
        this.versionSBLibMajor = versionSBLibMajor;
        this.versionSBLibMinor = versionSBLibMinor;
        this.features = features;
        this.applicationFirstAddress = applicationFirstAddress;
        this.versionMajor = versionMajor;
        this.versionMinor = versionMinor;
    }

    public static BootloaderIdentity fromArray(byte[] parse) {
        long vMajor = parse[0] & 0xff;
        long vMinor = parse[1] & 0xff;
        long features = Utils.streamToShort(parse, 2);
        long versionSBLibMajor = parse[4] & 0xff;
        long versionSBLibMinor= parse[5] & 0xff;
        long applicationFirstAddress = Utils.streamToLong(parse, 6);
        return new BootloaderIdentity(vMajor, vMinor, versionSBLibMajor, versionSBLibMinor, features, applicationFirstAddress );
    }

    public String toString() {
        return String.format("Version: %s, sbLib Version: %s, Features: 0x%04X, App-start: 0x%04X",
                              getVersion(), getVersionSBLib(), getFeatures(), getApplicationFirstAddress());
    }

    public long getFeatures()
    {
        return features;
    }

    public long getApplicationFirstAddress()
    {
        return applicationFirstAddress;
    }

    public long getVersionMajor()
    {
        return versionMajor;
    }

    public long getVersionMinor()
    {
        return versionMinor;
    }

    public long getVersionSBLibMajor() {
        return versionSBLibMajor;
    }

    public long getVersionSBLibMinor() {
        return versionSBLibMinor;
    }

    public String getVersion() {
        return String.format("%d.%02d", getVersionMajor(), getVersionMinor());
    }

    public String getVersionSBLib() {
        return hexVersionToString(getVersionSBLibMajor(), getVersionSBLibMinor());
    }
}
