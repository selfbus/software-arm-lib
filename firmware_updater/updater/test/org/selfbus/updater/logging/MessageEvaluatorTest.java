package org.selfbus.updater.logging;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.Logger;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.spi.LoggingEvent;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.slf4j.LoggerFactory;

import static ch.qos.logback.classic.Level.*;
import static org.junit.jupiter.api.Assertions.*;

class MessageEvaluatorTest {

    /**
     * Tests the evaluate method in the MessageEvaluator to ensure it correctly evaluates
     * logging events against its filter for log messages.
     */
    private final static Level[] allLevels = new Level[]{TRACE, DEBUG, INFO, WARN, ERROR};
    private final static Logger logger = (Logger) LoggerFactory.getLogger(MessageEvaluatorTest.class);
    MessageEvaluator evaluator = null;
    final String fqcn = logger.getClass().getName();

    @BeforeEach
    public void setUp() {
        evaluator = new MessageEvaluator();
        evaluator.setLevel(TRACE);
    }

    private boolean callEvaluate(Level level, String message) {
        ILoggingEvent event = new LoggingEvent(fqcn, logger, level, message, null, null);
        return evaluator.evaluate(event);
    }

    @Test
    void testLevel() {
        for (Level evaluatorLevel : allLevels) {
            evaluator.setLevel(evaluatorLevel);
            for (Level level : allLevels) {
                assertTrue(callEvaluate(level, ""));
            }
        }
    }

    @Test
    void testFilteredMessagesList() {
        evaluator.setLevel(INFO);
        for (Level level : new Level[]{ERROR, WARN, INFO}) {
            // Check some "random" log message is NOT filtered out
            assertTrue(callEvaluate(level, "some log message " + level.toString()));
            for (String filteredMessage : MessageEvaluator.FILTERED_MESSAGES) {
                // Check matching log message is filtered out
                assertFalse(callEvaluate(level, filteredMessage));
                // Check containing log message is filtered out
                assertFalse(callEvaluate(level, filteredMessage + "foo")); // test
            }
        }
    }
}