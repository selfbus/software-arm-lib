package org.selfbus.updater;

import org.selfbus.updater.bootloader.BootloaderStatistic;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;
import static org.fusesource.jansi.Ansi.*;
import static org.fusesource.jansi.Ansi.Color.*;

public class BootloaderStatisticTest {
    @Test
    public void testToString() {
        String colorOK = ansi().fgBright(GREEN).toString();
        String colorWarn = ansi().fgBright(YELLOW).toString();
        String colorReset = ansi().reset().toString();
        String info1 = "";
        String info2 = " ";

        BootloaderStatistic test1 = new BootloaderStatistic(0, 0);
        BootloaderStatistic test2 = new BootloaderStatistic(BootloaderStatistic.THRESHOLD_DISCONNECT, 0);
        BootloaderStatistic test3 = new BootloaderStatistic(0, BootloaderStatistic.THRESHOLD_REPEATED);
        BootloaderStatistic test4 = new BootloaderStatistic(BootloaderStatistic.THRESHOLD_DISCONNECT,
                BootloaderStatistic.THRESHOLD_REPEATED);
        BootloaderStatistic test5 = new BootloaderStatistic(BootloaderStatistic.THRESHOLD_DISCONNECT + 1,
                BootloaderStatistic.THRESHOLD_REPEATED);
        BootloaderStatistic test6 = new BootloaderStatistic(BootloaderStatistic.THRESHOLD_DISCONNECT,
                BootloaderStatistic.THRESHOLD_REPEATED + 1);
        BootloaderStatistic test7 = new BootloaderStatistic(BootloaderStatistic.THRESHOLD_DISCONNECT + 1,
                BootloaderStatistic.THRESHOLD_REPEATED + 1);
        BootloaderStatistic test8 = new BootloaderStatistic(99, 99);
        BootloaderStatistic test9 = new BootloaderStatistic(100, 99);
        BootloaderStatistic test10 = new BootloaderStatistic(100, 100);

        String expected1 = info1 + colorOK + " 0" + colorReset + info2 + colorOK + " 0" + colorReset;
        String expected2 = info1 + colorOK + " 1" + colorReset + info2 + colorOK + " 0" + colorReset;
        String expected3 = info1 + colorOK + " 0" + colorReset + info2 + colorOK + " 1" + colorReset;
        String expected4 = info1 + colorOK + " 1" + colorReset + info2 + colorOK + " 1" + colorReset;
        String expected5 = info1 + colorWarn + " 2" + colorReset + info2 + colorOK + " 1" + colorReset;
        String expected6 = info1 + colorOK + " 1" + colorReset + info2 + colorWarn + " 2" + colorReset;
        String expected7 = info1 + colorWarn + " 2" + colorReset + info2 + colorWarn + " 2" + colorReset;
        String expected8 = info1 + colorWarn + "99" + colorReset + info2 + colorWarn + "99" + colorReset;
        String expected9 = info1 + colorWarn + "100" + colorReset + info2 + colorWarn + "99" + colorReset;
        String expected10 = info1 + colorWarn + "100" + colorReset + info2 + colorWarn + "100" + colorReset;

        assertEquals(expected1, test1.toString());
        assertEquals(expected2, test2.toString());
        assertEquals(expected3, test3.toString());
        assertEquals(expected4, test4.toString());
        assertEquals(expected5, test5.toString());
        assertEquals(expected6, test6.toString());
        assertEquals(expected7, test7.toString());
        assertEquals(expected8, test8.toString());
        assertEquals(expected9, test9.toString());
        assertEquals(expected10, test10.toString());
    }
}
