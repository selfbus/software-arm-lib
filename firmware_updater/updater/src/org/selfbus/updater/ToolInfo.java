package org.selfbus.updater;


import org.selfbus.updater.bootloader.BootloaderIdentity;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.Settings;

import java.io.IOException;

/**
 * Provides information about the application
 */
public final class ToolInfo
{
    private static final long versionMajor = 1; ///\todo change also in ../README.md and build.gradle
    private static final long versionMinor = 23;

    private static final long minMajorVersionBootloader = 1; ///\todo change also in ../README.md
    private static final long minMinorVersionBootloader = 0;

    private static final Logger logger = LoggerFactory.getLogger(ToolInfo.class);
    private static final String author = "Selfbus";
    private static final String tool = "Selfbus-Updater";
    private static final String desc = "A Tool for updating firmware of a Selfbus device in a KNX network.";

    private ToolInfo() {}

    public static long versionMajor() { return versionMajor;}
    public static long versionMinor() { return versionMinor;}

    /**
     * Returns the version as string representation.
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
        return getToolAndVersion() + String.format("%s%s%s%s", System.lineSeparator(), desc,
                System.lineSeparator(), author);
    }

    public static String getToolAndVersion() {
        return String.format("%s %s (build %s)", getTool(), getVersion(), getManifestInfo());
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

    public static String getToolJarName() {
        return String.format("SB_updater-%s-all.jar", ToolInfo.getVersion());
    }

    private static String getManifestInfo() {
        // Only works correctly if built with gradle.
        // This will fail during debugging, e.g. in IDEA, due to a different content of MANIFEST.MF
        try (final var metaInf = ToolInfo.class.getResourceAsStream("/META-INF/MANIFEST.MF")) {
            if (metaInf == null) {
                return "";
            }

            final var manifest = new java.util.jar.Manifest(metaInf);
            final var attributes = manifest.getMainAttributes();
            return String.format("%s sha %s",
                    attributes.getValue("Build-Date"), attributes.getValue("Revision"));
        }
        catch (IOException ignore) {

        }
        return "";
    }
}
