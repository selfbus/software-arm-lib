package org.selfbus.updater;

import org.fusesource.jansi.AnsiConsole;
import org.selfbus.updater.bootloader.BootloaderStatistic;
import tuwien.auto.calimero.*;
import org.selfbus.updater.bootloader.BootDescriptor;
import org.selfbus.updater.bootloader.BootloaderIdentity;
import org.selfbus.updater.bootloader.BootloaderUpdater;
import org.selfbus.updater.upd.UDPProtocolVersion;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.link.KNXNetworkLink;

import java.io.IOException;

import static org.fusesource.jansi.Ansi.*;
import static org.fusesource.jansi.Ansi.Color.*;
import static org.selfbus.updater.Utils.shortenPath;

import org.selfbus.updater.gui.GuiMain;

/**
 * A Tool for updating the firmware of a Selfbus device in a KNX network.
 * <p>
 * {@link Updater} is a {@link Runnable} tool implementation allowing
 * a user to update Selfbus KNX devices.<br>
 * <br>
 * This tool supports KNX network access using a KNXnet/IP, USB, FT1.2 or TPUART
 * connection. It uses the {@link DeviceManagement} to access the Selfbus KNX device.
 * <p>
 * When running this tool from the console, the {@code main} method of this class is invoked,
 * otherwise use this class in the context appropriate to a {@link Runnable}.<br>
 */
public class Updater implements Runnable {
    @SuppressWarnings("unused")
    private Updater() {} // disable default constructor

    private final static Logger logger = LoggerFactory.getLogger(Updater.class);
    private CliOptions cliOptions = null;
    private SBKNXLink sbKNXLink = null;

    /**
     * Constructs an instance of the {@link #Updater} class.
     *
     * @param cliOptions the command-line options to be used
     */
    public Updater(CliOptions cliOptions) {
        logger.debug(ToolInfo.getFullInfo());
        logger.debug(Settings.getLibraryHeader(false));
        this.cliOptions = cliOptions;
        this.sbKNXLink = new SBKNXLink();
        this.sbKNXLink.setCliOptions(cliOptions);
    }

    public static void initJansi() {
        AnsiConsole.systemInstall();
    }

    public static void finalizeJansi() {
        System.out.print(AnsiCursor.on()); // make sure we enable the cursor
        AnsiConsole.systemUninstall();
    }

    public static void main(final String[] args) {
        initJansi();

        if (args.length == 0) {
            GuiMain.startSwingGui();
            finalizeJansi();
            return;
        }

        // read in user-supplied command line options
        CliOptions options = new CliOptions(args, String.format("SB_updater-%s-all.jar", ToolInfo.getVersion()) ,
                "Selfbus KNX-Firmware update tool options", "", PHYS_ADDRESS_BOOTLOADER, PHYS_ADDRESS_OWN);
        if (options.version()) {
            logger.info(ansi().fgBright(GREEN).bold().a(Credits.getAsciiLogo()).reset().toString());
            logger.info(ansi().fgBright(GREEN).bold().a(Credits.getAuthors()).reset().toString());
            ToolInfo.showVersion();
            finalizeJansi();
            return;
        }

        logger.info(ansi().fgBright(GREEN).bold().a(ToolInfo.getToolAndVersion()).reset().toString());
        if (options.help()) {
            logger.info(options.helpToString());
            finalizeJansi();
            return;
        }

        try {
            final Updater updater = new Updater(options);
            final ShutdownHandler shutDownHandler = new ShutdownHandler().register();
            updater.run();
            shutDownHandler.unregister();
        } finally {
            finalizeJansi();
            logger.debug("main exit");
        }
    }

    /**
     * Called by this tool on completion.
     *
     * @param thrown
     *            the thrown exception if operation completed due to a raised
     *            exception, <code>null</code> otherwise
     * @param canceled
     *            whether the operation got canceled before its planned end
     */
    protected void onCompletion(final Exception thrown, final boolean canceled) {
        if (canceled)
            logger.info("reading device info canceled");
        if (thrown != null) {
            logger.error("Operation did not finish.", thrown);
        }
    }

