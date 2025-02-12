package org.selfbus.updater.logging;

import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.spi.ThrowableProxy;
import tuwien.auto.calimero.KNXAckTimeoutException;
import tuwien.auto.calimero.KNXTimeoutException;

import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * The {@code ThrowableEvaluator} class extends {@link LevelEventEvaluator}
 * to provide a custom evaluation of logging events that filters out specific exceptions
 * with matching messages.
 *
 * <p>This evaluator is designed to exclude specific logging events from being processed,
 * based on their associated throwable (exception) type and message content. The
 * {@code exceptionFilterMap} maintains a mapping of throwable types and corresponding
 * messages that should be filtered.
 *
 * <p>Example usage:
 * <ul>
 *   <li>If the throwable is of type {@code KNXAckTimeoutException} and its message matches
 *   {@code "maximum send attempts, no service acknowledgment received"}, the event will
 *   be filtered out (ignored).</li>
 *   <li>If the throwable is of type {@code KNXTimeoutException} and its message matches
 *   {@code "no confirmation reply received for DM ..."}, the event will also be filtered out.</li>
 * </ul>
 *
 * <p>If no throwable is associated with the event or the throwable does not meet the
 * filtering criteria, the event is not filtered.
 *
 * <p>This can be useful in logging systems where specific repetitive or expected exceptions
 * (e.g., timeout exceptions) need to be ignored to reduce log noise.
 */
public class ThrowableEvaluator extends LevelEventEvaluator {

    /**
     * The {@code ConditionalFilter} record represents a pair of conditions used to filter
     * throwable logging events based on specific log messages and associated exception messages.
     *
     * @param logMessage       The log message as a filtering condition.
     * @param exceptionMessage The message of the exception that serves as a filtering condition.
     */
    public record ConditionalFilter(String logMessage, String exceptionMessage) {}

    /**
     * A customizable map that defines configurable filter criteria for throwable events.
     * <p>
     * Each entry in the map specifies a throwable class (the key) and a corresponding list
     * of {@link ConditionalFilter}s (the value). The key represents the type of exception,
     * while the value contains specific filtering conditions. A filtering condition is
     * defined by a combination of a log message and an exception message. Only events
     * containing a throwable that matches the exception type and both of its associated
     * filter conditions will be excluded.
     * </p>
     *
     * @see ConditionalFilter
     */
    public static final Map<Class<? extends Throwable>, List<ConditionalFilter>> EXCEPTION_FILTER_MAP = Map.of(
        KNXAckTimeoutException.class, List.of(
            // Loxone Miniserver Gen1 returns always status 0x04 on KNX tunnel requests with sequence number 255
            new ConditionalFilter(
                "close connection - maximum send attempts",
                "maximum send attempts, no service acknowledgment received"
            )
        ),
        KNXTimeoutException.class, List.of(
            // Loxone Miniserver Gen1 does not respond to
            // tuwien.auto.calimero.link.KNXNetworkLinkIP.configureWithServerSettings(..) which sends
            // M_PropRead.req OT=8 (cEMI Server), PID=51 (PID_MEDIUM_TYPE)
            new ConditionalFilter(
                "response timeout waiting for confirmation",
                "no confirmation reply received for DM prop-read.req objtype 8 instance 1 pid 51 start 1 elements 1"
            ),
            new ConditionalFilter(
                "skip link configuration (use defaults)",
                "no confirmation reply received for DM prop-read.req objtype 8 instance 1 pid 51 start 1 elements 1"
            ),

            // and M_PropRead.req OT=0 (Device), PID=56 (PID_MAX_APDULENGTH)
            new ConditionalFilter(
                "response timeout waiting for confirmation",
                "no confirmation reply received for DM prop-read.req objtype 0 instance 1 pid 56 start 1 elements 1"
            ),
            new ConditionalFilter(
                "skip link configuration (use defaults)",
                "no confirmation reply received for DM prop-read.req objtype 0 instance 1 pid 56 start 1 elements 1"
            )
        )
    );

    /**
     * Evaluates a logging event to determine whether it should be processed or filtered.
     *
     * @param event the {@link ILoggingEvent} instance to be evaluated.
     * @return {@code true} if the event should be processed; {@code false} if the event
     *         should be filtered out.
     * @throws NullPointerException if a null input is encountered during evaluation.
     *         This is unlikely since events usually undergo prior validation.
     */
    @Override
    public boolean evaluate(ILoggingEvent event) throws NullPointerException {
        if (super.evaluate(event)) {
            return true; // Ignore log level
        }

        ThrowableProxy throwableProxy = (ThrowableProxy) event.getThrowableProxy();
        if (throwableProxy == null) {
            return true; // no stacktrace present
        }

        String logMessage = event.getFormattedMessage();
        Throwable throwable = throwableProxy.getThrowable();
        // Check if the current exception is present in the filter map
        for (Map.Entry<Class<? extends Throwable>, List<ConditionalFilter>> exceptionFilter : EXCEPTION_FILTER_MAP.entrySet()) {
            if (exceptionFilter.getKey().isInstance(throwable)) {
                // Check if the exception message matches one of the filtered messages
                for (ConditionalFilter f : exceptionFilter.getValue()) {
                    // Check if logMessage and exception message match the filter
                    if ((Objects.equals(logMessage, f.logMessage())) &&
                        (Objects.equals(throwable.getMessage(), f.exceptionMessage()))) {
                        return false; // Filter this event out
                    }
                }
            }
        }
        return true;
    }

}
