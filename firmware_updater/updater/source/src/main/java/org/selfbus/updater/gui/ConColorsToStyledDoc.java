package org.selfbus.updater.gui;

import org.selfbus.updater.AnsiCursor;

import javax.swing.*;
import javax.swing.text.*;
import java.awt.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static org.fusesource.jansi.Ansi.ansi;

public final class ConColorsToStyledDoc {
    @SuppressWarnings("unused")
    private ConColorsToStyledDoc() {}  // avoids instance creation

    private static final Style stringStyle = new javax.swing.text.StyleContext().addStyle("testStyle", null);

    public static final java.awt.Color DefaultForegroundColor = java.awt.Color.white;
    public static final java.awt.Color DefaultBackgroundColor = java.awt.Color.black;
    private static final String AnsiEscape = "\033"; // ASCII escape character (ESC)
    private static final String AnsiBracket = "[";   // Ansi second character [

    /**
     * Regular expression for matching ANSI escape sequences.
     * <p>
     * Includes sequences for text styles, colors and some cursor movements.
     * <p>
     * <ul>
     *   <li>{@code ([;\\d*)[A-Hmsu]} - Matches a sequence of zero or more digits and semicolons
     *   followed by a character indicating the type of ANSI control or formatting action.
     *       <ul>
     *         <li>A-H - Cursor movement</li>
     *         <li>m   - Set Graphics Mode (color, bold, underline, etc.)</li>
     *         <li>s   - Save Cursor Position</li>
     *         <li>u   - Restore Cursor Position</li>
     *       </ul>
     *   </li>
     *   <li>{@code (\\?25[hl])} - Cursor on/off</li>
     * </ul>
     * <p>
     * <p>
     * Great overview on ANSI escape codes and their usage:
     * <a href="https://gist.github.com/dominikwilkowski/60eed2ea722183769d586c76f22098dd#colors">ANSI escape codes</a>
     * </p>
     */
    private static final String RegExAnsi = AnsiEscape + "\\" + AnsiBracket + "(([;\\d]*)[A-Hmsu]|(\\?25[hl]))";
    private static final String RegExAnsiCursor = AnsiEscape + "\\" + AnsiBracket + "(([;\\d]*)[A-Hsu])";
    private static final String AnsiCodeSeparator = ";";

    private static void insertStyledString(String toInsert, String colorCode, JTextPane textPane) throws BadLocationException {
        Style newStyle = colorCodeToStyle(colorCode.split(AnsiCodeSeparator));
        StyledDocument document = (StyledDocument) textPane.getDocument();
        document.insertString(document.getLength(), toInsert, newStyle);
        textPane.setCaretPosition(document.getLength());
    }

    /*
     * hier werden die Steuerzeichen für die Farben der Windows Konsole in Java Parameter gewandelt
     * z.B. wird ein String "\033[0;31m dieser Text wird rot sein,\033[44m jetzt auf blauem Hintergrund" übergeben
     * Dieser String wird entsprechend der Steuerbefehle in ein StyledDocument gefüllt
     */
    public static void Convert(String input, JTextPane textPane) throws BadLocationException {
        Pattern pattern = Pattern.compile(RegExAnsi);
        Matcher matcher = pattern.matcher(input);

        String cleanedString = "";
        String ansiCode = "";
        int index = 0;
        while (matcher.find()) {
            if (matcher.start() > index) { // possible text before the ANSI escape sequence
                cleanedString = input.substring(index, matcher.start());
                insertStyledString(cleanedString, ansiCode, textPane);
            }
            ansiCode = matcher.group();

            if (processAnsiCursor(ansiCode, textPane)) {
                index = matcher.end();
                ansiCode = "";
                continue;
            }

            ansiCode = ansiCode.replace(AnsiEscape + AnsiBracket, "");
            index = matcher.end();
        }

        if (index < input.length()) { // possible remaining text after the last ANSI escape sequence
            cleanedString = input.substring(index);
            insertStyledString(cleanedString, ansiCode, textPane);
        }

        // immer die letzte Zeile zeigen
        textPane.setCaretPosition(textPane.getDocument().getLength());
    }

