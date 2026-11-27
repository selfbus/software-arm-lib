package org.selfbus.updater.gui;

import javax.swing.JTextField;
import javax.swing.JComboBox;
import java.util.Objects;

public final class CliConverter {
    @SuppressWarnings("unused")
    private CliConverter() {}

    public static String argument(String cliLongOption, JTextField textField) {
        return buildArgument(cliLongOption, textField.getText(), true);
    }

    public static String argument(String cliLongOption, JComboBox<String> comboBox) {
        return buildArgument(cliLongOption, Objects.requireNonNull(comboBox.getSelectedItem()).toString(), true);
    }

    public static String argument(String cliLongOption) {
        return buildArgument(cliLongOption, "", false);
    }

    private static String buildArgument(String cliLongOption, String argValue, boolean argValueRequired) {
        cliLongOption = cliLongOption.trim();
        argValue = argValue.trim();

        if (argValueRequired && argValue.isEmpty()) {
            return ""; // Discard the argument if argValue is required, but not provided
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
