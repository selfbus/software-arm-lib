package org.selfbus.updater.logging;

import ch.qos.logback.classic.Logger;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.spi.LoggingEvent;
import ch.qos.logback.core.spi.FilterReply;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.slf4j.LoggerFactory;

import java.util.List;
import java.lang.reflect.Constructor;
import java.util.Map;

import static ch.qos.logback.classic.Level.*;
import static org.junit.jupiter.api.Assertions.assertSame;

public class ThrowableFilterTest {

    /**
     * Tests the evaluate method in the ThrowableFilter to ensure it correctly evaluates
     * logging events against its filter for exceptions and corresponding messages.
     */
    private final static Logger logger = (Logger) LoggerFactory.getLogger(ThrowableFilterTest.class);
    ThrowableFilter throwableFilter = null;
    final String fqcn = logger.getClass().getName();

    @BeforeEach
    public void setUp() {
        throwableFilter = new ThrowableFilter();
        throwableFilter.start();
    }

    private FilterReply callDecide(String message, Throwable throwable) {
        ILoggingEvent event = new LoggingEvent(fqcn, logger, TRACE, message, throwable, null);
        return throwableFilter.decide(event);
    }

    @Test
    void testThrowableIsNull() {
        assertSame(FilterReply.NEUTRAL, callDecide("testThrowableIsNull", null));
    }

    private Throwable createExceptionInstance(Class<? extends Throwable> exceptionClass, String message) throws Exception {
        // Use reflection to find a compatible constructor (one that accepts a String)
        Constructor<? extends Throwable> constructor = exceptionClass.getConstructor(String.class);
        return constructor.newInstance(message);
    }

    @Test
    void testExceptionFilterMap() throws Exception {
        // Iterate over all exceptions
        for (Map.Entry<Class<? extends Throwable>, List<ThrowableFilter.ConditionalFilter>> entry :
                ThrowableFilter.EXCEPTION_FILTER_MAP.entrySet()) {
            Class<? extends Throwable> exceptionClass = entry.getKey();

            // Iterate over all log messages / exception messages
            for (ThrowableFilter.ConditionalFilter filter : entry.getValue()) {
                String logMessage = filter.logMessage();
                String exceptionMessage = filter.exceptionMessage();

                // Check without exception is not filtered
                assertSame(FilterReply.NEUTRAL,callDecide(logMessage, null));

                Throwable exception = createExceptionInstance(exceptionClass, exceptionMessage);
                // Check matching to EXCEPTION_FILTER_MAP is filtered out
                assertSame(FilterReply.DENY, callDecide(logMessage, exception));

                // Check not matching log messages to EXCEPTION_FILTER_MAP is NOT filtered
                assertSame(FilterReply.NEUTRAL, callDecide(logMessage + "foo", exception));

                exception = createExceptionInstance(exceptionClass, exceptionMessage + "foo");
                // Check not matching to EXCEPTION_FILTER_MAP is NOT filtered
                assertSame(FilterReply.NEUTRAL, callDecide(logMessage, exception));

                // Check not matching log messages to EXCEPTION_FILTER_MAP is NOT filtered
                assertSame(FilterReply.NEUTRAL, callDecide(logMessage + "foo", exception));
            }
        }
    }
}