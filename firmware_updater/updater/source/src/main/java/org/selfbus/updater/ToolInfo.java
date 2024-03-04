package org.selfbus.updater;


import org.selfbus.updater.bootloader.BootloaderIdentity;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.Settings;

/**
 * Provides information about the application
 */
public final class ToolInfo
{
    private static final long versionMajor = 1; ///\todo change also in ../README.md and build.gradle
    private static final long versionMinor = 12;

    private static final long minMajorVersionBootloader = 1; ///\todo change also in ../README.md
    private static final long minMinorVersionBootloader = 0;

    private static final Logger logger = LoggerFactory.getLogger(ToolInfo.class.getName());
    private static final String author = "Selfbus";
    private static final String tool = "Selfbus-Updater";
    private static final String desc = "A Tool for updating firmware of a Selfbus device in a KNX network.";

    private ToolInfo() {}

    public static long versionMajor() { return versionMajor;}
    public static long versionMinor() { return versionMinor;}

    /**
     * Returns the version as string representation.
     * <p>
     * The returned version
     *
     * @return version as string
     */
    public static String getVersion() {
        return String.format("%d.%02d", versionMajor(), versionMinor());
    }

    /**
     * Returns the author as string representation.
     *
     * @return author as string
     */
    public static String getAuthor() {
        return author;
    }

    /**
     * Returns version tool-name
     *
     * @return header as string
     */
    public static String getTool() {
        return tool;
    }

    public static String getFullInfo() {
        return getToolAndVersion() + String.format("\n%s\n%s", desc, author);
    }

    public static String getToolAndVersion() {
        return String.format("%s %s", getTool(), getVersion());
    }

    public static void showVersion() {
        logger.info(ToolInfo.getToolAndVersion());
        logger.info(Settings.getLibraryHeader(false));
    }

    public static long minMajorVersionBootloader() {return minMajorVersionBootloader;}
    public static long minMinorVersionBootloader() {return minMinorVersionBootloader;}

    public static String minVersionBootloader() {
        return new BootloaderIdentity(minMajorVersionBootloader(),
                                      minMinorVersionBootloader(),
                        0, 0, 0, 0).getVersion();
    }
}
