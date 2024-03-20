package org.selfbus.updater;

import org.selfbus.updater.upd.UPDCommand;
import org.selfbus.updater.upd.UPDProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.KNXRemoteException;
import tuwien.auto.calimero.KNXTimeoutException;
import tuwien.auto.calimero.link.KNXLinkClosedException;
import tuwien.auto.calimero.mgmt.KNXDisconnectException;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import static org.selfbus.updater.upd.UDPResult.*;

/**
 * Provides full flash mode for the bootloader (MCU)
 */
public class FlashFullMode {
    private final static Logger logger = LoggerFactory.getLogger(FlashFullMode.class.getName());

    /**
     * Normal update routine, sending complete image
     */
    public static ResponseResult doFullFlash(DeviceManagement dm, BinImage newFirmware, int dataSendDelay,
                                             boolean eraseFirmwareRange, boolean logStatistics)
            throws IOException, KNXDisconnectException, KNXTimeoutException, KNXLinkClosedException,
            InterruptedException, UpdaterException, KNXRemoteException {
        ResponseResult resultSendData, resultProgramData;
        ResponseResult resultTotal = new ResponseResult(); // collect so static data

        long totalLength = newFirmware.length();
        ByteArrayInputStream fis = new ByteArrayInputStream(newFirmware.getBinData());

        if (eraseFirmwareRange) {
            dm.eraseAddressRange(newFirmware.startAddress(), totalLength); // erase affected flash range
        }

        byte[] buffer = new byte[dm.getBlockSize()];
        long progAddress = newFirmware.startAddress();

        String logMessage = String.format("\nStart sending application data (%d bytes)", totalLength);
        if (dataSendDelay > 0) {
            logMessage += String.format(" with telegram delay of %dms", dataSendDelay);
        }
        logger.info(logMessage);

        int nRead = 0;
        boolean repeat = false;
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
            long flashTimeStart = System.currentTimeMillis(); // time this run started
            resultSendData = dm.doFlash(txBuffer, -1, dataSendDelay);
            resultTotal.addCounters(resultSendData); // keep track of static data

            // logging of connection speed
            long flashTimeDuration = System.currentTimeMillis() - flashTimeStart;
            float bytesPerSecond = (float) resultSendData.written() / (flashTimeDuration / 1000f);
            String col;
            if (bytesPerSecond >= 50.0) {
                col = ConColors.BRIGHT_GREEN;
            } else {
                col = ConColors.BRIGHT_YELLOW;
            }
            String percentageDone = String.format("%5.1f", (float) 100 * (resultTotal.written()) / totalLength);
            String progressInfo = String.format("%s %s%% %s%6.2f B/s%s", ConColors.BRIGHT_GREEN, percentageDone, col, bytesPerSecond, ConColors.RESET);
            logger.trace(progressInfo);
            // Check if printed Utils.PROGRESS_MARKER and progressInfo would exceed console width
            int progressMarkerLength = dm.getBlockSize()/(dm.getMaxPayload() * Utils.PROGRESS_MARKER.length());
            if ((progressMarkerLength + progressInfo.length()) > Utils.CONSOLE_WIDTH) {
                System.out.println();
            }
            System.out.print(progressInfo);

            // flash the previously sent data
            int crc32 = Utils.crc32Value(txBuffer);
            byte[] progPars = new byte[2 + 4 + 4];
            Utils.shortToStream(progPars, 0, (short)txBuffer.length);
            Utils.longToStream(progPars, 2, progAddress);
            Utils.longToStream(progPars, 6, crc32);

            String programFlashInfo = String.format("0x%04X-0x%04X", progAddress, progAddress + txBuffer.length - 1);
            if (txBuffer.length != dm.getBlockSize())
            {
                programFlashInfo = String.format("%s (%d bytes)", programFlashInfo, txBuffer.length);
            }

            logger.trace("Program device {} {}", programFlashInfo, String.format("crc32 0x%08X", crc32));
            System.out.print(" "+ programFlashInfo);
            System.out.println();

            resultProgramData = dm.sendWithRetry(UPDCommand.PROGRAM, progPars, -1);

            long result = UPDProtocol.checkResult(resultProgramData.data());
            if ((result == BYTECOUNT_RECEIVED_TOO_LOW.id) || (result == BYTECOUNT_RECEIVED_TOO_HIGH.id)) {
                repeat = true;
                resultTotal.setWritten(resultTotal.written() - txBuffer.length); // do not count failed transfer
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
        return resultTotal;
    }
}
