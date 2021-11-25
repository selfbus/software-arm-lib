package org.hkfree.knxduino.updater;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import java.time.Duration;
import java.util.Arrays;

import org.apache.commons.cli.ParseException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.util.zip.CRC32;

import cz.jaybee.intelhex.Parser;
import cz.jaybee.intelhex.Region;
import cz.jaybee.intelhex.listeners.BinWriter;
import cz.jaybee.intelhex.listeners.RangeDetector;

import net.harawata.appdirs.AppDirsFactory;
import org.hkfree.knxduino.updater.tests.flashdiff.BinImage;
import org.hkfree.knxduino.updater.tests.flashdiff.FlashDiff;
import tuwien.auto.calimero.IndividualAddress;
import tuwien.auto.calimero.KNXException;
import tuwien.auto.calimero.KNXFormatException;
import tuwien.auto.calimero.KNXIllegalArgumentException;
import tuwien.auto.calimero.KNXRemoteException;
import tuwien.auto.calimero.KNXTimeoutException;
import tuwien.auto.calimero.Settings;
import tuwien.auto.calimero.link.KNXLinkClosedException;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.link.KNXNetworkLinkFT12;
import tuwien.auto.calimero.link.KNXNetworkLinkIP;
import tuwien.auto.calimero.link.medium.KNXMediumSettings;
import tuwien.auto.calimero.link.medium.RFSettings;
import tuwien.auto.calimero.link.medium.TPSettings;
import tuwien.auto.calimero.mgmt.Destination;
import tuwien.auto.calimero.mgmt.KNXDisconnectException;
import tuwien.auto.calimero.mgmt.ManagementClient;
import com.google.common.primitives.Bytes;  	// For search in byte array
import tuwien.auto.calimero.mgmt.UpdatableManagementClientImpl;

/**
 * A tool for Calimero updating a KNXduino device in a KNX network.
 * <p>
 * Updater is a {@link Runnable} tool implementation allowing a user to update
 * KNXduino devices.<br>
 * <br>
 * This tool supports KNX network access using a KNXnet/IP connection or FT1.2
 * connection. It uses the {@link ManagementClient} functionality of the library
 * to read KNX device description, properties, and memory locations. It collects
 * and shows device information similar to the ETS.
 * <p>
 * When running this tool from the console, the <code>main</code>- method of
 * this class is invoked, otherwise use this class in the context appropriate to
 * a {@link Runnable}.<br>
 * In console mode, the KNX device information, as well as errors and problems
 * during its execution are written to <code>System.out</code> and logback <code>LOGGER</code>.
 * <p>
 * CAUTION:
 * To avoid disconnect problems caused by groupwrite telegrams and thereby send
 * multiple Acknowledge telegrams there is a change in
 * tuwien.auto.calimero.mgmt.TransportListenerImpl.java at line 512
 * disconnectIndicate(p, true); is commented out
 *<p>
 * Because I had to change the calimero lib, I had to import the files into the project
 * Due to other changes it was necessary to move the UpdatableManagementClientImpl into
 * tuwien.auto.calimero.mgmt folder
 * Maybe in future someone finds a better way to do this....
 *
 * @author Deti Fliegl
 * @author Pavel Kriz
 * @author Stefan Haller
 * @author Oliver Stefan
 */
