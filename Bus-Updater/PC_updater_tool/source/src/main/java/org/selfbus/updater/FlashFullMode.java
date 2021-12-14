package org.selfbus.updater;

import org.selfbus.updater.upd.UPDCommand;
import org.selfbus.updater.upd.UPDProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.KNXRemoteException;
import tuwien.auto.calimero.KNXTimeoutException;
import tuwien.auto.calimero.link.KNXLinkClosedException;
import tuwien.auto.calimero.mgmt.Destination;
import tuwien.auto.calimero.mgmt.KNXDisconnectException;
import tuwien.auto.calimero.mgmt.UpdatableManagementClientImpl;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.zip.CRC32;

/**
 * Provides full flash mode for the bootloader (MCU)
 */
public class FlashFullMode {
    private final static Logger logger = LoggerFactory.getLogger(FlashFullMode.class.getName());

    /**
     * Normal update routine, sending complete image
     * This works on sector page right now, so the complete affected flash is erased first
     */
    public static void doFullFlash(UpdatableManagementClientImpl mc, Destination pd, BinImage newFirmware, int dataSendDelay)
            throws IOException, KNXDisconnectException, KNXTimeoutException, KNXLinkClosedException,
            InterruptedException, UpdaterException, KNXRemoteException {
        byte[] result;

        long startAddress = newFirmware.startAddress();
        long totalLength = newFirmware.length();
        ByteArrayInputStream fis = new ByteArrayInputStream(newFirmware.getBinData());
        DeviceManagement.eraseAddressRange(mc, pd, startAddress, totalLength);

        byte[] buf = new byte[Flash.FLASH_PAGE_SIZE];   // Read one flash page
        int nRead;                                      // Bytes read from file into buffer
        int payload = Flash.MAX_PAYLOAD;                // maximum start payload size
        int total = 0;
        CRC32 crc32Block = new CRC32();
        int progSize = 0;	                   // Bytes sent so far
        long progAddress = startAddress;
        boolean doProg = false;
        int timeoutCount = 0;
        int connectionDrops = 0;

        logger.info("\nStart sending application data ({} bytes) with telegram delay of {}ms", totalLength, dataSendDelay);

        // Get time when starting to transfer data
        long flash_time_start = System.currentTimeMillis();

        // Read up to size of buffer, 1 Page of 256Bytes from file
        while ((nRead = fis.read(buf)) != -1) {
            logger.info("Sending {} bytes: {}%", nRead, String.format("%3.1f", (float)100*(total+nRead)/totalLength));

            int nDone = 0;
            // Bytes left to write
            while (nDone < nRead) {

                // Calculate payload size for next telegram
                // sufficient data left, use maximum payload size
                ///\todo  	nDone 253, progSize 0, payLoad 3, nRead 256
                logger.debug("nDone {}, progSize {}, payLoad {}, nRead {}", nDone, progSize, payload, nRead);
                if (progSize + payload < nRead)
                {
                    payload = Flash.MAX_PAYLOAD;	// maximum payload size
                }
                else
                {
                    payload = nRead - progSize;	// remaining bytes
                    doProg = true;
                }
                ///\todo progSize 0, payLoad 11, nRead 256, doProg false
                logger.debug("progSize {}, payLoad {}, nRead {}, doProg {}", progSize, payload, nRead, doProg);

                if (payload > Flash.MAX_PAYLOAD)
                {
                    // this is just a safety backup, normally we should never land here
                    logger.error("payload {} > MAX_PAYLOAD {} should never happen here.", payload, Flash.MAX_PAYLOAD);
                    payload = Flash.MAX_PAYLOAD;
                    doProg = false;
                }
/*
            	// Handle last telegram with <= MAX_PAYLOAD bytes
                if ((total + nRead) == totalLength)
                {
                    if (nRead <= MAX_PAYLOAD) { // Handle last telegram with less than MAX_PAYLOAD bytes
                        payload = nRead;
                        doProg = true;
                    } else if ((payload + MAX_PAYLOAD) == ) {
                        doProg = true;
                    }
                }
*/
                // Data left to send
                if (payload > 0) {
                    // Message length is payload +1 byte for position
                    byte[] txBuf = new byte[payload + 1];

                    //Shift payload into send buffer
                    ///\todo happened while a Selfbus Firmware's Update and a parallel ETS programming device MDT BE-GT2Tx.01 Glastaster II Smart mit Temperatursensor
                    ///\todo java.lang.ArrayIndexOutOfBoundsException: arraycopy: last source index 264 out of bounds for byte[256]
                    ///\todo nDone 253, progSize 0, payLoad 3, nRead 256
                    logger.debug("nDone {}, progSize{}, payLoad {}, nRead {}, doProg {}", nDone, progSize, payload, nRead, doProg);
                    System.arraycopy(buf, nDone, txBuf, 1, payload);
                    /* old version
                    for (int i = 0; i < payload; i++) {
                        txBuf[i + 1] = buf[i + nDone];	//Shift payload into send buffer
                    }
                    */

                    // First byte contains start address of following data
                    txBuf[0] = (byte)progSize;

                    if (dataSendDelay > 0) {
                        Thread.sleep(dataSendDelay); //Reduce bus load during data upload, without 2:04, 50ms 2:33, 60ms 2:41, 70ms 2:54, 80ms 3:04
                    }

                    try{
                        result = mc.sendUpdateData(pd, UPDCommand.SEND_DATA.id, txBuf);

                        if (UPDProtocol.checkResult(result, false) != 0) {
                            DeviceManagement.restartProgrammingDevice(mc, pd);
                            throw new UpdaterException("Selfbus update failed.");
                        }
                        // Update CRC, skip byte 0 which is not part of payload
                        crc32Block.update(txBuf, 1, payload);
                        nDone += payload;		// keep track of buffer bytes send (to determine end of while-loop)
                        progSize += payload;	// keep track of page/sector bytes send
                        total += payload;		// keep track of total bytes send from file
                    }

                    catch (KNXTimeoutException e) { // timeout, we need to resend the last sent data
                        logger.warn("{}Timeout {}{}", ConColors.RED, e.getMessage(), ConColors.RESET);
                        timeoutCount++;
                        continue;
                    }
                    catch (KNXDisconnectException | KNXRemoteException e) {
                        logger.warn("{}failed {}{}", ConColors.BRIGHT_RED, e.getMessage(), ConColors.RESET);
                        logger.debug("nDone {}, progSize {}, payLoad {}, nRead {}", nDone, progSize, payload, nRead);
                        // reset all back to beginning of page
                        total -= nDone;
                        progSize = 0;
                        nDone = 0;
                        crc32Block.reset();
                        connectionDrops++;
                        continue;
                    }
                }

                while (doProg) {
                    byte[] progPars = new byte[3 * 4];
                    long crc = crc32Block.getValue();
                    Utils.longToStream(progPars, 0, progSize);
                    Utils.longToStream(progPars, 4, progAddress);
                    Utils.longToStream(progPars, 8, (int) crc);
                    System.out.println();
                    logger.info("Program device at flash address 0x{} with {} bytes and CRC32 0x{}",
                            String.format("%04X", progAddress), String.format("%3d", progSize), String.format("%08X", crc));
                    try {
                        result = mc.sendUpdateData(pd, UPDCommand.PROGRAM.id, progPars);
                        if (UPDProtocol.checkResult(result) != 0) {
                            DeviceManagement.restartProgrammingDevice(mc, pd);
                            throw new UpdaterException("Selfbus update failed.");
                        }
                        progAddress += progSize;
                        progSize = 0;    // reset page/sector byte counter
                        crc32Block.reset();
                        doProg = false;
                    }
                    catch (KNXTimeoutException | KNXDisconnectException | KNXRemoteException e) {
                        logger.warn("{}failed {}{}", ConColors.RED, e.getMessage(), ConColors.RESET);
                        connectionDrops++;
                    }
                }
            }
        }
        fis.close();

        long flash_time_duration = System.currentTimeMillis() - flash_time_start;
        float bytesPerSecond = (float)total/(flash_time_duration/1000f);
        String col;
        if (bytesPerSecond >= 50.0) {
            col = ConColors.BRIGHT_GREEN;
        }
        else {
            col = ConColors.BRIGHT_RED;
        }
        ///\todo find a better way to build the infoMsg, check possible logback functions
        String infoMsg = String.format("Wrote %d bytes from file to device in %tM:%<tS. %s(%.2f B/s)%s",
                total, flash_time_duration, col, bytesPerSecond, ConColors.RESET);

        if (timeoutCount > 0) {
            infoMsg += String.format(" %sTimeout(s): %d%s", ConColors.BRIGHT_RED, timeoutCount, ConColors.RESET);
        }
        else {
            infoMsg += String.format(" %sTimeout: %d%s", ConColors.BRIGHT_GREEN, timeoutCount, ConColors.RESET);
        }
        if (connectionDrops > 0) {
            infoMsg += String.format(" %sDrop(s): %d%s", ConColors.BRIGHT_RED, connectionDrops, ConColors.RESET);
        }
        else {
            infoMsg += String.format(" %sDrop: %d%s", ConColors.BRIGHT_GREEN, connectionDrops, ConColors.RESET);
        }

        logger.info("{}", infoMsg);
    }

}
