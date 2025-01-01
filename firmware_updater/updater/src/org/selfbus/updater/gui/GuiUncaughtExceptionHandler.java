package org.selfbus.updater.gui;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.swing.*;
import java.io.PrintWriter;
import java.io.StringWriter;

import static org.fusesource.jansi.Ansi.Color.*;
import static org.fusesource.jansi.Ansi.ansi;

public class GuiUncaughtExceptionHandler implements Thread.UncaughtExceptionHandler {
    private final static Logger logger = LoggerFactory.getLogger(GuiUncaughtExceptionHandler.class);
    private final JFrame parentFrame;

    public GuiUncaughtExceptionHandler(JFrame parentFrame) {
        this.parentFrame = parentFrame;
    }

    /**
     * Handles uncaught exceptions in threads by logging the error details and displaying
     * an error dialog to inform the user.
     *
     * @param thread The thread in which the uncaught exception occurred.
     * @param throwable The uncaught exception or error.
     */
    @Override
    public void uncaughtException(Thread thread, Throwable throwable) {
        logger.error("{}Uncaught exception in thread {} {}{}",
                ansi().fg(RED) , thread.getName(), throwable.toString(), ansi().reset(), throwable);

        // todo see logback issue #876
        // https://github.com/qos-ch/logback/issues/876
        // Don´t delete .toString()
        // Show exception details in a dialog
        SwingUtilities.invokeLater(() -> JOptionPane.showMessageDialog(
                parentFrame,
                "An unexpected error occurred:" + System.lineSeparator() +
                        throwable.toString() + System.lineSeparator() +
                        getLimitedStackTrace(throwable, 7),
                "Exception",
                JOptionPane.ERROR_MESSAGE
        ));
    }

    /**
     * Converts the stack trace to a String and limits its depth.
     *
     * @param throwable The exception or error
     * @param maxDepth  The maximum number of stack trace elements to include
     * @return A String containing the limited stack trace
     */
    private static String getLimitedStackTrace(Throwable throwable, int maxDepth) {
        StringWriter stringWriter = new StringWriter();
        PrintWriter printWriter = new PrintWriter(stringWriter);

        // Get the stack trace elements
        StackTraceElement[] stackTrace = throwable.getStackTrace();
        if (maxDepth < 0)
        {
            maxDepth = stackTrace.length;
        }
        int limit = Math.min(stackTrace.length, maxDepth); // Ensure we don't exceed the actual trace length
        for (int i = 0; i < limit; i++) {
            printWriter.println("      at " + stackTrace[i].toString());
        }

        // Indicate if there are more stack trace elements beyond the limit
        if (stackTrace.length > maxDepth) {
            printWriter.println("      ... (" + (stackTrace.length - maxDepth) + " more elements)");
        }

        return stringWriter.toString();
    }
}
