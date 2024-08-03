package org.selfbus.updater.gui;

import javax.swing.text.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ConColorsToStyledDoc {

    private static final Style stringStyle = new javax.swing.text.StyleContext().addStyle("testStyle", null);

    public static final java.awt.Color DefaultForegroundColor = java.awt.Color.white;
    public static final java.awt.Color DefaultBackgroundColor = java.awt.Color.black;
    private static final String RegExAnsi = "\033\\[([;\\d]*)m"; // Regular expression to match ANSI escape sequences
    private static final String AnsiCodeSeparator = ";";

    private static void insertStyledString(String toInsert, String colorCode, StyledDocument document) throws BadLocationException {
        Style newStyle = colorCodeToStyle(colorCode.split(AnsiCodeSeparator));
        document.insertString(document.getLength(), toInsert, newStyle);
    }

    /*
     * hier werden die Steuerzeichen für die Farben der Windows Konsole in Java Parameter gewandelt
     * z.B. wird ein String "\033[0;31m dieser Text wird rot sein,\033[44m jetzt auf blauem Hintergrund" übergeben
     * Dieser String wird entsprechend der Steuerbefehle in ein StyledDocument gefüllt
     */
    public static void Convert(String input, StyledDocument originDocument) throws BadLocationException {
        Pattern pattern = Pattern.compile(RegExAnsi);
        Matcher matcher = pattern.matcher(input);

        String cleanedString = "";
        String colorCode = "";
        int index = 0;
        while (matcher.find()) {
            if (matcher.start() > index) { // possible text before the ANSI escape sequence
                cleanedString = input.substring(index, matcher.start());
                insertStyledString(cleanedString, colorCode, originDocument);
            }
            colorCode = matcher.group(1);
            index = matcher.end();
        }

        if (index < input.length()) { // possible remaining text after the last ANSI escape sequence
            cleanedString = input.substring(index);
            insertStyledString(cleanedString, colorCode, originDocument);
        }
    }

    private static void resetStringStyle() {
        StyleConstants.setForeground(ConColorsToStyledDoc.stringStyle, DefaultForegroundColor);
        StyleConstants.setBackground(ConColorsToStyledDoc.stringStyle, DefaultBackgroundColor);
        StyleConstants.setBold(ConColorsToStyledDoc.stringStyle,false);
        StyleConstants.setItalic(ConColorsToStyledDoc.stringStyle,false);
        StyleConstants.setUnderline(ConColorsToStyledDoc.stringStyle,false);
        StyleConstants.setStrikeThrough(ConColorsToStyledDoc.stringStyle,false);
    }

    private static Style colorCodeToStyle(String [] colorCodes) {
        for (String code : colorCodes) {
            if (code.isEmpty()) {
                resetStringStyle();
                continue;
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
                    throw new IllegalStateException("Extended foreground colors not implemented");
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
                    throw new IllegalStateException("Extended background colors not implemented");
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

    public static Style testColorCodeToStyle(String [] colorCodes) {
        return colorCodeToStyle(colorCodes);
    }

    public static Style testStringStyle() {
       return stringStyle;
    }
}
