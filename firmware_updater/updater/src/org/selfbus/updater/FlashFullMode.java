package org.selfbus.updater;

import org.selfbus.updater.upd.UPDCommand;
import org.selfbus.updater.upd.UPDProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.KNXTimeoutException;
import tuwien.auto.calimero.link.KNXLinkClosedException;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import static org.fusesource.jansi.Ansi.*;
import static org.fusesource.jansi.Ansi.Color.*;
import static org.selfbus.updater.upd.UDPResult.*;

/**
 * Provides full flash mode for the bootloader (MCU)
 */
public class FlashFullMode {
    private final static Logger logger = LoggerFactory.getLogger(FlashFullMode.class);

    /**
     * Normal update routine, sending complete image
     */
    public static ResponseResult doFullFlash(DeviceManagement dm, BinImage newFirmware, int dataSendDelay,
                                             boolean eraseFirmwareRange, boolean logStatistics)
            throws IOException, KNXTimeoutException, KNXLinkClosedException,
            InterruptedException, UpdaterException {
        ResponseResult resultSendData, resultProgramData;
        ResponseResult resultTotal = new ResponseResult(); // collect so static data

        long totalLength = newFirmware.length();
        ByteArrayInputStream fis = new ByteArrayInputStream(newFirmware.getBinData());

        if (eraseFirmwareRange) {
            dm.eraseAddressRange(newFirmware.startAddress(), totalLength); // erase affected flash range
        }

        byte[] buffer = new byte[dm.getBlockSize()];
        long progAddress = newFirmware.startAddress();

        String logMessage = String.format("Start sending application data (%d bytes)", totalLength);
        if (dataSendDelay > 0) {
            logMessage += String.format(" with telegram delay of %dms", dataSendDelay);
        }
        logger.info(logMessage);
        dm.startProgressInfo();

        int nRead = 0;
        boolean repeat = false;
        SpinningCursor.reset();

        ProgressInfo progressInfo = new ProgressInfo(totalLength);
        while (fis.available() > 0) {
            if (!repeat) {
                nRead = fis.read(buffer);  // Read up to size of buffer
            }
            else {
                repeat = false;
                System.out.println();
            }

            byte[] txBuffer = new byte[nRead];
            System.arraycopy(buffer, 0, txBuffer, 0, nRead);

            // send data to the bootloader
            resultSendData = dm.doFlash(txBuffer, dm.getMaxUpdCommandRetry(), dataSendDelay, progressInfo);
            resultTotal.addCounters(resultSendData); // keep track of static data

            // flash the previously sent data
            int crc32 = Utils.crc32Value(txBuffer);
            byte[] progPars = new byte[2 + 4 + 4];
            Utils.shortToStream(progPars, 0, (short)txBuffer.length);
            Utils.longToStream(progPars, 2, progAddress);
            Utils.longToStream(progPars, 6, crc32);

            String debugInfo = String.format("%s 0x%04X-0x%04X", progressInfo, progAddress, progAddress + txBuffer.length - 1);
            if (txBuffer.length != dm.getBlockSize()) {
                debugInfo = String.format("%s (%d bytes)", debugInfo, txBuffer.length);
            }
            logger.debug("{} with crc32 {}", debugInfo, String.format("crc32 0x%08X", crc32));

            resultProgramData = dm.sendWithRetry(UPDCommand.PROGRAM, progPars, dm.getMaxUpdCommandRetry());

            long result = UPDProtocol.checkResult(resultProgramData.data());
            if ((result == BYTECOUNT_RECEIVED_TOO_LOW.id) || (result == BYTECOUNT_RECEIVED_TOO_HIGH.id)) {
                repeat = true;
                // do not count failed transfer
                progressInfo.update(-txBuffer.length);
            }
            else if (result == IAP_COMPARE_ERROR.id) {
                throw new UpdaterException(String.format("ProgramData update failed. %s",
                        String.format(ansi().fg(RED).a("Try again with option '--%s 256'").reset().toString(), CliOptions.OPT_LONG_BLOCKSIZE)));
            }
            else if (result == IAP_SUCCESS.id) {
                progAddress += txBuffer.length;
                if (logStatistics) {
                    dm.requestBootLoaderStatistic();
                }
            }
            else {
                dm.restartProgrammingDevice();
                throw new UpdaterException("ProgramData update failed.");
            }
            resultTotal.addCounters(resultProgramData); // keep track of statistic data
        }
        fis.close();
        dm.finalProgressInfo(progressInfo);
        resultTotal.setWritten(progressInfo.getBytesDone());
        return resultTotal;
    }
}