    /**
     * Processes ANSI cursor control codes and updates the cursor position in the given {@link JTextPane}.
     *
     * <p><strong>Warning:</strong> This method is partially implemented. Some cursor movement codes are not yet handled and will throw an exception if encountered.</p>     *
     * <p>When encountering an unsupported ANSI cursor movement code, an {@link IllegalStateException} is thrown.</p>
     *
     * @param ansiCode the ANSI code to process
     * @param textPane the {@link JTextPane} whose cursor position will be updated
     * @return {@code true} if the ANSI code was successfully processed; {@code false} otherwise
     * @throws BadLocationException if an error occurs when updating the cursor position
     * @throws IllegalStateException if an not implemented or unexpected ANSI code is encountered
     */
    private static boolean processAnsiCursor(String ansiCode, JTextPane textPane) throws BadLocationException {
        if (ansiCode.equals(AnsiCursor.off()) || ansiCode.equals(AnsiCursor.on())) {
            return true; // there is no visible cursor in the JTextPane
        }

        if (ansiCode.equals(ansi().restoreCursorPosition().toString()) ||
            ansiCode.equals(ansi().saveCursorPosition().toString())) {
            //todo implement save and restore cursor position
            throw new IllegalStateException("Save/Restore cursor position not implemented: " + ansiCode);
        }

        Pattern pattern = Pattern.compile(RegExAnsiCursor);
        Matcher matcher = pattern.matcher(ansiCode);
        String cursorMovement = "";
        while (matcher.find()) {
            cursorMovement = matcher.group(1);
            if (cursorMovement.isEmpty()) {
                continue;
            }

            Point moveCursor = getCursorPosition(textPane);
            int cursorMovementCode = cursorMovement.charAt(cursorMovement.length() - 1);
            int number;
            if (cursorMovement.length() > 1) {
                number = Integer.parseInt(cursorMovement.substring(0, cursorMovement.length() - 1));
            }
            else {
                number = 1;
            }

            switch (cursorMovementCode) {
                //todo implement remaining cursor movements
                case 65: // A Move up by n rows
                case 66: // B Move down by n rows
                case 67: // C Move right by n columns
                case 68: // D Move left by n columns
                case 72: // H Move cursor to row n and column m
                    throw new IllegalStateException("Not implemented cursorMovement: " + cursorMovement);

                case 69: // E Move cursor to beginning of line and n lines down
                    moveCursor.x = 1;
                    moveCursor.y += number;
                    setCursorPosition(textPane, moveCursor);
                    return true;

                case 70: // F Move cursor to beginning of line and n lines up
                    moveCursor.x = 1;
                    moveCursor.y -= number;
                    setCursorPosition(textPane, moveCursor);
                    return true;

                case 71: // G Move cursor to column n
                    moveCursor.x = number;
                    setCursorPosition(textPane, moveCursor);
                    return true;

                default:
                    throw new IllegalStateException("Unexpected cursorMovement: " + cursorMovement);
            }
        }
        return false;
    }

    /**
     * Resets the styling attributes of the {@link #stringStyle}
     */
    private static void resetStringStyle() {
        StyleConstants.setForeground(ConColorsToStyledDoc.stringStyle, DefaultForegroundColor);
        StyleConstants.setBackground(ConColorsToStyledDoc.stringStyle, DefaultBackgroundColor);
        StyleConstants.setBold(ConColorsToStyledDoc.stringStyle,false);
        StyleConstants.setItalic(ConColorsToStyledDoc.stringStyle,false);
        StyleConstants.setUnderline(ConColorsToStyledDoc.stringStyle,false);
        StyleConstants.setStrikeThrough(ConColorsToStyledDoc.stringStyle,false);
    }

