package org.selfbus.updater.logging;

import org.slf4j.Marker;
import org.slf4j.MarkerFactory;

/**
 * The {@code Markers} class provides a set of predefined logging markers
 * that can be used to categorize and filter log messages in the application.
 */
public class Markers {

    /**
     * Marker used to categorize log messages intended exclusively for the console and GUI.
     */
    public static final Marker CONSOLE_GUI_ONLY = MarkerFactory.getMarker("CONSOLE_GUI_ONLY");

    /**
     * Marker used to categorize log messages intended exclusively for the console and GUI,
     * specifically to handle cases where newline characters should be omitted.
     */
    public static final Marker CONSOLE_GUI_NO_NEWLINE = MarkerFactory.getMarker("CONSOLE_GUI_NO_NEWLINE");
}
