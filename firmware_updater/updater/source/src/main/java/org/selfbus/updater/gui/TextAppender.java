package org.selfbus.updater.gui;

import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.AppenderBase;

import javax.swing.*;
import javax.swing.text.BadLocationException;

import static javax.swing.SwingUtilities.invokeLater;

public class TextAppender extends AppenderBase<ILoggingEvent> {
    private JTextPane textPane;

    @SuppressWarnings("unused")
    public TextAppender() {
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
        String eventMessage = event.getFormattedMessage();
        // Make sure, that every eventMessage ends with a line separator
        if (eventMessage.lastIndexOf(System.lineSeparator()) != eventMessage.length() - System.lineSeparator().length()) {
            eventMessage += System.lineSeparator();
        }

        final String finalMessage = eventMessage;
        invokeLater(() -> {
            try {
                // Append formatted message to text area using the Thread.
                updateTextPane(finalMessage);
            }
            catch (Throwable e) {
                throw new RuntimeException(e);
            }
        });
    }

    public JTextPane getTextPane() {
        return textPane;
    }

    public void setTextPane(JTextPane textPane) {
        this.textPane = textPane;
    }
}