package org.selfbus.updater;

import org.apache.commons.cli.ParseException;

public class CliInvalidException extends ParseException {
    public CliInvalidException(String message) {
        super(message);
    }

    public CliInvalidException(Throwable e) {
        super(e);
    }
}
