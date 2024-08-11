package org.selfbus.updater.upd;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static org.fusesource.jansi.Ansi.*;
import static org.fusesource.jansi.Ansi.Color.*;

/**
 * Implementation of the UPD/UDP protocol handling
 */
public final class UPDProtocol {
    @SuppressWarnings("unused")
    private UPDProtocol() {}

    private static final Logger logger = LoggerFactory.getLogger(UPDProtocol.class);

    private static final int COMMAND_POSITION = 2;
    public static final int DATA_POSITION = 3;
    /**
     * uid/guid length of the mcu used for unlocking/flashing.
     */
    public static final int UID_LENGTH_USED = 12;
    /**
     * uid/guid length of the mcu.
     */
    public static final int UID_LENGTH_MAX = 16;

    public static UDPResult checkResult(byte[] result) {
        return checkResult(result, true);
    }

    public static UDPResult checkResult(byte[] result, boolean verbose) {
        UPDCommand command = UPDCommand.valueOf(result[getCommandPosition()]);
        if (command != UPDCommand.SEND_LAST_ERROR) {
            logger.error("checkResult called on other than {}, ", UPDCommand.SEND_LAST_ERROR);
            logger.error("   result[{}]=0x{}", getCommandPosition(), String.format("%02X", result[getCommandPosition()]));
            return UDPResult.INVALID;
        }

        UDPResult udpResult = UDPResult.valueOf(result[DATA_POSITION]);
        if (udpResult.isError()) {
            logger.error("{}UDPResult: {}{}", ansi().fgBright(RED), udpResult, ansi().reset());
        } else {
            if (verbose) {
                logger.trace("{}done ({}){}", ansi().fgBright(GREEN), udpResult, ansi().reset());
            }
        }
        return udpResult;
    }

    public static byte[] uidToByteArray(String str) {
        String[] tokens = str.split(":");
        if (tokens.length != UPDProtocol.UID_LENGTH_USED) {
            logger.warn("ignoring --uid {}, wrong size {}, expected {}", str, tokens.length, UPDProtocol.UID_LENGTH_USED);
            return null;
        }
        byte[] uid = new byte[tokens.length];
        for (int n = 0; n < tokens.length; n++) {
            uid[n] = (byte) Integer.parseUnsignedInt(tokens[n], 16);
        }
        return uid;
    }

    public static String byteArrayToHex(byte[] bytes) {
        if (bytes == null) {
            return "";
        }

        StringBuilder txt = new StringBuilder();
        for (int i = 0; i < bytes.length; i++) {
            if (i != 0) {
                txt.append(":");
            }
            txt.append(String.format("%02X", bytes[i]));
        }
        return txt.toString();
    }

    public static int getCommandPosition() {
        return COMMAND_POSITION;
    }
}
