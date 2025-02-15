package org.selfbus.updater.logging;

import org.fusesource.jansi.Ansi;

import static org.fusesource.jansi.Ansi.Color.*;

public final class Color {
    @SuppressWarnings("unused")
    private Color() {}

    // old style colors
    public static final Ansi.Color INFO = YELLOW;
    public static final Ansi.Color OK = GREEN;
    public static final Ansi.Color WARN = RED;

    //todo more red/green friendly?
//    public static final Ansi.Color INFO = WHITE;
//    public static final Ansi.Color OK = YELLOW;
//    public static final Ansi.Color WARN = MAGENTA;
}