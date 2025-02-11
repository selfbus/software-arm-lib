package org.selfbus.updater.logging;

import ch.qos.logback.classic.spi.ILoggingEvent;

import java.util.List;
;

public class MessageEvaluator extends LevelEventEvaluator {

    // Customizable List of log messages to filter out
    private static final List<String> FILTERED_MESSAGES = List.of(
            "received service acknowledgment status sequence number out of order"
    );

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
