package org.selfbus.updater.logging;

import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.filter.Filter;
import ch.qos.logback.core.spi.FilterReply;
import org.slf4j.Marker;
import org.slf4j.MarkerFactory;

@SuppressWarnings("unused")
public class ExcludeMarkerFilter extends Filter<ILoggingEvent> {

    String marker;

    @Override
    public FilterReply decide(ILoggingEvent event) {
        Marker markerToMatch = MarkerFactory.getMarker(marker);
        if (event.getMarkerList() == null) {
            return FilterReply.NEUTRAL;
        }

        if (event.getMarkerList().contains(markerToMatch)) {
            return FilterReply.DENY;
        }
        else {
            return FilterReply.NEUTRAL;
        }
    }

    @SuppressWarnings("unused")
    public String getMarker() {
        return marker;
    }

    @SuppressWarnings("unused")
    public void setMarker(String marker) {
        this.marker = marker;
    }
}
