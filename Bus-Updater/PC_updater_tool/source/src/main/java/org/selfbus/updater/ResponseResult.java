package org.selfbus.updater;

import java.util.Arrays;

public class ResponseResult {
    private byte[] data;
    private long timeoutCount;
    private long dropCount;

    ResponseResult() {
        this.timeoutCount = 0;
        this.dropCount = 0;
        this.data = null;
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

    public void incTimeoutCount() {
        timeoutCount++;
    }

    public void incDropCount() {
        dropCount++;
    }

}
