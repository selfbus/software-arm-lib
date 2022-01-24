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
    public static ResponseResult doFullFlash(DeviceManagement dm, BinImage newFirmware, int dataSendDelay, boolean eraseFirmwareRange)
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

        logger.info("\nStart sending application data ({} bytes) with telegram delay of {}ms", totalLength, dataSendDelay);

        int nRead;
        while ((nRead = fis.read(buffer)) != -1) { // Read up to size of buffer, default 1 Page of 256Bytes from file
            byte[] txBuffer = new byte[nRead];
            System.arraycopy(buffer, 0, txBuffer, 0, nRead);

            logger.info("Sending {} bytes: {}%", txBuffer.length, String.format("%3.1f", (float) 100 * (resultTotal.written() + txBuffer.length) / totalLength));

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
            System.out.println(String.format("%s(%.2f B/s)%s", col, bytesPerSecond, ConColors.RESET));


            // flash the previously sent data
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
            dm.requestBootLoaderStatistic(); ///\todo remove on release
        }
        fis.close();
        return resultTotal;
    }
}
