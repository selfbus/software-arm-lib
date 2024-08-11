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

    public static final int COMMAND_POSITION = 2;
    public static final int DATA_POSITION = 3;
    /**
     * uid/guid length of the mcu used for unlocking/flashing.
     */
    public static final int UID_LENGTH_USED = 12;
    /**
     * uid/guid length of the mcu.
     */
    public static final int UID_LENGTH_MAX = 16;

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
            logger.error(ansi().fgBright(RED).a("{} resultCode=0x{}").reset().toString(), udpResult, String.format("%02X", udpResult.id));
        } else {
            if (verbose) {
                logger.trace("done ({})", ansi().fgBright(GREEN).a(udpResult.id).reset().toString());
            }
        }
        return udpResult.id;
    }
}
