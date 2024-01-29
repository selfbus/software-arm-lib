package org.selfbus.updater.gui;

import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.AppenderBase;

import javax.swing.*;
import javax.swing.text.StyledDocument;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import static javax.swing.SwingUtilities.invokeLater;

public class ListAppender extends AppenderBase<ILoggingEvent> {

    private static volatile ArrayList<JTextArea> textAreas = new ArrayList<>();
    private static volatile  ArrayList<JTextPane> textPanes = new ArrayList<>();


    private int maxLines = 100;
    private static List<ILoggingEvent> eventList = new LinkedList<>();

    // Add the target JTextArea to be populated and updated by the logging information.
    public static void addLog4j2TextAreaAppender(final JTextArea textArea)
    {
        ListAppender.textAreas.add(textArea);
    }

    public static void addLog4j2TextPaneAppender(final JTextPane textPane){
        ListAppender.textPanes.add(textPane);
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
                            /*if (textPane.getText().isEmpty())
                            {
                                textPane.setText(finalMessage);
                            } else
                            {*/
                                StyledDocument document = (StyledDocument) textPane.getDocument();

                                ConColorsToStyledDoc.Convert(finalMessage, document);

                                //document.insertString(document.getLength(), finalMessage,null );

                                //textPane.append("\n" + message);
                                /*if (maxLines > 0 & textPane.getLineCount() > maxLines + 1)
                                {
                                    int endIdx = textArea.getDocument().getText(0, textArea.getDocument().getLength()).indexOf("\n");
                                    textArea.getDocument().remove(0, endIdx + 1);
                                }*/
                           // }
                            //String content = textPane.getText();
                            //textPane.setText(content.substring(0, content.length() - 1));
                        }
                    } catch (Throwable throwable)
                    {
                        throwable.printStackTrace();
                    }
                }
                /*
                for (JTextArea textArea : textAreas)
                {
                    try
                    {
                        if (textArea != null)
                        {
                            if (textArea.getText().length() == 0)
                            {
                                textArea.setText(message);
                            } else
                            {
                                textArea.append("\n" + message);
                                if (maxLines > 0 & textArea.getLineCount() > maxLines + 1)
                                {
                                    int endIdx = textArea.getDocument().getText(0, textArea.getDocument().getLength()).indexOf("\n");
                                    textArea.getDocument().remove(0, endIdx + 1);
                                }
                            }
                            String content = textArea.getText();
                            textArea.setText(content.substring(0, content.length() - 1));
                        }
                    } catch (Throwable throwable)
                    {
                        throwable.printStackTrace();
                    }
                }

                 */


            });
        } catch (IllegalStateException exception)
        {
            exception.printStackTrace();
        }
    }

    public List<ILoggingEvent> getEventList() {
        return eventList;
    }
}

/*
package org.selfbus.updater.gui;

import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.AppenderBase;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

// Der LogAppender nimmt die Ausgaben des Loggers entgegen und schrreibt diese in ein Textfeld
public class LogAppender extends AppenderBase<ILoggingEvent> {

    private ConcurrentMap<String, ILoggingEvent> eventMap = new ConcurrentHashMap<>();

    private GuiMain guiMain;

    @Override
    protected void append(ILoggingEvent event) {
        eventMap.put(String.valueOf(System.currentTimeMillis()), event);
    }

    public Map<String, ILoggingEvent> getEventMap() {
        return eventMap;
    }
}

 */
