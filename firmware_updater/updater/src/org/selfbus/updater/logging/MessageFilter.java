package org.selfbus.updater.logging;

import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.filter.Filter;
import ch.qos.logback.core.spi.FilterReply;

import java.util.List;

/**
 * The {@code MessageFilter} class is a custom implementation of {@code Filter<E>}.
 * It is used to evaluate logging events and filter out specific log messages based on a
 * predefined list of messages.
 * <p>
 * This functionality can be useful for excluding log messages that are deemed
 * irrelevant or repetitive during runtime.
 */
public class MessageFilter extends Filter<ILoggingEvent> {

    /**
     * A list of log messages that should be filtered out.
     * If an event's formatted message contains any of the strings in this list,
     * the event will be excluded from logging.
     */
    public static final List<String> FILTERED_MESSAGES = List.of(
            // Loxone Miniserver Gen1 returns always status 0x04 on KNX tunnel requests with sequence number 255
            "received service acknowledgment status sequence number out of order",
            // from calimero-core KNXnetIPTunnel.java:
            // we could get a .con with its hop count already decremented by 1 (eibd does that)
            "received L_Data.con with hop count decremented by 1 (sent"
    );

    /**
     * Evaluates a logging event and determines whether the event should
     * be accepted or handled neutrally based on its log message content.
     *
     * @param event the logging event to be evaluated.
     * @return {@code FilterReply.DENY} if the event's message contains any of the predefined
     *         filtered messages, otherwise {@code FilterReply.NEUTRAL}.
     */
    @Override
    public FilterReply decide(ILoggingEvent event) {
        if (!isStarted()) {
            return FilterReply.NEUTRAL;
        }

        String message = event.getFormattedMessage();
        for (String filteredMessage : FILTERED_MESSAGES) {
            if (message.contains(filteredMessage)) {
                return FilterReply.DENY;
            }
        }
        return FilterReply.NEUTRAL;
    }
}
