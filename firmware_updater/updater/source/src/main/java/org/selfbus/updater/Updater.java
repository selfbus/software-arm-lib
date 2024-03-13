package org.selfbus.updater;

import com.google.common.primitives.Bytes;
import org.apache.commons.cli.ParseException;
import org.selfbus.updater.bootloader.BootDescriptor;
import org.selfbus.updater.bootloader.BootloaderIdentity;
import org.selfbus.updater.bootloader.BootloaderUpdater;
import org.selfbus.updater.upd.UDPProtocolVersion;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.IndividualAddress;
import tuwien.auto.calimero.KNXException;
import tuwien.auto.calimero.KNXFormatException;
import tuwien.auto.calimero.KNXIllegalArgumentException;
import tuwien.auto.calimero.Settings;
import tuwien.auto.calimero.knxnetip.SecureConnection;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.link.KNXNetworkLinkFT12;
import tuwien.auto.calimero.link.KNXNetworkLinkIP;
import tuwien.auto.calimero.link.KNXNetworkLinkTpuart;
import tuwien.auto.calimero.link.medium.KNXMediumSettings;
import tuwien.auto.calimero.link.medium.RFSettings;
import tuwien.auto.calimero.link.medium.TPSettings;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.util.Collections;

import static org.selfbus.updater.Utils.shortenPath;
import static org.selfbus.updater.Utils.tcpConnection;
import static tuwien.auto.calimero.knxnetip.KNXnetIPRouting.DefaultMulticast;

/**
 * A Tool for updating firmware of a Selfbus device in a KNX network.
 * <p>
 * Updater is a {@link Runnable} tool implementation allowing a user to update
 * KNXduino devices.<br>
 * <br>
 * This tool supports KNX network access using a KNXnet/IP connection FT1.2 or TPUART
 * connection. It uses the {@link SBManagementClientImpl} functionality of the library
 * to read KNX device description, properties, and memory locations. It collects
 * and shows device information similar to the ETS.
 * <p>
 * When running this tool from the console, the <code>main</code>- method of
 * this class is invoked, otherwise use this class in the context appropriate to
 * a {@link Runnable}.<br>
 * In console mode, the KNX device information, as well as errors and problems
 * during its execution are written to logback <code>LOGGER</code>.
 * <p>
 * CAUTION:
 * {@link SBManagementClientImpl} uses java reflections to get access to private field's/method's
 * of calimero-core's ManagementClientImpl
 *
 * @author Deti Fliegl
 * @author Pavel Kriz
 * @author Stefan Haller
 * @author Oliver Stefan
 */
public class Updater implements Runnable {
    private final static Logger logger = LoggerFactory.getLogger(Updater.class.getName());
    private final CliOptions cliOptions;

    /**
     * Creates a new Updater instance using the supplied options.
     * <p>
     * Mandatory arguments are an IP host/address or a FT1.2 port identifier,
     * depending on the type of connection to the KNX network, and the KNX
     * device individual address ("area.line.device"). See
     * {@link #main(String[])} for the list of options.
     *
     * @param args
     *            list with options
     * @throws KNXIllegalArgumentException
     *             on unknown/invalid options
     * @throws ParseException
     *             on invalid command line parameters
     */
    public Updater(final String[] args) throws ParseException, KNXFormatException {
        logger.debug(ToolInfo.getFullInfo());
        logger.debug(Settings.getLibraryHeader(false));
        logger.info(ConColors.BRIGHT_BOLD_GREEN +
                "     _____ ________    __________  __  _______    __  ______  ____  ___  ________________ \n" +
                "    / ___// ____/ /   / ____/ __ )/ / / / ___/   / / / / __ \\/ __ \\/   |/_  __/ ____/ __ \\\n" +
                "    \\__ \\/ __/ / /   / /_  / __  / / / /\\__ \\   / / / / /_/ / / / / /| | / / / __/ / /_/ /\n" +
                "   ___/ / /___/ /___/ __/ / /_/ / /_/ /___/ /  / /_/ / ____/ /_/ / ___ |/ / / /___/ _, _/ \n" +
                "  /____/_____/_____/_/   /_____/\\____//____/   \\____/_/   /_____/_/  |_/_/ /_____/_/ |_|  \n" +
                "  by Dr. Stefan Haller, Oliver Stefan et al.                       " + ToolInfo.getToolAndVersion() +
                ConColors.RESET);
        try {
            // read in user-supplied command line options
            this.cliOptions = new CliOptions(args, String.format("SB_updater-%s-all.jar", ToolInfo.getVersion()) ,
                    "Selfbus KNX-Firmware update tool options", "", PHYS_ADDRESS_BOOTLOADER, PHYS_ADDRESS_OWN);
        } catch (final KNXIllegalArgumentException | KNXFormatException | ParseException e) {
            throw e;
        } catch (final RuntimeException e) {
            throw new KNXIllegalArgumentException(e.getMessage(), e);
        }
    }

