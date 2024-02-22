package org.selfbus.updater.gui;

import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.AppenderBase;

import javax.swing.*;
import javax.swing.text.StyledDocument;
import java.util.ArrayList;

import static javax.swing.SwingUtilities.invokeLater;

public class TextAppender extends AppenderBase<ILoggingEvent> {

    private static final ArrayList<JTextPane> textPanes = new ArrayList<>();

    // Add the target JTextPane to be populated and updated by the logging information.
    public static void addLog4j2TextPaneAppender(final JTextPane textPane){
        TextAppender.textPanes.add(textPane);
    }

    @Override
    protected void append(ILoggingEvent event) {
        String message = event.getFormattedMessage();
        message = message + System.lineSeparator(); // jeden Eintrag als newline enden lassen

        String finalMessage = message;

        // Append formatted message to text area using the Thread.
        try
        {
            invokeLater(() ->
            {
                for (JTextPane textPane : textPanes)
                {
                    try
                    {
                        if (textPane != null)
                        {
                                StyledDocument document = (StyledDocument) textPane.getDocument();
                                ConColorsToStyledDoc.Convert(finalMessage, document);

                                // immer die letzte Zeile zeigen
                                textPane.setCaretPosition(textPane.getDocument().getLength());
                        }
                    } catch (Throwable throwable)
                    {
                        throwable.printStackTrace();
                    }
                }
            });
        } catch (IllegalStateException exception)
        {
            exception.printStackTrace();
        }
    }
}