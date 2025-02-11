package org.selfbus.updater.logging;

import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.spi.ThrowableProxy;
import tuwien.auto.calimero.KNXAckTimeoutException;
import tuwien.auto.calimero.KNXTimeoutException;

import java.util.List;
import java.util.Map;
import java.util.Objects;

public class ThrowableEvaluator extends LevelEventEvaluator {

    // Customizable Map of exception classes and corresponding messages to filter out
    Map<Class<? extends Throwable>, List<String>> exceptionFilterMap = Map.of(
            KNXAckTimeoutException.class, List.of(
                    "maximum send attempts, no service acknowledgment received"
            ),
            KNXTimeoutException.class, List.of(
                    "no confirmation reply received for DM prop-read.req objtype 8 instance 1 pid 51 start 1 elements 1"
            )
    );

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