    private static final class ShutdownHandler extends Thread {
        private final Thread t = Thread.currentThread();

        ShutdownHandler register() {
            Runtime.getRuntime().addShutdownHook(this);
            return this;
        }

        void unregister() {
            Runtime.getRuntime().removeShutdownHook(this);
        }

        public void run() {
            t.interrupt();
        }
    }

    /**
     * Minimum delay between two UPDCommand.SEND_DATA telegrams in milliseconds
     */
    public static final int DELAY_MIN = 0;
    /**
     * Maximum delay between two UPDCommand.SEND_DATA telegrams in milliseconds
     */
    public static final int DELAY_MAX = 500;
    /**
     * Default delay between two UPDCommand.SEND_DATA telegrams in milliseconds
     */
    public static final int DELAY_DEFAULT = 100;
    /**
     *  Physical address the bootloader is using
     */
    public static final IndividualAddress PHYS_ADDRESS_BOOTLOADER = new IndividualAddress(15, 15,192);
    /**
     *  Physical address the Selfbus Updater is using
     */
    public static final IndividualAddress PHYS_ADDRESS_OWN = new IndividualAddress(0, 0,0);

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Runnable#run()
     */
    public void run() {
        Exception thrown = null;
        boolean canceled = false;
        KNXNetworkLink link = null;
        try {
            byte[] uid = cliOptions.uid();
            final String hexFileName = cliOptions.fileName();
            BinImage newFirmware = null;

            if (!hexFileName.isEmpty()) {
                // check if the firmware file exists
                if (!Utils.fileExists(hexFileName)) {
                    logger.error(ansi().fg(RED).a("File '{}' does not exist!").reset().toString(), cliOptions.fileName());
                    throw new UpdaterException("Selfbus update failed.");
                }
                // Load Firmware hex file
                logger.info("Loading file '{}'", hexFileName);
                newFirmware = BinImage.readFromHex(hexFileName);
                // Check for APP_VERSION string in new firmware
                if (newFirmware.getAppVersion().isEmpty()) {
                    logger.warn(ansi().fgBright(RED).a("  Missing APP_VERSION string in new firmware!").reset().toString());
                    throw new UpdaterException("Missing APP_VERSION string in firmware!");
                }
            }
            else {
                System.out.println();
                logger.info(ansi().bg(RED).fg(BLACK).a("No firmware file (*.hex) specified! Specify with --{}").reset().toString(),
                        CliOptions.OPT_LONG_FILENAME);
                logger.info(ansi().fgBright(YELLOW).a("Reading only device information").reset().toString());
                System.out.println();
            }

            link = sbKNXLink.openLink();

            DeviceManagement dm = new DeviceManagement(link, cliOptions.progDevice(), cliOptions.priority());

            logger.debug("Telegram priority: {}", cliOptions.priority());

            //for option --device restart the device in bootloader mode
            if (cliOptions.device() != null) { // phys. knx address of the device in normal operation
                dm.checkDeviceInProgrammingMode(null); // check that before no device is in programming mode
                dm.restartDeviceToBootloader(cliOptions.device());
            }

            dm.checkDeviceInProgrammingMode(cliOptions.progDevice());

            if (uid == null) {
                uid = dm.requestUIDFromDevice();
            }

            dm.unlockDeviceWithUID(uid);

            if ((cliOptions.dumpFlashStartAddress() >= 0) && (cliOptions.dumpFlashEndAddress() >= 0)) {
                logger.warn(ansi().fgBright(GREEN).a("Dumping flash content range 0x{}-0x{} to bootloader's serial port.").reset().toString(),
                        String.format("%04X", cliOptions.dumpFlashStartAddress()), String.format("%04X", cliOptions.dumpFlashEndAddress()));
                dm.dumpFlashRange(cliOptions.dumpFlashStartAddress(), cliOptions.dumpFlashEndAddress());
                return;
            }

            BootloaderIdentity bootLoaderIdentity = dm.requestBootloaderIdentity();

            // Request current main firmware boot descriptor from device
            BootDescriptor bootDescriptor = dm.requestBootDescriptor();
            if (newFirmware != null) {
                logger.info("New firmware:              {}", newFirmware);
            }

            logger.debug("Requesting APP_VERSION");
            String appVersion = dm.requestAppVersionString();
            if (appVersion != null) {
                logger.info("Current APP_VERSION: {}", ansi().fgBright(GREEN).a(appVersion).reset().toString());
            }
            else {
                logger.info("Requesting APP_VERSION {}", ansi().fgBright(RED).a("  failed!").reset().toString());
            }

            //  From here on we need a valid firmware
            if (newFirmware == null) {
                if (cliOptions.device() != null) {
                    dm.restartProgrammingDevice();
                }
                // to get here `uid == null` must be true, so it's fine to exit with no-error
                link.close();
                System.exit(0);
            }

            // store new firmware bin file in cache directory
            String cacheFileName = FlashDiffMode.createCacheFileName(newFirmware.startAddress(), newFirmware.length(), newFirmware.crc32());
            BinImage imageCache = BinImage.copyFromArray(newFirmware.getBinData(), newFirmware.startAddress());
            imageCache.writeToBinFile(cacheFileName);

            logger.info("File APP_VERSION   : {}", ansi().fgBright(GREEN).a(newFirmware.getAppVersion()).reset().toString());

            // Check if FW image has correct offset for MCUs bootloader size
            if (newFirmware.startAddress() < bootLoaderIdentity.applicationFirstAddress()) {
                logger.error(ansi().fgBright(RED).a("  Error! The specified firmware image would overwrite parts of the bootloader. Check FW offset setting in the linker!").reset().toString());
                logger.error(ansi().fgBright(RED).a("  Firmware needs to start at or beyond 0x{}").reset().toString(), String.format("%04X", bootLoaderIdentity.applicationFirstAddress()));
                throw new UpdaterException("Firmware offset not correct!");
            }
            else if (newFirmware.startAddress() == bootLoaderIdentity.applicationFirstAddress()) {
                logger.debug(ansi().fgBright(GREEN).a("  Firmware starts directly beyond bootloader.").reset().toString());
            }
            else {
                logger.debug(ansi().fgBright(YELLOW).a("  Info: There are {} bytes of unused flash between bootloader and firmware.").reset().toString(),
                        newFirmware.startAddress() - bootLoaderIdentity.applicationFirstAddress());
            }

            if (cliOptions.eraseFullFlash()) {
                logger.warn(ansi().fgBright(RED).a("Deleting the entire flash except from the bootloader itself!").reset().toString());
                dm.eraseFlash();
            }

            boolean diffMode = false;
            if (!(cliOptions.full())) {
                if (bootDescriptor.valid()) {
                    diffMode = FlashDiffMode.setupDifferentialMode(bootDescriptor);
                }
                else {
                    logger.error(ansi().fgBright(RED).a("  BootDescriptor is not valid -> switching to full mode").reset().toString());
                }
            }

            if ((bootLoaderIdentity.versionMajor()) <= 1 && (bootLoaderIdentity.versionMinor() < 20)) {
                dm.setProtocolVersion(UDPProtocolVersion.UDP_V0);
            }
            else {
                dm.setProtocolVersion(UDPProtocolVersion.UDP_V1);
            }

            if (!dm.setBlockSize(cliOptions.getBlockSize())) {
                logger.info(ansi().fg(YELLOW).a("Connected bootloader doesn't support block size {}. Using {} bytes.").reset().toString(),
                        cliOptions.getBlockSize(), dm.getBlockSize());
            }

            if (!cliOptions.NO_FLASH()) { // is flashing firmware disabled? for debugging use only!
                // Start to flash the new firmware
                ResponseResult resultTotal;
                logger.info(ansi().bg(GREEN).fg(BLACK).a("Starting to send new firmware now:").reset().toString());
                if (diffMode && FlashDiffMode.isInitialized()) {
                    logger.warn(ansi().fgBright(RED).a("Differential mode is EXPERIMENTAL -> Use with caution.").reset().toString());
                    resultTotal = FlashDiffMode.doDifferentialFlash(dm, newFirmware.startAddress(), newFirmware.getBinData());
                }
                else {
                    resultTotal = FlashFullMode.doFullFlash(dm, newFirmware, cliOptions.delay(), !cliOptions.eraseFullFlash(), cliOptions.logStatistics());
                }
                dm.requestBootLoaderStatistic();

                String updaterStatisticMsg = "Updater:    ";
                String colored;
                if (resultTotal.dropCount() > BootloaderStatistic.THRESHOLD_DISCONNECT) {
                    colored = ansi().fgBright(YELLOW).toString();
                } else {
                    colored = ansi().fgBright(GREEN).toString();
                }
                updaterStatisticMsg += String.format("#Disconnect: %s%2d%s", colored, resultTotal.dropCount(), ansi().reset().toString());
                if (resultTotal.timeoutCount() > BootloaderStatistic.THRESHOLD_REPEATED) {
                    colored = ansi().fgBright(YELLOW).toString();
                } else {
                    colored = ansi().fgBright(GREEN).toString();
                }
                updaterStatisticMsg += String.format(" #Timeout       : %s%2d%s", colored, resultTotal.timeoutCount(), ansi().reset().toString());
                logger.info("{}", updaterStatisticMsg);
            }
            else {
                logger.warn("--{} => {}", CliOptions.OPT_LONG_NO_FLASH,
                        ansi().fg(RED).a("only boot description block will be written").reset().toString());
            }

            BootDescriptor newBootDescriptor = new BootDescriptor(newFirmware.startAddress(),
                    newFirmware.endAddress(), (int) newFirmware.crc32(), newFirmware.getAppVersionAddress());
            logger.info("Updating boot descriptor with {}", newBootDescriptor);
            dm.programBootDescriptor(newBootDescriptor, cliOptions.delay());
            String deviceInfo = cliOptions.progDevice().toString();
            if (cliOptions.device() != null) {
                deviceInfo = cliOptions.device().toString();
            }
            logger.info("Finished programming device {} with '{}'", ansi().fgBright(YELLOW).a(deviceInfo).reset().toString(),
                    ansi().fgBright(YELLOW).a(shortenPath(cliOptions.fileName(), 1)).reset().toString());
            logger.info(ansi().bg(GREEN).fg(BLACK).a("Firmware Update done, Restarting device").reset().toString());
            dm.restartProgrammingDevice();

            if (newFirmware.getAppVersion().contains(BootloaderUpdater.BOOTLOADER_UPDATER_ID_STRING)) {
                logger.info(ansi().bg(GREEN).fg(BLACK).a("Wait {} second(s) for Bootloader Updater to finish its job").reset().toString(),
                        String.format("%.2f", BootloaderUpdater.BOOTLOADER_UPDATER_MAX_RESTART_TIME_MS / 1000.0f));
                Thread.sleep(BootloaderUpdater.BOOTLOADER_UPDATER_MAX_RESTART_TIME_MS);
            }
        } catch (final KNXException | UpdaterException | RuntimeException e) {
            thrown = e;
        } catch (final InterruptedException e) {
            canceled = true;
            Thread.currentThread().interrupt();
        } catch (IOException e) {
            logger.error("IOException ", e);
        } catch (Throwable e) {
            logger.error("Throwable ", e);
        } finally {
            if (link != null)
                link.close();
            onCompletion(thrown, canceled);
        }
    }

    public String requestUid(){
        KNXNetworkLink link;
        try {
            link = this.sbKNXLink.openLink();
            DeviceManagement dm = new DeviceManagement(link, cliOptions.progDevice(), cliOptions.priority());

            //for option --device restart the device in bootloader mode
            if (cliOptions.device() != null) { // phys. knx address of the device in normal operation
                dm.checkDeviceInProgrammingMode(null); // check that before no device is in programming mode
                dm.restartDeviceToBootloader(cliOptions.device());
            }

            dm.checkDeviceInProgrammingMode(cliOptions.progDevice());

            byte[] uid = dm.requestUIDFromDevice();

            if (cliOptions.device() != null) {
                dm.restartProgrammingDevice();
            }
            link.close();
            return Utils.byteArrayToHex(uid);

        } catch (InterruptedException | UpdaterException | KNXException e) {
            throw new RuntimeException(e);
        }
    }
}