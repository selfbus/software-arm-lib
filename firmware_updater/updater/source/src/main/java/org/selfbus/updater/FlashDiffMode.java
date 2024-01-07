package org.selfbus.updater;

import net.harawata.appdirs.AppDirsFactory;
import org.selfbus.updater.bootloader.BootDescriptor;
import org.selfbus.updater.tests.flashdiff.FlashDiff;
import org.selfbus.updater.upd.UDPResult;
import org.selfbus.updater.upd.UPDCommand;
import org.selfbus.updater.upd.UPDProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import io.calimero.KNXRemoteException;
import io.calimero.KNXTimeoutException;
import io.calimero.link.KNXLinkClosedException;
import io.calimero.mgmt.KNXDisconnectException;

import java.io.File;
import java.util.Arrays;
import java.util.concurrent.atomic.AtomicReference;

import static org.selfbus.updater.Mcu.MAX_PAYLOAD;

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

    @SuppressWarnings("unused")
    public static String hexCacheDirectory() {
        return hexCacheDirectory;
    }

    public static boolean isInitialized() {
        return initialized && (bootDsc != null);
    }

    // Differential update routine
    @SuppressWarnings("unused")
    public static ResponseResult doDifferentialFlash(DeviceManagement dm, long startAddress, byte[] binData)
            throws KNXDisconnectException, KNXTimeoutException, KNXRemoteException, KNXLinkClosedException, InterruptedException, UpdaterException {
        ///\todo add connection reset and sending again on failure, like in full flash mode
        if (!isInitialized()) {
            throw new UpdaterException("Differential mode not initialized!");
        }
        if (!Utils.fileExists(oldFileName)) {
            throw new UpdaterException(String.format("Firmware file %s not found!", Utils.shortenPath(oldFileName)));
        }

        File oldImageCacheFile = new File(oldFileName);

        FlashDiff differ = new FlashDiff();
        BinImage img2 = BinImage.copyFromArray(binData, startAddress);
        BinImage img1 = BinImage.readFromBin(oldImageCacheFile.getAbsolutePath(), bootDsc.startAddress());

        AtomicReference<ResponseResult> result = new AtomicReference<>();
        differ.generateDiff(img1, img2, (outputDiffStream, crc32) -> {

            // process compressed page
            //TODO check why 5 bytes are added to size in FlashDiff.java / generateDiff(...)
            logger.info("Sending new firmware ({} diff bytes)", (outputDiffStream.size() - 5));
            byte[] buf = new byte[MAX_PAYLOAD + 1];
            int i = 0;
            while (i < outputDiffStream.size()) {
                // fill data for one telegram
                int j = 0;
                while (i < outputDiffStream.size() && j < buf.length) {
                    buf[j++] = outputDiffStream.get(i++);
                }
                // transmit telegram
                byte[] txBuf = Arrays.copyOf(buf, j); // avoid padded last telegram
                result.set(dm.sendWithRetry(UPDCommand.SEND_DATA_TO_DECOMPRESS, txBuf, -1));
                //\todo switch to full flash mode on a NOT_IMPLEMENTED instead of exiting
                if (UPDProtocol.checkResult(result.get().data(), false) != UDPResult.IAP_SUCCESS.id) {
                    dm.restartProgrammingDevice();
                    throw new UpdaterException("Selfbus update failed.");
                }
            }
            // diff data of a single page transmitted
            // flash the page
            byte[] progPars = new byte[4];
            Utils.longToStream(progPars, 0, (int) crc32);
            System.out.println();
            logger.info("Program device next page diff, CRC32 0x{}", String.format("%08X", crc32));
            result.set(dm.sendWithRetry(UPDCommand.PROGRAM_DECOMPRESSED_DATA, progPars, -1));
            if (UPDProtocol.checkResult(result.get().data()) != UDPResult.IAP_SUCCESS.id) {
                dm.restartProgrammingDevice();
                throw new UpdaterException("Selfbus update failed.");
            }
        });
        result.get().setWritten(differ.getTotalBytesTransferred());
        return result.get();
    }
}
