package org.selfbus.updater.bootloader;

import org.selfbus.updater.Utils;

public class BootloaderStatistic {
    private final int disconnectCount;
    private final int repeatedIgnoredTelegramCount;


    public BootloaderStatistic(int disconnectCount, int repeatedIgnoredTelegramCount) {
        this.disconnectCount = disconnectCount;
        this.repeatedIgnoredTelegramCount = repeatedIgnoredTelegramCount;
    }

    public static BootloaderStatistic fromArray(byte[] parse) {
        int disConnectCount = Utils.streamToShort(parse, 0);
        int repeatedIgnoredTelegramCount = Utils.streamToShort(parse, 2);
        return new BootloaderStatistic(disConnectCount, repeatedIgnoredTelegramCount);
    }

    public String toString() {
        return String.format("#Disconnect: %d #Repeated ignored: %d",
                              getDisconnectCount(), getRepeatedIgnoredTelegramCount());
    }

    public long getDisconnectCount()
    {
        return disconnectCount;
    }

    public long getRepeatedIgnoredTelegramCount()
    {
        return repeatedIgnoredTelegramCount;
    }
}
