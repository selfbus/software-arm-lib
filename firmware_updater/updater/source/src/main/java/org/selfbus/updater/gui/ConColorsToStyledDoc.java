package org.selfbus.updater.gui;

import javax.swing.text.*;
import java.awt.*;
import java.util.Objects;

public class ConColorsToStyledDoc {

    private static StyleContext sc = new StyleContext();
    private static Style stringStyle = sc.getStyle(StyleContext.DEFAULT_STYLE);

    public static StyledDocument Convert(String docWithConColors, StyledDocument originDocument) throws BadLocationException {


        if(docWithConColors.contains("\033")) {
            String colorCode ="";

            int startPart = 0;

            int startIndex = docWithConColors.indexOf("\033");

            String[] splitDoc = docWithConColors.split("\033");

            // wenn der String vor der der ersten ConColor Zeichen hat, werden diese ohne Formatierung angehängt
            if(startIndex != 0){
                startPart = 1;
                originDocument.insertString(originDocument.getLength(), splitDoc[0], null);
            }

            for (int docPartCnt = startPart; docPartCnt < splitDoc.length; docPartCnt++) {
                String docPart = splitDoc[docPartCnt];
                if(!docPart.isEmpty()) {
                    for (int i = 2; i < 7; i++) {
                        String endChar = docPart.substring(i, i + 1);
                        if (endChar.equals("m")) {
                            colorCode = docPart.substring(1, i);
                            break;
                        }
                    }
                    String cleanedString = docPart.replace("[" + colorCode + "m", "");

                    Style partStyle = convertConColorToStyle(colorCode);

                    int oldDocLength = originDocument.getLength();

                    originDocument.insertString(originDocument.getLength(), cleanedString, null);

                    originDocument.setCharacterAttributes(oldDocLength, cleanedString.length(), partStyle,false);
                }
            }
        }
        else{
            // ohne ConColors im Datenstring
            originDocument.insertString(originDocument.getLength(), docWithConColors, null);
        }
        return originDocument;
    }

    private static Style convertConColorToStyle(String colorCode){
        // ColorCode kommt z.B. 0;30 oder 0;105 oder 41 oder 0 oder ...

        if(colorCode.contains(";")){
            // Regular Colors or High Intensity or High Intensity Background or Regular Bold or High Intensity Bold
            String[] colorCodeParts = colorCode.split(";");
            if(Objects.equals(colorCodeParts[0], "0")){ // Regular Colors or High Intensity or High Intensity Background
                if(Integer.parseInt(colorCodeParts[1]) < 100){
                    // Regular Colors or High Intensity
                    Color color = ConColorToColor(Integer.parseInt(colorCodeParts[1].substring(1,2)));
                    StyleConstants.setForeground(stringStyle, color);
                }else{
                    // High Intensity Background
                    Color color = ConColorToColor(Integer.parseInt(colorCodeParts[1].substring(2,3)));
                    StyleConstants.setBackground(stringStyle, color);
                }
            }else if(Objects.equals(colorCodeParts[0], "1")){
                // Regular Bold or High Intensity Bold
                Color color = ConColorToColor(Integer.parseInt(colorCodeParts[1].substring(1,2)));
                StyleConstants.setForeground(stringStyle, color);
                StyleConstants.setBold(stringStyle,true);
            }
        }else{
            // Regular Background or Reset
            if(colorCode.equals("0")){
                // Reset
                StyleConstants.setForeground(stringStyle, Color.black);
                StyleConstants.setBackground(stringStyle, Color.white);
                StyleConstants.setBold(stringStyle,false);
            }
            else{
                // Regular Background
                Color color = ConColorToColor(Integer.parseInt(colorCode.substring(1,2)));
                StyleConstants.setBackground(stringStyle, color);
            }
        }
        return stringStyle;
    }

    private static Color ConColorToColor(int colorNumber){
        Color retColor = null;

        switch(colorNumber) {
            case 0:
                retColor = Color.black;
                break;
            case 1:
                retColor = Color.red;
                break;
            case 2:
                retColor = Color.green;
                break;
            case 3:
                retColor = Color.orange;
                break;
            case 4:
                retColor = Color.blue;
                break;
            case 5:
                retColor = Color.pink;
                break;
            case 6:
                retColor = Color.cyan;
                break;
            case 7:
                retColor = Color.white;
        }
        return retColor;
    }
}
