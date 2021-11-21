package org.hkfree.knxduino.updater;

import java.io.*;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.util.zip.CRC32;

import cz.jaybee.intelhex.Parser;
import cz.jaybee.intelhex.Region;
import cz.jaybee.intelhex.listeners.BinWriter;
import cz.jaybee.intelhex.listeners.RangeDetector;
import net.harawata.appdirs.AppDirs;
import net.harawata.appdirs.AppDirsFactory;
import org.hkfree.knxduino.updater.tests.flashdiff.BinImage;
import org.hkfree.knxduino.updater.tests.flashdiff.FlashDiff;
import tuwien.auto.calimero.*;
import tuwien.auto.calimero.knxnetip.KNXnetIPConnection;
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
import tuwien.auto.calimero.mgmt.UpdatableManagementClientImpl;
import com.google.common.primitives.Bytes;  	// For search in byte array

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
 * during its execution are written to <code>System.out</code>.
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
    private static final String version = "0.56";
    private static final String tool = "Selfbus Updater " + version;
    private static final String sep = System.getProperty("line.separator");
    private final static Logger LOGGER = LoggerFactory.getLogger(Updater.class.getName());
    private final Map<String, Object> options = new HashMap<>();

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
     */
    public Updater(final String[] args) {
        System.out.println(ConColors.BRIGHT_BOLD_GREEN + "\n" +
                "\n" +
                "     _____ ________    __________  __  _______    __  ______  ____  ___  ________________ \n" +
                "    / ___// ____/ /   / ____/ __ )/ / / / ___/   / / / / __ \\/ __ \\/   |/_  __/ ____/ __ \\\n" +
                "    \\__ \\/ __/ / /   / /_  / __  / / / /\\__ \\   / / / / /_/ / / / / /| | / / / __/ / /_/ /\n" +
                "   ___/ / /___/ /___/ __/ / /_/ / /_/ /___/ /  / /_/ / ____/ /_/ / ___ |/ / / /___/ _, _/ \n" +
                "  /____/_____/_____/_/   /_____/\\____//____/   \\____/_/   /_____/_/  |_/_/ /_____/_/ |_|  \n" +
                "  by Dr. Stefan Haller, Oliver Stefan et al.                                 Version 0.56B \n\n" +
                ConColors.RESET);
        // read in user-supplied command line options
        try {
            parseOptions(args);
        } catch (final KNXIllegalArgumentException e) {
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
            LOGGER.error("completed ", thrown);
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
            InterruptedException {
        LOGGER.debug("Creating KNX network link...");
        final KNXMediumSettings medium = (KNXMediumSettings) options
                .get("medium");
        if (options.containsKey("serial")) {
            // create FT1.2 network link
            final String p = (String) options.get("serial");
            try {
                return new KNXNetworkLinkFT12(Integer.parseInt(p), medium);
            } catch (final NumberFormatException e) {
                return new KNXNetworkLinkFT12(p, medium);
            }
        }
        // create local and remote socket address for network link
        InetSocketAddress local = createLocalSocket(
                (InetAddress) options.get("localhost"),
                (Integer) options.get("localport"));
        if (local == null) local = new InetSocketAddress(0);
        final InetSocketAddress host = new InetSocketAddress(
                (InetAddress) options.get("host"),
                ((Integer) options.get("port")).intValue());
        if (options.containsKey("routing")) {
            //KNXNetworkLinkIP.ROUTING
            //return KNXNetworkLinkIP.newRoutingLink(local, host,	options.containsKey("nat"), medium);
            throw new RuntimeException("not implemented yet");
            //return KNXNetworkLinkIP.newRoutingLink(local, host, medium);
        } else {
            //KNXNetworkLinkIP.TUNNELING;
            return KNXNetworkLinkIP.newTunnelingLink(local, host, options.containsKey("nat"), medium);
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
            if (isOption(arg, "-help", "-h")) {
                options.put("help", null);
                return;
            }
            if (isOption(arg, "-version", null)) {
                options.put("version", null);
                return;
            }
            if (isOption(arg, "-full", null))
                options.put("full", null);
            else if (isOption(arg, "-verbose", "-v"))
                options.put("verbose", null);
            else if (isOption(arg, "-NO_FLASH", "-f0"))
                options.put("NO_FLASH", null);
            else if (isOption(arg, "-localhost", null))
                parseHost(args[++i], true, options);
            else if (isOption(arg, "-localport", null))
                options.put("localport", Integer.decode(args[++i]));
            else if (isOption(arg, "-port", "-p"))
                options.put("port", Integer.decode(args[++i]));
            else if (isOption(arg, "-nat", "-n"))
                options.put("nat", null);
            else if (isOption(arg, "-routing", null))
                options.put("routing", null);
            else if (isOption(arg, "-serial", "-s"))
                options.put("serial", null);
            else if (isOption(arg, "-medium", "-m"))
                options.put("medium", getMedium(args[++i]));
            else if (options.containsKey("serial")
                    && options.get("serial") == null)
                // add argument as serial port number/identifier to serial
                // option
                options.put("serial", arg);
            else if (!options.containsKey("host"))
                // otherwise add a host key with argument as host
                parseHost(arg, false, options);
            else if (isOption(arg, "-progDevice", "-D")) {
                try {
                    options.put("progDevice", new IndividualAddress(args[++i]));
                } catch (final KNXFormatException e) {
                    throw new KNXIllegalArgumentException("KNX device "
                            + e.toString(), e);
                }
            } else if (isOption(arg, "-device", "-d")) {
                try {
                    options.put("device", new IndividualAddress(args[++i]));
                } catch (final KNXFormatException e) {
                    throw new KNXIllegalArgumentException("KNX device "
                            + e.toString(), e);
                }
            } else if (isOption(arg, "-appVersionPtr", ""))
                options.put("appVersionPtr", Integer.decode(args[++i]));
            else if (isOption(arg, "-fileName", "-f"))
                options.put("fileName", args[++i]);
            else if (isOption(arg, "-uid", "-u")) {
                String str = args[++i];
                String[] tokens = str.split(":");

                if (tokens.length == UID_LENGTH_USED) {
                    byte uid[] = new byte[tokens.length];
                    for (int n = 0; n < tokens.length; n++) {
                        uid[n] = (byte) Integer.parseUnsignedInt(tokens[n], 16);
                    }
                    options.put("uid", uid);
                }
                else {
                    LOGGER.warn("ignoring -uid {}, wrong size {}, expected {}", str, tokens.length, UID_LENGTH_USED);
                }
            }
            else if (isOption(arg, "-delay", null))
                    options.put("delay", Integer.decode(args[++i]));
            else
                throw new KNXIllegalArgumentException("unknown option " + arg);
        }
        if (options.containsKey("host") == options.containsKey("serial"))
            throw new KNXIllegalArgumentException(
                    "specify either IP host or serial port");
    }

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

    private static void parseHost(final String host, final boolean local,
                                  final Map<String, Object> options) {
        try {
            if (local) {
                options.put("localhost", InetAddress.getByName(host));
            }
            else {
                options.put("host", InetAddress.getByName(host));
            }
        } catch (final UnknownHostException e) {
            throw new KNXIllegalArgumentException(
                    "failed to read host " + host, e);
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
            //return TPSettings.TP1;
            //else if (id.equals("p110"))
            //	return new PLSettings(false);
            //else if (id.equals("p132"))
            //	return new PLSettings(true);
        else if (id.equals("rf"))
            return new RFSettings(null);
        else
            throw new KNXIllegalArgumentException("unknown medium");
    }

    private static boolean isOption(final String arg, final String longOpt,
                                    final String shortOpt) {
        return arg.equals(longOpt) || shortOpt != null && arg.equals(shortOpt);
    }

    private static void showUsage() {
        final StringBuffer sb = new StringBuffer();
        sb.append(tool).append(sep).append(sep);
        sb.append("usage: SB_updater.jar")
                .append(" [options] <KNX Interface> -fileName <filename.hex> -device <KNX device address>")
                .append(sep).append(sep);
        sb.append("options:").append(sep);
        sb.append(" -help -h                 show this help message")
                .append(sep);
        sb.append(" -fileName                Filename of hex file to program")
                .append(sep);
        sb.append(" -version                 show tool/library version and exit")
                .append(sep);
        sb.append(" -verbose -v              enable verbose status output")
                .append(sep);
        sb.append(" -localhost <id>          local IP/host name").append(sep);
        sb.append(
                " -localport <number>      local UDP port (default system assigned)")
                .append(sep);
        sb.append(" -port -p <number>        UDP port on <host> (default ")
                .append(KNXnetIPConnection.DEFAULT_PORT).append(")")
                .append(sep);
        // sb.append(" -host <id>              remote IP/host name").append(sep);
        sb.append(" -nat -n                  enable Network Address Translation")
                .append(sep);
        sb.append(" -serial -s               use FT1.2 serial communication")
                .append(sep);
        sb.append(" -routing                 use KNXnet/IP routing").append(sep);
        sb.append(
                " -medium -m <id>          KNX medium [tp0|tp1|p110|p132|rf] "
                        + "(default tp1)").append(sep);
        sb.append(
                " -progDevice              KNX device address of bootloader (default 15.15.192)")
                .append(sep);
        sb.append(
                " -device <knxid>          KNX device address in normal operating mode (default none)")
                .append(sep);
        sb.append(" -appVersionPtr <hex/dev> pointer to APP_VERSION string in new firmware file")
                .append(sep);
        sb.append(
                " -uid <hex>               send UID to unlock (default: request UID to unlock)")
                .append(sep);
        sb.append(String.format("                          only the first %d bytes of UID are used", UID_LENGTH_USED))
                .append(sep);
        sb.append(" -full                    force full upload mode (disable diff)")
                .append(sep);
        sb.append(" -delay                   delay telegrams during data transmission to reduce bus load (valid 0-500)")
        		.append(sep);
        sb.append(" -NO_FLASH                for debugging use only, disable flashing firmware!")
                .append(sep);
        LOGGER.info(sb.toString());
    }

    private static void showVersion() {
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

    public static final int FLASH_SECTOR_SIZE = 4096; //!< Selfbus ARM controller flash sector size
    public static final int FLASH_PAGE_SIZE = 256;    //!< Selfbus ARM controller flash page size
    private static final int MAX_PAYLOAD = 11;        //!< maximum payload one APCI_MEMORY_READ_PDU/APCI_MEMORY_WRITE_PDU can handle
    private static final int UID_LENGTH_USED = 12;    //!< uid/guid length of the mcu used for unlocking/flashing

    int streamToInteger(byte[] stream, int offset) {
        //TODO this doesn't work for UDP_NOT_IMPLEMENTED
        // with stream[0]=0xFF and stream[1]=0xFF return is -1 but should be 0xFFFF
        // byte is signed in java -127...128, so that's our problem...
        return (stream[offset + 3] << 24) | (stream[offset + 2] << 16)
                | (stream[offset + 1] << 8) | stream[offset + 0];
    }

    void integerToStream(byte[] stream, int offset, long val) {
        stream[offset + 3] = (byte) (val >> 24);
        stream[offset + 2] = (byte) (val >> 16);
        stream[offset + 1] = (byte) (val >> 8);
        stream[offset + 0] = (byte) (val);
    }

    int checkResult(byte[] result) {
        return checkResult(result, true);
    }

    int checkResult(byte[] result, boolean verbose) {
        if (result[3] != UPDCommand.SEND_LAST_ERROR.id) {
            return 0;
        }

        int resultCode = streamToInteger(result, 4);
        UDPResult udpResult = UDPResult.valueOfIndex(resultCode);
        String msgResult;
        Boolean isError = true;
        final String PROGRESS_DOT = ".";
        switch (udpResult) {
            case UID_MISMATCH:
                msgResult = "failed, UID mismatch.";
                break;
            case SECTOR_NOT_ALLOWED_TO_ERASE:
                msgResult = "Sector not allowed being erased.";
                break;
            case RAM_BUFFER_OVERFLOW:
                msgResult = "RAM Buffer Overflow";
                break;
            case CRC_ERROR:
                msgResult = "CRC error, try option -full for a clean and full flash";
                break;
            case ADDRESS_NOT_ALLOWED_TO_FLASH:
                msgResult = "Address not allowed to flash";
                break;
            case DEVICE_LOCKED:
                msgResult = "Device locked. Programming mode on device must be active!";
                break;
            case APPLICATION_NOT_STARTABLE:
                msgResult = "Application not startable";
                break;
            case WRONG_DESCRIPTOR_BLOCK:
                msgResult = "Boot Descriptor block wrong";
                break;
            case ERASE_FAILED:
                msgResult = "Flash page erase failed";
                break;
            case FLASH_ERROR:
                msgResult = "Flash page could not be programmed";
                break;
            case PAGE_NOT_ALLOWED_TO_ERASE:
                msgResult = "Flash page not allowed to erase";
                break;
            case NOT_IMPLEMENTED:
                msgResult = "Command not implemented";
                break;
            case UNKNOWN_COMMAND:
                msgResult = "Command unknown";
                break;
            case OK:
                isError = false;
                if (verbose) {
                    msgResult = "done (" + resultCode + ")";
                } else {
                    msgResult = PROGRESS_DOT;
                }
                break;
            default:
                msgResult = "unknown error (" + resultCode + ").";
        }

        if (isError == true) {
            System.out.println(ConColors.BRIGHT_RED + msgResult + ConColors.RESET); // Print errors in red
            LOGGER.error(msgResult);
        }
        else
        {
            System.out.print(ConColors.BRIGHT_GREEN + msgResult + ConColors.RESET); // Success in green
            if (msgResult != PROGRESS_DOT)
            {
                System.out.println();
            }
            LOGGER.debug(msgResult);
        }
        return resultCode;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Runnable#run()
     */
    public void run() {
        //TODO ??? as with the other tools, maybe put this into the try block to
        // also call onCompletion
        AppDirs appDirs = AppDirsFactory.getInstance();
        //Diese Angaben werden genutzt, um den Ordner für die Cache Files des DiffUpdaters abzulegen
        //Unter Windows: C:\Users\[currentUser]\AppData\Local\Selfbus\Selfbus-Updater\Cache\[version]
        String hexCacheDir = appDirs.getUserCacheDir("Selfbus-Updater", version, "Selfbus");

        if (options.isEmpty()) {
            LOGGER.info(tool);
            showVersion();
            LOGGER.info("type -help for help message");
            return;
        }
        if (options.containsKey("help")) {
            showUsage();
            return;
        }
        if (options.containsKey("version")) {
            showVersion();
            return;
        }

        Exception thrown = null;
        boolean canceled = false;
        KNXNetworkLink link = null;

        try {
            UpdatableManagementClientImpl mc;
            Destination pd;
            String fName = "";
            int appVersionAddr = 0;
            IndividualAddress progDevice = new IndividualAddress(15, 15, 192);
            IndividualAddress device = null;
            byte uid[] = null;
            byte result[] = null;
            boolean DO_FLASH_WRITE = true;
            int data_send_delay = 0;	// no delay

            if (options.containsKey("fileName")) {
                fName = (String) options.get("fileName");
            }
            if (options.containsKey("appVersionPtr")) {
                appVersionAddr = (int) options.get("appVersionPtr");
            }
            if (options.containsKey("progDevice")) {
                progDevice = (IndividualAddress) options.get("progDevice");
            }
            if (options.containsKey("device")) {
                device = (IndividualAddress) options.get("device");
            }
            if (options.containsKey("uid")) {
                uid = (byte[]) options.get("uid");
            }
            if (options.containsKey("NO_FLASH")) {
                DO_FLASH_WRITE = false;	// Disable flashing firmware, for debugging use only
            }
            if (options.containsKey("delay")) {
            	data_send_delay = (int) options.get("delay");
            	if ((data_send_delay <0) || (data_send_delay >500))
            		data_send_delay = 100;	// set to 100ms in case of invalid waiting time
            }

            link = createLink();
            LOGGER.debug("Creating UpdatableManagementClient...");
            mc = new UpdatableManagementClientImpl(link);


            if (device != null) {
                Destination d;
                d = mc.createDestination(device, true);
                LOGGER.info("Attempting to restart device " + device.toString() + " in bootloader mode ...");
                try {
                    mc.restart(d, 0, 255);
                } catch (final KNXException e) {

                }

                /*
                try {
                    //TODO this was a wrong attempt, resulting in a T_DISCONNECT_PDU for the progDevice
                    mc = new UpdatableManagementClientImpl(link); //.restart will result in a timeout and exception
                } catch(final KNXException e){
                    System.out.println("Restarting device " + device.toString() + " failed");
                    throw new RuntimeException("Selfbus update failed.");
                }
                 */
                //Thread.sleep(1000);	// currently not needed because calimero currently times out waiting for an ACK
            }

            pd = mc.createDestination(progDevice, true);

            if (uid == null) {
                LOGGER.info("Requesting UID from " + progDevice.toString() + " ... ");
                result = mc.sendUpdateData(pd, UPDCommand.REQUEST_UID.id, new byte[0]);
                checkResult(result, true);
                if ((result.length >= UID_LENGTH_USED) && (result.length <= 16)){
                    uid = new byte[UID_LENGTH_USED];
                    // TODO this can be merged with the section below
                    System.out.print("got: ");
                    for (int i = 0; i < result.length - 4; i++) {
                        uid[i] = result[i + 4];
                        if (i != 0) {
                            System.out.print(":");
                        }
                        System.out.print(String.format("%02X", uid[i] & 0xff));
                    }
                    System.out.println();
                } else {
                    System.out.println("Request UID failed");
                    mc.restart(pd);
                    throw new RuntimeException("Selfbus update failed.");
                }
            }

            String txt = "Unlocking device with UID ";
            for (int i = 0; i < uid.length; i++) {
                if (i != 0) {
                    txt += ":";
                }
                txt += String.format("%02X", uid[i] & 0xff);
            }
            txt += " ...";
            LOGGER.info(txt);

            result = mc.sendUpdateData(pd, UPDCommand.UNLOCK_DEVICE.id, uid);
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException("Selfbus update failed.");
            }

            result = mc.sendUpdateData(pd, UPDCommand.REQUEST_BL_IDENTITY.id, new byte[] {0});
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException("Requesting Bootloader Identity failed!");
            }
            long BL_Identity = (result[4] & 0xFF) + ((result[5] & 0xFF) << 8) + ((result[6] & 0xFF) << 16) + ((long)(result[7] & 0xFF) << 24);
            long BL_Features = (result[8] & 0xFF) + ((result[9] & 0xFF) << 8) + ((result[10] & 0xFF) << 16) + ((long)(result[11] & 0xFF) << 24);
            long BL_Size = (result[12] & 0xFF) + ((result[13] & 0xFF) << 8) + ((result[14] & 0xFF) << 16) + ((long)(result[15] & 0xFF) << 24);
            System.out.println("Device Bootloader Identity   : " + ConColors.BRIGHT_YELLOW + "0x" +  String.format("%04X", BL_Identity)
                    + ", Features: 0x" + String.format("%04X", BL_Features) + ", Bootloader Size: 0x" + String.format("%04X", BL_Size) + ConColors.RESET);

            System.out.print("\n\rRequesting App Version String ...");
            result = mc.sendUpdateData(pd, UPDCommand.APP_VERSION_REQUEST.id, new byte[] {0});
            if (checkResult(result) != 0) {
                //mc.restart(pd);
                System.out.println(ConColors.BRIGHT_RED + " failed!" + ConColors.RESET);
            }
            String app_version = new String(result,4,12);	// Convert 12 bytes to string starting from result[4]
            System.out.println("\n  Current App Version String is: " + ConColors.BRIGHT_GREEN + app_version + ConColors.RESET + "\n");


            // Load Firmware file
            System.out.println("Loading new firmware file ...");
            FileInputStream hexFis = new FileInputStream(fName);

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
            System.out.println("  Hex file parsed: start at 0x" + String.format("%04X", startAddress) + " end at 0x"
                    + String.format("%04X", endAddress) + ", length " + totalLength + " bytes");

            // Handle App Version Pointer
            if (appVersionAddr > 0xD0 && appVersionAddr < binData.length) {  // manually provided and not in vector or outside file length
                // Use manual set AppVersion address
                String file_version = new String(binData,appVersionAddr,12);	// Get app version pointers content
                System.out.println("  File App Version String is : " + ConColors.BRIGHT_RED + file_version + ConColors.RESET +
                        " manually specified at address 0x" + String.format("%08X", appVersionAddr));
            }

            // Search for AppVersion pointer in flash file if not set manually
            else {
                byte[] APP_VER_PTR_MAGIC = {'!','A','V','P','!','@',':'};	// App Pointer follows this MAGIC
                String file_version = ("Sadly EMPTY!");
                // Search magic in image file
                appVersionAddr = Bytes.indexOf(binData, APP_VER_PTR_MAGIC) + APP_VER_PTR_MAGIC.length;
                if (appVersionAddr <= 0xD0 || appVersionAddr >= binData.length) {
                    appVersionAddr = 0;		// missing, or not valid set to 0

                    System.out.println(ConColors.BRIGHT_RED + "  Could not find the App Version string, setting to 0. "
                            + "Please specify manually with -appVersionPtr!" + ConColors.RESET);
                }
                else {
                    file_version = new String(binData,appVersionAddr,12);	// Convert app version pointers content to string

                    System.out.println("  File App Version String is : " + ConColors.BRIGHT_GREEN + file_version + ConColors.RESET +
                            " found at address 0x" + String.format("%08X", appVersionAddr));
                }
            }

            try (FileOutputStream fos = new FileOutputStream("dump.bin")) {
                fos.write(binData);
            }

            CRC32 crc32File = new CRC32();
            crc32File.update(binData, 0, totalLength);

            // store bin file in cache
            File imageCacheFile = new File(hexCacheDir + File.separator + "image-" + Long.toHexString(startAddress) + "-" + totalLength + "-" + Long.toHexString(crc32File.getValue()) + ".bin" );
            imageCacheFile.getParentFile().mkdirs();
            try (FileOutputStream fos = new FileOutputStream(imageCacheFile)) {
                fos.write(binData);
            }

            // Check if FW image has correct offset for MCUs bootloader size
            if(startAddress < BL_Size)
            {
                System.out.println(ConColors.BRIGHT_RED+"  Error! The specified firmware image would overwrite parts of the bootloader. Check FW offset setting in the linker!");
                System.out.println("  Firmware needs to start beyond 0x" + String.format("%04X", BL_Size) + ConColors.RESET);
                throw new RuntimeException("Firmware offset not correct!");
            }
            else if(startAddress == BL_Size)
            {
                System.out.println(ConColors.BRIGHT_GREEN+"  Firmware starts directly beyond bootloader." + ConColors.RESET);
            }
            else
            {
                System.out.println(ConColors.BRIGHT_YELLOW+"  Info: There are " + Long.toString(startAddress-BL_Size) + " bytes of unused flash between bootloader and firmware." + ConColors.RESET);
            }

            // Request current main firmware boot descriptor from device
            System.out.println("\nRequesting Boot Descriptor ...");
            result = mc.sendUpdateData(pd, UPDCommand.REQUEST_BOOT_DESC.id, new byte[] {0});
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException("Boot descriptor request failed.");
            }
            long descrStartAddr = (result[4] & 0xFF) + ((result[5] & 0xFF) << 8) + ((result[6] & 0xFF) << 16) + ((long)(result[7] & 0xFF) << 24);
            long descrEndAddr = (result[8] & 0xFF) + ((result[9] & 0xFF) << 8) + ((result[10] & 0xFF) << 16) + ((long)(result[11] & 0xFF) << 24);
            long descrCrc = (result[12] & 0xFF) + ((result[13] & 0xFF) << 8) + ((result[14] & 0xFF) << 16) + ((long)(result[15] & 0xFF) << 24);
            long descrLength = descrEndAddr - descrStartAddr;
            System.out.println("  Current firmware: starts at 0x" +  String.format("%04X", descrStartAddr) + " ends at 0x" + String.format("%04X", descrEndAddr) + " CRC is 0x" + String.format("%08X", descrCrc));

            // try to find old firmware file in cache
            boolean diffMode = false;
            File oldImageCacheFile = new File(hexCacheDir + File.separator + "image-" + Long.toHexString(descrStartAddr) + "-" + descrLength + "-" + Long.toHexString(descrCrc) + ".bin" );
            if (!(options.containsKey("full")) && oldImageCacheFile.exists()) {
                //TODO check that "Current App Version String" and "File App Version String" represent the same application,
                //     if they are not the same app, we should switch top full flash mode
                System.out.println("  Found current device firmware in cache " + oldImageCacheFile.getAbsolutePath() + ConColors.BRIGHT_GREEN +"\n\r  --> switching to diff upload mode" + ConColors.RESET);
                diffMode = true;
            }
/*
            Boolean diffMode = false;
            Long startAddress = 0x7000L;
            int totalLength = 0x200;
            eraseFlashPages(mc, pd, startAddress, totalLength);
            LOGGER.error("finished");
*/
            // Start to flash the new firmware
            System.out.println("\n" + ConColors.BRIGHT_BG_BLUE + "Starting to send new firmware now: " + ConColors.RESET);
            if (diffMode) {
                doDiffFlash(mc, pd, startAddress, binData, oldImageCacheFile);
            } else if (DO_FLASH_WRITE){
                doFullFlash(mc, pd, startAddress, totalLength, fis, data_send_delay);
            } else {
                eraseFlashPages(mc, pd, startAddress, totalLength);
            }

            byte bootDescriptor[] = new byte[16];
            appVersionAddr += startAddress;	//DONE why should we add here something?, yes we have to: Add FW offset to get absolute position in MCU
            integerToStream(bootDescriptor, 0, startAddress);
            integerToStream(bootDescriptor, 4, endAddress);
            integerToStream(bootDescriptor, 8, (int) crc32File.getValue());
            integerToStream(bootDescriptor, 12, appVersionAddr);
            System.out
                    .println("\n" + ConColors.BG_RED + "Preparing boot descriptor with start address 0x"
                            + String.format("%04X", startAddress)
                            + ", end address 0x"
                            + String.format("%04X", endAddress)
                            + ", CRC32 0x"
                            + String.format("%08X", crc32File.getValue())
                            + ", APP_VERSION pointer 0x"
                            + String.format("%04X", appVersionAddr) + ConColors.RESET);

            int nDone = 0;
            boolean timeoutOccurred = false;
            while (nDone < bootDescriptor.length) {
                int txSize = 11;
                int remainBytes = bootDescriptor.length - nDone;
                if (remainBytes < 11) {
                    txSize = remainBytes;
                }
                byte txBuf[] = new byte[txSize + 1];
                for (int i = 0; i < txSize; i++) {
                    txBuf[i + 1] = bootDescriptor[nDone + i];
                }
                // First Byte contains message number
                txBuf[0] = (byte)nDone;

                try {
                    result = mc.sendUpdateData(pd, UPDCommand.SEND_DATA.id, txBuf);
                    if (checkResult(result, false) != 0) {
                        mc.restart(pd);
                        throw new RuntimeException("Selfbus update failed.");
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
            crc32Block.update(bootDescriptor);

            byte programBootDescriptor[] = new byte[9];
            integerToStream(programBootDescriptor, 0,
                    (int) bootDescriptor.length);
            integerToStream(programBootDescriptor, 4,
                    (int) crc32Block.getValue());
            System.out.print("Update boot descriptor with CRC32 0x"
                    + String.format("%08X", crc32Block.getValue())
                    + ", length "
                    + Long.toString(bootDescriptor.length) + " ");
            result = mc.sendUpdateData(pd, UPDCommand.UPDATE_BOOT_DESC.id,
                    programBootDescriptor);
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException("Selfbus update failed.");
            }
            Thread.sleep(500);
            System.out.println(ConColors.BG_GREEN + "Firmware Update done, Restarting device now ..." + ConColors.RESET + "\n\r\n\r");
            //mc.restart(pd);
            mc.sendUpdateData(pd, UPDCommand.RESET.id, new byte[] {0});  // Clean restart by application rather than lib

        } catch (final KNXException e) {
            thrown = e;
        } catch (final RuntimeException e) {
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
    private void doDiffFlash(UpdatableManagementClientImpl mc, Destination pd, long startAddress, byte[] binData, File oldBinFile) throws IOException, KNXDisconnectException, KNXTimeoutException, KNXRemoteException, KNXLinkClosedException, InterruptedException {
    	long flash_time_start = System.currentTimeMillis();
    	FlashDiff differ = new FlashDiff();
        BinImage img2 = BinImage.copyFromArray(binData, startAddress);
        BinImage img1 = BinImage.readFromBin(oldBinFile.getAbsolutePath());
        differ.generateDiff(img1, img2, (outputDiffStream, crc32) -> {
            byte[] result;
            // process compressed page
            System.out.print("Sending new firmware (" + (outputDiffStream.size() - 5) + " diff bytes) "); //TODO 5 bytes are added to size in FlashDiff.java / generateDiff(...), check why
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
                    mc.restart(pd);
                    throw new RuntimeException("Selfbus update failed.");
                }
            }
            // diff data of a single page transmitted
            // flash the page
            byte[] progPars = new byte[3 * 4];
            integerToStream(progPars, 0, 0);
            integerToStream(progPars, 4, 0);
            integerToStream(progPars, 8, (int) crc32);
            if (options.containsKey("verbose")) {
                System.out.println();
                System.out.print("Program device next page diff, CRC32 0x" + String.format("%08X", crc32) + " ... ");
            }
            result = mc.sendUpdateData(pd, UPDCommand.PROGRAM_DECOMPRESSED_DATA.id, progPars);
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException(
                        "Selfbus update failed.");
            }
        });
        long flash_time_duration = System.currentTimeMillis() - flash_time_start;
        System.out.printf("Diff-Update required %tM:%<tS.", flash_time_duration);
        float bytesPerSecond = (float)((float)(differ.getTotalBytesTransferred())/(flash_time_duration/1000));
        if (bytesPerSecond >= 50.0) {
            System.out.printf(ConColors.BRIGHT_GREEN + " (%.2f B/s)" + ConColors.RESET, bytesPerSecond);
        }
        else {
            System.out.printf(ConColors.BRIGHT_RED + " (%.2f B/s)" + ConColors.RESET, bytesPerSecond);
        }

    }

    private void eraseFlashPages(UpdatableManagementClientImpl mc, Destination pd, long startAddress, int totalLength)
            throws IOException, KNXDisconnectException, KNXTimeoutException, KNXRemoteException, KNXLinkClosedException, InterruptedException {
        byte[] result;

        // Erasing flash on sector base (4k), one telegram per sector
        int erasePages = (totalLength / FLASH_SECTOR_SIZE) + 1;
        long startPage = startAddress / FLASH_SECTOR_SIZE;
        byte[] sector = new byte[1];
        for (int i = 0; i < erasePages; i++) {
            sector[0] = (byte) (i + startPage);
            LOGGER.info("Erase sector " + String.format("%2d", sector[0]) + " ...");
            result = mc.sendUpdateData(pd, UPDCommand.ERASE_SECTOR.id, sector);
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException("Selfbus update failed.");
            }
        }
    }
    
    // Normal update routine, sending complete image #######################
    // This works on sector page right now, so the complete affected flash is erased first
    private void doFullFlash(UpdatableManagementClientImpl mc, Destination pd, long startAddress, int totalLength, ByteArrayInputStream fis, int data_send_delay)
            throws IOException, KNXDisconnectException, KNXTimeoutException, KNXRemoteException, KNXLinkClosedException, InterruptedException {
        byte[] result;

        eraseFlashPages(mc, pd, startAddress, totalLength);

        byte[] buf = new byte[FLASH_PAGE_SIZE];	// Read one flash page
        int nRead = 0;		                    // Bytes read from file into buffer
        int payload = MAX_PAYLOAD;	            // maximum start payload size
        int total = 0;
        CRC32 crc32Block = new CRC32();
        int progSize = 0;	                   // Bytes send so far
        long progAddress = startAddress;
        boolean doProg = false;
        boolean timeoutOccured = false;
        int timeoutCount = 0;
        
        if (data_send_delay != 0)
        	System.out.printf("%nStart sending application data (%d bytes) with telegram delay of %dms%n", totalLength, data_send_delay);
        else
        	System.out.printf("%nStart sending application data (%d bytes)%n", totalLength);

        // Get time when starting to transfer data
        long flash_time_start = System.currentTimeMillis();
        
        // Read up to size of buffer, 1 Page of 256Bytes from file
        while ((nRead = fis.read(buf)) != -1) {
        	
        	System.out.printf("Sending %d bytes: %3d%%%n", nRead, 100*(total+nRead)/totalLength);
        	
            int nDone = 0;
            // Bytes left to write
            while (nDone < nRead) {

            	// Calculate payload size for next telegram
            	// sufficient data left, use maximum payload size
            	if (progSize + payload < nRead)
				{
					payload = MAX_PAYLOAD;	// maximum payload size
				}
				else
				{
					payload = nRead - progSize;	// remaining bytes
					doProg = true;
				}

                if (payload > MAX_PAYLOAD)
                {
                    // this is just a safety backup, normally we should never land here
                    LOGGER.error("payload {} > MAX_PAYLOAD {} should never happen here.", payload, MAX_PAYLOAD);
                    payload = MAX_PAYLOAD;
                    doProg = false;
                }
/*
            	// Handle last telegram with <= 11 bytes
                if ((total + nRead) == totalLength)
                {
                    if (nRead <= MAX_PAYLOAD) { // Handle last telegram with less than 11 bytes
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

                    for (int i = 0; i < payload; i++) {
                        txBuf[i + 1] = buf[i + nDone];	//Shift payload into send buffer
                    }
                    // First byte contains start address of following data
                    txBuf[0] = (byte)progSize;

                    if (data_send_delay != 0)
                    	Thread.sleep(data_send_delay);	//Reduce bus load during data upload, ohne 2:04, 50ms 2:33, 60ms 2:41, 70ms 2:54, 80ms 3:04
                    try{
                        result = mc.sendUpdateData(pd, UPDCommand.SEND_DATA.id, txBuf);

                        if (checkResult(result, false) != 0) {
                            mc.restart(pd);
                            throw new RuntimeException(
                                    "Selfbus update failed.");
                        }
                    }
                    // wenn eine Timeout Exception gesendet wird, ist ein Fehler bei der Datenübertragung vorgekommen
                    // daher werden die letzten Daten noch einmal neu gesendet
                    catch(KNXTimeoutException e){
                        timeoutOccured = true;
                        timeoutCount++;
                    }

                    if(!timeoutOccured) { // wenn kein Timeout Fehler vorgekommen ist, soll die Abarbeitung fortgesetzt werden
                        // Update CRC, skip byte 0 which is not part of payload
                    	crc32Block.update(txBuf, 1, payload);
                        nDone += payload;		// keep track of buffer bytes send (to determine end of while-loop)
                        progSize += payload;	// keep track of page/sector bytes send
                        total += payload;		// keep track of total bytes send from file
                    }else{ // wenn ein Timeout passiert ist, muss der Merker wieder zurückgesetzt werden
                        LOGGER.warn("Timeout");
                        System.out.print(ConColors.BRIGHT_RED + "x" );
                        System.out.print(ConColors.RESET);
                        timeoutOccured = false;
                    }
                }

                if (doProg) {
                    doProg = false;
                    byte[] progPars = new byte[3 * 4];
                    long crc = crc32Block.getValue();
                    integerToStream(progPars, 0, progSize);
                    integerToStream(progPars, 4, progAddress);
                    integerToStream(progPars, 8, (int) crc);
                    System.out.println();
                    String txt = String.format("Program device at flash address 0x%04X with %3d bytes and CRC32 0x%08X ... ",
                                                progAddress, progSize, crc);
                    LOGGER.info(txt);
                    result = mc.sendUpdateData(pd, UPDCommand.PROGRAM.id,
                            progPars);
                    if (checkResult(result) != 0) {
                        mc.restart(pd);
                        throw new RuntimeException(
                                "Selfbus update failed.");
                    }
                    progAddress += progSize;
                    progSize = 0;	// reset page/sector byte counter
                    crc32Block.reset();
                }
            }
        }
        long flash_time_duration = System.currentTimeMillis() - flash_time_start;
        fis.close();
        System.out.printf("Wrote %d bytes from file to device in %tM:%<tS.", total, flash_time_duration);
        float bytesPerSecond = (float)((float)(total)/(flash_time_duration/1000));
        if (bytesPerSecond >= 50.0) {
            System.out.printf(ConColors.BRIGHT_GREEN + " (%.2f B/s)" + ConColors.RESET, bytesPerSecond);
        }
        else {
            System.out.printf(ConColors.BRIGHT_RED + " (%.2f B/s)" + ConColors.RESET, bytesPerSecond);
        }

        if (timeoutCount > 0) {
            System.out.printf(ConColors.BG_RED + "Timeout(s): %d" + ConColors.RESET, timeoutCount);
        }
        //Thread.sleep(100);	//TODO, can be removed
    }
}