public class Updater implements Runnable {
    private static final String version = "0.57"; ///\todo also change in README.md and build.gradle
    private static final String tool = "Selfbus Updater " + version;
    // private static final String sep = System.getProperty("line.separator");
    private final static Logger LOGGER = LoggerFactory.getLogger(Updater.class.getName());
    private final CliOptions cliOptions;
    // private final Map<String, Object> options = new HashMap<>();

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
        LOGGER.debug(tool);
        LOGGER.debug(Settings.getLibraryHeader(false));
        LOGGER.info(ConColors.BRIGHT_BOLD_GREEN + "\n" +
                "\n" +
                "     _____ ________    __________  __  _______    __  ______  ____  ___  ________________ \n" +
                "    / ___// ____/ /   / ____/ __ )/ / / / ___/   / / / / __ \\/ __ \\/   |/_  __/ ____/ __ \\\n" +
                "    \\__ \\/ __/ / /   / /_  / __  / / / /\\__ \\   / / / / /_/ / / / / /| | / / / __/ / /_/ /\n" +
                "   ___/ / /___/ /___/ __/ / /_/ / /_/ /___/ /  / /_/ / ____/ /_/ / ___ |/ / / /___/ _, _/ \n" +
                "  /____/_____/_____/_/   /_____/\\____//____/   \\____/_/   /_____/_/  |_/_/ /_____/_/ |_|  \n" +
                "  by Dr. Stefan Haller, Oliver Stefan et al.                       " + tool + " \n\n" +
                ConColors.RESET);
        // read in user-supplied command line options
        try {
            this.cliOptions = new CliOptions(args, String.format("SB_updater-%s-all.jar", version) , "Selfbus KNX-Firmware update tool options:", "");
            // parseOptions(args);
        } catch (final KNXIllegalArgumentException | KNXFormatException | ParseException e) {
            throw e;
        } catch (final RuntimeException e) {
            throw new KNXIllegalArgumentException(e.getMessage(), e);
        }
    }

    public static void main(final String[] args) {
        try {
            // System.setProperty(org.slf4j.impl.SimpleLogger.DEFAULT_LOG_LEVEL_KEY, "TRACE");
            final Updater d = new Updater(args);
            final ShutdownHandler sh = new ShutdownHandler().register();
            d.run();
            sh.unregister();
        } catch (final Throwable t) {
            LOGGER.error("parsing options ", t);
        } finally {
            LOGGER.info("\n\n");
            LOGGER.debug("main exit");
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
            LOGGER.info("reading device info canceled");
        if (thrown != null) {
            LOGGER.error("completed", thrown);
        }
    }

    /**
     * Restarts the <code>device</code> in normal or Bootloader mode
     * @param link
     *          the KNX network link  to communicate with the device
     * @param device
     *          the IndividualAddress of the device to restart
     */
    private void restartDeviceToBootloader(KNXNetworkLink link, IndividualAddress device) throws KNXLinkClosedException {
        UpdatableManagementClientImpl mcDevice = new UpdatableManagementClientImpl(link);
        Destination dest;
        dest = mcDevice.createDestination(device, true);
        try {
            LOGGER.info("\nRestarting device {} in bootloader mode using {}", device, link);
            mcDevice.restart(dest, 0, 255);
        } catch (final KNXException e) {
            ///\todo make restarting more reliable to determine weather it was really successful or not, right now we will always land here
            LOGGER.debug("KNXException ", e);
        } catch (final InterruptedException e) {
            LOGGER.error("InterruptedException ", e);
        }
/*
        if (m.isOpen()) {
            LOGGER.info("Disconnecting from {}", m);
            m.detach();
            m.close();
        }
 */
    }

    private void restartProgrammingDevice(UpdatableManagementClientImpl m, Destination dest)
            throws KNXTimeoutException, KNXLinkClosedException, InterruptedException {
        LOGGER.warn("restarting device {}", dest);
        m.restart(dest);
    }

    private byte[] requestUIDFromDevice(UpdatableManagementClientImpl m, Destination dest)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        LOGGER.info("\nRequesting UID from {}...", dest.getAddress());
        byte[] result;
        byte[] uid;
        result = m.sendUpdateData(dest, UPDCommand.REQUEST_UID.id, new byte[0]);
        if (result[3] != UPDCommand.RESPONSE_UID.id) {
            checkResult(result, true);
            restartProgrammingDevice(m, dest);
            throw new UpdaterException("Requesting UID failed!");
        }

        if ((result.length >= UID_LENGTH_USED) && (result.length <= UID_LENGTH_MAX)){
            uid = Arrays.copyOfRange(result, 4, UID_LENGTH_USED + 4);
            LOGGER.info("  got: {} length {}", Utils.byteArrayToHex(uid), uid.length);
            return uid;
        } else {
            uid = Arrays.copyOfRange(result, 4, result.length - 4);
            LOGGER.error("Request UID failed {} result.length={}, UID_LENGTH_USED={}, UID_LENGTH_MAX={}",
                    Utils.byteArrayToHex(uid),uid.length, UID_LENGTH_USED, UID_LENGTH_MAX);
            restartProgrammingDevice(m, dest);
            throw new UpdaterException("Selfbus update failed.");
        }
    }

    private BootLoaderIdentity requestBootLoaderIdentity(UpdatableManagementClientImpl m, Destination dest)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        LOGGER.info("\nRequesting Bootloader Identity...");
        byte[] result;
        result = m.sendUpdateData(dest, UPDCommand.REQUEST_BL_IDENTITY.id, new byte[] {0});
        if (result[3] != UPDCommand.RESPONSE_BL_IDENTITY.id)
        {
            checkResult(result);
            restartProgrammingDevice(m, dest);
            throw new UpdaterException("Requesting Bootloader Identity failed!");
        }

        BootLoaderIdentity bl = new BootLoaderIdentity(Arrays.copyOfRange(result, 4, result.length));
        LOGGER.info("  Device Bootloader: {}{}{}", ConColors.BRIGHT_YELLOW, bl, ConColors.RESET);
        return bl;
    }

    private BootDescriptor requestBootDescriptor(UpdatableManagementClientImpl m, Destination dest)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        LOGGER.info("\nRequesting Boot Descriptor...");
        byte[] result;
        result = m.sendUpdateData(dest, UPDCommand.REQUEST_BOOT_DESC.id, new byte[] {0});
        if (result[3] != UPDCommand.RESPONSE_BOOT_DESC.id) {
            checkResult(result);
            restartProgrammingDevice(m, dest);
            throw new UpdaterException("Boot descriptor request failed.");
        }
        BootDescriptor bootDescriptor = new BootDescriptor(Arrays.copyOfRange(result, 4, result.length));
        LOGGER.info("  Current firmware: {}", bootDescriptor);
        return bootDescriptor;
    }


    private String requestAppVersionString(UpdatableManagementClientImpl m, Destination dest)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException {
        byte[] result;
        LOGGER.info("\nRequesting App Version String...");
        result = m.sendUpdateData(dest, UPDCommand.APP_VERSION_REQUEST.id, new byte[] {0});
        if (result[3] != UPDCommand.APP_VERSION_RESPONSE.id){
            checkResult(result);
            //restartProgrammingDevice(mc, progDest);
            LOGGER.info("{}  failed!{}", ConColors.BRIGHT_RED, ConColors.RESET);
            return "";
        }
        String app_version = new String(result,4,result.length - 4);	// Convert 12 bytes to string starting from result[4]
        LOGGER.info("  Current App Version String is: {}{}{}", ConColors.BRIGHT_GREEN, app_version, ConColors.RESET);
        return app_version;
    }

    private void unlockDeviceWithUID(UpdatableManagementClientImpl m, Destination dest, byte[] uid)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        LOGGER.info("\nUnlocking device {} with UID {}...", dest.getAddress(), Utils.byteArrayToHex(uid));
        byte[] result;
        result = m.sendUpdateData(dest, UPDCommand.UNLOCK_DEVICE.id, uid);
        if (checkResult(result) != 0) {
            restartProgrammingDevice(m, dest);
            throw new UpdaterException("Selfbus update failed.");
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
    private KNXNetworkLink createLink() throws KNXException,
            InterruptedException, UpdaterException {
        final KNXMediumSettings medium = getMedium(cliOptions.medium());
        LOGGER.debug("Creating KNX network link {}...", medium);
        if (cliOptions.ft12().length() > 0) {
            // create FT1.2 network link
            try {
                return new KNXNetworkLinkFT12(Integer.parseInt(cliOptions.ft12()), medium);
            } catch (final NumberFormatException e) {
                return new KNXNetworkLinkFT12(cliOptions.ft12(), medium);
            }
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
            LOGGER.error("{}Routing not implemented.{}", ConColors.RED, ConColors.RESET);
            throw new UpdaterException("not implemented yet");
            //return KNXNetworkLinkIP.newRoutingLink(local, host, medium);
        } else {
            //KNXNetworkLinkIP.TUNNELING;
            return KNXNetworkLinkIP.newTunnelingLink(local, host, cliOptions.nat(), medium);
        }
    }

    /**
     * Reads all command line options, and puts those options into the options
     * map.
     * <p>
     * Default option values are added; on unknown options, a
     * KNXIllegalArgumentException is thrown.
     *
     * @param args
     *            array with command line options
     */
/*
    private void parseOptions(final String[] args) {
        if (args.length == 0)
            return;

        // add defaults
        options.put("port", KNXnetIPConnection.DEFAULT_PORT);
        // options.put("medium", TPSettings.TP1);
        options.put("medium", new TPSettings());

        int i = 0;
        for (; i < args.length; i++) {
            final String arg = args[i];
            if (isOption(arg, "--help", "-h")) {
                options.put("help", null);
                return;
            }
            if (isOption(arg, "--version", null)) {
                options.put("version", null);
                return;
            }
            if (isOption(arg, "--full", null))
                options.put("full", null);
            else if (isOption(arg, "--verbose", "-v"))
                options.put("verbose", null);
            else if (isOption(arg, "--NO_FLASH", "-f0"))
                options.put("NO_FLASH", null);
            else if (isOption(arg, "--localhost", null))
                Utils.parseHost(args[++i], true, options);
            else if (isOption(arg, "--localport", null))
                options.put("localport", Integer.decode(args[++i]));
            else if (isOption(arg, "--port", "-p"))
                options.put("port", Integer.decode(args[++i]));
            else if (isOption(arg, "--nat", "-n"))
                options.put("nat", null);
            else if (isOption(arg, "--routing", null))
                options.put("routing", null);
            else if (isOption(arg, "--serial", "-s"))
                options.put("serial", null);
            else if (isOption(arg, "--medium", "-m"))
                options.put("medium", getMedium(args[++i]));
            else if (options.containsKey("serial")
                    && options.get("serial") == null)
                // add argument as serial port number/identifier to serial
                // option
                options.put("serial", arg);
            else if (!options.containsKey("host"))
                // otherwise, add a host key with argument as host
                Utils.parseHost(arg, false, options);
            else if (isOption(arg, "--progDevice", "-D")) {
                try {
                    options.put("progDevice", new IndividualAddress(args[++i]));
                } catch (final KNXFormatException e) {
                    throw new KNXIllegalArgumentException("KNX progDevice " + e, e);
                }
            } else if (isOption(arg, "--device", "-d")) {
                try {
                    options.put("device", new IndividualAddress(args[++i]));
                } catch (final KNXFormatException e) {
                    throw new KNXIllegalArgumentException("KNX device " + e, e);
                }
            } else if (isOption(arg, "--appVersionPtr", ""))
                options.put("appVersionPtr", Integer.decode(args[++i]));
            else if (isOption(arg, "--fileName", "-f"))
                options.put("fileName", args[++i]);
            else if (isOption(arg, "--uid", "-u")) {
                String str = args[++i];
                String[] tokens = str.split(":");

                if (tokens.length == UID_LENGTH_USED) {
                    byte[] uid = new byte[tokens.length];
                    for (int n = 0; n < tokens.length; n++) {
                        uid[n] = (byte) Integer.parseUnsignedInt(tokens[n], 16);
                    }
                    options.put("uid", uid);
                }
                else {
                    LOGGER.warn("ignoring --uid {}, wrong size {}, expected {}", str, tokens.length, UID_LENGTH_USED);
                }
            }
            else if (isOption(arg, "--delay", null))
                    options.put("delay", Integer.decode(args[++i]));
            else
                throw new KNXIllegalArgumentException("unknown option " + arg);
        }
        if (options.containsKey("host") == options.containsKey("serial"))
            throw new KNXIllegalArgumentException(
                    "specify either IP host or serial port");
    }
*/
    private static InetSocketAddress createLocalSocket(final InetAddress host,
                                                       final Integer port) {
        final int p = port != null ? port.intValue() : 0;
        try {
            return host != null ? new InetSocketAddress(host, p)
                    : p != 0 ? new InetSocketAddress(
                    InetAddress.getLocalHost(), p) : null;
        } catch (final UnknownHostException e) {
            throw new KNXIllegalArgumentException("failed to get local host "
                    + e.getMessage(), e);
        }
    }

    private static KNXMediumSettings getMedium(final String id) {
        // for now, the local device address is always left 0 in the
        // created medium setting, since there is no user cmd line option for
        // this
        // so KNXnet/IP server will supply address
        //if (id.equals("tp0"))
        //	return TPSettings.TP0;
        if (id.equals("tp1"))
            return new TPSettings();
            //else if (id.equals("p110"))
            //	return new PLSettings(false);
            //else if (id.equals("p132"))
            //	return new PLSettings(true);
        else if (id.equals("rf"))
            return new RFSettings(null);
        else
            throw new KNXIllegalArgumentException("unknown medium");
    }
/*
    private static boolean isOption(final String arg, final String longOpt,
                                    final String shortOpt) {
        return arg.equals(longOpt) || arg.equals(shortOpt);
    }
*/
    private static void showVersion() {
        LOGGER.info(tool);
        LOGGER.info(Settings.getLibraryHeader(false));
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

    private static final int FLASH_SECTOR_SIZE = 4096; //!< Selfbus ARM controller flash sector size
    private static final int FLASH_PAGE_SIZE = 256;    //!< Selfbus ARM controller flash page size
    private static final int MAX_PAYLOAD = 11;         //!< maximum payload one APCI_MEMORY_READ_PDU/APCI_MEMORY_WRITE_PDU can handle
    public static final int UID_LENGTH_USED = 12;     //!< uid/guid length of the mcu used for unlocking/flashing
    private static final int UID_LENGTH_MAX = 16;      //!< uid/guid length of the mcu
    public static final int DELAY_MIN = 0;            //!< minimum delay between two UPDCommand.SEND_DATA telegrams in milliseconds
    public static final int DELAY_MAX = 500;          //!< maximum delay between two UPDCommand.SEND_DATA telegrams in milliseconds
    public static final int DELAY_DEFAULT = 100;      //!< default delay between two UPDCommand.SEND_DATA telegrams in milliseconds
    public static final int PHYS_ADDRESS_BOOTLOADER_AREA = 15;    //!< area part of the physical address the bootloader is using
    public static final int PHYS_ADDRESS_BOOTLOADER_LINE = 15;    //!< line part of the physical address the bootloader is using
    public static final int PHYS_ADDRESS_BOOTLOADER_DEVICE = 192; //!< device part of the physical address the bootloader is using

    private static final int VECTOR_TABLE_END = 0xD0;  //!> vector table end of the mcu
    private static final int BL_ID_STRING_LENGTH = 12; //!> length of bootloader identity string
    private static final byte[] APP_VER_PTR_MAGIC = {'!','A','V','P','!','@',':'};	//!> App Pointer follows this MAGIC word

    private static final String PROGRESS_MARKER = "."; //!< symbol to print progress to console

    int checkResult(byte[] result) {
        return checkResult(result, true);
    }

    int checkResult(byte[] result, boolean verbose) {
        if (result[3] != UPDCommand.SEND_LAST_ERROR.id) {
            LOGGER.error("checkResult called on other than UPDCommand.SEND_LAST_ERROR.id=0x{}, result[3]=0x{}",
                         String.format("%04X", UPDCommand.SEND_LAST_ERROR.id),
                         String.format("%04X", result[3]));
            return 0;
        }

        int resultCode = Utils.streamToInteger(result, 4);
        UDPResult udpResult = UDPResult.valueOfIndex(resultCode);

        if (udpResult.isError()) {
            LOGGER.error("{}{} resultCode=0x{}{}", ConColors.BRIGHT_RED, udpResult, String.format("%04X", resultCode), ConColors.RESET);
        } else {
            if (verbose) {
                LOGGER.info("{}done ({}){}", ConColors.BRIGHT_GREEN, resultCode, ConColors.RESET);
            } else {
                System.out.printf("%s%s%s", ConColors.BRIGHT_GREEN, PROGRESS_MARKER, ConColors.RESET); // Success in green
                LOGGER.debug(PROGRESS_MARKER);
            }
        }
        return resultCode;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Runnable#run()
     */
    public void run() {
        //TODO maybe as with the other tools, put this into a try block to also call onCompletion
         if (cliOptions.help()) {
             LOGGER.info(cliOptions.helpToString());
            return;
        }
        if (cliOptions.version()) {
            showVersion();
            return;
        }

        Exception thrown = null;
        boolean canceled = false;
        KNXNetworkLink link = null;

        try {
            // hexCacheDir will be used as a cache directory for the cached *.hex files used by the differential update mode
            // under windows: C:\Users\[currentUser]\AppData\Local\Selfbus\Selfbus-Updater\Cache\[version]
            String hexCacheDir = AppDirsFactory.getInstance().getUserCacheDir("Selfbus-Updater", version, "Selfbus");
            LOGGER.debug("hexCacheDir: {}", hexCacheDir);

            UpdatableManagementClientImpl mc;
            Destination progDest;
            IndividualAddress progDevice = cliOptions.progDevice(); //!< phys. knx address of the device in bootloader mode
            IndividualAddress device = cliOptions.device();         //!< phys. knx address of the device in normal operation

            int appVersionAddress = cliOptions.appVersionPtr();
            byte[] uid = cliOptions.uid();
            byte[] result;

            // check that the firmware file exists
            if (!Utils.fileExists(cliOptions.fileName())) {
                LOGGER.error("{}File {} does not exist!{}", ConColors.RED, cliOptions.fileName(), ConColors.RESET);
                throw new UpdaterException("Selfbus update failed.");
            }


            link = createLink();
            LOGGER.debug("Creating UpdatableManagementClient");
            mc = new UpdatableManagementClientImpl(link);
            mc.responseTimeout(Duration.ofSeconds(3));

            //for option -device restart the device in bootloader mode
            if (device != null) {
                ///\todo this still leads to a T_DISCONNECT at requestUIDFromDevice below, not sure why
                restartDeviceToBootloader(link, device);
            }

            progDest = mc.createDestination(progDevice, true, false, false);

            if (uid == null) {
                uid = requestUIDFromDevice(mc, progDest);
            }

            unlockDeviceWithUID(mc, progDest, uid);
            BootLoaderIdentity bootLoaderIdentity = requestBootLoaderIdentity(mc, progDest);
            String appVersion = requestAppVersionString(mc, progDest);

            // Load Firmware file
            LOGGER.info("Loading new firmware file '{}'", cliOptions.fileName());
            FileInputStream hexFis = new FileInputStream(cliOptions.fileName());

            // init parser
            Parser parser = new Parser(hexFis);

            // 1st iteration - calculate maximum output range
            RangeDetector rangeDetector = new RangeDetector();
            parser.setDataListener(rangeDetector);
            parser.parse();
            hexFis.getChannel().position(0);
            Region outputRegion = rangeDetector.getFullRangeRegion();
            long startAddress = outputRegion.getAddressStart();

            // 2nd iteration - actual write of the output
            ByteArrayOutputStream os = new ByteArrayOutputStream();
            BinWriter writer = new BinWriter(outputRegion, os, false);
            parser.setDataListener(writer);
            parser.parse();
            byte[] binData = os.toByteArray();
            int totalLength = binData.length;
            long endAddress = startAddress + totalLength; //TODO -1 is missing?
            ByteArrayInputStream fis = new ByteArrayInputStream(binData);
            LOGGER.info("  Hex file parsed: start at 0x{} end at 0x{}, length {} bytes",
                    String.format("%04X", startAddress), String.format("%04X", endAddress),totalLength + "");

            // Handle App Version Pointer
            if (appVersionAddress > VECTOR_TABLE_END && appVersionAddress < binData.length) {  // manually provided and not in vector or outside file length
                // Use manual set AppVersion address
                String file_version = new String(binData, appVersionAddress, BL_ID_STRING_LENGTH);	// Get app version pointers content
                LOGGER.info("  File App Version String is : {}{}{} manually specified at address 0x{}",
                        ConColors.BRIGHT_RED, file_version, ConColors.RESET, Integer.toHexString(appVersionAddress));
            }
            else {
                // Search for AppVersion pointer in flash file if not set manually
                // Search magic in image file
                appVersionAddress = Bytes.indexOf(binData, APP_VER_PTR_MAGIC) + APP_VER_PTR_MAGIC.length;
                if (appVersionAddress <= VECTOR_TABLE_END || appVersionAddress >= binData.length) {
                    appVersionAddress = 0;		// missing, or not valid set to 0
                    LOGGER.warn("  {}Could not find the App Version string, setting to 0. Please specify manually with -appVersionPtr!{}",
                            ConColors.BRIGHT_RED, ConColors.RESET);
                }
                else {
                    String fileVersion = new String(binData,appVersionAddress,BL_ID_STRING_LENGTH);	// Convert app version pointers content to string
                    LOGGER.info("  File App Version String is : {}{}{} found at address 0x{}",
                            ConColors.BRIGHT_GREEN, fileVersion, ConColors.RESET, Integer.toHexString(appVersionAddress));
                }
            }

            ///\todo why we need this dump.bin, where is it used?
            try (FileOutputStream fos = new FileOutputStream("dump.bin")) {
                fos.write(binData);
            }

            CRC32 crc32File = new CRC32();
            crc32File.update(binData, 0, totalLength);

            // store bin file in cache
            File imageCacheFile = new File(hexCacheDir + File.separator + "image-" + Long.toHexString(startAddress) + "-" + totalLength + "-" + Long.toHexString(crc32File.getValue()) + ".bin" );
            if (!imageCacheFile.getParentFile().exists()) {
                if (!imageCacheFile.getParentFile().mkdirs()) {
                    LOGGER.warn("{}Could not create bin-file cache directory {}{}",
                            ConColors.RED, imageCacheFile.getParentFile().toString(), ConColors.RESET);
                }
            }
            try (FileOutputStream fos = new FileOutputStream(imageCacheFile)) {
                fos.write(binData);
            }

            // Check if FW image has correct offset for MCUs bootloader size
            if(startAddress < bootLoaderIdentity.size())
            {
                LOGGER.error("{}  Error! The specified firmware image would overwrite parts of the bootloader. Check FW offset setting in the linker!{}", ConColors.BRIGHT_RED, ConColors.RESET);
                LOGGER.error("{}  Firmware needs to start beyond 0x{}{}", ConColors.BRIGHT_RED, String.format("%04X", bootLoaderIdentity.size()), ConColors.RESET);
                throw new UpdaterException("Firmware offset not correct!");
            }
            else if (startAddress == bootLoaderIdentity.size())
            {
                LOGGER.info("  {}Firmware starts directly beyond bootloader.{}", ConColors.BRIGHT_GREEN, ConColors.RESET);
            }
            else
            {
                LOGGER.info("  {}Info: There are {} bytes of unused flash between bootloader and firmware.{}",
                            ConColors.BRIGHT_YELLOW, startAddress-bootLoaderIdentity.size(), ConColors.RESET);
            }

            // Request current main firmware boot descriptor from device
            BootDescriptor bootDescriptor = requestBootDescriptor(mc, progDest);

            // try to find old firmware file in cache
            boolean diffMode = false;
            File oldImageCacheFile = new File(hexCacheDir + File.separator + "image-" +
                                                Long.toHexString(bootDescriptor.startAddress()) + "-" +
                                                bootDescriptor.length() + "-" +
                                                Integer.toHexString(bootDescriptor.crc32()) + ".bin" );
            if (!(cliOptions.full()) && oldImageCacheFile.exists()) {
                //TODO check that "Current App Version String" and "File App Version String" represent the same application,
                //     if they are not the same app, we should switch top full flash mode
                LOGGER.info("  Found current device firmware in cache {}", oldImageCacheFile.getAbsolutePath());
                LOGGER.info("  {}--> switching to diff upload mode{}", ConColors.BRIGHT_GREEN, ConColors.RESET);
                diffMode = true;
            }

            // Start to flash the new firmware
            LOGGER.info("\n{}{}Starting to send new firmware now:{}", ConColors.BLACK, ConColors.BG_GREEN, ConColors.RESET);
            if (diffMode) {
                doDiffFlash(mc, progDest, startAddress, binData, oldImageCacheFile);
            } else if (!cliOptions.NO_FLASH()){ // Disable flashing firmware, for debugging use only
                doFullFlash(mc, progDest, startAddress, totalLength, fis, cliOptions.delay());
            }

            BootDescriptor newBootDescriptor = new BootDescriptor(startAddress, endAddress, (int) crc32File.getValue(), startAddress + appVersionAddress);
            LOGGER.info("\n{}Preparing boot descriptor with {}{}", ConColors.BG_RED, newBootDescriptor, ConColors.RESET);
            byte[] streamBootDescriptor = newBootDescriptor.toStream();

            int nDone = 0;
            boolean timeoutOccurred = false;
            while (nDone < streamBootDescriptor.length) {
                int txSize = MAX_PAYLOAD;
                int remainBytes = streamBootDescriptor.length - nDone;
                if (remainBytes < MAX_PAYLOAD) {
                    txSize = remainBytes;
                }
                byte[] txBuf = new byte[txSize + 1];
                if (txSize >= 0) {
                    System.arraycopy(streamBootDescriptor, nDone, txBuf, 1, txSize);
                }

                // First Byte contains message number
                txBuf[0] = (byte)nDone;

                try {
                    result = mc.sendUpdateData(progDest, UPDCommand.SEND_DATA.id, txBuf);
                    if (checkResult(result, false) != 0) {
                        restartProgrammingDevice(mc, progDest);
                        throw new UpdaterException("Selfbus update failed.");
                    }
                }
                catch(KNXTimeoutException e){
                    timeoutOccurred = true;
                }
                if(!timeoutOccurred){
                    nDone += txSize;
                }else{
                    timeoutOccurred = false;
                }
            }

            CRC32 crc32Block = new CRC32();
            crc32Block.update(streamBootDescriptor);
            int newCrc32 = (int)crc32Block.getValue();

            byte[] programBootDescriptor = new byte[9];
            Utils.integerToStream(programBootDescriptor, 0, streamBootDescriptor.length);
            Utils.integerToStream(programBootDescriptor, 4, newCrc32);
            System.out.println();
            LOGGER.info("Updating boot descriptor with CRC32 0x{}, length {}",
                    Integer.toHexString(newCrc32), Long.toString(streamBootDescriptor.length));
            result = mc.sendUpdateData(progDest, UPDCommand.UPDATE_BOOT_DESC.id, programBootDescriptor);
            if (checkResult(result) != 0) {
                restartProgrammingDevice(mc, progDest);
                throw new UpdaterException("Selfbus update failed.");
            }
            Thread.sleep(500); ///\todo check if this delay is really needed
            LOGGER.info("{}Firmware Update done, Restarting device now...{}", ConColors.BG_GREEN, ConColors.RESET);
            result = mc.sendUpdateData(progDest, UPDCommand.RESET.id, new byte[] {0});  // Clean restart by application rather than lib
            if (checkResult(result) != 0) {
                restartProgrammingDevice(mc, progDest);
                throw new UpdaterException("Selfbus update failed.");
            }

        } catch (final KNXException | UpdaterException | RuntimeException e) {
            thrown = e;
        } catch (final InterruptedException e) {
            canceled = true;
            Thread.currentThread().interrupt();
        } catch (FileNotFoundException e) {
            LOGGER.error("FileNotFoundException ", e);
        } catch (IOException e) {
            LOGGER.error("IOException ", e);
        } catch (Throwable e) {
            LOGGER.error("Throwable ", e);
        } finally {
            if (link != null)
                link.close();
            onCompletion(thrown, canceled);
        }
    }

    
    
    // Differential update routine #######################
    private void doDiffFlash(UpdatableManagementClientImpl mc, Destination pd, long startAddress, byte[] binData, File oldBinFile)
            throws IOException, KNXDisconnectException, KNXTimeoutException, KNXRemoteException, KNXLinkClosedException, InterruptedException, UpdaterException {
    	long flash_time_start = System.currentTimeMillis();
    	FlashDiff differ = new FlashDiff();
        BinImage img2 = BinImage.copyFromArray(binData, startAddress);
        BinImage img1 = BinImage.readFromBin(oldBinFile.getAbsolutePath());
        differ.generateDiff(img1, img2, (outputDiffStream, crc32) -> {
            byte[] result;
            // process compressed page
            //TODO check why 5 bytes are added to size in FlashDiff.java / generateDiff(...)
            LOGGER.info("Sending new firmware ({} diff bytes)", (outputDiffStream.size() - 5));
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
                result = mc.sendUpdateData(pd, UPDCommand.SEND_DATA_TO_DECOMPRESS.id, txBuf);
                if (checkResult(result, false) != 0) {
                    restartProgrammingDevice(mc, pd);
                    throw new UpdaterException("Selfbus update failed.");
                }
            }
            // diff data of a single page transmitted
            // flash the page
            byte[] progPars = new byte[3 * 4];
            Utils.integerToStream(progPars, 0, 0);
            Utils.integerToStream(progPars, 4, 0);
            Utils.integerToStream(progPars, 8, (int) crc32);
            System.out.println();
            LOGGER.info("Program device next page diff, CRC32 0x{}", String.format("%08X", crc32));
            result = mc.sendUpdateData(pd, UPDCommand.PROGRAM_DECOMPRESSED_DATA.id, progPars);
            if (checkResult(result) != 0) {
                restartProgrammingDevice(mc, pd);
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
        ///\todo find a better way to build the infoMsg, check possible logback functions
        String infoMsg = String.format("Diff-Update: Wrote %s%d%s bytes from file to device in %tM:%<tS. %s(%.2f B/s)%s",
                ConColors.BRIGHT_GREEN, total, ConColors.RESET, flash_time_duration, col, bytesPerSecond, ConColors.RESET);
/*
        if (timeoutCount > 0) {
            infoMsg += String.format(" Timeout(s): %d%s", ConColors.BG_RED, timeoutCount, ConColors.RESET);
        }
*/
        LOGGER.info("{}", infoMsg);
    }

    private void eraseFlashPages(UpdatableManagementClientImpl mc, Destination pd, long startAddress, int totalLength)
            throws KNXDisconnectException, KNXTimeoutException, KNXRemoteException, KNXLinkClosedException, InterruptedException, UpdaterException {
        byte[] result;

        // Erasing flash on sector base (4k), one telegram per sector
        int erasePages = (totalLength / FLASH_SECTOR_SIZE) + 1;
        long startPage = startAddress / FLASH_SECTOR_SIZE;
        byte[] sector = new byte[1];
        for (int i = 0; i < erasePages; i++) {
            sector[0] = (byte) (i + startPage);
            LOGGER.info("Erase sector {}...", String.format("%2d", sector[0]));
            result = mc.sendUpdateData(pd, UPDCommand.ERASE_SECTOR.id, sector);
            if (checkResult(result) != 0) {
                restartProgrammingDevice(mc, pd);
                throw new UpdaterException("Selfbus update failed.");
            }
        }
    }
    
    // Normal update routine, sending complete image #######################
    // This works on sector page right now, so the complete affected flash is erased first
    private void doFullFlash(UpdatableManagementClientImpl mc, Destination pd, long startAddress, int totalLength, ByteArrayInputStream fis, int dataSendDelay)
            throws IOException, KNXDisconnectException, KNXTimeoutException, KNXRemoteException, KNXLinkClosedException, InterruptedException, UpdaterException {
        byte[] result;

        eraseFlashPages(mc, pd, startAddress, totalLength);

        byte[] buf = new byte[FLASH_PAGE_SIZE];	// Read one flash page
        int nRead;		                    // Bytes read from file into buffer
        int payload = MAX_PAYLOAD;	            // maximum start payload size
        int total = 0;
        CRC32 crc32Block = new CRC32();
        int progSize = 0;	                   // Bytes send so far
        long progAddress = startAddress;
        boolean doProg = false;
        boolean timeoutOccurred = false;
        int timeoutCount = 0;
        
        LOGGER.info("\nStart sending application data ({} bytes) with telegram delay of {}ms", totalLength, dataSendDelay);

        // Get time when starting to transfer data
        long flash_time_start = System.currentTimeMillis();
        
        // Read up to size of buffer, 1 Page of 256Bytes from file
        while ((nRead = fis.read(buf)) != -1) {
            LOGGER.info("Sending {} bytes: {}%", nRead, String.format("%3d", 100*(total+nRead)/totalLength));
        	// System.out.printf("Sending %d bytes: %3d%%%n", nRead, 100*(total+nRead)/totalLength);

            int nDone = 0;
            // Bytes left to write
            while (nDone < nRead) {

            	// Calculate payload size for next telegram
            	// sufficient data left, use maximum payload size
                LOGGER.debug("nDone {}, progSize {}, payLoad {}, nRead {}", nDone, progSize, payload, nRead);
            	if (progSize + payload < nRead)
				{
					payload = MAX_PAYLOAD;	// maximum payload size
				}
				else
				{
					payload = nRead - progSize;	// remaining bytes
					doProg = true;
				}
                LOGGER.debug("progSize {}, payLoad {}, nRead {}, doProg {}", progSize, payload, nRead, doProg);

                if (payload > MAX_PAYLOAD)
                {
                    // this is just a safety backup, normally we should never land here
                    LOGGER.error("payload {} > MAX_PAYLOAD {} should never happen here.", payload, MAX_PAYLOAD);
                    payload = MAX_PAYLOAD;
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
                    System.arraycopy(buf, nDone, txBuf, 1, payload);

                    // First byte contains start address of following data
                    txBuf[0] = (byte)progSize;

                    if (dataSendDelay > 0)
                    	Thread.sleep(dataSendDelay); //Reduce bus load during data upload, without 2:04, 50ms 2:33, 60ms 2:41, 70ms 2:54, 80ms 3:04
                    try{
                        result = mc.sendUpdateData(pd, UPDCommand.SEND_DATA.id, txBuf);

                        if (checkResult(result, false) != 0) {
                            restartProgrammingDevice(mc, pd);
                            throw new UpdaterException("Selfbus update failed.");
                        }
                    }
                        // mc.sendUpdateData timed out, we need to resend the last sent data
                        catch(KNXTimeoutException e){
                            timeoutOccurred = true;
                            timeoutCount++;
                    }

                    if(!timeoutOccurred) {
                        // Update CRC, skip byte 0 which is not part of payload
                    	crc32Block.update(txBuf, 1, payload);
                        nDone += payload;		// keep track of buffer bytes send (to determine end of while-loop)
                        progSize += payload;	// keep track of page/sector bytes send
                        total += payload;		// keep track of total bytes send from file
                    }
                    else { // in case of a Timeout, reset the marker timeoutOccurred
                        LOGGER.warn("{}Timeout{}", ConColors.BRIGHT_RED, ConColors.RESET);
                        // System.out.print(ConColors.BRIGHT_RED + "x" + ConColors.RESET);
                        timeoutOccurred = false;
                    }
                }

                if (doProg) {
                    doProg = false;
                    byte[] progPars = new byte[3 * 4];
                    long crc = crc32Block.getValue();
                    Utils.integerToStream(progPars, 0, progSize);
                    Utils.integerToStream(progPars, 4, progAddress);
                    Utils.integerToStream(progPars, 8, (int) crc);
                    System.out.println();
                    LOGGER.info("Program device at flash address 0x{} with {} bytes and CRC32 0x{}...",
                            String.format("%04X", progAddress), String.format("%3d", progSize), String.format("%08X", crc));
                    result = mc.sendUpdateData(pd, UPDCommand.PROGRAM.id, progPars);
                    if (checkResult(result) != 0) {
                        restartProgrammingDevice(mc, pd);
                        throw new UpdaterException("Selfbus update failed.");
                    }
                    progAddress += progSize;
                    progSize = 0;	// reset page/sector byte counter
                    crc32Block.reset();
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
            infoMsg += String.format(" %sTimeout(s): %d%s", ConColors.BG_RED, timeoutCount, ConColors.RESET);
        }
        LOGGER.info("{}", infoMsg);
    }
}
