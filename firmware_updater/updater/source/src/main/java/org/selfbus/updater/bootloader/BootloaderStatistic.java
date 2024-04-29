package org.selfbus.updater.bootloader;

import org.selfbus.updater.ConColors;
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
        String result;
        String colored;
        if (getDisconnectCount() > BootloaderStatistic.THRESHOLD_DISCONNECT) {
            colored = ConColors.BRIGHT_YELLOW;
        } else {
            colored = ConColors.BRIGHT_GREEN;
        }
        result = String.format("#Disconnect: %s%2d%s", colored, getDisconnectCount(), ConColors.RESET);
        if (getRepeatedT_ACKcount() > BootloaderStatistic.THRESHOLD_REPEATED) {
            colored = ConColors.BRIGHT_YELLOW;
        } else {
            colored = ConColors.BRIGHT_GREEN;
        }
        result += String.format(" #repeated T_ACK: %s%2d%s", colored, getRepeatedT_ACKcount(), ConColors.RESET);
        return result;
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
