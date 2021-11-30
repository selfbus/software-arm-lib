package org.hkfree.knxduino.updater;

/**
 *  Holds Bootloader identity information
 *  see software-arm-lib/Bus-Updater/src/update.cpp (method updRequestBootloaderIdentity) for more information.
 */
public class BootLoaderIdentity {
    private long identity;
    private long features;
    private long size;

    public BootLoaderIdentity(byte[] parse) {
        this.identity = (parse[0] & 0xFF) +
                        ((parse[1] & 0xFF) << 8) +
                        ((parse[2] & 0xFF) << 16) +
                        ((long)(parse[3] & 0xFF) << 24);
        this.features = (parse[4] & 0xFF) +
                        ((parse[5] & 0xFF) << 8) +
                        ((parse[6] & 0xFF) << 16) +
                        ((long)(parse[7] & 0xFF) << 24);
        this.size = (parse[8] & 0xFF) +
                    ((parse[9] & 0xFF) << 8) +
                    ((parse[10] & 0xFF) << 16) +
                    ((long)(parse[11] & 0xFF) << 24);
    }

    public String toString() {
        return String.format("Identity: 0x%04X, Features: 0x%04X, Size: 0x%04X",
                             identity, features, size);
    }

    public long identity()
    {
        return this.identity;
    }

    public long features()
    {
        return this.identity;
    }

    public long size()
    {
        return this.size;
    }
}
