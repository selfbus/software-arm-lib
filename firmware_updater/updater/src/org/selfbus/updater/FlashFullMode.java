package org.selfbus.updater;

import org.selfbus.updater.bootloader.BootloaderStatistic;
import org.selfbus.updater.devicemgnt.DeviceManagement;
import org.selfbus.updater.upd.UDPResult;
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

        long progAddress = newFirmware.startAddress();

        String logMessage = String.format("Start sending application data (%d bytes)", totalLength);
        if (dataSendDelay > 0) {
            logMessage += String.format(" with telegram delay of %dms", dataSendDelay);
        }
        logger.info(logMessage);

        SpinningCursor.reset();
        ProgressInfo progressInfo;
        BootloaderStatistic bootloaderStatistic;
        if (logStatistics) {
            bootloaderStatistic = dm.requestBootLoaderStatistic();
            progressInfo = new ProgressInfoAdvanced(totalLength, bootloaderStatistic);
        }
        else {
            bootloaderStatistic = null;
            progressInfo = new ProgressInfo(totalLength);
        }
        dm.startProgressInfo(progressInfo);

        int nRead = 0;
        byte[] buffer = null;
        while ((fis.available() > 0) || (buffer != null)) {
            if (buffer == null) {
                // Start or buffer was successfully transferred
                buffer = new byte[dm.getBlockSize()];
                nRead = fis.read(buffer);  // Read up to size of buffer
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
            logger.debug("{} with crc32 {}", debugInfo, String.format("0x%08X", crc32));

            resultProgramData = dm.sendWithRetry(UPDCommand.PROGRAM, progPars, dm.getMaxUpdCommandRetry());
            resultTotal.addCounters(resultProgramData); // keep track of statistic data

            UDPResult result = UPDProtocol.checkResult(resultProgramData.data());
            switch (result) {
                case IAP_SUCCESS:
                    buffer = null;
                    progAddress += txBuffer.length;
                    if (bootloaderStatistic != null) {
                        bootloaderStatistic = dm.requestBootLoaderStatistic();
                    }
                    break;

                case BYTECOUNT_RECEIVED_TOO_LOW:
                case BYTECOUNT_RECEIVED_TOO_HIGH:
                    // do not count failed transfer and send buffer again in next while run
                    progressInfo.update(-txBuffer.length);
                    break;

                case IAP_COMPARE_ERROR:
                    throw new UpdaterException(String.format("ProgramData update failed. %sTry again with option '--%s 256'%s",
                            ansi().fg(RED), CliOptions.OPT_LONG_BLOCKSIZE, ansi().reset()));
                default:
                    dm.restartProgrammingDevice();
                    throw new UpdaterException("ProgramData update failed.");
            }
        }
        fis.close();
        dm.logAndPrintProgressInfo(progressInfo);
        resultTotal.setWritten(progressInfo.getBytesDone());
        return resultTotal;
    }
}
