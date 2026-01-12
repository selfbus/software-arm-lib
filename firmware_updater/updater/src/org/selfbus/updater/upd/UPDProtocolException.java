package org.selfbus.updater.upd;

public class UPDProtocolException extends Exception {
    @SuppressWarnings("unused")
    private UPDProtocolException(Throwable e) {
        super(e);
    }

    public UPDProtocolException(String message) {
        super(message);
    }
}
