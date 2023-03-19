package org.selfbus.updater.bootloader;

import org.selfbus.updater.Utils;

/**
 *  Holds Bootloader identity information
 *  see software-arm-lib/Bus-Updater/src/update.cpp (method updRequestBootloaderIdentity) for more information.
 */
public class BootloaderIdentity {
    private final long versionMajor;
    private final long versionMinor;
    private final long features;
    private final long applicationFirstAddress;

    public BootloaderIdentity(long versionMajor, long versionMinor, long features, long applicationFirstAddress) {
        this.features = features;
        this.applicationFirstAddress = applicationFirstAddress;
        this.versionMajor = versionMajor;
        this.versionMinor = versionMinor;
    }

    public static BootloaderIdentity fromArray(byte[] parse) {
        long vMajor = parse[0] & 0xff;
        long vMinor = parse[1] & 0xff;
        long features = Utils.streamToLong(parse, 2);
        long applicationFirstAddress = Utils.streamToLong(parse, 6);
        return new BootloaderIdentity(vMajor, vMinor, features, applicationFirstAddress );
    }

    public String toString() {
        return String.format("Version: %s, Features: 0x%04X, App-start: 0x%04X",
                              version(), features(), applicationFirstAddress());
    }

    public long features()
    {
        return features;
    }

    public long applicationFirstAddress()
    {
        return applicationFirstAddress;
    }

    public long versionMajor()
    {
        return versionMajor;
    }

    public long versionMinor()
    {
        return versionMinor;
    }

    public String version() {
        return String.format("%d.%2d", versionMajor(), versionMinor()).replace(' ', '0');
    }
}
