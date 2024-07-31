package org.selfbus.updater;

/**
 * Provides constants for console color output (font color/style and background)
 */
public class ConColors {
    // Reset to default colors
    public static final String RESET  = "\033[0m";  	// Reset

    // Regular Colors
    public static final String BLACK  = "\033[0;30m";   // BLACK
    public static final String RED    = "\033[0;31m";   // RED
    public static final String YELLOW = "\033[0;33m";   // YELLOW

    // Regular Background
    public static final String BG_RED    = "\033[41m";  // RED
    public static final String BG_GREEN  = "\033[42m";  // GREEN

    // High Intensity
    public static final String BRIGHT_RED    = "\033[0;91m";  // RED
    public static final String BRIGHT_GREEN  = "\033[0;92m";  // GREEN
    public static final String BRIGHT_YELLOW = "\033[0;93m";  // YELLOW
}

