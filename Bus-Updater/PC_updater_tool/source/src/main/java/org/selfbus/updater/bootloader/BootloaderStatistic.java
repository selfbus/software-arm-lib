package org.selfbus.updater.bootloader;

import org.selfbus.updater.Utils;

public class BootloaderStatistic {
    private final int disconnectCount;
    private final int ignoredNdataIndividual;


    public BootloaderStatistic(int disconnectCount, int ignoredNdataIndividual) {
        this.disconnectCount = disconnectCount;
        this.ignoredNdataIndividual = ignoredNdataIndividual;
    }

    public static BootloaderStatistic fromArray(byte[] parse) {
        int disConnectCount = Utils.streamToShort(parse, 0);
        int ignoredNdataIndividual = Utils.streamToShort(parse, 2);
        return new BootloaderStatistic(disConnectCount, ignoredNdataIndividual);
    }

    public String toString() {
        return String.format("#Disconnect: %d #Ignored N_DATA_INDIVIDUAL: %d",
                              getDisconnectCount(), getIgnoredNdataIndividual());
    }

    public long getDisconnectCount()
    {
        return disconnectCount;
    }

    public long getIgnoredNdataIndividual()
    {
        return ignoredNdataIndividual;
    }
}
