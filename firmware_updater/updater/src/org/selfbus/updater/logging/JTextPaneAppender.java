package org.selfbus.updater.logging;

import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.spi.IThrowableProxy;
import ch.qos.logback.classic.spi.StackTraceElementProxy;
import ch.qos.logback.core.AppenderBase;
import org.selfbus.updater.gui.ConColorsToStyledDoc;

import javax.swing.*;
import javax.swing.text.BadLocationException;

import static org.selfbus.updater.logging.Markers.CONSOLE_GUI_NO_NEWLINE;

public class JTextPaneAppender extends AppenderBase<ILoggingEvent> {
    private JTextPane textPane;

    @SuppressWarnings("unused")
    public JTextPaneAppender() {
        this.textPane = null;
    }

    private void updateTextPane(String message) throws BadLocationException {
        if (getTextPane() == null) {
            return;
        }
        ConColorsToStyledDoc.Convert(message, getTextPane());
    }

    @Override
    protected void append(ILoggingEvent event) {
        if (getTextPane() == null) {
            return;
        }

        StringBuilder eventMessage = new StringBuilder(event.getFormattedMessage());

        // Check for ThrowableProxy to retrieve the stack trace
        IThrowableProxy throwableProxy = event.getThrowableProxy();
        if (throwableProxy != null) {
            // Append the stack trace to the log message
            eventMessage.append(System.lineSeparator());
            appendStackTrace(throwableProxy, eventMessage);
        }

        String logMessage;
        if ((event.getMarkerList() != null) && (event.getMarkerList().contains(CONSOLE_GUI_NO_NEWLINE))) {
            logMessage = eventMessage.toString();
        }
        else {
            // Ensure same behavior like ConsoleAppender
            // It gets a new line added to every logging message
            // by it´s pattern <pattern>%message%n</pattern> (notice the n = new line) in logback.xml
            logMessage = eventMessage.append(System.lineSeparator()).toString();
        }

        String finalLogMessage = logMessage;
        SwingUtilities.invokeLater(() -> {
            try {
                // Append formatted message to text area using the Thread
                updateTextPane(finalLogMessage);
            }
            catch (BadLocationException e) {
                throw new RuntimeException(e);
            }
        });
    }

    private void appendStackTrace(IThrowableProxy throwableProxy, StringBuilder builder) {
        builder.append(throwableProxy.getClassName())
                .append(": ")
                .append(throwableProxy.getMessage())
                .append(System.lineSeparator());

        // Loop through each stack trace element and append it
        for (StackTraceElementProxy elementProxy : throwableProxy.getStackTraceElementProxyArray()) {
            builder.append("\t ")
                    .append(elementProxy.toString())
                    .append(System.lineSeparator());
        }

        // If there are any nested causes, recursively append them
        IThrowableProxy cause = throwableProxy.getCause();
        if (cause != null) {
            builder.append("Caused by: ");
            appendStackTrace(cause, builder);
        }
    }

    public JTextPane getTextPane() {
        return textPane;
    }

    public void setTextPane(JTextPane textPane) {
        this.textPane = textPane;
    }
}