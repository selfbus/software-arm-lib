package org.selfbus.updater;

import org.fusesource.jansi.Ansi;

import static org.fusesource.jansi.Ansi.Color.GREEN;
import static org.fusesource.jansi.Ansi.Color.YELLOW;
import static org.fusesource.jansi.Ansi.ansi;

public class ProgressInfo {
    private long startTimeMs;
    private long lastUpdateTimeMs;
    private float minBytesPerSecond;
    private float maxBytesPerSecond;
    private float currentBytesPerSecond;
    private long totalBytes;
    private long bytesDone;
    private float percentageDone;

    @SuppressWarnings("unused")
    private ProgressInfo() {}

    public ProgressInfo(long totalByteCount) {
        minBytesPerSecond = -1.0f;
        maxBytesPerSecond = -1.0f;
        currentBytesPerSecond = -1.0f;
        totalBytes = totalByteCount;
        bytesDone = 0;
        startTimeMs = System.currentTimeMillis();
        lastUpdateTimeMs = startTimeMs;
    }

    public void update(long byteCount) {
        // logging of connection speed
        long duration = System.currentTimeMillis() - lastUpdateTimeMs;
        currentBytesPerSecond = (float) byteCount / (duration / 1000.0f);
        lastUpdateTimeMs = System.currentTimeMillis();

        if (minBytesPerSecond < 0) {
            minBytesPerSecond = currentBytesPerSecond;
        }

        if (maxBytesPerSecond < 0) {
            maxBytesPerSecond = currentBytesPerSecond;
        }

        minBytesPerSecond = Math.min(minBytesPerSecond, currentBytesPerSecond);
        maxBytesPerSecond = Math.max(maxBytesPerSecond, currentBytesPerSecond);
        bytesDone += byteCount;
        percentageDone = 100.0f * (bytesDone) / getTotalBytes();
    }

    private String getSpeed(boolean averageSpeed) {
        float bytesPerSecond;
        if (averageSpeed) {
            long duration = getLastUpdateTimeMs() - getStartTimeMs();
            bytesPerSecond = (float) getBytesDone() / (duration / 1000.0f);
        }
        else {
            bytesPerSecond = getCurrentBytesPerSecond();
        }
        Ansi.Color color;
        if (bytesPerSecond >= 50.0) {
            color = GREEN;
        }
        else {
            color = YELLOW;
        }
        return String.format("%s%5.1f%s", ansi().fgBright(color), bytesPerSecond, ansi().reset());
    }

    public String getHeader() {
        return "   Done Speed   Avg   Min   Max  Time";
    }

    public String toString() {
        return String.format("%5.1f%% %s %s %5.1f %5.1f %tM:%<tS", getPercentageDone(), getSpeed(false),
                getSpeed(true), getMinBytesPerSecond(), getMaxBytesPerSecond(), getTimeElapsedMs());
    }

    public long getStartTimeMs() {
        return startTimeMs;
    }

    public float getMinBytesPerSecond() {
        return minBytesPerSecond;
    }

    public float getMaxBytesPerSecond() {
        return maxBytesPerSecond;
    }

    public long getTotalBytes() {
        return totalBytes;
    }

    public long getBytesDone() {
        return bytesDone;
    }

    public float getCurrentBytesPerSecond() {
        return currentBytesPerSecond;
    }

    public float getPercentageDone() {
        return percentageDone;
    }

    public long getLastUpdateTimeMs() {
        return lastUpdateTimeMs;
    }

    public long getTimeElapsedMs() {
        return System.currentTimeMillis() - getStartTimeMs();
    }
}
