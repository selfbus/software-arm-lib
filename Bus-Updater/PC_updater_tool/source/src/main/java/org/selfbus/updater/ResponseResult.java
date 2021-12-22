package org.selfbus.updater;

import java.util.Arrays;

public class ResponseResult {
    private byte[] data;
    private long timeoutCount;
    private long dropCount;
    private long written;

    ResponseResult() {
        this.timeoutCount = 0;
        this.dropCount = 0;
        this.data = null;
        this.written = 0;
    }

    byte[] data() {
        return data;
    }

    public void copyFromArray(byte[] byteArray) {
        data = Arrays.copyOf(byteArray, byteArray.length);
    }

    public long timeoutCount() {
        return timeoutCount;
    }

    public long dropCount() {
        return dropCount;
    }

    public long written() {
        return written;
    }

    public void incTimeoutCount() {
        timeoutCount++;
    }

    public void incDropCount() {
        dropCount++;
    }

    public void addWritten(long toAdd) {
        written += toAdd;
    }


    public void addCounters(ResponseResult toAdd)
    {
        timeoutCount += toAdd.timeoutCount();
        dropCount += toAdd.dropCount();
    }
}
