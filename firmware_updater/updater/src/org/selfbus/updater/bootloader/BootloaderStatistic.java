package org.selfbus.updater.bootloader;

import org.selfbus.updater.Utils;

import static org.fusesource.jansi.Ansi.*;
import static org.fusesource.jansi.Ansi.Color.*;

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
            colored = ansi().fgBright(YELLOW).toString();
        } else {
            colored = ansi().fgBright(GREEN).toString();
        }
        result = String.format("#Disconnect: %s%2d%s", colored, getDisconnectCount(), ansi().reset());
        if (getRepeatedT_ACKcount() > BootloaderStatistic.THRESHOLD_REPEATED) {
            colored = ansi().fgBright(YELLOW).toString();
        } else {
            colored = ansi().fgBright(GREEN).toString();
        }
        result += String.format(" #repeated T_ACK: %s%2d%s", colored, getRepeatedT_ACKcount(), ansi().reset());
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
