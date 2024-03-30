package org.selfbus.updater.bootloader;

import org.selfbus.updater.Utils;

public class BootloaderStatistic {
    public static final int THRESHOLD_DISCONNECT = 1;
    public static final int THRESHOLD_REPEATED = 1;
    private final int disconnectCount;
    private final int repeatedT_ACKcount;


    public BootloaderStatistic(int disconnectCount, int repeatedT_ACKcount) {
        this.disconnectCount = disconnectCount;
        this.repeatedT_ACKcount = repeatedT_ACKcount;
    }

    public static BootloaderStatistic fromArray(byte[] parse) {
        int disConnectCount = Utils.streamToShort(parse, 0);
        int repeatedT_ACKcount = Utils.streamToShort(parse, 2);
        return new BootloaderStatistic(disConnectCount, repeatedT_ACKcount);
    }

    public String toString() {
        return String.format("#Disconnect: %d #repeated T_ACK: %d",
                              getDisconnectCount(), getRepeatedT_ACKcount());
    }

    public long getDisconnectCount()
    {
        return disconnectCount;
    }

    public long getRepeatedT_ACKcount()
    {
        return repeatedT_ACKcount;
    }
}
