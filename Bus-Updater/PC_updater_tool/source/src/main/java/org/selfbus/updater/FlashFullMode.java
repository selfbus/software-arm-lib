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

/**
 * Provides full flash mode for the bootloader (MCU)
 */
public class FlashFullMode {
    private final static Logger logger = LoggerFactory.getLogger(FlashFullMode.class.getName());

    /**
     * Normal update routine, sending complete image
     */
    public static void doFullFlash(DeviceManagement dm, BinImage newFirmware, int dataSendDelay, boolean eraseFlash)
            throws IOException, KNXDisconnectException, KNXTimeoutException, KNXLinkClosedException,
            InterruptedException, UpdaterException, KNXRemoteException {
        ResponseResult resultSendData, resultProgramData;
        ResponseResult resultTotal = new ResponseResult(); // collect so static data

        long totalLength = newFirmware.length();
        ByteArrayInputStream fis = new ByteArrayInputStream(newFirmware.getBinData());

        if (eraseFlash) {
            dm.eraseAddressRange(newFirmware.startAddress(), totalLength); // erase affected flash range
        }

        long flashTimeStart = System.currentTimeMillis(); // time flash process started
        byte[] buffer = new byte[Flash.FLASH_PAGE_SIZE]; // buffer to hold one flash page
        long progAddress = newFirmware.startAddress();

        logger.info("\nStart sending application data ({} bytes) with telegram delay of {}ms", totalLength, dataSendDelay);

        int nRead;
        while ((nRead = fis.read(buffer)) != -1) { // Read up to size of buffer, default 1 Page of 256Bytes from file
            byte[] txBuffer = new byte[nRead];
            System.arraycopy(buffer, 0, txBuffer, 0, nRead);

            logger.info("Sending {} bytes: {}%", txBuffer.length, String.format("%3.1f", (float) 100 * (resultTotal.written() + txBuffer.length) / totalLength));

            // send data to the bootloader
            resultSendData = dm.doFlash(txBuffer, -1, dataSendDelay);
            resultTotal.addCounters(resultSendData); // keep track of static data

            // flash the previously sent boot descriptor
            int crc32 = Utils.crc32Value(txBuffer);
            byte[] progPars = new byte[3 * 4];
            Utils.longToStream(progPars, 0, txBuffer.length);
            Utils.longToStream(progPars, 4, progAddress);
            Utils.longToStream(progPars, 8, crc32);
            System.out.println();
            logger.info("Program device at flash address 0x{} with {} bytes and CRC32 0x{}",
                    String.format("%04X", progAddress), String.format("%3d", txBuffer.length), String.format("%08X", crc32));

            resultProgramData = dm.sendWithRetry(UPDCommand.PROGRAM, progPars, -1);
            if (UPDProtocol.checkResult(resultProgramData.data()) != 0) {
                dm.restartProgrammingDevice();
                throw new UpdaterException("ProgramData update failed.");
            }
            resultTotal.addCounters(resultProgramData); // keep track of static data
            progAddress += txBuffer.length;
        }
        fis.close();

        // logging of some static data
        long flashTimeDuration = System.currentTimeMillis() - flashTimeStart;
        float bytesPerSecond = (float)resultTotal.written() / (flashTimeDuration / 1000f);
        String col;
        if (bytesPerSecond >= 50.0) {
            col = ConColors.BRIGHT_GREEN;
        }
        else {
            col = ConColors.BRIGHT_RED;
        }
        ///\todo find a better way to build the infoMsg, check possible logback functions
        String infoMsg = String.format("Wrote %d bytes from file to device in %tM:%<tS. %s(%.2f B/s)%s",
                resultTotal.written(), flashTimeDuration, col, bytesPerSecond, ConColors.RESET);

        if (resultTotal.timeoutCount() > 0) {
            infoMsg += String.format(" %sTimeout(s): %d%s", ConColors.BRIGHT_RED, resultTotal.timeoutCount(), ConColors.RESET);
        }
        else {
            infoMsg += String.format(" %sTimeout: %d%s", ConColors.BRIGHT_GREEN, resultTotal.timeoutCount(), ConColors.RESET);
        }
        if (resultTotal.dropCount() > 0) {
            infoMsg += String.format(" %sDrop(s): %d%s", ConColors.BRIGHT_RED, resultTotal.dropCount(), ConColors.RESET);
        }
        else {
            infoMsg += String.format(" %sDrop: %d%s", ConColors.BRIGHT_GREEN, resultTotal.dropCount(), ConColors.RESET);
        }
        logger.info("{}", infoMsg);
    }
}
