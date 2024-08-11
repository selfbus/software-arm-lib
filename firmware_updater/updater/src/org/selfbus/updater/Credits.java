package org.selfbus.updater;

public final class Credits {
    @SuppressWarnings("unused")
    private Credits() {}

    private static final String[] authors = {
            "Deti Fliegl",
            "Pavel Kriz",
            "Dr. Stefan Haller",
            "Oliver Stefan",
            "et al.",
    };

    @SuppressWarnings("SameReturnValue")
    public static String getAsciiLogo() {
        return """
                  _____ ________    __________  __  _______    __  ______  ____  ___  ________________
                 / ___// ____/ /   / ____/ __ )/ / / / ___/   / / / / __ \\/ __ \\/   |/_  __/ ____/ __ \\
                 \\__ \\/ __/ / /   / /_  / __  / / / /\\__ \\   / / / / /_/ / / / / /| | / / / __/ / /_/ /
                ___/ / /___/ /___/ __/ / /_/ / /_/ /___/ /  / /_/ / ____/ /_/ / ___ |/ / / /___/ _, _/
               /____/_____/_____/_/   /_____/\\____//____/   \\____/_/   /_____/_/  |_/_/ /_____/_/ |_|""";
    }

    public static String getAuthors() {
        StringBuilder result = new StringBuilder("by ");
        for (int i = 0; i < authors.length; i++) {
            result.append(authors[i]);
            if (i < authors.length - 1) {
                result.append(", ");
            }
        }
        return result.toString();
    }
}
