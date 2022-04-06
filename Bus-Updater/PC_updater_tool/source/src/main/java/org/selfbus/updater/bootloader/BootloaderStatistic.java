package org.selfbus.updater.bootloader;

public class BootloaderStatistic {
    private final int dummy2;
    private final int disConnectCount;
    private final int repeatedTelegramCount;
    private final int repeatedIgnoredTelegramCount;
    private final int dummy;


    public BootloaderStatistic(int dummy2, int disConnectCount, int repeatedTelegramCount, int repeatedIgnoredTelegramCount, int dummy) {
        this.dummy2 = dummy2;
        this.disConnectCount = disConnectCount;
        this.repeatedTelegramCount = repeatedTelegramCount;
        this.repeatedIgnoredTelegramCount = repeatedIgnoredTelegramCount;
        this.dummy = dummy;
    }

    public static BootloaderStatistic fromArray(byte[] parse) {
        int dummy2 = (parse[0] & 0xFF) + ((parse[1] & 0xFF) << 8);
        int disConnectCount = (parse[2] & 0xFF) + ((parse[3] & 0xFF) << 8);
        int repeatedTelegramCount = (parse[4] & 0xFF) + ((parse[5] & 0xFF) << 8);
        int repeatedIgnoredTelegramCount = (parse[6] & 0xFF) + ((parse[7] & 0xFF) << 8);;
        int dummy = (parse[8] & 0xFF) + ((parse[9] & 0xFF) << 8);;
        return new BootloaderStatistic(dummy2, disConnectCount, repeatedTelegramCount, repeatedIgnoredTelegramCount, dummy);
    }

    public String toString() {
        long diff = repeatedTelegramCount() - repeatedIgnoredTelegramCount();
        return String.format("#Disconnect: %d #Repeated: %d #Ignored: %d #Diff: %d",
                              disConnectCount(), repeatedTelegramCount(), repeatedIgnoredTelegramCount(), diff);
    }

    public long dummy2()
    {
        return dummy2;
    }

    public long disConnectCount()
    {
        return disConnectCount;
    }

    public long repeatedTelegramCount()
    {
        return repeatedTelegramCount;
    }

    public long repeatedIgnoredTelegramCount()
    {
        return repeatedIgnoredTelegramCount;
    }

    public long dummy()
    {
        return dummy;
    }
}