    public static void main(final String[] args) {
        try {
            final Updater d = new Updater(args);
            final ShutdownHandler sh = new ShutdownHandler().register();
            d.run();
            sh.unregister();
        } catch (final Throwable t) {
            logger.error("parsing options ", t);
        } finally {
            logger.info("\n\n");
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

    private KNXNetworkLink createSecureTunnelingLink(InetSocketAddress local, InetSocketAddress remote,
                                                     KNXMediumSettings medium) throws KNXException, InterruptedException {
        // KNX IP Secure TCP tunneling v2 connection
        logger.info("Connect using KNX IP Secure tunneling...");
        byte[] deviceAuthCode = SecureConnection.hashDeviceAuthenticationPassword(cliOptions.devicePassword().toCharArray());
        byte[] userKey = SecureConnection.hashUserPassword(cliOptions.userPassword().toCharArray());
        final var session = tcpConnection(local, remote).newSecureSession(cliOptions.userId(), userKey, deviceAuthCode);
        return KNXNetworkLinkIP.newSecureTunnelingLink(session, medium);
    }

    private KNXNetworkLink createTunnelingLinkV2(InetSocketAddress local, InetSocketAddress remote,
                                                 KNXMediumSettings medium) throws KNXException, InterruptedException {
        logger.info("Connect using TCP tunneling v2...");
        final var session = tcpConnection(local, remote);
        return KNXNetworkLinkIP.newTunnelingLink(session, medium);
    }

    private KNXNetworkLink createTunnelingLinkV1(InetSocketAddress local, InetSocketAddress remote, boolean useNat,
                                                     KNXMediumSettings medium) throws KNXException, InterruptedException {
        logger.info("{}Connect using UDP tunneling v1 (nat:{})...{}", ConColors.YELLOW, useNat, ConColors.RESET);
        return KNXNetworkLinkIP.newTunnelingLink(local, remote, useNat, medium);
    }

    private KNXNetworkLink createRoutingLink(InetSocketAddress local, KNXMediumSettings medium) throws KNXException {
        logger.info("{}Connect using routing (multicast:{})...{}", ConColors.YELLOW, DefaultMulticast, ConColors.RESET);

        return KNXNetworkLinkIP.newRoutingLink(local.getAddress(), DefaultMulticast, medium);
    }

    /**
     * Creates the KNX network link to access the network specified in
     * <code>options</code>.
     * <p>
     *
     * @return the KNX network link
     * @throws KNXException
     *             on problems on link creation
     * @throws InterruptedException
     *             on interrupted thread
     */
    private KNXNetworkLink createLink(IndividualAddress ownAddress) throws KNXException,
            InterruptedException, UpdaterException {
        final KNXMediumSettings medium = getMedium(cliOptions.medium(), ownAddress);
        logger.debug("Creating KNX network link {}...", medium);
        if (!cliOptions.ft12().isEmpty()) {
            // create FT1.2 network link
            try {
                return new KNXNetworkLinkFT12(Integer.parseInt(cliOptions.ft12()), medium);
            } catch (final NumberFormatException e) {
                return new KNXNetworkLinkFT12(cliOptions.ft12(), medium);
            }
        } else if (!cliOptions.tpuart().isEmpty()) {
            // create TPUART network link
            KNXNetworkLinkTpuart linkTpuart = new KNXNetworkLinkTpuart(cliOptions.tpuart(), medium, Collections.emptyList());
            linkTpuart.addAddress(cliOptions.ownAddress()); //\todo check if this is rly needed
            return linkTpuart;
        }

        // create local and remote socket address for network link
        InetSocketAddress local = createLocalSocket(cliOptions.localhost(), cliOptions.localport());

        final InetSocketAddress remote = new InetSocketAddress(cliOptions.knxInterface(), cliOptions.port());

        // Connect using KNX IP Secure
        if ((!cliOptions.devicePassword().isEmpty()) && (!cliOptions.userPassword().isEmpty())) {
            return createSecureTunnelingLink(local, remote, medium);
        }

        if (cliOptions.tunnelingV2()) {
            return createTunnelingLinkV2(local, remote, medium);
        }

        if (cliOptions.tunnelingV1()) {
            return createTunnelingLinkV1(local, remote, cliOptions.nat(), medium);
        }

        if (cliOptions.routing()) {
            return createRoutingLink(local, medium);
        }

        // try unsecure TCP tunneling v2 connection
        try {
            return createTunnelingLinkV2(local, remote, medium);
        }
        catch (final KNXException | InterruptedException e) {
            logger.info("failed with {}", e.toString());
        }

        // try unsecure UDP tunneling v1 connection with nat option set on cli
        try {
            return createTunnelingLinkV1(local, remote, cliOptions.nat(), medium);
        }
        catch (final KNXException | InterruptedException e) {
            logger.info("{}failed with {}{}", ConColors.YELLOW, e, ConColors.RESET);
        }

        // last chance try unsecure UDP tunneling v1 connection with INVERTED nat option set on cli
        return createTunnelingLinkV1(local, remote, !cliOptions.nat(), medium);
    }

    static InetSocketAddress createLocalSocket(final InetAddress host, final Integer port)
    {
        final int p = port != null ? port.intValue() : 0;
        return host != null ? new InetSocketAddress(host, p) : new InetSocketAddress(p);
    }

    private static KNXMediumSettings getMedium(final String id, IndividualAddress ownAddress) {
        if (id.equals("tp1")) {
            return new TPSettings(ownAddress);
        }
        else if (id.equals("rf")) {
            return new RFSettings(null);
        }
        //else if (id.equals("tp0"))
        //	return TPSettings.TP0;
        //else if (id.equals("p110"))
        //	return new PLSettings(false);
        //else if (id.equals("p132"))
        //	return new PLSettings(true);
        else
            throw new KNXIllegalArgumentException("unknown medium");
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

    private static void printStatisticData(long flashTimeStart, ResponseResult result) {
        // logging of some static data
        long flashTimeDuration = System.currentTimeMillis() - flashTimeStart;
        float bytesPerSecond = (float) result.written() / (flashTimeDuration / 1000f);
        String col;
        if (bytesPerSecond >= 50.0) {
            col = ConColors.BRIGHT_GREEN;
        } else {
            col = ConColors.BRIGHT_RED;
        }
        ///\todo find a better way to build the infoMsg, check possible logback functions
        String infoMsg = String.format("Wrote %d bytes from file to device in %tM:%<tS. %s(%.2f B/s)%s",
                result.written(), flashTimeDuration, col, bytesPerSecond, ConColors.RESET);

        if (result.dropCount() > 0) {
            infoMsg += String.format(" %sDisconnects: %d%s", ConColors.BRIGHT_RED, result.dropCount(), ConColors.RESET);
        } else {
            infoMsg += String.format(" %sDisconnect: %d%s", ConColors.BRIGHT_GREEN, result.dropCount(), ConColors.RESET);
        }
        if (result.timeoutCount() > 0) {
            infoMsg += String.format(" %sTimeouts: %d%s", ConColors.BRIGHT_RED, result.timeoutCount(), ConColors.RESET);
        } else {
            infoMsg += String.format(" %sTimeout: %d%s", ConColors.BRIGHT_GREEN, result.timeoutCount(), ConColors.RESET);
        }
        logger.info("{}", infoMsg);
    }

    public static final int DELAY_MIN = 0;            //!< minimum delay between two UPDCommand.SEND_DATA telegrams in milliseconds
    public static final int DELAY_MAX = 500;          //!< maximum delay between two UPDCommand.SEND_DATA telegrams in milliseconds
    public static final int DELAY_DEFAULT = 100;      //!< default delay between two UPDCommand.SEND_DATA telegrams in milliseconds
    public static final IndividualAddress PHYS_ADDRESS_BOOTLOADER = new IndividualAddress(15, 15,192); //!< physical address the bootloader is using
    public static final IndividualAddress PHYS_ADDRESS_OWN = new IndividualAddress(0, 0,0); //!< physical address the Selfbus Updater is using

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
            if (cliOptions.help()) {
                logger.info(cliOptions.helpToString());
                return;
            }
            if (cliOptions.version()) {
                ToolInfo.showVersion();
                return;
            }

            byte[] uid = cliOptions.uid();
            final String hexFileName = cliOptions.fileName();
            BinImage newFirmware = null;

            // if --uid is set, we need a valid firmware file
            if (uid != null) {
                // check if the firmware file exists
                if (!Utils.fileExists(hexFileName)) {
                    logger.error("{}File {} does not exist!{}", ConColors.RED, cliOptions.fileName(), ConColors.RESET);
                    throw new UpdaterException("Selfbus update failed.");
                }
            }

            if (!hexFileName.isEmpty()) {
                // Load Firmware hex file
                logger.info("Loading file '{}'...", hexFileName);
                newFirmware = BinImage.readFromHex(hexFileName);
                logger.info("Firmware: {}", newFirmware);
            }

            link = createLink(cliOptions.ownAddress()); // default 15.15.193

            DeviceManagement dm = new DeviceManagement(link, cliOptions.progDevice(), cliOptions.priority());

            logger.info("KNX connection: {}", link);
            logger.info("Telegram priority: {}", cliOptions.priority());

            //for option --device restart the device in bootloader mode
            if (cliOptions.device() != null) { // phys. knx address of the device in normal operation
                dm.checkDeviceInProgrammingMode(null); // check that before no device is in programming mode
                dm.restartDeviceToBootloader(link, cliOptions.device());
            }

            dm.checkDeviceInProgrammingMode(cliOptions.progDevice());

            if (uid == null) {
                uid = dm.requestUIDFromDevice();
            }

            dm.unlockDeviceWithUID(uid);

            if ((cliOptions.dumpFlashStartAddress() >= 0) && (cliOptions.dumpFlashEndAddress() >= 0)) {
                logger.warn("{}Dumping flash content range 0x{}-0x{} to bootloader's serial port.{}",
                        ConColors.BRIGHT_GREEN, String.format("%04X", cliOptions.dumpFlashStartAddress()), String.format("%04X", cliOptions.dumpFlashEndAddress()), ConColors.RESET);
                dm.dumpFlashRange(cliOptions.dumpFlashStartAddress(), cliOptions.dumpFlashEndAddress());
                return;
            }

            BootloaderIdentity bootLoaderIdentity = dm.requestBootloaderIdentity();
            logger.info("Requesting App Version String...");
            String appVersion = dm.requestAppVersionString();
            if (appVersion != null) {
                logger.info("  Current App Version String: {}{}{}", ConColors.BRIGHT_GREEN, appVersion, ConColors.RESET);
            }
            else {
                logger.info("{}  failed!{}", ConColors.BRIGHT_RED, ConColors.RESET);
            }

            //  From here on we need a valid firmware
            if (newFirmware == null) {
                if (cliOptions.device() != null) {
                    dm.restartProgrammingDevice();
                }
                // to get here `uid == null` must be true, so it's fine to exit with no-error
                System.exit(0);
            }

            if (cliOptions.eraseFullFlash()) {
                logger.warn("{}Deleting the entire flash except from the bootloader itself!{}", ConColors.BRIGHT_RED, ConColors.RESET);
                dm.eraseFlash();
            }

            // store new firmware bin file in cache directory
            String cacheFileName = FlashDiffMode.createCacheFileName(newFirmware.startAddress(), newFirmware.length(), newFirmware.crc32());
            BinImage imageCache = BinImage.copyFromArray(newFirmware.getBinData(), newFirmware.startAddress());
            imageCache.writeToBinFile(cacheFileName);

            // Handle App Version Pointer
            int appVersionAddress = cliOptions.appVersionPtr();
            String fileVersion = "";
            if (appVersionAddress > Mcu.VECTOR_TABLE_END && appVersionAddress < (newFirmware.length() - Mcu.BL_ID_STRING_LENGTH)) {  // manually provided and not in vector or outside file length
                // Use manual set AppVersion address
                fileVersion = new String(newFirmware.getBinData(), appVersionAddress, Mcu.BL_ID_STRING_LENGTH); // Get app version pointers content
                logger.info("  File App Version String   : {}{}{} manually specified at address 0x{}",
                        ConColors.BRIGHT_RED, fileVersion, ConColors.RESET, Integer.toHexString(appVersionAddress));
            }
            else {
                // Search for AppVersion pointer in flash file if not set manually, Search magic bytes in image file
                appVersionAddress = Bytes.indexOf(newFirmware.getBinData(), Mcu.APP_VER_PTR_MAGIC) + Mcu.APP_VER_PTR_MAGIC.length;
                if (appVersionAddress <= Mcu.VECTOR_TABLE_END || appVersionAddress >= (newFirmware.length() - Mcu.BL_ID_STRING_LENGTH)) {
                    appVersionAddress = 0; // missing, or not valid set to 0
                    logger.warn("  {}Could not find the App Version string, setting to 0. Please specify manually with {}{}",
                            ConColors.BRIGHT_RED, CliOptions.OPT_LONG_APP_VERSION_PTR, ConColors.RESET);
                }
                else {
                    fileVersion = new String(newFirmware.getBinData(), appVersionAddress, Mcu.BL_ID_STRING_LENGTH); // Convert app version pointers content to string
                    logger.info("  File App Version String   : {}{}{} found at address 0x{}",
                            ConColors.BRIGHT_GREEN, fileVersion, ConColors.RESET, Integer.toHexString(appVersionAddress));
                }
            }

            // Check if FW image has correct offset for MCUs bootloader size
            if (newFirmware.startAddress() < bootLoaderIdentity.getApplicationFirstAddress()) {
                logger.error("{}  Error! The specified firmware image would overwrite parts of the bootloader. Check FW offset setting in the linker!{}", ConColors.BRIGHT_RED, ConColors.RESET);
                logger.error("{}  Firmware needs to start at or beyond 0x{}{}", ConColors.BRIGHT_RED, String.format("%04X", bootLoaderIdentity.getApplicationFirstAddress()), ConColors.RESET);
                throw new UpdaterException("Firmware offset not correct!");
            }
            else if (newFirmware.startAddress() == bootLoaderIdentity.getApplicationFirstAddress()) {
                logger.info("  {}Firmware starts directly beyond bootloader.{}", ConColors.BRIGHT_GREEN, ConColors.RESET);
            }
            else {
                logger.info("  {}Info: There are {} bytes of unused flash between bootloader and firmware.{}",
                        ConColors.BRIGHT_YELLOW, newFirmware.startAddress() - bootLoaderIdentity.getApplicationFirstAddress(), ConColors.RESET);
            }

            // Request current main firmware boot descriptor from device
            BootDescriptor bootDescriptor = dm.requestBootDescriptor();

            boolean diffMode = false;
            if (!(cliOptions.full())) {
                if (bootDescriptor.valid()) {
                    diffMode = FlashDiffMode.setupDifferentialMode(bootDescriptor);
                }
                else {
                    logger.error("{}  BootDescriptor is not valid -> switching to full mode{}", ConColors.BRIGHT_RED, ConColors.RESET);
                }
            }

            if ((bootLoaderIdentity.getVersionMajor()) <= 1 && (bootLoaderIdentity.getVersionMinor() < 20)) {
                dm.setProtocolVersion(UDPProtocolVersion.UDP_V0);
            }
            else {
                dm.setProtocolVersion(UDPProtocolVersion.UDP_V1);
            }

            if (!cliOptions.NO_FLASH()) { // is flashing firmware disabled? for debugging use only!
                // Start to flash the new firmware
                long flashTimeStart = System.currentTimeMillis(); // time flash process started
                ResponseResult resultTotal;
                logger.info("\n{}{}Starting to send new firmware now:{}", ConColors.BLACK, ConColors.BG_GREEN, ConColors.RESET);
                if (diffMode && FlashDiffMode.isInitialized()) {
                    logger.error("{}Differential mode is EXPERIMENTAL -> Use with caution.{}", ConColors.BRIGHT_RED, ConColors.RESET);
                    resultTotal = FlashDiffMode.doDifferentialFlash(dm, newFirmware.startAddress(), newFirmware.getBinData());
                }
                else {
                    resultTotal = FlashFullMode.doFullFlash(dm, newFirmware, cliOptions.delay(), !cliOptions.eraseFullFlash(), cliOptions.logStatistics());
                }
                logger.info("\nRequesting Bootloader statistic...");
                dm.requestBootLoaderStatistic();
                printStatisticData(flashTimeStart, resultTotal);
            }
            else {
                logger.warn("--{} => {}only boot description block will be written{}", CliOptions.OPT_LONG_NO_FLASH, ConColors.RED, ConColors.RESET);
            }

            BootDescriptor newBootDescriptor = new BootDescriptor(newFirmware.startAddress(),
                    newFirmware.endAddress(),
                    (int) newFirmware.crc32(),
                    newFirmware.startAddress() + appVersionAddress);
            logger.info("\n{}Preparing boot descriptor with {}{}", ConColors.BG_RED, newBootDescriptor, ConColors.RESET);
            dm.programBootDescriptor(newBootDescriptor, cliOptions.delay());
            String deviceInfo = cliOptions.progDevice().toString();
            if (cliOptions.device() != null) {
                deviceInfo = cliOptions.device().toString();
            }
            logger.info("\nFinished programming {}device {} with {}{}\n", ConColors.BRIGHT_YELLOW, deviceInfo, shortenPath(cliOptions.fileName(), 1), ConColors.RESET);
            logger.info("{}Firmware Update done, Restarting device now...{}", ConColors.BG_GREEN, ConColors.RESET);
            dm.restartProgrammingDevice();

            if (fileVersion.contains(BootloaderUpdater.BOOTLOADER_UPDATER_ID_STRING)) {
                logger.info("{}Wait {} second(s) for Bootloader Updater to finish its job...{}", ConColors.BG_GREEN,
                        String.format("%.2f", BootloaderUpdater.BOOTLOADER_UPDATER_MAX_RESTART_TIME_MS / 1000.0f), ConColors.RESET);
                Thread.sleep(BootloaderUpdater.BOOTLOADER_UPDATER_MAX_RESTART_TIME_MS);
            }
        } catch (final KNXException | UpdaterException | RuntimeException e) {
            thrown = e;
        } catch (final InterruptedException e) {
            canceled = true;
            Thread.currentThread().interrupt();
        } catch (FileNotFoundException e) {
            logger.error("FileNotFoundException ", e);
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
}
