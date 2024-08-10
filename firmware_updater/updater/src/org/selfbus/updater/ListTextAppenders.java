package org.selfbus.updater;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.Logger;
import ch.qos.logback.classic.spi.LoggingEvent;
import ch.qos.logback.core.Appender;
import org.selfbus.updater.gui.TextAppender;
import org.slf4j.LoggerFactory;

import java.util.Iterator;

public final class ListTextAppenders {
    @SuppressWarnings("unused")
    private ListTextAppenders() {} // avoids instance creation

    private static TextAppender jTextPaneAppender = null;
    private static Logger logger = null;

    public static TextAppender searchAppender() {
        // Get the LoggerContext from the LoggerFactory
        LoggerContext loggerContext = (LoggerContext) LoggerFactory.getILoggerFactory();

        // Iterate over all loggers in the context
        for (Logger iterLogger : loggerContext.getLoggerList()) {
            // Get all appenders for the iterLogger
            Iterator<Appender<ILoggingEvent>> appenderIterator = iterLogger.iteratorForAppenders();

            while (appenderIterator.hasNext()) {
                Appender<ILoggingEvent> appender = appenderIterator.next();
                if (appender instanceof TextAppender) {
                    logger = iterLogger;
                    jTextPaneAppender = (TextAppender) appender;
                    return jTextPaneAppender;
                }
            }
        }
        jTextPaneAppender = null;
        return jTextPaneAppender;
    }

    public static TextAppender getJTextPaneAppender() {
        if (jTextPaneAppender != null) {
            return jTextPaneAppender;
        }
        else {
            return searchAppender();
        }
    }

    public static void appendEvent(Level level, String logMessage) {
        if (getJTextPaneAppender() == null) {
            return;
        }

        if (logger == null) {
            return;
        }

        ILoggingEvent loggingEvent = new LoggingEvent(logger.getName(), logger,
                level, logMessage, null, null);
        getJTextPaneAppender().doAppend(loggingEvent);
    }
}

