package org.selfbus.updater;

public final class AnsiCursor {
    @SuppressWarnings("unused")
    private AnsiCursor() {} // avoids instance creation

    public static String on() {
        // cursor on
        return "\033[?25h";
    }
    public static String off() {
        // cursor off
        return "\033[?25l";
    }
}
