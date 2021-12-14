package org.selfbus.updater;

import net.harawata.appdirs.AppDirsFactory;
import org.selfbus.updater.bootloader.BootDescriptor;
import org.selfbus.updater.tests.flashdiff.FlashDiff;
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

import java.io.File;
import java.util.Arrays;

/**
 * experimental (WIP) Provides differential flash mode for the bootloader (MCU)
 */
public final class FlashDiffMode {
    private final static Logger logger = LoggerFactory.getLogger(FlashDiffMode.class.getName());
    // hexCacheDir will be used as a cache directory for the cached *.hex files used by the differential update mode
    // windows: C:\Users\[currentUser]\AppData\Local\Selfbus\Selfbus-Updater\Cache\[version]
    // linux  : /home/[user-home]]/.cache/Selfbus-Updater/[version]
    private static final String hexCacheDirectory = AppDirsFactory.getInstance().getUserCacheDir(
            ToolInfo.getTool(), ToolInfo.getVersion(), ToolInfo.getAuthor());
    private final static String IMAGE_IDENTIFIER = "image";
    private final static String FILE_NAME_SEP = "-";
    private final static String FILE_EXTENSION = ".bin";
    public final static int DEFAULT_DISPLAYED_PATH_DEPTH = 4;
    private static boolean initialized = false;
    private static BootDescriptor bootDsc = null;
    private static String oldFileName = null;

    private FlashDiffMode() {}

    public static String createCacheFileName(long startAddress, long length, long crc32) {
        return String.format("%s%s%s%s0x%s%s%s%s0x%s%s", hexCacheDirectory, File.separator, IMAGE_IDENTIFIER, FILE_NAME_SEP,
                Long.toHexString(startAddress), FILE_NAME_SEP, length, FILE_NAME_SEP,
                Integer.toHexString((int)crc32), FILE_EXTENSION);
    }

    public static String createCacheFileName(BootDescriptor bootDsc) {
        return createCacheFileName(bootDsc.startAddress(), bootDsc.length(), bootDsc.crc32());
    }

    public static boolean setupDifferentialMode(BootDescriptor bootDescriptor) {
        bootDsc = bootDescriptor;

        // try to find old firmware file in cache
        oldFileName = createCacheFileName(bootDescriptor);

        //TODO check that "Current App Version String" and "File App Version String" represent the same application,
        //     if they are not the same app, we should switch to full flash mode
        if (Utils.fileExists(oldFileName)) {
            logger.info("  Found current device firmware in cache {}", Utils.shortenPath(oldFileName, DEFAULT_DISPLAYED_PATH_DEPTH));
            logger.info("  {}--> switching to diff upload mode{}", ConColors.BRIGHT_GREEN, ConColors.RESET);
            initialized = true;
        }
        else {
            logger.warn("  Current device firmware not found in cache {}", Utils.shortenPath(oldFileName, DEFAULT_DISPLAYED_PATH_DEPTH));
            logger.warn("  {}--> switching to FULL upload mode{}", ConColors.BRIGHT_RED, ConColors.RESET);
            initialized = false;
        }
        return  initialized;
    }

    public static String hexCacheDirectory() {
        return hexCacheDirectory;
    }

    public static boolean isInitialized() {
        return initialized && (bootDsc != null);
    }

    // Differential update routine
    public static boolean doDifferentialFlash(UpdatableManagementClientImpl mc, Destination pd, long startAddress, byte[] binData)
            throws KNXDisconnectException, KNXTimeoutException, KNXRemoteException, KNXLinkClosedException, InterruptedException, UpdaterException {
        ///\todo add connection reset and sending again on failure, like in full flash mode
        if (!isInitialized()) {
            return false;
        }
        if (!Utils.fileExists(oldFileName)) {
            return false;
        }

        File oldImageCacheFile = new File(oldFileName);

        long flash_time_start = System.currentTimeMillis();
        FlashDiff differ = new FlashDiff();
        BinImage img2 = BinImage.copyFromArray(binData, startAddress);
        BinImage img1 = BinImage.readFromBin(oldImageCacheFile.getAbsolutePath(), bootDsc.startAddress());
        differ.generateDiff(img1, img2, (outputDiffStream, crc32) -> {
            byte[] result;
            // process compressed page
            //TODO check why 5 bytes are added to size in FlashDiff.java / generateDiff(...)
            logger.info("Sending new firmware ({} diff bytes)", (outputDiffStream.size() - 5));
            byte[] buf = new byte[12];
            int i = 0;
            while (i < outputDiffStream.size()) {
                // fill data for one telegram
                int j = 0;
                while (i < outputDiffStream.size() && j < buf.length) {
                    buf[j++] = outputDiffStream.get(i++);
                }
                // transmit telegram
                byte[] txBuf = Arrays.copyOf(buf, j); // avoid padded last telegram
                result = DeviceManagement.sendWithRetry(mc, pd, UPDCommand.SEND_DATA_TO_DECOMPRESS, txBuf, -1);
                //\todo switch to full flash mode on a NOT_IMPLEMENTED instead of exiting
                if (UPDProtocol.checkResult(result, false) != 0) {
                    DeviceManagement.restartProgrammingDevice(mc, pd);
                    throw new UpdaterException("Selfbus update failed.");
                }
            }
            // diff data of a single page transmitted
            // flash the page
            byte[] progPars = new byte[3 * 4];
            Utils.longToStream(progPars, 0, 0);
            Utils.longToStream(progPars, 4, 0);
            Utils.longToStream(progPars, 8, (int) crc32);
            System.out.println();
            logger.info("Program device next page diff, CRC32 0x{}", String.format("%08X", crc32));
            ///\todo harden against drops and timeouts
            result = mc.sendUpdateData(pd, UPDCommand.PROGRAM_DECOMPRESSED_DATA.id, progPars);
            if (UPDProtocol.checkResult(result) != 0) {
                DeviceManagement.restartProgrammingDevice(mc, pd);
                throw new UpdaterException("Selfbus update failed.");
            }
        });

        long flash_time_duration = System.currentTimeMillis() - flash_time_start;
        long total = differ.getTotalBytesTransferred();
        float bytesPerSecond = (float)total/(flash_time_duration/1000f);
        String col;
        if (bytesPerSecond >= 50.0) {
            col = ConColors.BRIGHT_GREEN;
        }
        else {
            col = ConColors.BRIGHT_RED;
        }
        String infoMsg = String.format("Diff-Update: Wrote %s%d%s bytes from file to device in %tM:%<tS. %s(%.2f B/s)%s",
                ConColors.BRIGHT_GREEN, total, ConColors.RESET, flash_time_duration, col, bytesPerSecond, ConColors.RESET);
/*
        if (timeoutCount > 0) {
            infoMsg += String.format(" Timeout(s): %d%s", ConColors.BG_RED, timeoutCount, ConColors.RESET);
        }
*/
        logger.info("{}", infoMsg);
        return true; ///\todo implement a correct return value
    }
}
