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
     * A customizable map that defines the filter configuration for throwable events.
     * <p>Each entry in the map associates a specific exception class (key) with
     * a list of specific error messages (value). If an exception of a matching type
     * and message is encountered during evaluation, the event may be filtered out.
     */
    final Map<Class<? extends Throwable>, List<String>> exceptionFilterMap = Map.of(
            KNXAckTimeoutException.class, List.of(
                    // Loxone Miniserver Gen1 returns always status 0x04 on KNX tunnel requests with sequence number 255
                    "maximum send attempts, no service acknowledgment received"
            ),
            KNXTimeoutException.class, List.of(
                    "no confirmation reply received for DM prop-read.req objtype 8 instance 1 pid 51 start 1 elements 1"
                    // Loxone Miniserver Gen1 does not respond to
                    // tuwien.auto.calimero.link.KNXNetworkLinkIP.configureWithServerSettings(..) which sends
                    // M_PropRead.req OT=8 (cEMI Server), PID=51 (PID_MEDIUM_TYPE)
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

        Throwable throwable = throwableProxy.getThrowable();
        // Check if the current exception is present in the filter map
        for (Map.Entry<Class<? extends Throwable>, List<String>> exceptionFilter : exceptionFilterMap.entrySet()) {
            if (exceptionFilter.getKey().isInstance(throwable)) {
                // Check if the exception message matches one of the filtered messages
                for (String filteredMessage : exceptionFilter.getValue()) {
                    if (Objects.equals(throwable.getMessage(), filteredMessage)) {
                        return false; // Filter this event out
                    }
                }
            }
        }
        return true;
    }

}
