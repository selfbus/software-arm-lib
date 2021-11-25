package org.hkfree.knxduino.updater;

public class UpdaterException extends Exception {
    /**
     * Constructs a new <code>UpdaterException</code> without a detail message.
     */
    public UpdaterException()
    {}

    /**
     * Constructs a new <code>UpdaterException</code> with the specified detail message.
     * <p>
     *
     * @param s the detail message
     */
    public UpdaterException(final String s)
    {
        super(s);
    }

    /**
     * Constructs a new <code>UpdaterException</code> with the specified detail message and
     * cause.
     * <p>
     *
     * @param s the detail message
     * @param cause the cause in form of a throwable object, can be <code>null</code>
     */
    public UpdaterException(final String s, final Throwable cause)
    {
        super(s, cause);
    }
}