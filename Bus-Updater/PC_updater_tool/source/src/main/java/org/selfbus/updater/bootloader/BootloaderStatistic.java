package org.selfbus.updater.bootloader;

public class BootloaderStatistic {
    private final int telegramCount;
    private final int disConnectCount;
    private final int hotfix_1_RepeatedControlTelegramCount;
    private final int hotfix_2_RepeatedDataTelegramCount;
    private final int repeatedTelegramTotalCount;


    public BootloaderStatistic(int telegramCount, int disConnectCount, int hotfix_1_RepeatedControlTelegramCount, int hotfix_2_RepeatedDataTelegramCount, int repeatedTelegramTotalCount) {
        this.telegramCount = telegramCount;
        this.disConnectCount = disConnectCount;
        this.hotfix_1_RepeatedControlTelegramCount = hotfix_1_RepeatedControlTelegramCount;
        this.hotfix_2_RepeatedDataTelegramCount = hotfix_2_RepeatedDataTelegramCount;
        this.repeatedTelegramTotalCount = repeatedTelegramTotalCount;
    }

    public static BootloaderStatistic fromArray(byte[] parse) {
        int telegramCount = (parse[0] & 0xFF) + ((parse[1] & 0xFF) << 8);
        int disConnectCount = (parse[2] & 0xFF) + ((parse[3] & 0xFF) << 8);
        int hotfix_1 = (parse[4] & 0xFF) + ((parse[5] & 0xFF) << 8);
        int hotfix_2 = (parse[6] & 0xFF) + ((parse[7] & 0xFF) << 8);;
        int repeatedTelegramTotalCount = (parse[8] & 0xFF) + ((parse[9] & 0xFF) << 8);;
        return new BootloaderStatistic(telegramCount, disConnectCount, hotfix_1, hotfix_2, repeatedTelegramTotalCount);
    }

    public String toString() {
        long repeatedMissed = repeatedTelegramTotalCount() - hotfix_1_RepeatedControlTelegramCount() - hotfix_2_RepeatedDataTelegramCount();
        return String.format("#Telegram: %d #Disconnect: %d Hotfix #RepeatedControlTelegram: %d #RepeatedDataTelegram: %d #RepeatedTotal: %d #Diff: %d",
                telegramCount(), disConnectCount(), hotfix_1_RepeatedControlTelegramCount(), hotfix_2_RepeatedDataTelegramCount(), repeatedTelegramTotalCount(), repeatedMissed);
    }

    public long telegramCount()
    {
        return telegramCount;
    }

    public long disConnectCount()
    {
        return disConnectCount;
    }

    public long hotfix_1_RepeatedControlTelegramCount()
    {
        return hotfix_1_RepeatedControlTelegramCount;
    }

    public long hotfix_2_RepeatedDataTelegramCount()
    {
        return hotfix_2_RepeatedDataTelegramCount;
    }

    public long repeatedTelegramTotalCount()
    {
        return repeatedTelegramTotalCount;
    }
}
