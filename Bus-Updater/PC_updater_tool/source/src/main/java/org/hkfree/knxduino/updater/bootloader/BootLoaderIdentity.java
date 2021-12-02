package org.hkfree.knxduino.updater.bootloader;

/**
 *  Holds Bootloader identity information
 *  see software-arm-lib/Bus-Updater/src/update.cpp (method updRequestBootloaderIdentity) for more information.
 */
public class BootLoaderIdentity {
    private final long versionMajor;
    private final long versionMinor;
    private final long identity;
    private final long features;
    private final long applicationFirstAddress;

    public BootLoaderIdentity(long versionMajor, long versionMinor, long identity, long features, long applicationFirstAddress) {
        this.identity = identity;
        this.features = features;
        this.applicationFirstAddress = applicationFirstAddress;
        if ((versionMajor == 0) && (versionMinor == 0)) {
            //\todo this is somehow a waste of number space encoding version this way
            this.versionMajor = (identity >> 16) & 0x0F;
            this.versionMinor = identity & 0xFF;
        }
        else {
            this.versionMajor = versionMajor;
            this.versionMinor = versionMinor;
        }
    }

    public static BootLoaderIdentity fromArray(byte[] parse) {
        long identity = (parse[0] & 0xFF) +
                        ((parse[1] & 0xFF) << 8) +
                        ((parse[2] & 0xFF) << 16) +
                        ((long)(parse[3] & 0xFF) << 24);
        long features = (parse[4] & 0xFF) +
                        ((parse[5] & 0xFF) << 8) +
                        ((parse[6] & 0xFF) << 16) +
                        ((long)(parse[7] & 0xFF) << 24);
        long applicationFirstAddress = (parse[8] & 0xFF) +
                    ((parse[9] & 0xFF) << 8) +
                    ((parse[10] & 0xFF) << 16) +
                    ((long)(parse[11] & 0xFF) << 24);
        return new BootLoaderIdentity(0, 0, identity, features, applicationFirstAddress );
    }

    public String toString() {
        return String.format("Version: %s Identity: 0x%04X, Features: 0x%04X, App-start: 0x%04X",
                              version(), identity(), features(), applicationFirstAddress());
    }

    public long identity()
    {
        return identity;
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
        return String.format("%1X.%2X", versionMajor(), versionMinor());
    }
}
