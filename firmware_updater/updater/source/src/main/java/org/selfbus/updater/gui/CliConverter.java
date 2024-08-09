package org.selfbus.updater.gui;

import javax.swing.JTextField;
import javax.swing.JComboBox;
import java.util.ArrayList;
import java.util.Objects;

public final class CliConverter {
    @SuppressWarnings("unused")
    CliConverter() {}

    public static String argument(String cliLongOption, JTextField textField) {
        return argument(cliLongOption, textField.getText(), true);
    }

    public static String argument(String cliLongOption, JComboBox<String> comboBox) {
        return argument(cliLongOption, Objects.requireNonNull(comboBox.getSelectedItem()).toString(), true);
    }

    public static String argument(String cliLongOption) {
        return argument(cliLongOption, "", false);
    }

    public static String toString(ArrayList<String> list) {
        StringBuilder result = new StringBuilder();
        for (String s : list) {
            if (!s.isEmpty()) {
                result.append(" ").append(s);
            }
        }
        return result.toString().trim();
    }

    public static String argument(String cliLongOption, String argValue, boolean argValueRequired) {
        cliLongOption = cliLongOption.trim();
        argValue = argValue.trim();

        if ((argValueRequired) && (argValue.isEmpty())) {
            return ""; //todo maybe throw an exception in this case?
        }

        if (argValue.isEmpty()) {
            return String.format("--%s", cliLongOption);
        }

        if (cliLongOption.isEmpty()) {
            return argValue;
        }

        return (String.format("--%s=%s", cliLongOption, argValue));
    }
}
