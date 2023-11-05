package org.selfbus.updater.upd;

import ch.qos.logback.classic.Level;
import org.selfbus.updater.ConColors;
import org.selfbus.updater.Utils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Implementation of the UPD/UDP protocol handling
 */
public final class UPDProtocol {
    private static final Logger logger = LoggerFactory.getLogger(UPDProtocol.class.getName());

    public static final int COMMAND_POSITION = 2;
    public static final int DATA_POSITION = 3;
    public static final int UID_LENGTH_USED = 12;     //!< uid/guid length of the mcu used for unlocking/flashing
    public static final int UID_LENGTH_MAX = 16;      //!< uid/guid length of the mcu

    private UPDProtocol() {}

    public static long checkResult(byte[] result) {
        return checkResult(result, true);
    }

    public static long checkResult(byte[] result, boolean verbose) {
        if (result[COMMAND_POSITION] != UPDCommand.SEND_LAST_ERROR.id) {
            logger.error("checkResult called on other than UPDCommand.SEND_LAST_ERROR.id=0x{}, result[{}]=0x{}",
                    String.format("%02X", UPDCommand.SEND_LAST_ERROR.id),
                    COMMAND_POSITION,
                    String.format("%02X", result[COMMAND_POSITION]));
            return 0;
        }

        UDPResult udpResult = UDPResult.valueOfIndex(result[DATA_POSITION]);
        if (udpResult.isError()) {
            logger.error("{}{} resultCode=0x{}{}", ConColors.BRIGHT_RED, udpResult, String.format("%02X", udpResult.id), ConColors.RESET);
        } else {
            if (verbose) {
                ch.qos.logback.classic.Logger root = (ch.qos.logback.classic.Logger) LoggerFactory.getLogger(Logger.ROOT_LOGGER_NAME);
                if (root.getLevel() == Level.TRACE) {
                    // only to display the message on the console
                    logger.debug("{}done ({}){}", ConColors.BRIGHT_GREEN, udpResult.id, ConColors.RESET);
                }
                else {
                    logger.trace("{}done ({}){}", ConColors.BRIGHT_GREEN, udpResult.id, ConColors.RESET);
                }

            } else {
                System.out.printf("%s%s%s", ConColors.BRIGHT_GREEN, Utils.PROGRESS_MARKER, ConColors.RESET); // Success in green
                logger.debug(Utils.PROGRESS_MARKER);
            }
        }
        return udpResult.id;
    }
}