    /**
     * Converts an array of color codes to a {@link Style} with the corresponding styles applied.
     * <p>
     * This method parses ANSI color codes and applies the styles to the {@link #stringStyle}, such as
     * setting text to bold, italic, underlined, strikethrough, and changing foreground and background colors.
     * </p>
     *
     * @param colorCodes an array of ANSI color codes as {@link String} values
     * @return the {@link #stringStyle} object with the styles applied
     * @throws IllegalStateException if an 256 fore/background color code (38 or 48) or an unexpected code value is encountered
     */
    private static Style colorCodeToStyle(String [] colorCodes) {
        for (String code : colorCodes) {
            if (code.isEmpty() || code.equals("m")) {
                resetStringStyle();
                continue;
            }

            if (code.charAt(code.length() - 1) == 'm') {
                code = code.substring(0, code.length() - 1); // strip of 'm'
            }

            switch (Integer.parseInt(code)) {
                case 0: // Reset
                    resetStringStyle();
                    break;
                case 1: // Bold on
                    StyleConstants.setBold(stringStyle,true);
                    break;
                case 2: // Bold off
                    StyleConstants.setBold(stringStyle,false);
                    break;
                case 3: // Italic on
                    StyleConstants.setItalic(stringStyle,true);
                    break;
                case 4: // Underline on
                    StyleConstants.setUnderline(stringStyle,true);
                    break;
                //
                // 5-8 reserved
                //
                case 9: // Strike on
                    StyleConstants.setStrikeThrough(stringStyle,true);
                    break;
                //
                // 10-20 reserved
                //
                case 21: // Bold off (same as 2)
                    StyleConstants.setBold(stringStyle,false);
                    break;
                // 22 reserved
                case 23: // Italic off
                    StyleConstants.setItalic(stringStyle,false);
                    break;
                case 24: // Underline off
                    StyleConstants.setUnderline(stringStyle,false);
                    break;
                // 25-28 reserved
                case 29: // Strike off
                    StyleConstants.setStrikeThrough(stringStyle,false);
                    break;
                //
                // Foreground colors
                //
                case 30: // Foreground color black
                    StyleConstants.setForeground(stringStyle, java.awt.Color.black);
                    break;
                case 31: // Foreground color red
                    StyleConstants.setForeground(stringStyle, java.awt.Color.red);
                    break;
                case 32: // Foreground color green
                    StyleConstants.setForeground(stringStyle, java.awt.Color.green);
                    break;
                case 33: // Foreground color yellow
                    // java.awt.Color.yellow would be correct, but java.awt.Color.orange just looks better IMHO
                    StyleConstants.setForeground(stringStyle, java.awt.Color.orange);
                    break;
                case 34: // Foreground color blue
                    StyleConstants.setForeground(stringStyle, java.awt.Color.blue);
                    break;
                case 35: // Foreground color magenta
                    //todo check which one looks better: java.awt.Color.magenta or java.awt.Color.pink?
                    StyleConstants.setForeground(stringStyle, java.awt.Color.pink);
                    break;
                case 36: // Foreground color cyan
                    StyleConstants.setForeground(stringStyle, java.awt.Color.cyan);
                    break;
                case 37: // Foreground color white
                    StyleConstants.setForeground(stringStyle, java.awt.Color.white);
                    break;
                case 38:
                    throw new IllegalStateException("256 color foreground not implemented");
                case 39: // Foreground color default
                    StyleConstants.setForeground(stringStyle, DefaultForegroundColor);
                    break;
                //
                // Background colors
                //
                case 40: // Background color black
                    StyleConstants.setBackground(stringStyle, java.awt.Color.black);
                    break;
                case 41: // Background color red
                    StyleConstants.setBackground(stringStyle, java.awt.Color.red);
                    break;
                case 42: // Background color green
                    StyleConstants.setBackground(stringStyle, java.awt.Color.green);
                    break;
                case 43: // Background color yellow
                    // java.awt.Color.yellow would be correct, but java.awt.Color.orange just looks better IMHO
                    StyleConstants.setBackground(stringStyle, java.awt.Color.orange);
                    break;
                case 44: // Background color blue
                    StyleConstants.setBackground(stringStyle, java.awt.Color.blue);
                    break;
                case 45: // Background color magenta
                    //todo check which one looks better: java.awt.Color.magenta or java.awt.Color.pink?
                    StyleConstants.setBackground(stringStyle, java.awt.Color.pink);
                    break;
                case 46: // Background color cyan
                    StyleConstants.setBackground(stringStyle, java.awt.Color.cyan);
                    break;
                case 47: // Background color white
                    StyleConstants.setBackground(stringStyle, java.awt.Color.white);
                    break;
                case 48:
                    throw new IllegalStateException("256 color background not implemented");
                case 49: // Background color default
                    StyleConstants.setBackground(stringStyle, DefaultBackgroundColor);
                    break;
                //
                // 50-89 reserved
                //

                //
                // Bright foreground colors
                //
                //todo find better java.awt.Color values, right now, they are the same as "normal"
                case 90: // Bright foreground color black
                    StyleConstants.setForeground(stringStyle, java.awt.Color.black);
                    break;
                case 91: // Bright foreground color red
                    StyleConstants.setForeground(stringStyle, java.awt.Color.red);
                    break;
                case 92: // Bright foreground color green
                    StyleConstants.setForeground(stringStyle, java.awt.Color.green);
                    break;
                case 93: // Bright foreground color yellow
                    // java.awt.Color.yellow would be correct, but java.awt.Color.orange just looks better IMHO
                    StyleConstants.setForeground(stringStyle, java.awt.Color.orange);
                    break;
                case 94: // Bright foreground color blue
                    StyleConstants.setForeground(stringStyle, java.awt.Color.blue);
                    break;
                case 95: // Bright foreground color magenta
                    //todo check which one looks better: java.awt.Color.magenta or java.awt.Color.pink?
                    StyleConstants.setForeground(stringStyle, java.awt.Color.pink);
                    break;
                case 96: // Bright foreground color cyan
                    StyleConstants.setForeground(stringStyle, java.awt.Color.cyan);
                    break;
                case 97: // Bright foreground color white
                    StyleConstants.setForeground(stringStyle, java.awt.Color.white);
                    break;
                //
                // 98-99 reserved
                //

                //
                // Bright background colors
                //
                //todo find better java.awt.Color values, right now, they are the same as "normal"
                case 100: // Bright background color black
                    StyleConstants.setBackground(stringStyle, java.awt.Color.black);
                    break;
                case 101: // Bright background color red
                    StyleConstants.setBackground(stringStyle, java.awt.Color.red);
                    break;
                case 102: // Bright background color green
                    StyleConstants.setBackground(stringStyle, java.awt.Color.green);
                    break;
                case 103: // Bright background color yellow
                    // java.awt.Color.yellow would be correct, but java.awt.Color.orange just looks better IMHO
                    StyleConstants.setBackground(stringStyle, java.awt.Color.orange);
                    break;
                case 104: // Bright background color blue
                    StyleConstants.setBackground(stringStyle, java.awt.Color.blue);
                    break;
                case 105: // Bright background color magenta
                    //todo check which one looks better: java.awt.Color.magenta or java.awt.Color.pink?
                    StyleConstants.setBackground(stringStyle, java.awt.Color.pink);
                    break;
                case 106: // Bright background color cyan
                    StyleConstants.setBackground(stringStyle, java.awt.Color.cyan);
                    break;
                case 107: // Bright background color white
                    StyleConstants.setBackground(stringStyle, java.awt.Color.white);
                    break;
                default:
                    throw new IllegalStateException("Unexpected code value: " + Integer.parseInt(code));
            }
        }
        return stringStyle;
    }

