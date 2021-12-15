package org.selfbus.updater;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;

import org.apache.commons.cli.ParseException;
import org.selfbus.updater.bootloader.BootDescriptor;
import org.selfbus.updater.bootloader.BootLoaderIdentity;
import org.selfbus.updater.upd.UPDCommand;
import org.selfbus.updater.upd.UPDProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Collections;
import java.util.zip.CRC32;

import tuwien.auto.calimero.*;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.link.KNXNetworkLinkFT12;
import tuwien.auto.calimero.link.KNXNetworkLinkIP;
import tuwien.auto.calimero.link.KNXNetworkLinkTpuart;
import tuwien.auto.calimero.link.medium.KNXMediumSettings;
import tuwien.auto.calimero.link.medium.RFSettings;
import tuwien.auto.calimero.link.medium.TPSettings;
import com.google.common.primitives.Bytes;  	// For search in byte array

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
        logger.info(ConColors.BRIGHT_BOLD_GREEN + "\n" +
                "\n" +
                "     _____ ________    __________  __  _______    __  ______  ____  ___  ________________ \n" +
                "    / ___// ____/ /   / ____/ __ )/ / / / ___/   / / / / __ \\/ __ \\/   |/_  __/ ____/ __ \\\n" +
                "    \\__ \\/ __/ / /   / /_  / __  / / / /\\__ \\   / / / / /_/ / / / / /| | / / / __/ / /_/ /\n" +
                "   ___/ / /___/ /___/ __/ / /_/ / /_/ /___/ /  / /_/ / ____/ /_/ / ___ |/ / / /___/ _, _/ \n" +
                "  /____/_____/_____/_/   /_____/\\____//____/   \\____/_/   /_____/_/  |_/_/ /_____/_/ |_|  \n" +
                "  by Dr. Stefan Haller, Oliver Stefan et al.                       " + ToolInfo.getToolAndVersion() + " \n\n" +
                ConColors.RESET);
        try {
            // read in user-supplied command line options
            this.cliOptions = new CliOptions(args, String.format("SB_updater-%s-all.jar", ToolInfo.getVersion()) , "Selfbus KNX-Firmware update tool options", "");
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
        if (cliOptions.ft12().length() > 0) {
            // create FT1.2 network link
            try {
                return new KNXNetworkLinkFT12(Integer.parseInt(cliOptions.ft12()), medium);
            } catch (final NumberFormatException e) {
                return new KNXNetworkLinkFT12(cliOptions.ft12(), medium);
            }
        } else if (cliOptions.tpuart().length() > 0) {
            // create TPUART network link
            KNXNetworkLinkTpuart linkTpuart = new KNXNetworkLinkTpuart(cliOptions.tpuart(), medium, Collections.emptyList());
            linkTpuart.addAddress(cliOptions.ownAddress()); //\todo check if this is rly needed
            return linkTpuart;
        }

        // create local and remote socket address for network link
        InetSocketAddress local;
        local = createLocalSocket(cliOptions.localhost(), cliOptions.localport());
        if (local == null) {
            local = new InetSocketAddress(0);
        }

        final InetSocketAddress host;
        host = new InetSocketAddress(cliOptions.knxInterface(), cliOptions.port());
        if (cliOptions.routing()) {
            //KNXNetworkLinkIP.ROUTING
            //return KNXNetworkLinkIP.newRoutingLink(local, host,	cliOptions.nat(), medium);
            logger.error("{}Routing not implemented.{}", ConColors.RED, ConColors.RESET);
            throw new UpdaterException("Routing not implemented.");
            //return KNXNetworkLinkIP.newRoutingLink(local, host, medium);
        } else {
            //KNXNetworkLinkIP.TUNNELING;
            return KNXNetworkLinkIP.newTunnelingLink(local, host, cliOptions.nat(), medium);
        }
        ///\todo for secure connections try
        // KNXNetworkLinkIP.newSecureTunnelingLink(local, host, cliOptions.nat(), byte[] deviceAuthCode, int userID, byte[] userKey);
    }

    private static InetSocketAddress createLocalSocket(final InetAddress host,
                                                       final Integer port) {
        final int p = port != null ? port : 0;
        try {
            return host != null ? new InetSocketAddress(host, p)
                    : p != 0 ? new InetSocketAddress(
                    InetAddress.getLocalHost(), p) : null;
        } catch (final UnknownHostException e) {
            throw new KNXIllegalArgumentException("failed to get local host "
                    + e.getMessage(), e);
        }
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



    public static final int DELAY_MIN = 0;            //!< minimum delay between two UPDCommand.SEND_DATA telegrams in milliseconds
    public static final int DELAY_MAX = 500;          //!< maximum delay between two UPDCommand.SEND_DATA telegrams in milliseconds
    public static final int DELAY_DEFAULT = 100;      //!< default delay between two UPDCommand.SEND_DATA telegrams in milliseconds
    public static final int PHYS_ADDRESS_BOOTLOADER_AREA = 15;    //!< area part of the physical address the bootloader is using
    public static final int PHYS_ADDRESS_BOOTLOADER_LINE = 15;    //!< line part of the physical address the bootloader is using
    public static final int PHYS_ADDRESS_BOOTLOADER_DEVICE = 192; //!< device part of the physical address the bootloader is using

    public static final int PHYS_ADDRESS_OWN_AREA = 0;    //!< area part of the physical address the Selfbus Updater is using
    public static final int PHYS_ADDRESS_OWN_LINE = 0;    //!< line part of the physical address the Selfbus Updater is using
    public static final int PHYS_ADDRESS_OWN_DEVICE = 0; //!< device part of the physical address the Selfbus Updater is using

    public static final int RESPONSE_TIMEOUT_SEC = 2; //!< Time in seconds the Updater shall wait for a KNX Response

    private static final int VECTOR_TABLE_END = 0xD0;  //!> vector table end of the mcu
    private static final int BL_ID_STRING_LENGTH = 12; //!> length of bootloader identity string
    private static final byte[] APP_VER_PTR_MAGIC = {'!','A','V','P','!','@',':'};	//!> App Pointer follows this MAGIC word

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

            // check if the firmware file to be programmed really exists
            if (!Utils.fileExists(cliOptions.fileName())) {
                logger.error("{}File {} does not exist!{}", ConColors.RED, cliOptions.fileName(), ConColors.RESET);
                throw new UpdaterException("Selfbus update failed.");
            }

            int appVersionAddress = cliOptions.appVersionPtr();
            byte[] uid = cliOptions.uid();
            ResponseResult result;
            link = createLink(cliOptions.ownAddress()); // default 15.15.193

            DeviceManagement dm = new DeviceManagement(link, cliOptions.progDevice(), RESPONSE_TIMEOUT_SEC);

            logger.info("KNX connection: {}\n", link);
            logger.debug("Creating UpdatableManagementClient");

            //for option -device restart the device in bootloader mode
            if (cliOptions.device() != null) { //!< phys. knx address of the device in normal operation
                dm.restartDeviceToBootloader(link, cliOptions.device());
            }

            if (uid == null) {
                uid = dm.requestUIDFromDevice();
            }

            dm.unlockDeviceWithUID(uid);

            if (cliOptions.eraseFlash()) {
                logger.warn("{}Deleting the entire flash except from the bootloader itself!{}", ConColors.BRIGHT_RED, ConColors.RESET);
                dm.eraseFlash();
            }

            if ((cliOptions.dumpFlashStartAddress() >= 0) && (cliOptions.dumpFlashEndAddress() >= 0)) {
                logger.warn("{}Dumping flash content range 0x{}-0x{} to bootloader's serial port.{}",
                        ConColors.BRIGHT_GREEN, String.format("%04X", cliOptions.dumpFlashStartAddress()), String.format("%04X", cliOptions.dumpFlashEndAddress()),  ConColors.RESET);
                dm.dumpFlashRange(cliOptions.dumpFlashStartAddress(), cliOptions.dumpFlashEndAddress());
                return;
            }

            BootLoaderIdentity bootLoaderIdentity = dm.requestBootLoaderIdentity();
            logger.info("\nRequesting App Version String...");
            String appVersion = dm.requestAppVersionString();
            if (appVersion != null) {
                logger.info("  Current App Version String is: {}{}{}", ConColors.BRIGHT_GREEN, appVersion, ConColors.RESET);
            }
            else {
                logger.info("{}  failed!{}", ConColors.BRIGHT_RED, ConColors.RESET);
            }

            // Load Firmware hex file
            logger.info("Loading file '{}'...", cliOptions.fileName());
            BinImage newFirmware = BinImage.readFromHex(cliOptions.fileName());
            logger.info("  Hex file parsed: {}", newFirmware);

            // store new firmware bin file in cache directory
            String cacheFileName = FlashDiffMode.createCacheFileName(newFirmware.startAddress(), newFirmware.length(), newFirmware.crc32());
            BinImage imageCache = BinImage.copyFromArray(newFirmware.getBinData(), newFirmware.startAddress());
            imageCache.writeToBinFile(cacheFileName);

            // Handle App Version Pointer
            if (appVersionAddress > VECTOR_TABLE_END && appVersionAddress < newFirmware.length()) {  // manually provided and not in vector or outside file length
                // Use manual set AppVersion address
                String fileVersion = new String(newFirmware.getBinData(), appVersionAddress, BL_ID_STRING_LENGTH);	// Get app version pointers content
                logger.info("  File App Version String is : {}{}{} manually specified at address 0x{}",
                        ConColors.BRIGHT_RED, fileVersion, ConColors.RESET, Integer.toHexString(appVersionAddress));
            }
            else {
                // Search for AppVersion pointer in flash file if not set manually, Search magic bytes in image file
                appVersionAddress = Bytes.indexOf(newFirmware.getBinData(), APP_VER_PTR_MAGIC) + APP_VER_PTR_MAGIC.length;
                if (appVersionAddress <= VECTOR_TABLE_END || appVersionAddress >= newFirmware.length()) {
                    appVersionAddress = 0;		// missing, or not valid set to 0
                    logger.warn("  {}Could not find the App Version string, setting to 0. Please specify manually with {}{}",
                            ConColors.BRIGHT_RED, CliOptions.OPT_LONG_APP_VERSION_PTR, ConColors.RESET);
                }
                else {
                    String fileVersion = new String(newFirmware.getBinData(), appVersionAddress, BL_ID_STRING_LENGTH);	// Convert app version pointers content to string
                    logger.info("  File App Version String is : {}{}{} found at address 0x{}",
                            ConColors.BRIGHT_GREEN, fileVersion, ConColors.RESET, Integer.toHexString(appVersionAddress));
                }
            }

            // Check if FW image has correct offset for MCUs bootloader size
            if (newFirmware.startAddress() < bootLoaderIdentity.applicationFirstAddress()) {
                logger.error("{}  Error! The specified firmware image would overwrite parts of the bootloader. Check FW offset setting in the linker!{}", ConColors.BRIGHT_RED, ConColors.RESET);
                logger.error("{}  Firmware needs to start at or beyond 0x{}{}", ConColors.BRIGHT_RED, String.format("%04X", bootLoaderIdentity.applicationFirstAddress()), ConColors.RESET);
                throw new UpdaterException("Firmware offset not correct!");
            }
            else if (newFirmware.startAddress() == bootLoaderIdentity.applicationFirstAddress()) {
                logger.info("  {}Firmware starts directly beyond bootloader.{}", ConColors.BRIGHT_GREEN, ConColors.RESET);
            }
            else {
                logger.info("  {}Info: There are {} bytes of unused flash between bootloader and firmware.{}",
                            ConColors.BRIGHT_YELLOW, newFirmware.startAddress() - bootLoaderIdentity.applicationFirstAddress(), ConColors.RESET);
            }

            // Request current main firmware boot descriptor from device
            BootDescriptor bootDescriptor = dm.requestBootDescriptor();

            boolean diffMode = false;
            if ((!(cliOptions.full()))) {
                if (bootDescriptor.valid()) {
                    diffMode = FlashDiffMode.setupDifferentialMode(bootDescriptor);
                }
                else {
                    logger.error("{}  BootDescriptor is not valid -> switching to full mode{}", ConColors.BRIGHT_RED, ConColors.RESET);
                }
            }

            if (!cliOptions.NO_FLASH()) { // is flashing firmware disabled? for debugging use only!
                // Start to flash the new firmware
                logger.info("\n{}{}Starting to send new firmware now:{}", ConColors.BLACK, ConColors.BG_GREEN, ConColors.RESET);
                if (diffMode && FlashDiffMode.isInitialized()) {
                    logger.error("{}Differential mode is currently disabled -> switching to full mode{}", ConColors.BRIGHT_RED, ConColors.RESET);
                    // FlashDiffMode.doDifferentialFlash(dm, newFirmware.startAddress(), newFirmware.getBinData());
                    FlashFullMode.doFullFlash(dm, newFirmware, cliOptions.delay());
                }
                else {
                    FlashFullMode.doFullFlash(dm, newFirmware, cliOptions.delay());
                }
            }
            else {
                logger.warn("--NO_FLASH => {}only boot description block will be written{}", ConColors.RED, ConColors.RESET);
            }

            BootDescriptor newBootDescriptor = new BootDescriptor(newFirmware.startAddress(),
                    newFirmware.endAddress(),
                    (int)newFirmware.crc32(),
                    newFirmware.startAddress() + appVersionAddress);
            logger.info("\n{}Preparing boot descriptor with {}{}", ConColors.BG_RED, newBootDescriptor, ConColors.RESET);
            byte[] streamBootDescriptor = newBootDescriptor.toStream();

            ///\todo separate FlashFullMode.doFullFlash(..) into erasePages and doFlash. Use doFlash for programming newBootDescriptor
            int nDone = dm.doFlash(streamBootDescriptor, -1);
            if (cliOptions.delay() > 0) {
                Thread.sleep(cliOptions.delay()); //Reduce bus load during data upload
            }

            ///todo this UPDATE_BOOT_DESC part, also needs a complete rework
            CRC32 crc32Block = new CRC32();
            crc32Block.update(streamBootDescriptor);
            int newCrc32 = (int)crc32Block.getValue();
            byte[] programBootDescriptor = new byte[9];
            Utils.longToStream(programBootDescriptor, 0, streamBootDescriptor.length);
            Utils.longToStream(programBootDescriptor, 4, newCrc32);
            System.out.println();
            logger.info("Updating boot descriptor with CRC32 0x{}, length {}",
                    Integer.toHexString(newCrc32), Long.toString(streamBootDescriptor.length));
            result = dm.sendWithRetry(UPDCommand.UPDATE_BOOT_DESC, programBootDescriptor, DeviceManagement.MAX_UPD_COMMAND_RETRY);
            if (UPDProtocol.checkResult(result.data()) != 0) {
                dm.restartProgrammingDevice();
                throw new UpdaterException("Selfbus update failed.");
            }

            Thread.sleep(500); ///\todo check if this delay is really needed
            logger.info("{}Firmware Update done, Restarting device now...{}", ConColors.BG_GREEN, ConColors.RESET);
            result = dm.sendWithRetry(UPDCommand.RESET, new byte[]{0}, DeviceManagement.MAX_UPD_COMMAND_RETRY);  // Clean restart by application rather than lib
            if (UPDProtocol.checkResult(result.data()) != 0) {
                dm.restartProgrammingDevice();
                throw new UpdaterException("Selfbus update failed.");
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
