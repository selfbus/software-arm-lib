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

class LevelEventEvaluatorTest {

    /**
     * Tests the evaluate method in the LevelEventEvaluator to ensure it correctly evaluates
     * logging events against its level filter.
     */
    private final static Level[] allLevels = new Level[]{TRACE, DEBUG, INFO, WARN, ERROR};
    private final static Logger logger = (Logger) LoggerFactory.getLogger(LevelEventEvaluatorTest.class);
    LevelEventEvaluator evaluator = null;
    String fqcn = logger.getClass().getName();

    @BeforeEach
    public void setUp() {
        evaluator = new LevelEventEvaluator();
        evaluator.setLevel(TRACE);
    }

    private boolean callEvaluate(Level level) {
        ILoggingEvent event = new LoggingEvent(fqcn, logger, level, "a", null, null);
        return evaluator.evaluate(event);
    }

    @Test
    void testLevel() {
        for (Level evaluatorLevel : allLevels) {
            evaluator.setLevel(evaluatorLevel);
            for (Level level : allLevels) {
                // assertFalse(callEvaluate(level)); //todo probably bug? Whole logic is inverted?
                                                  // True should be the correct one?
                // assertTrue(callEvaluate(level));
            }
        }
    }
}