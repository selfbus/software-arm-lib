package org.selfbus.updater;

import org.selfbus.updater.upd.UDPResult;
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

/**
 * Provides full flash mode for the bootloader (MCU)
 */
public class FlashFullMode {
    private final static Logger logger = LoggerFactory.getLogger(FlashFullMode.class.getName());

    /**
     * Normal update routine, sending complete image
     */
    public static ResponseResult doFullFlash(DeviceManagement dm, BinImage newFirmware, int dataSendDelay, boolean eraseFirmwareRange, boolean logStatistics)
            throws IOException, KNXDisconnectException, KNXTimeoutException, KNXLinkClosedException,
            InterruptedException, UpdaterException, KNXRemoteException {
        ResponseResult resultSendData, resultProgramData;
        ResponseResult resultTotal = new ResponseResult(); // collect so static data

        long totalLength = newFirmware.length();
        ByteArrayInputStream fis = new ByteArrayInputStream(newFirmware.getBinData());

        if (eraseFirmwareRange) {
            dm.eraseAddressRange(newFirmware.startAddress(), totalLength); // erase affected flash range
        }

        byte[] buffer = new byte[Mcu.FLASH_PAGE_SIZE]; // buffer to hold one flash page
        long progAddress = newFirmware.startAddress();

        String logMessage = String.format("\nStart sending application data (%d bytes)", totalLength);
        if (dataSendDelay > 0) {
            logMessage += String.format(" with telegram delay of %dms", dataSendDelay);
        }
        logger.info(logMessage);

        int nRead;
        while ((nRead = fis.read(buffer)) != -1) { // Read up to size of buffer, default 1 Page of 256Bytes from file
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
                col = ConColors.BRIGHT_RED;
            }
            String percentageDone = String.format("%3.1f", (float) 100 * (resultTotal.written()) / totalLength);
            String progressInfo = String.format("%s %s%% (%.2f B/s)%s", col, percentageDone, bytesPerSecond, ConColors.RESET);
            logger.trace(progressInfo);
            System.out.print(progressInfo);

            // flash the previously sent data
            int crc32 = Utils.crc32Value(txBuffer);
            byte[] progPars = new byte[2 + 4 + 4];
            Utils.shortToStream(progPars, 0, (short)txBuffer.length);
            Utils.longToStream(progPars, 2, progAddress);
            Utils.longToStream(progPars, 6, crc32);

            String programFlashInfo = String.format("@0x%04X crc32 0x%08X", progAddress, crc32);
            logger.trace("Program device at flash {} ({} bytes)", programFlashInfo, txBuffer.length);
            System.out.print(" "+ programFlashInfo);
            System.out.println();

            resultProgramData = dm.sendWithRetry(UPDCommand.PROGRAM, progPars, -1);
            if (UPDProtocol.checkResult(resultProgramData.data()) != UDPResult.IAP_SUCCESS.id) {
                dm.restartProgrammingDevice();
                throw new UpdaterException("ProgramData update failed.");
            }
            resultTotal.addCounters(resultProgramData); // keep track of static data
            progAddress += txBuffer.length;

            if (logStatistics) {
                dm.requestBootLoaderStatistic();
            }
        }
        fis.close();
        return resultTotal;
    }
}
