package org.selfbus.updater.logging;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.boolex.EventEvaluatorBase;

import static ch.qos.logback.classic.Level.TRACE;

public class LevelEventEvaluator extends EventEvaluatorBase<ILoggingEvent> {

    private Level level = TRACE;

    @Override
    public boolean evaluate(ILoggingEvent event) throws NullPointerException {
        Level eventLevel = event.getLevel();
        return !eventLevel.isGreaterOrEqual(getLevel());
    }

    @SuppressWarnings("unused")
    public Level getLevel() {
        return level;
    }

    @SuppressWarnings("unused")
    public void setLevel(Level level) {
        this.level = level;
    }
}
