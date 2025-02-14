package org.selfbus.updater.logging;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.filter.ThresholdFilter;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.Appender;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.lang.management.ManagementFactory;

public final class LoggingManager {
    /**
     * The name of the console appender used for logging.
     * This constant is referenced in the logging configuration defined in
     * {@code src/resources/logback/uiAppenders.xml}.
     * <p>
     * Note: If the value of this constant is changed, ensure to update the
     * corresponding appender name in the `logback/uiAppenders.xml` file.
     */
    public static final String CONSOLE_APPENDER_NAME = "CONSOLE";

    /**
     * The name of the appender used for logging messages to a {@code JTextPane}.
     * This constant is referenced in the logging configuration defined in
     * {@code src/resources/logback/uiAppenders.xml}.
     * <p>
     * Note: If the value of this constant is changed, ensure to update the
     * corresponding appender name in the `logback/uiAppenders.xml` file.
     * */
    public static final String JTEXTPANE_APPENDER_NAME = "JTextPane";

    private final static Logger logger = LoggerFactory.getLogger(LoggingManager.class);

    private static LoggerContext getLoggerContext() {
        return (LoggerContext) LoggerFactory.getILoggerFactory();
    }

    public static Appender<ILoggingEvent> getAppender(String appenderName) {
        return getLoggerContext().getLogger(Logger.ROOT_LOGGER_NAME).getAppender(appenderName);
    }

    @SuppressWarnings("unused")
    public static void disableAppender(String appenderName) {
        Appender<ILoggingEvent> appender = getAppender(appenderName);

        if (appender == null) {
            logger.warn("disableAppender: appender {} not found.", appenderName);
            return;
        }

        logger.trace("Stopping and disabling {} appender.", appenderName);
        appender.stop();
        getLoggerContext().getLogger(Logger.ROOT_LOGGER_NAME).detachAppender(appenderName);
        logger.trace("{} appender has been disabled.", appenderName);
    }

    public static void setAppenderLogLevel(Appender<ILoggingEvent> appender, Level newLogLevel) {
        if (appender == null) {
            logger.warn("appender is null");
            return;
        }

        //todo this might be quite problematic, we loose all other possible defined filters here
        appender.clearAllFilters();
        logger.info("Deleted all filters of {}", appender.getName());

        ThresholdFilter thresholdFilter = new ThresholdFilter();
        thresholdFilter.setLevel(newLogLevel.toString());
        thresholdFilter.setContext(getLoggerContext());
        thresholdFilter.start();

        appender.addFilter(thresholdFilter);
        logger.trace("{} appender log level set to: {}", appender.getName(), newLogLevel);
    }

    public static void setAppenderLogLevel(String appenderName, Level newLogLevel) {
        Appender<ILoggingEvent> appender = getAppender(appenderName);
        setAppenderLogLevel(appender, newLogLevel);
    }

    public static boolean isRunningInIntelliJ() {
        // Check for IntelliJ debugging JVM arguments
        return ManagementFactory.getRuntimeMXBean().getInputArguments().stream()
                .anyMatch(arg -> arg.contains("javaagent") || arg.contains("agentlib"));
    }
}
