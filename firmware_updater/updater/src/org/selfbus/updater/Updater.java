package org.selfbus.updater;

import ch.qos.logback.classic.Level;
import org.apache.commons.cli.ParseException;
import org.fusesource.jansi.AnsiConsole;
import org.selfbus.updater.bootloader.BootloaderStatistic;
import tuwien.auto.calimero.*;
import org.selfbus.updater.bootloader.BootDescriptor;
import org.selfbus.updater.bootloader.BootloaderIdentity;
import org.selfbus.updater.bootloader.BootloaderUpdater;
import org.selfbus.updater.upd.UDPProtocolVersion;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static org.fusesource.jansi.Ansi.*;
import static org.fusesource.jansi.Ansi.Color.*;
import static org.selfbus.updater.LoggingManager.CONSOLE_APPENDER_NAME;
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
    private static DeviceManagement dm = null;

    /**
     * Constructs an instance of the {@link #Updater} class.
     *
     * @param cliOptions the command-line options to be used
     */
    public Updater(CliOptions cliOptions) {
        logger.debug(ToolInfo.getFullInfo());
        logger.debug(Settings.getLibraryHeader(false));
        this.cliOptions = cliOptions;
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
            logger.info("GUI start => console output set to log level {}", Level.WARN);
            LoggingManager.setAppenderLogLevel(CONSOLE_APPENDER_NAME, Level.WARN);
            GuiMain.startSwingGui();
            finalizeJansi();
            return;
        }

        try {
            logger.info("{}{}{}", ansi().fgBright(GREEN).bold(), ToolInfo.getToolAndVersion(), ansi().reset());
            // read in user-supplied command line options
            CliOptions options = new CliOptions(args, ToolInfo.getToolJarName() ,
                    "Selfbus KNX-Firmware update tool options", "");
            if (options.getVersionIsSet()) {
                logger.info("{}{}{}", ansi().fgBright(GREEN).bold(), Credits.getAsciiLogo(), ansi().reset());
                logger.info("{}{}{}", ansi().fgBright(GREEN).bold(), Credits.getAuthors(), ansi().reset());
                ToolInfo.showVersion();
                finalizeJansi();
                return;
            }

            if (options.getHelpIsSet()) {
                logger.info(options.helpToString());
                finalizeJansi();
                return;
            }

            if (options.getDiscoverIsSet()) {
                logger.info("Discovering KNX network...");
                DiscoverKnxInterfaces.toText(DiscoverKnxInterfaces.getAllnetIPInterfaces());
                DiscoverKnxInterfaces.toText(DiscoverKnxInterfaces.getUsbInterfaces());
                finalizeJansi();
                return;
            }

            final Updater updater = new Updater(options);
            final ShutdownHandler shutdownHandler = new ShutdownHandler().register();
            updater.run();
            shutdownHandler.unregister();
        }
        catch (KNXFormatException | ParseException | InterruptedException e) {
            logger.error("", e); // todo see logback issue https://github.com/qos-ch/logback/issues/876
        }
        finally {
            if (dm != null) {
                dm.close();
            }
            finalizeJansi();
            logger.debug("main exit");
        }
    }

    private static final class ShutdownHandler extends Thread {
        private final Thread t = Thread.currentThread();

        ShutdownHandler register() {
            Runtime.getRuntime().addShutdownHook(this);
            logger.trace("ShutdownHandler registered");
            return this;
        }

        void unregister() {
            try {
                Runtime.getRuntime().removeShutdownHook(this);
                logger.trace("ShutdownHandler unregistered");
            }
            catch (IllegalStateException ignored) {
                // Shutdown is currently in progress
            }
        }

        public void run() {
            try {
                t.interrupt();
                logger.trace("ShutdownHandler called");
                if (dm != null) {
                    dm.close();
                    dm = null;
                }
            }
            catch (Exception e) {
                logger.error("Error while shutting down", e);
            }
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
        try {
            final String hexFileName = cliOptions.getFileName();
            BinImage newFirmware = null;

            if (!hexFileName.isEmpty()) {
                // check if the firmware file exists
                if (!Utils.fileExists(hexFileName)) {
                    logger.error("{}File '{}' does not exist!{}", ansi().fg(RED), cliOptions.getFileName(), ansi().reset());
                    throw new UpdaterException(String.format("File '%s' does not exist!", cliOptions.getFileName()));
                }
                // Load Firmware hex file
                logger.info("Loading file '{}'", hexFileName);
                newFirmware = BinImage.readFromHex(hexFileName);
                // Check for APP_VERSION string in new firmware
                if (newFirmware.getAppVersion().isEmpty()) {
                    logger.warn("{}  Missing APP_VERSION string in new firmware!{}", ansi().fgBright(RED), ansi().reset());
                    throw new UpdaterException("Missing APP_VERSION string in firmware!");
                }
            }
            else {
                System.out.println();
                logger.info("{}No firmware file (*.hex) specified! Specify with --{}{}",
                        ansi().bg(RED).fg(BLACK), CliOptions.OPT_LONG_FILENAME, ansi().reset());
                logger.info("{}Reading only device information{}", ansi().fgBright(YELLOW), ansi().reset());
                System.out.println();
            }

            dm = new DeviceManagement(cliOptions);

            logger.debug("Telegram priority: {}", cliOptions.getPriority());
            dm.open();
            logger.info("KNX connection: {}", dm.getLinkInfo());

            //for option --device restart the device in bootloader mode
            if (cliOptions.getDevicePhysicalAddress() != null) { // phys. knx address of the device in normal operation
                dm.checkDeviceInProgrammingMode(null); // check that before no device is in programming mode
                dm.restartDeviceToBootloader(cliOptions.getDevicePhysicalAddress());
            }

            dm.checkDeviceInProgrammingMode(cliOptions.getProgDevicePhysicalAddress());
            String uid = cliOptions.getUid();
            if (uid.isEmpty()) {
                uid = dm.requestUIDFromDevice();
            }

            dm.unlockDeviceWithUID(uid);

            if ((cliOptions.getDumpFlashStartAddress() >= 0) && (cliOptions.getDumpFlashEndAddress() >= 0)) {
                logger.warn("{}Dumping flash content range {} to bootloader's serial port.{}",
                        ansi().fgBright(GREEN),
                        String.format("0x%04X-0x%04X", cliOptions.getDumpFlashStartAddress(), cliOptions.getDumpFlashEndAddress()),
                        ansi().reset());
                dm.dumpFlashRange(cliOptions.getDumpFlashStartAddress(), cliOptions.getDumpFlashEndAddress());
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
            if (appVersion.isEmpty()) {
                logger.info("Current APP_VERSION: {}invalid{} ", ansi().fgBright(RED), ansi().reset());
            } else {
                logger.info("Current APP_VERSION: {}{}{}", ansi().fgBright(GREEN), appVersion, ansi().reset());
            }

            //  From here on we need a valid firmware
            if (newFirmware == null) {
                if (cliOptions.getDevicePhysicalAddress() != null) {
                    dm.restartProgrammingDevice();
                }
                // to get here `uid == null` must be true, so it's fine to exit with no-error
                dm.close();
                return;
            }

            // store new firmware bin file in cache directory
            String cacheFileName = FlashDiffMode.createCacheFileName(newFirmware.startAddress(), newFirmware.length(), newFirmware.crc32());
            BinImage imageCache = BinImage.copyFromArray(newFirmware.getBinData(), newFirmware.startAddress());
            imageCache.writeToBinFile(cacheFileName);

            logger.info("File APP_VERSION   : {}{}{}", ansi().fgBright(GREEN), newFirmware.getAppVersion(), ansi().reset());

            // Check if FW image has correct offset for MCUs bootloader size
            if (newFirmware.startAddress() < bootLoaderIdentity.applicationFirstAddress()) {
                logger.error("{}  Error! The specified firmware image would overwrite parts of the bootloader. Check FW offset setting in the linker!{}",
                        ansi().fgBright(RED), ansi().reset());
                logger.error("{}  Firmware needs to start at or beyond 0x{}{}", ansi().fgBright(RED),
                        String.format("%04X", bootLoaderIdentity.applicationFirstAddress()), ansi().reset());
                throw new UpdaterException("Firmware offset not correct!");
            }
            else if (newFirmware.startAddress() == bootLoaderIdentity.applicationFirstAddress()) {
                logger.debug("{}  Firmware starts directly beyond bootloader.{}", ansi().fgBright(GREEN), ansi().reset());
            }
            else {
                logger.debug("{}  Info: There are {} bytes of unused flash between bootloader and firmware.{}",
                        ansi().fgBright(YELLOW), newFirmware.startAddress() - bootLoaderIdentity.applicationFirstAddress(),
                        ansi().reset());
            }

            if (cliOptions.getEraseFullFlashIsSet()) {
                logger.warn("{}Deleting the entire flash except from the bootloader itself!{}",
                        ansi().fgBright(RED), ansi().reset());
                dm.eraseFlash();
            }

            boolean diffMode = false;
            if (!(cliOptions.getFlashingFullModeIsSet())) {
                if (bootDescriptor.valid()) {
                    diffMode = FlashDiffMode.setupDifferentialMode(bootDescriptor);
                }
                else {
                    logger.warn("{}  BootDescriptor is not valid -> switching to full mode{}",
                            ansi().fgBright(RED), ansi().reset());
                }
            }

            if ((bootLoaderIdentity.versionMajor()) <= 1 && (bootLoaderIdentity.versionMinor() < 20)) {
                dm.setProtocolVersion(UDPProtocolVersion.UDP_V0);
            }
            else {
                dm.setProtocolVersion(UDPProtocolVersion.UDP_V1);
            }

            if (!dm.setBlockSize(cliOptions.getBlockSize())) {
                logger.info("{}Connected bootloader doesn't support block size {}. Using {} bytes.{}",
                        ansi().fg(YELLOW), cliOptions.getBlockSize(), dm.getBlockSize(), ansi().reset());
            }

            if (!cliOptions.getNoFlashIsSet()) { // is flashing firmware disabled? for debugging use only!
                // Start to flash the new firmware
                ResponseResult resultTotal;
                logger.info("{}Starting to send new firmware now:{}", ansi().bg(GREEN).fg(BLACK), ansi().reset());
                if (diffMode && FlashDiffMode.isInitialized()) {
                    logger.warn("{}Differential mode is EXPERIMENTAL -> Use with caution.{}",
                            ansi().fgBright(RED), ansi().reset());
                    resultTotal = FlashDiffMode.doDifferentialFlash(dm, newFirmware.startAddress(), newFirmware.getBinData());
                }
                else {
                    resultTotal = FlashFullMode.doFullFlash(dm, newFirmware, cliOptions.getDelayMs(), !cliOptions.getEraseFullFlashIsSet(), cliOptions.getLogStatisticsIsSet());
                }
                BootloaderStatistic bootloaderStatistic = dm.requestBootLoaderStatistic();
                if (bootloaderStatistic != null) {
                    logger.info("Bootloader: #Disconnect: {} #repeated T_ACK: {}",
                            bootloaderStatistic.getDisconnectCountColored(), bootloaderStatistic.getRepeatedT_ACKcountColored());
                }
                BootloaderStatistic updaterStatistic = new BootloaderStatistic((int)resultTotal.dropCount(),
                        (int)resultTotal.timeoutCount());
                logger.info("Updater   : #Disconnect: {} #repeated T_ACK: {}",
                        updaterStatistic.getDisconnectCountColored(), updaterStatistic.getRepeatedT_ACKcountColored());
            }
            else {
                logger.warn("--{} => {}only boot description block will be written{}", CliOptions.OPT_LONG_NO_FLASH,
                        ansi().fg(RED), ansi().reset());
            }

            BootDescriptor newBootDescriptor = new BootDescriptor(newFirmware.startAddress(),
                    newFirmware.endAddress(), (int) newFirmware.crc32(), newFirmware.getAppVersionAddress());
            logger.info("Updating boot descriptor with {}", newBootDescriptor);
            dm.programBootDescriptor(newBootDescriptor, cliOptions.getDelayMs());
            String deviceInfo = cliOptions.getProgDevicePhysicalAddress().toString();
            if (cliOptions.getDevicePhysicalAddress() != null) {
                deviceInfo = cliOptions.getDevicePhysicalAddress().toString();
            }
            logger.info("Finished programming device {}{}{} with '{}{}{}'",
                    ansi().fgBright(YELLOW), deviceInfo, ansi().reset(),
                    ansi().fgBright(YELLOW), shortenPath(cliOptions.getFileName(), 1), ansi().reset());
            logger.info("{}Firmware Update done, Restarting device{}", ansi().bg(GREEN).fg(BLACK), ansi().reset());
            dm.restartProgrammingDevice();
            dm.close();

            if (newFirmware.getAppVersion().contains(BootloaderUpdater.BOOTLOADER_UPDATER_ID_STRING)) {
                logger.info("{}Wait {} second(s) for Bootloader Updater to finish its job{}",
                        ansi().bg(GREEN).fg(BLACK),
                        String.format("%.2f", BootloaderUpdater.BOOTLOADER_UPDATER_MAX_RESTART_TIME_MS / 1000.0f),
                        ansi().reset());
                Thread.sleep(BootloaderUpdater.BOOTLOADER_UPDATER_MAX_RESTART_TIME_MS);
            }

            logger.info("Operation finished successfully.");
        }
        catch (final InterruptedException | IllegalStateException e) {
            Thread.currentThread().interrupt();
            logger.info("{}Operation canceled.", System.lineSeparator());
            logger.debug("", e); // todo see logback issue https://github.com/qos-ch/logback/issues/876
        }
        catch (Throwable e) {
            logger.error("", e); // todo see logback issue https://github.com/qos-ch/logback/issues/876
            logger.error("Operation did not finish.");
        }
        finally {
            if (dm != null) {
                dm.close();
            }
        }
    }

    public String requestUid() throws KNXException, UpdaterException {
        try {
            DeviceManagement dm = new DeviceManagement(cliOptions);

            //for option --device restart the device in bootloader mode
            if (cliOptions.getDevicePhysicalAddress() != null) { // phys. knx address of the device in normal operation
                dm.checkDeviceInProgrammingMode(null); // check that before no device is in programming mode
                dm.restartDeviceToBootloader(cliOptions.getDevicePhysicalAddress());
            }

            dm.checkDeviceInProgrammingMode(cliOptions.getProgDevicePhysicalAddress());

            String uid = dm.requestUIDFromDevice();

            dm.unlockDeviceWithUID(uid);
            dm.requestBootloaderIdentity();
            dm.requestBootDescriptor();
            String appVersion = dm.requestAppVersionString();
            if (appVersion.isEmpty()) {
                logger.info("APP_VERSION: {}invalid{} ", ansi().fgBright(RED), ansi().reset());
            } else {
                logger.info("APP_VERSION: {}{}{}", ansi().fgBright(GREEN), appVersion, ansi().reset());
            }

            if (cliOptions.getDevicePhysicalAddress() != null) {
                dm.restartProgrammingDevice();
            }
            dm.close();
            return uid;
        }
        catch (final InterruptedException e) {
            logger.info("Operation requestUid canceled.");
            Thread.currentThread().interrupt();
            return "";
        }
        catch (UpdaterException | KNXException e) {
            logger.error("{}An error occurred while retrieving the UID. {}{}{}",
                    ansi().fg(RED), System.lineSeparator(), e, ansi().reset());
            throw e;
        }
    }
}
