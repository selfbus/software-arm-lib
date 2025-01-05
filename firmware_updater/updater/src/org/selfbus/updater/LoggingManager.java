package org.selfbus.updater;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.filter.ThresholdFilter;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.spi.LoggingEvent;
import ch.qos.logback.core.Appender;
import ch.qos.logback.core.filter.Filter;
import ch.qos.logback.core.spi.FilterReply;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


public final class LoggingManager {
    private final static Logger logger = LoggerFactory.getLogger(LoggingManager.class);


    // In case CONSOLE_APPENDER_NAME is changed,
    // change also CONSOLE appender name in src/resources/logback.xml
    public static final String CONSOLE_APPENDER_NAME = "CONSOLE";

    public static void disableAppender(String appenderName) {
        LoggerContext loggerContext = (LoggerContext) LoggerFactory.getILoggerFactory();
        Appender<ILoggingEvent> appender = loggerContext.getLogger(Logger.ROOT_LOGGER_NAME).getAppender(appenderName);

        if (appender == null) {
            logger.warn("disableAppender: appender {} not found.", appenderName);
            return;
        }

        logger.trace("Stopping and disabling {} appender.", appenderName);
        appender.stop();
        loggerContext.getLogger(Logger.ROOT_LOGGER_NAME).detachAppender(appenderName);
        logger.trace("{} appender has been disabled.", appenderName);
    }

    public static void setAppenderLogLevel(String appenderName, Level newLogLevel) {
        LoggerContext loggerContext = (LoggerContext) LoggerFactory.getILoggerFactory();
        Appender<ILoggingEvent> appender = loggerContext.getLogger(Logger.ROOT_LOGGER_NAME).getAppender(appenderName);

        if (appender == null) {
            logger.warn("setAppenderLogLevel: appender {} not found.", appenderName);
            return;
        }

        appender.clearAllFilters();

        ThresholdFilter thresholdFilter = new ThresholdFilter();
        thresholdFilter.setLevel(newLogLevel.toString());
        thresholdFilter.setContext(loggerContext);
        thresholdFilter.start();

        appender.addFilter(thresholdFilter);
        logger.trace("{} appender log level set to: {}", appenderName, newLogLevel);
    }

    public static Level getAppenderLogLevel(String appenderName) {
        LoggerContext loggerContext = (LoggerContext) LoggerFactory.getILoggerFactory();
        Appender<ILoggingEvent> appender = loggerContext.getLogger(Logger.ROOT_LOGGER_NAME).getAppender(appenderName.toUpperCase());

        if (appender == null) {
            return null;
        }

        final Level[] testLevels = {Level.INFO, Level.DEBUG, Level.WARN, Level.ERROR, Level.TRACE};

        for (Filter<ILoggingEvent> filter : appender.getCopyOfAttachedFiltersList()) {
            if (filter instanceof ThresholdFilter thresholdFilter) {
                // Test with different levels to infer the threshold
                for (Level testLevel : testLevels) {
                    LoggingEvent testEvent = new LoggingEvent();
                    testEvent.setLevel(testLevel);
                    if (thresholdFilter.decide(testEvent) != FilterReply.DENY) {
                        return testLevel;
                    }
                }
            }
        }
        return null;
    }

    public static boolean isConsoleActive() {
        return (LoggingManager.getAppenderLogLevel(CONSOLE_APPENDER_NAME) == Level.INFO);
    }
}
