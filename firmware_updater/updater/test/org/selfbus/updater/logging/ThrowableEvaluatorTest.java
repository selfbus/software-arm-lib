package org.selfbus.updater.logging;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.Logger;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.spi.LoggingEvent;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.slf4j.LoggerFactory;

import java.util.List;
import java.lang.reflect.Constructor;
import java.util.Map;

import static ch.qos.logback.classic.Level.*;
import static org.junit.jupiter.api.Assertions.*;

public class ThrowableEvaluatorTest {

    /**
     * Tests the evaluate method in the ThrowableEvaluator to ensure it correctly evaluates
     * logging events against its filter for exceptions and corresponding messages.
     */
    private final static Level[] allLevels = new Level[]{TRACE, DEBUG, INFO, WARN, ERROR};
    private final static Logger logger = (Logger) LoggerFactory.getLogger(ThrowableEvaluatorTest.class);
    ThrowableEvaluator evaluator = null;
    final String fqcn = logger.getClass().getName();

    @BeforeEach
    public void setUp() {
        evaluator = new ThrowableEvaluator();
        evaluator.setLevel(TRACE);
    }

    private boolean callEvaluate(Level level, String message, Throwable throwable) {
        ILoggingEvent event = new LoggingEvent(fqcn, logger, level, message, throwable, null);
        return evaluator.evaluate(event);
    }

    @Test
    void testLevel() {
        for (Level evaluatorLevel : allLevels) {
            evaluator.setLevel(evaluatorLevel);
            for (Level level : allLevels) {
                assertTrue(callEvaluate(level, "", null));
            }
        }
    }

    @Test
    void testThrowableIsNull() {
        for (Level evaluatorLevel : allLevels) {
            evaluator.setLevel(evaluatorLevel);
            for (Level level : allLevels) {
                assertTrue(callEvaluate(level, "testThrowableIsNull", null));
            }
        }
    }

    private Throwable createExceptionInstance(Class<? extends Throwable> exceptionClass, String message) throws Exception {
        // Use reflection to find a compatible constructor (one that accepts a String)
        Constructor<? extends Throwable> constructor = exceptionClass.getConstructor(String.class);
        return constructor.newInstance(message);
    }

    @Test
    void testExceptionFilterMap() throws Exception {
        evaluator.setLevel(INFO);
        // Iterate over all exceptions
        for (Map.Entry<Class<? extends Throwable>, List<ThrowableEvaluator.ConditionalFilter>> entry :
                ThrowableEvaluator.EXCEPTION_FILTER_MAP.entrySet()) {
            Class<? extends Throwable> exceptionClass = entry.getKey();

            // Iterate over all logmessage / exception message
            for (ThrowableEvaluator.ConditionalFilter filter : entry.getValue()) {
                String logMessage = filter.logMessage();
                String exceptionMessage = filter.exceptionMessage();

                // Check without exception is not filtered
                for (Level level : allLevels) {
                    assertTrue(callEvaluate(level, logMessage, null));
                }

                Throwable exception = createExceptionInstance(exceptionClass, exceptionMessage);
                for (Level level : new Level[]{ERROR, WARN, INFO}) {
                    // Check matching to EXCEPTION_FILTER_MAP is filtered out
                    assertFalse(callEvaluate(level, logMessage, exception));

                    // Check not matching logmessage to EXCEPTION_FILTER_MAP is NOT filtered
                    assertTrue(callEvaluate(level, logMessage + "foo", exception));
                }

                exception = createExceptionInstance(exceptionClass, exceptionMessage + "foo");
                for (Level level : new Level[]{ERROR, WARN, INFO}) {
                    // Check not matching to EXCEPTION_FILTER_MAP is NOT filtered
                    assertTrue(callEvaluate(level, logMessage, exception));

                    // Check not matching logmessage to EXCEPTION_FILTER_MAP is NOT filtered
                    assertTrue(callEvaluate(level, logMessage + "foo", exception));
                }
            }
        }
    }
}