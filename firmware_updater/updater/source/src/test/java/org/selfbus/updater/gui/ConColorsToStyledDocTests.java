package org.selfbus.updater.gui;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;
import static org.selfbus.updater.gui.ConColorsToStyledDoc.*;

import javax.swing.text.*;
import java.awt.*;

public class ConColorsToStyledDocTests {

    @BeforeEach
    public void setup() {
        shuffleStyle(false);
    }

    private void assertStyleForeground(java.awt.Color expectedColor) {
        assertEquals(expectedColor, StyleConstants.getForeground(testStringStyle()));
    }

    private void assertStyleBackground(java.awt.Color expectedColor) {
        assertEquals(expectedColor, StyleConstants.getBackground(testStringStyle()));
    }

    private void assertStyleBold(boolean expectedBold) {
        assertEquals(expectedBold, StyleConstants.isBold(testStringStyle()));
    }

    private void assertStyleItalic(boolean expectedItalic) {
        assertEquals(expectedItalic, StyleConstants.isItalic(testStringStyle()));
    }

    private void assertStyleUnderline(boolean expectedUnderline) {
        assertEquals(expectedUnderline, StyleConstants.isUnderline(testStringStyle()));
    }

    private void assertStyleStrikeThrough(boolean expectedStrikeThrough) {
        assertEquals(expectedStrikeThrough, StyleConstants.isStrikeThrough(testStringStyle()));
    }

    private void shuffleStyle(boolean setAttribute) {
        // fore and background intentionally swapped
        StyleConstants.setForeground(testStringStyle(), DefaultBackgroundColor);
        StyleConstants.setBackground(testStringStyle(), DefaultForegroundColor);
        StyleConstants.setBold(testStringStyle(),setAttribute);
        StyleConstants.setItalic(testStringStyle(),setAttribute);
        StyleConstants.setUnderline(testStringStyle(),setAttribute);
        StyleConstants.setStrikeThrough(testStringStyle(),setAttribute);
    }

    @Test
    public void testJansiReset() {
        shuffleStyle(true);
        testColorCodeToStyle(new String[]{""});
        assertStyleForeground(DefaultForegroundColor);
        assertStyleBackground(DefaultBackgroundColor);
        assertStyleBold(false);
        assertStyleItalic(false);
        assertStyleUnderline(false);
        assertStyleStrikeThrough(false);
    }

    @Test
    public void testStandardReset() {
        shuffleStyle(true);
        testColorCodeToStyle(new String[]{"0"});
        // Assuming empty string should reset the style
        assertStyleForeground(DefaultForegroundColor);
        assertStyleBackground(DefaultBackgroundColor);
        assertStyleBold(false);
        assertStyleItalic(false);
        assertStyleUnderline(false);
        assertStyleStrikeThrough(false);
    }

    @Test
    public void testForegroundColors() {
        java.awt.Color backgroundColor = StyleConstants.getBackground(testStringStyle());
        testColorCodeToStyle(new String[]{"30"});
        assertStyleForeground(Color.black);
        assertStyleBackground(backgroundColor);

        testColorCodeToStyle(new String[]{"1", "31"});
        assertStyleForeground(Color.red);
        assertStyleBackground(backgroundColor);

        testColorCodeToStyle(new String[]{"2", "32", "3"});
        assertStyleForeground(Color.green);
        assertStyleBackground(backgroundColor);
    }

    @Test
    public void testBackgroundColors() {
        java.awt.Color foregroundColor = StyleConstants.getForeground(testStringStyle());
        testColorCodeToStyle(new String[]{"1", "40"});
        assertStyleBackground(java.awt.Color.black);
        assertStyleForeground(foregroundColor);

        testColorCodeToStyle(new String[]{"2", "41"});
        assertStyleBackground(java.awt.Color.red);
        assertStyleForeground(foregroundColor);

        testColorCodeToStyle(new String[]{"42", "3"});
        assertStyleBackground(java.awt.Color.green);
        assertStyleForeground(foregroundColor);

        testColorCodeToStyle(new String[]{"43"});
        assertStyleBackground(java.awt.Color.orange);
        assertStyleForeground(foregroundColor);
    }

    @Test
    public void testBoldOn() {
        shuffleStyle(false);
        testColorCodeToStyle(new String[]{"1"});
        assertStyleBold(true);
    }

    @Test
    public void testBoldOff() {
        shuffleStyle(true);
        testColorCodeToStyle(new String[]{"2"});
        assertStyleBold(false);
    }

    @Test
    public void testItalicOn() {
        shuffleStyle(false);
        testColorCodeToStyle(new String[]{"3"});
        assertStyleItalic(true);
    }

    @Test
    public void testItalicOff() {
        shuffleStyle(true);
        testColorCodeToStyle(new String[]{"23"});
        assertStyleItalic(false);
    }

    @Test
    public void testUnderlineOn() {
        shuffleStyle(false);
        testColorCodeToStyle(new String[]{"4"});
        assertStyleUnderline(true);
    }

    @Test
    public void testUnderlineOff() {
        shuffleStyle(false);
        testColorCodeToStyle(new String[]{"24"});
        assertStyleUnderline(false);
    }

    @Test
    public void testStrikeOn() {
        shuffleStyle(false);
        testColorCodeToStyle(new String[]{"9"});
        assertStyleStrikeThrough(true);
    }

    @Test
    public void testStrikeOff() {
        shuffleStyle(false);
        testColorCodeToStyle(new String[]{"29"});
        assertStyleStrikeThrough(false);
    }

    @Test
    public void testExceptionOnInvalidCode() {
        assertThrows(IllegalStateException.class, () -> {
            testColorCodeToStyle(new String[]{"999"});
        });
    }

    @Test
    public void testExceptionOnExtendedColors() {
        assertThrows(IllegalStateException.class, () -> {
            testColorCodeToStyle(new String[]{"0","38"});
        });
        assertThrows(IllegalStateException.class, () -> {
            testColorCodeToStyle(new String[]{"1", "48"});
        });
    }

    @Test
    public void testConvert() {
        StyledDocument document = new DefaultStyledDocument();
        try {
            //todo add more testcases
            String[][] testCases = {
                    {"\033[mreset_jansi\033[m", "reset_jansi"}, // reset like jansi it sends
                    {"\033[0mreset_standard\033[0m", "reset_standard"}, // Ansi default reset
                    {"normal_start\033[1;32mboldGreen\033[0mnormal_end", "normal_startboldGreennormal_end"},
                    {"Device 1.1.32 reported \033[92m1\033[m second(s) for restarting", "Device 1.1.32 reported 1 second(s) for restarting"}
            };

            for (String[] underTest : testCases) {
                document.remove(0, document.getLength());
                ConColorsToStyledDoc.Convert(underTest[0], document);
                //todo check also the style and not only the text
                assertEquals(underTest[1], document.getText(0, document.getLength()));
            }
        } catch (BadLocationException e) {
            throw new RuntimeException(e);
        }
    }
}
