package org.selfbus.updater.logging;

import ch.qos.logback.classic.Logger;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.spi.LoggingEvent;
import ch.qos.logback.core.spi.FilterReply;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.slf4j.LoggerFactory;

import static ch.qos.logback.classic.Level.TRACE;
import static org.junit.jupiter.api.Assertions.*;

class MessageFilterTest {

    /**
     * Tests the evaluate method in the MessageFilter to ensure it correctly evaluates
     * logging events against its filter for log messages.
     */
    private final static Logger logger = (Logger) LoggerFactory.getLogger(MessageFilterTest.class);
    MessageFilter messageFilter = null;
    final String fqcn = logger.getClass().getName();

    @BeforeEach
    public void setUp() {
        messageFilter = new MessageFilter();
        messageFilter.start();
    }

    private FilterReply callDecide(String message) {
        ILoggingEvent event = new LoggingEvent(fqcn, logger, TRACE, message, null, null);
        return messageFilter.decide(event);
    }

    @Test
    void testFilteredMessagesList() {
        // Check some "random" log message is NOT filtered out
        assertSame(FilterReply.NEUTRAL, callDecide("some log message "));
        for (String filteredMessage : MessageFilter.FILTERED_MESSAGES) {
            // Check matching log message is filtered out
            assertSame(FilterReply.DENY, callDecide(filteredMessage));
            // Check containing log message is filtered out
            assertSame(FilterReply.DENY, callDecide(filteredMessage + "foo")); // test
        }
    }
}