package org.selfbus.updater.progress;


import org.selfbus.updater.bootloader.BootloaderStatistic;

public class ProgressInfoAdvanced extends ProgressInfo {
    private final BootloaderStatistic bootloaderStatistic;

    public ProgressInfoAdvanced(long totalByteCount, BootloaderStatistic bootloaderStatistic) {
        super(totalByteCount);
        this.bootloaderStatistic = bootloaderStatistic;
    }

    @Override
    public String getHeader() {
        return super.getHeader() + " #D #R";
    }

    @Override
    public String toString() {
        String result = super.toString();
        return String.format("%s %s", result, getBootloaderStatistic());
    }

    public BootloaderStatistic getBootloaderStatistic() {
        return bootloaderStatistic;
    }
}
