package org.selfbus.updater.logging;

import java.util.Arrays;

import ch.qos.logback.classic.encoder.PatternLayoutEncoder;
import ch.qos.logback.classic.spi.ILoggingEvent;

import static org.selfbus.updater.logging.Markers.CONSOLE_GUI_NO_NEWLINE;

@SuppressWarnings("unused")
public class ConsoleEncoder extends PatternLayoutEncoder {
    @Override
    public void start() {
        super.start();
        if (!LoggingManager.isRunningInIntelliJ()) {
            addInfo("IntelliJ not detected => Marker processing enabled.");
        }
    }

    @Override
    public byte[] encode(ILoggingEvent event) {
        final byte[] superByteArray = super.encode(event);
        // No marker
        if (event.getMarkerList() == null) {
            return superByteArray;
        }

        // Not the marker we are looking for
        if (!(event.getMarkerList().contains(CONSOLE_GUI_NO_NEWLINE))) {
            return superByteArray;
        }

        final byte[] lineSeparator = System.lineSeparator().getBytes();
        // shorter than the line separator
        if (superByteArray.length < lineSeparator.length) {
            return superByteArray;
        }

        byte[] compare = Arrays.copyOfRange(superByteArray, superByteArray.length - lineSeparator.length, superByteArray.length);
        if (!Arrays.equals(compare, lineSeparator)) {
            return superByteArray;
        }

        final byte[] trimmed = Arrays.copyOfRange(superByteArray, 0, superByteArray.length - lineSeparator.length);
        if (LoggingManager.isRunningInIntelliJ()) {
            // IntelliJs debug console does not support ansi cursor movements
            // so we return the original one from the base pattern decoder
            return superByteArray;
        }
        else {
            return trimmed;
        }
    }
}
