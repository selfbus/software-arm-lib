package org.selfbus.updater.bootloader;

import org.selfbus.updater.Utils;

import static org.fusesource.jansi.Ansi.*;
import static org.selfbus.updater.logging.Color.*;

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

    private static String toColoredThreshold(int value, int threshold) {
        String ansiColor;
        if (value > threshold) {
            ansiColor = ansi().fgBright(INFO).toString();
        } else {
            ansiColor = ansi().fgBright(OK).toString();
        }
        return String.format("%s%2d%s", ansiColor, value, ansi().reset());
    }

    public String toString() {
        String result = getDisconnectCountColored();
        result += " " + getRepeatedT_ACKcountColored();
        return result;
    }

    public int getDisconnectCount()
    {
        return disconnectCount;
    }

    public int getRepeatedT_ACKcount()
    {
        return repeatedT_ACKcount;
    }

    public String getDisconnectCountColored() {
        return toColoredThreshold(getDisconnectCount(), BootloaderStatistic.THRESHOLD_DISCONNECT);
    }

    public String getRepeatedT_ACKcountColored() {
        return toColoredThreshold(getRepeatedT_ACKcount(), BootloaderStatistic.THRESHOLD_REPEATED);
    }
}
