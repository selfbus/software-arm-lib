package org.selfbus.updater.bootloader;

import org.selfbus.updater.Utils;

public class BootloaderStatistic {
    private final int disconnectCount;
    private final int reserved;


    public BootloaderStatistic(int disconnectCount, int reserved) {
        this.disconnectCount = disconnectCount;
        this.reserved = reserved;
    }

    public static BootloaderStatistic fromArray(byte[] parse) {
        int disConnectCount = Utils.streamToShort(parse, 0);
        int reserved = Utils.streamToShort(parse, 2);
        return new BootloaderStatistic(disConnectCount, reserved);
    }

    public String toString() {
        return String.format("#Disconnect: %d",
                              getDisconnectCount());
    }

    public long getDisconnectCount()
    {
        return disconnectCount;
    }

    public long getReserved()
    {
        return reserved;
    }
}