    /**
     * Returns the current virtual cursor position within the given {@link JTextPane} as a {@link Point}.
     * The position is represented as (column, row) with 1-based indexing like in consoles.
     *
     * @param textPane the {@link JTextPane} from which to retrieve the cursor position
     * @return a {@link Point} representing the cursor position, where the x-coordinate is the column
     *         number and the y-coordinate is the row number, both 1-based
     */
    private static Point getCursorPosition(JTextPane textPane) {
        int caretPosition = textPane.getCaretPosition();
        // Get the row at the caret position
        Element docRoot = textPane.getStyledDocument().getDefaultRootElement();
        int row = docRoot.getElementIndex(caretPosition);

        // Get the start offset of the column
        Element lineElement = docRoot.getElement(row);
        int lineStartOffset = lineElement.getStartOffset();
        int column = caretPosition - lineStartOffset;
        return new Point(column + 1, row + 1); // 1-based indexing for ANSI console
    }

    /**
     * Sets the virtual cursor position in the specified {@link JTextPane} to the given {@link Point}.
     * The {@link Point} specifies the row and column (1-based index).
     *
     * @param textPane The {@link JTextPane} in which to set the cursor position.
     * @param newPosition A {@link Point} where the x-coordinate represents the column
     *                    and the y-coordinate represents the row. Both coordinates are 1-based.
     * @throws BadLocationException If the specified position is invalid within the document.
     */
    public static void setCursorPosition(JTextPane textPane, Point newPosition) throws BadLocationException {
        StyledDocument doc = textPane.getStyledDocument();
        int rowIndex = Math.max(0, newPosition.y - 1); // rowIndex starts from 0
        int columnIndex = Math.max(0, newPosition.x - 1); // columnIndex starts from 0

        // Get the line start offset
        int lineStartOffset = doc.getDefaultRootElement().getElement(rowIndex).getStartOffset();

        // Calculate the new caret position
        int newCaretPosition = lineStartOffset + columnIndex;

        if (newCaretPosition < 0) {
            newCaretPosition = 0;
        }

        if (newCaretPosition > doc.getLength()) {
            newCaretPosition = doc.getLength();
        }

        StyledDocument document = (StyledDocument) textPane.getDocument();
        int currentLength = document.getLength();
        // Set the caret position
        if (newCaretPosition < currentLength) {
            document.remove(newCaretPosition, currentLength - newCaretPosition);
        }

        textPane.setCaretPosition(document.getLength());
    }

    /**
     * This method is intended for unit tests only.
     * It invokes the {@link #colorCodeToStyle} method with the provided color codes.
     *
     * @param colorCodes an array of color codes to be converted to styles.
     * @warning This method is intended for unit tests only.
     */
    public static void testColorCodeToStyle(String [] colorCodes) {
        colorCodeToStyle(colorCodes);
    }

    /**
     * This method is intended for unit tests only.
     * It returns the current {@link #stringStyle}.
     *
     * @return the current string style.
     * @warning This method is intended for unit tests only.
     */
    public static Style testStringStyle() {
       return stringStyle;
    }
}
