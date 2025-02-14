package org.selfbus.updater;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.selfbus.updater.progress.SpinningCursor;

import static org.junit.jupiter.api.Assertions.*;

public class SpinningCursorTest {

    @BeforeEach
    void setUp() {
        SpinningCursor.reset();
    }

    @Test
    public void testInitialCursorState() {
        // Ensure the initial state is the first character in the cursor array
        assertEquals('|', SpinningCursor.getNext());
    }

    @Test
    public void testCursorRotation() {
        // Simulate rotating through the cursor characters
        assertEquals('|', SpinningCursor.getNext());
        assertEquals('/', SpinningCursor.getNext());
        assertEquals('-', SpinningCursor.getNext());
        assertEquals('\\', SpinningCursor.getNext());
        // After the last character, it should reset and start over
        assertEquals('|', SpinningCursor.getNext());
    }

    @Test
    public void testCursorReset() {
        SpinningCursor.getNext(); // Move to the first character
        SpinningCursor.reset(); // Reset to initial state
        assertEquals('|', SpinningCursor.getNext()); // Should still be at the beginning
    }

    @Test
    public void testCursorIndexWrapping() {
        // Rotate through all characters and check wrapping behavior
        for (int i = 0; i < 100; i++) {
            SpinningCursor.getNext();
        }
        assertEquals('|', SpinningCursor.getNext());
    }

    @Test
    public void testCursorSetBlank() {
        SpinningCursor.getNext();
        SpinningCursor.setBlank();
        assertEquals(' ', SpinningCursor.getNext());
        assertEquals('|', SpinningCursor.getNext());
        SpinningCursor.reset();
        assertEquals('|', SpinningCursor.getNext());
    }
}

