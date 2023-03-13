package org.selfbus.updater.bootloader;

import org.selfbus.updater.Utils;

public class BootloaderStatistic {
    private final int disConnectCount;
    private final int repeatedIgnoredTelegramCount;


    public BootloaderStatistic(int disConnectCount, int repeatedIgnoredTelegramCount) {
        this.disConnectCount = disConnectCount;
        this.repeatedIgnoredTelegramCount = repeatedIgnoredTelegramCount;
    }

    public static BootloaderStatistic fromArray(byte[] parse) {
        int disConnectCount = Utils.streamToShort(parse, 0);
        int repeatedIgnoredTelegramCount = Utils.streamToShort(parse, 2);
        return new BootloaderStatistic(disConnectCount, repeatedIgnoredTelegramCount);
    }

    public String toString() {
        return String.format("#Disconnect: %d #Repeated ignored: %d",
                              disConnectCount(), repeatedIgnoredTelegramCount());
    }

    public long disConnectCount()
    {
        return disConnectCount;
    }

    public long repeatedIgnoredTelegramCount()
    {
        return repeatedIgnoredTelegramCount;
    }
}
