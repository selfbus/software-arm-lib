package org.selfbus.updater;

public final class SpinningCursor {
    @SuppressWarnings("unused")
    private SpinningCursor() {} // avoids instance creation

    private static final char[] cursorPool = {'|', '/', '-', '\\'};
    private static int index = 0;

    public static void reset() {
        index = 0;
    }
    public static char getNext() {
        if (index > (cursorPool.length - 1)) {
            reset();
        }
        char nextCursor = cursorPool[index];
        index++;

        return nextCursor;
    }
}
