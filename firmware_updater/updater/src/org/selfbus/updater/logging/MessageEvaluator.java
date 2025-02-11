package org.selfbus.updater.logging;

import ch.qos.logback.classic.spi.ILoggingEvent;

import java.util.List;

/**
 * The {@code MessageEvaluator} class is a custom implementation of {@code LevelEventEvaluator}.
 * It is used to evaluate logging events and filter out specific log messages based on a
 * predefined list of messages.
 * <p>
 * This functionality can be useful for excluding log messages that are deemed
 * irrelevant or repetitive during runtime.
 */
public class MessageEvaluator extends LevelEventEvaluator {

    /**
     * A list of log messages that should be filtered out.
     * If an event's formatted message contains any of the strings in this list,
     * the event will be excluded from logging.
     */
    private static final List<String> FILTERED_MESSAGES = List.of(
            // Loxone Miniserver Gen1 returns always status 0x04 on KNX tunnel requests with sequence number 255
            "received service acknowledgment status sequence number out of order",
            // from calimero-core KNXnetIPTunnel.java:
            // we could get a .con with its hop count already decremented by 1 (eibd does that)
            "received L_Data.con with hop count decremented by 1 (sent"
    );

    /**
     * Evaluates the provided logging event.
     * <p>
     * This method first delegates evaluation to the superclass method. If the superclass
     * evaluation passes, the event is logged. Otherwise, it checks whether the event's
     * message contains any of the filtered messages specified in {@code FILTERED_MESSAGES}.
     * If a match is found, the event will not be logged.
     *
     * @param event the {@code ILoggingEvent} to evaluate
     * @return {@code true} if the event passes all filters and should be logged, {@code false} otherwise
     * @throws NullPointerException if the event or its formatted message is null
     */
    @Override
    public boolean evaluate(ILoggingEvent event) throws NullPointerException {
        if (super.evaluate(event)) {
            return true;
        }

        String message = event.getFormattedMessage();
        for (String filteredMessage : FILTERED_MESSAGES) {
            if (message.contains(filteredMessage)) {
                return false;  // Filter this event out
            }
        }
        return true;
    }
}
