package org.selfbus.updater.bootloader;

public class BootloaderStatistic {
    private final int disConnectCount;
    private final int repeatedIgnoredTelegramCount;


    public BootloaderStatistic(int disConnectCount, int repeatedIgnoredTelegramCount) {
        this.disConnectCount = disConnectCount;
        this.repeatedIgnoredTelegramCount = repeatedIgnoredTelegramCount;
    }

    public static BootloaderStatistic fromArray(byte[] parse) {
        int disConnectCount = (parse[0] & 0xFF) + ((parse[1] & 0xFF) << 8);
        int repeatedIgnoredTelegramCount = (parse[2] & 0xFF) + ((parse[3] & 0xFF) << 8);;
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
