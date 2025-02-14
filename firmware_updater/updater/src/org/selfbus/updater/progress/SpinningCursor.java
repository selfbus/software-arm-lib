package org.selfbus.updater.progress;

public final class SpinningCursor {
    @SuppressWarnings("unused")
    private SpinningCursor() {} // avoids instance creation

    private static final char[] cursorPool = {' ', '|', '/', '-', '\\'}; // Index 0 is the blank cursor
    private static int index = 1;

    public static void reset() {
        index = 1;
    }
    public static char getNext() {
        if (index > (cursorPool.length - 1)) {
            reset();
        }
        char nextCursor = cursorPool[index];
        index++;

        return nextCursor;
    }

    public static void setBlank() {
        index = 0;
    }
}
