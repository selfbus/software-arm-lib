package org.hkfree.knxduino.updater;

import java.io.*;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.Base64;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.zip.CRC32;

import cz.jaybee.intelhex.Parser;
import cz.jaybee.intelhex.Region;
import cz.jaybee.intelhex.listeners.BinWriter;
import cz.jaybee.intelhex.listeners.RangeDetector;
import net.harawata.appdirs.AppDirs;
import net.harawata.appdirs.AppDirsFactory;
import org.hkfree.knxduino.updater.tests.flashdiff.BinImage;
import org.hkfree.knxduino.updater.tests.flashdiff.FlashDiff;
import org.hkfree.knxduino.updater.tests.flashdiff.FlashPage;
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
 * Due to other changes it was nessesary to move the UpdatableManagementClientImpl into
 * tuwien.auto.calimero.mgmt folder
 * Maybe in future someone finds a better way to do this....
 *
 * @author Deti Fliegl
 * @author Pavel Kriz
 * @author Stefan Haller
 * @author Oliver Stefan
 */
public class Updater implements Runnable {
    private static final String tool = "Selfbus Updater 0.54";
    private static final String sep = System.getProperty("line.separator");
    private final static Logger LOGGER = Logger.getLogger(Updater.class.getName());

    private final Map options = new HashMap();

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
            final Updater d = new Updater(args);
            final ShutdownHandler sh = new ShutdownHandler().register();
            d.run();
            sh.unregister();
        } catch (final Throwable t) {
            LOGGER.log(Level.SEVERE, "parsing options " + t);
            t.printStackTrace();
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
            LOGGER.log(Level.INFO, "reading device info canceled");
        if (thrown != null) {
            LOGGER.log(Level.SEVERE, "completed " + thrown);
            thrown.printStackTrace();
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
        options.put("port", new Integer(KNXnetIPConnection.DEFAULT_PORT));
        options.put("medium", TPSettings.TP1);

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
            if (isOption(arg, "-full", null)) {
                options.put("full", null);
                return;
            }
            if (isOption(arg, "-verbose", "-v"))
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

                if (tokens.length == 12) {
                    byte uid[] = new byte[12];
                    for (int n = 0; n < tokens.length; n++) {
                        uid[n] = (byte) Integer.parseUnsignedInt(tokens[n], 16);
                    }
                    options.put("uid", uid);
                }

            } else
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
                                  final Map options) {
        try {
            options.put(local ? "localhost" : "host",
                    InetAddress.getByName(host));
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
            return TPSettings.TP1;
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
        sb.append("usage: KNXduino-updater.jar")
                .append(" [options] <host|port> -fileName <filename.hex> -device <KNX device address>")
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
                " -progDevice              KNX device address used for programming (default 15.15.192)")
                .append(sep);
        sb.append(
                " -device <knxid>          KNX device address in normal operating mode (default none)")
                .append(sep);
        sb.append(" -appVersionPtr <hex/dev> pointer to APP_VERSION string in new firmware file")
                .append(sep);
        sb.append(
                " -uid <hex>               send UID to unlock (default: request UID to unlock)")
                .append(sep);
        sb.append(" -full                    force full upload mode (disable diff)")
                .append(sep);
        LOGGER.log(Level.INFO, sb.toString());
    }

    private static void showVersion() {
        LOGGER.log(Level.INFO, Settings.getLibraryHeader(false));
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

    public static final int UPD_ERASE_PAGES = 0;
    public static final int UPD_SEND_DATA = 1;
    public static final int UPD_PROGRAM = 2;
    public static final int UPD_UPDATE_BOOT_DESC = 3;
    public static final int UPD_SEND_DATA_TO_DECOMPRESS = 4;
    public static final int UPD_PROGRAM_DECOMPRESSED_DATA = 5;
    public static final int UPD_REQ_DATA = 10;
    public static final int UPD_GET_LAST_ERROR = 20;
    public static final int UPD_SEND_LAST_ERROR = 21;
    public static final int UPD_UNLOCK_DEVICE = 30;
    public static final int UPD_REQUEST_UID = 31;
    public static final int UPD_RESPONSE_UID = 32;
    public static final int UPD_APP_VERSION_REQUEST = 33;
    public static final int UPD_APP_VERSION_RESPONSE = 34;
    public static final int UDP_RESET = 35;
    public static final int UPD_REQUEST_BOOT_DESC = 36;
    public static final int UPD_RESPONSE_BOOT_DESC = 37;
    public static final int UPD_REQUEST_BL_IDENTITY = 40;
    public static final int UPD_RESPONSE_BL_IDENTITY = 41;
    public static final int UPD_SET_EMULATION = 100;

    public static final int UDP_UNKONW_COMMAND = 0x100; // <! received command
    // is not defined
    public static final int UDP_CRC_ERROR = 0x101; // <! CRC calculated on the
    // device
    // <! and by the updater don't match
    public static final int UPD_ADDRESS_NOT_ALLOWED_TO_FLASH = 0x102;// <!
    // specifed
    // address
    // cannot
    // be
    // programmed
    public static final int UPD_SECTOR_NOT_ALLOWED_TO_ERASE = 0x103; // <! the
    // specified
    // sector
    // cannot
    // be
    // erased
    public static final int UPD_RAM_BUFFER_OVERFLOW = 0x104; // <! internal
    // buffer for
    // storing the
    // data
    // <! would overflow
    public static final int UPD_WRONG_DESCRIPTOR_BLOCK = 0x105; // <! the boot
    // descriptor
    // block does
    // not exist
    public static final int UPD_APPLICATION_NOT_STARTABLE = 0x106; // <! the
    // programmed
    // application
    // is not
    // startable
    public static final int UPD_DEVICE_LOCKED = 0x107; // <! the device is still
    // locked
    public static final int UPD_UID_MISMATCH = 0x108;	//! UID sent to unlock
    // the device is invalid
    public static final int UPD_ERASE_FAILED = 0x109;
    // page erase failed
    public static final int UPD_FLASH_ERROR = 0x110;
    // page program (flash) failed
    public static final int UDP_NOT_IMPLEMENTED = 0xFFFF; // <! this command is
    // not yet
    // implemented

    public static final int FLASH_SECTOR_SIZE = 4096; // Selfbus ARM controller flash sector size
    public static final int FLASH_PAGE_SIZE = 256;    // Selfbus ARM controller flash page size

    int streamToInteger(byte[] stream, int offset) {
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
        int resultCode = 0;
        if (result[3] == UPD_SEND_LAST_ERROR) {
            resultCode = streamToInteger(result, 4);
            System.out.print(ConColors.BRIGHT_RED);		// Print erros in red
            switch (resultCode) {
                case UPD_UID_MISMATCH:
                    System.out.println("failed, UID mismatch.");
                    break;
                case UPD_SECTOR_NOT_ALLOWED_TO_ERASE:
                    System.out.println("Sector not allowed being erased.");
                    break;
                case UPD_RAM_BUFFER_OVERFLOW:
                    System.out.println("RAM Buffer Overflow");
                    break;
                case UDP_CRC_ERROR:
                    System.out.println("CRC error");
                    break;
                case UPD_ADDRESS_NOT_ALLOWED_TO_FLASH:
                    System.out.println("Address not allowed to flash");
                    break;
                case UPD_DEVICE_LOCKED:
                    System.out.println("Device locked");
                    break;
                case UPD_APPLICATION_NOT_STARTABLE:
                    System.out.println("Application not startable");
                    break;
                case UPD_WRONG_DESCRIPTOR_BLOCK:
                    System.out.println("Boot Descripter block wrong");
                    break;
                case UPD_ERASE_FAILED:
                    System.out.println("Flash page erase failed");
                    break;
                case UPD_FLASH_ERROR:
                    System.out.println("Flash page could not be programmed");
                    break;

                default:
                    if (resultCode == 0) {
                        System.out.print(ConColors.BRIGHT_GREEN);	// Success in green
                        if (verbose) {
                            System.out.println("done (" + resultCode + ")");
                        } else
                            System.out.print(".");
                    } else {
                        System.out.println(ConColors.BRIGHT_RED + "unknown error (" + resultCode + ").");
                    }
            }
            System.out.print(ConColors.RESET);		// switch back to default colors
        }
        return resultCode;
    }


    public class ConColors {
        // Reset to default colors
        public static final String RESET  = "\033[0m";  	// Reset

        // Regular Colors
        public static final String BLACK  = "\033[0;30m";   // BLACK
        public static final String RED    = "\033[0;31m";   // RED
        public static final String GREEN  = "\033[0;32m";   // GREEN
        public static final String YELLOW = "\033[0;33m";   // YELLOW
        public static final String BLUE   = "\033[0;34m";   // BLUE
        public static final String PURPLE = "\033[0;35m";   // PURPLE
        public static final String CYAN   = "\033[0;36m";   // CYAN
        public static final String WHITE  = "\033[0;37m";   // WHITE

        // Regular Bold
        public static final String BOLD_BLACK  = "\033[1;30m";  // BLACK
        public static final String BOLD_RED    = "\033[1;31m";  // RED
        public static final String BOLD_GREEN  = "\033[1;32m";  // GREEN
        public static final String BOLD_YELLOW = "\033[1;33m";  // YELLOW
        public static final String BOLD_BLUE   = "\033[1;34m";  // BLUE
        public static final String BOLD_PURPLE = "\033[1;35m";  // PURPLE
        public static final String BOLD_CYAN   = "\033[1;36m";  // CYAN
        public static final String BOLD_WHITE  = "\033[1;37m";  // WHITE

        // Regular Background
        public static final String BG_BLACK  = "\033[40m";  // BLACK
        public static final String BG_RED    = "\033[41m";  // RED
        public static final String BG_GREEN  = "\033[42m";  // GREEN
        public static final String BG_YELLOW = "\033[43m";  // YELLOW
        public static final String BG_BLUE   = "\033[44m";  // BLUE
        public static final String BG_PURPLE = "\033[45m";  // PURPLE
        public static final String BG_CYAN   = "\033[46m";  // CYAN
        public static final String BG_WHITE  = "\033[47m";  // WHITE

        // High Intensity
        public static final String BRIGHT_BLACK  = "\033[0;90m";  // BLACK
        public static final String BRIGHT_RED    = "\033[0;91m";  // RED
        public static final String BRIGHT_GREEN  = "\033[0;92m";  // GREEN
        public static final String BRIGHT_YELLOW = "\033[0;93m";  // YELLOW
        public static final String BRIGHT_BLUE   = "\033[0;94m";  // BLUE
        public static final String BRIGHT_PURPLE = "\033[0;95m";  // PURPLE
        public static final String BRIGHT_CYAN   = "\033[0;96m";  // CYAN
        public static final String BRIGHT_WHITE  = "\033[0;97m";  // WHITE

        // High Intensity Bold
        public static final String BRIGHT_BOLD_BLACK  = "\033[1;90m";  // BLACK
        public static final String BRIGHT_BOLD_RED    = "\033[1;91m";  // RED
        public static final String BRIGHT_BOLD_GREEN  = "\033[1;92m";  // GREEN
        public static final String BRIGHT_BOLD_YELLOW = "\033[1;93m";  // YELLOW
        public static final String BRIGHT_BOLD_BLUE   = "\033[1;94m";  // BLUE
        public static final String BRIGHT_BOLD_PURPLE = "\033[1;95m";  // PURPLE
        public static final String BRIGHT_BOLD_CYAN   = "\033[1;96m";  // CYAN
        public static final String BRIGHT_BOLD_WHITE  = "\033[1;97m";  // WHITE

        // High Intensity Background
        public static final String BRIGHT_BG_BLACK  = "\033[0;100m";  // BLACK
        public static final String BRIGHT_BG_RED    = "\033[0;101m";  // RED
        public static final String BRIGHT_BG_GREEN  = "\033[0;102m";  // GREEN
        public static final String BRIGHT_BG_YELLOW = "\033[0;103m";  // YELLOW
        public static final String BRIGHT_BG_BLUE   = "\033[0;104m";  // BLUE
        public static final String BRIGHT_BG_PURPLE = "\033[0;105m";  // PURPLE
        public static final String BRIGHT_BG_CYAN   = "\033[0;106m";  // CYAN
        public static final String BRIGHT_BG_WHITE  = "\033[0;107m";  // WHITE
    }




    /*
     * (non-Javadoc)
     *
     * @see java.lang.Runnable#run()
     */
    public void run() {
        // ??? as with the other tools, maybe put this into the try block to
        // also call onCompletion
        AppDirs appDirs = AppDirsFactory.getInstance();
        //Diese Angaben werden genutzt, um den Ordner für die Cache Files des DiffUpdaters abzulegen
        //Unter Windows: C:\Users\[currentUser]\AppData\Local\Selfbus\Selfbus-Updater\Cache\0.53
        String hexCacheDir = appDirs.getUserCacheDir("Selfbus-Updater", "0.54", "Selfbus");

        if (options.isEmpty()) {
            LOGGER.log(Level.INFO, tool);
            showVersion();
            LOGGER.log(Level.INFO, "type -help for help message");
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


        System.out.println(ConColors.BRIGHT_BOLD_GREEN + "\n" +
                "\n" +
                "     _____ ________    __________  __  _______    __  ______  ____  ___  ________________ \n" +
                "    / ___// ____/ /   / ____/ __ )/ / / / ___/   / / / / __ \\/ __ \\/   |/_  __/ ____/ __ \\\n" +
                "    \\__ \\/ __/ / /   / /_  / __  / / / /\\__ \\   / / / / /_/ / / / / /| | / / / __/ / /_/ /\n" +
                "   ___/ / /___/ /___/ __/ / /_/ / /_/ /___/ /  / /_/ / ____/ /_/ / ___ |/ / / /___/ _, _/ \n" +
                "  /____/_____/_____/_/   /_____/\\____//____/   \\____/_/   /_____/_/  |_/_/ /_____/_/ |_|  \n" +
                "  by Stefan Haller, Oliver Stefan et al.                                      Version 0.54  \n\n" +
                ConColors.RESET);

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

            link = createLink();
            mc = new UpdatableManagementClientImpl(link);


            if (device != null) {
                Destination d;
                d = mc.createDestination(device, true);
                System.out.println("Attempting to restart device " + device.toString() + " in bootloader mode ...");
                try {
                    mc.restart(d, 0, 255);
                } catch (final KNXException e) {
                }
                Thread.sleep(1000);	// currently not needed because calimero currently times out waiting for an ACK
            }

            pd = mc.createDestination(progDevice, true);

            if (uid == null) {
                System.out.print("Requesting UID from" + progDevice.toString() + " ... ");
                result = mc.sendUpdateData(pd, UPD_REQUEST_UID, new byte[0]);
                if (result.length == 16) {
                    uid = new byte[12];
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
                    //uid = new byte[12];
                    //for (int i = 0; i < result.length - 4; i++) {
                    //uid[i] = result[i + 4];
                    //}
                } else {
                    System.out.println("Reqest UID failed");
                    mc.restart(pd);
                    throw new RuntimeException("Selfbus udpate failed.");
                }
            }

            System.out.print("Unlocking device with UID ");
            for (int i = 0; i < uid.length; i++) {
                if (i != 0) {
                    System.out.print(":");
                }
                System.out.print(String.format("%02X", uid[i] & 0xff));
            }
            System.out.print(" ... ");

            result = mc.sendUpdateData(pd, UPD_UNLOCK_DEVICE, uid);
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException("Selfbus udpate failed.");
            }

            result = mc.sendUpdateData(pd, UPD_REQUEST_BL_IDENTITY, new byte[] {0});
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException("Requesting Bootloader Identity failed!");
            }
            long BL_Identity = (result[4] & 0xFF) + ((result[5] & 0xFF) << 8) + ((result[6] & 0xFF) << 16) + ((long)(result[7] & 0xFF) << 24);
            long BL_Features = (result[8] & 0xFF) + ((result[9] & 0xFF) << 8) + ((result[10] & 0xFF) << 16) + ((long)(result[11] & 0xFF) << 24);
            long BL_Size = (result[12] & 0xFF) + ((result[13] & 0xFF) << 8) + ((result[14] & 0xFF) << 16) + ((long)(result[15] & 0xFF) << 24);
            System.out.println("Device Bootloader Identity   : " + ConColors.BRIGHT_YELLOW + "0x" +  Long.toHexString(BL_Identity)
                    + ", Features: 0x" + Long.toHexString(BL_Features) + ", Bootloader Size: 0x" + Long.toHexString(BL_Size) + ConColors.RESET);

            System.out.print("\n\rRequesting App Version String ...");
            result = mc.sendUpdateData(pd, UPD_APP_VERSION_REQUEST, new byte[] {0});
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
            ByteArrayInputStream fis = new ByteArrayInputStream(binData);
            System.out.println("  Hex file parsed: starting at 0x" + Long.toHexString(startAddress) + ", length " + totalLength
                    + " bytes");

            // Handle App Version Pointer
            if (appVersionAddr > 0xD0 && appVersionAddr < binData.length) {  // manually provided and not in vector or outside file length
                // Use manual set AppVersion address
                String file_version = new String(binData,appVersionAddr,12);	// Get app version pointers content
                System.out.println("  File App Version String is : " + ConColors.BRIGHT_RED + file_version + ConColors.RESET +
                        " manually specified at address 0x" + Long.toHexString(appVersionAddr));
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
                            " found at address 0x" + Long.toHexString(appVersionAddr));
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
                System.out.println(ConColors.BRIGHT_RED+"  Error! The specified firmware image would overwrite parts of the bootloader. Check FW offsest setting in the linker!");
                System.out.println("  Firmware needs to start beyond 0x" + Long.toHexString(BL_Size) + ConColors.RESET);
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
            result = mc.sendUpdateData(pd, UPD_REQUEST_BOOT_DESC, new byte[] {0});
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException("Boot descriptor request failed.");
            }
            long descrStartAddr = (result[4] & 0xFF) + ((result[5] & 0xFF) << 8) + ((result[6] & 0xFF) << 16) + ((long)(result[7] & 0xFF) << 24);
            long descrEndAddr = (result[8] & 0xFF) + ((result[9] & 0xFF) << 8) + ((result[10] & 0xFF) << 16) + ((long)(result[11] & 0xFF) << 24);
            long descrCrc = (result[12] & 0xFF) + ((result[13] & 0xFF) << 8) + ((result[14] & 0xFF) << 16) + ((long)(result[15] & 0xFF) << 24);
            long descrLength = descrEndAddr - descrStartAddr;
            System.out.println("  Current firmware: starts at 0x" +  Long.toHexString(descrStartAddr) + " ends at 0x" + Long.toHexString(descrEndAddr) + " CRC is 0x" + Long.toHexString(descrCrc));

            // try find file in cache
            boolean diffMode = false;
            File oldImageCacheFile = new File(hexCacheDir + File.separator + "image-" + Long.toHexString(descrStartAddr) + "-" + descrLength + "-" + Long.toHexString(descrCrc) + ".bin" );
            if (!(options.containsKey("full")) && oldImageCacheFile.exists()) {
                System.out.println("  Found current firmware in cache " + oldImageCacheFile.getAbsolutePath() + ConColors.BRIGHT_GREEN +"\n\r  --> switching to diff upload mode" + ConColors.RESET);
                diffMode = true;
            }

            // Start to flash the new firmware
            System.out.println("\n" + ConColors.BRIGHT_BG_BLUE + "Starting to send new firmware now: " + ConColors.RESET);
            if (diffMode) {
                doDiffFlash(mc, pd, startAddress, binData, oldImageCacheFile);
            } else if (DO_FLASH_WRITE){
                doFullFlash(mc, pd, startAddress, totalLength, fis);
            }

            byte bootDescriptor[] = new byte[16];
            long endAddress = startAddress + totalLength;
            appVersionAddr += startAddress;	// Add FW offset to get absolute position in MCU
            integerToStream(bootDescriptor, 0, startAddress);
            integerToStream(bootDescriptor, 4, endAddress);
            integerToStream(bootDescriptor, 8, (int) crc32File.getValue());
            integerToStream(bootDescriptor, 12, appVersionAddr);
            System.out
                    .println("\n" + ConColors.BG_RED + "Preparing boot descriptor with start address 0x"
                            + Long.toHexString(startAddress)
                            + ", end address 0x"
                            + Long.toHexString(endAddress)
                            + ", CRC32 0x"
                            + Long.toHexString(crc32File.getValue())
                            + ", APP_VERSION pointer 0x"
                            + Long.toHexString(appVersionAddr) + ConColors.RESET);

            int nDone = 0;
            boolean timeoutOccured = false;
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
                    result = mc.sendUpdateData(pd, UPD_SEND_DATA, txBuf);
                    if (checkResult(result, false) != 0) {
                        mc.restart(pd);
                        throw new RuntimeException("Selfbus udpate failed.");
                    }
                }
                catch(KNXTimeoutException e){
                    timeoutOccured = true;
                }
                if(!timeoutOccured){
                    nDone += txSize;
                }else{
                    timeoutOccured = false;
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
                    + Long.toHexString(crc32Block.getValue())
                    + ", length "
                    + Long.toHexString(bootDescriptor.length) + " ");
            result = mc.sendUpdateData(pd, UPD_UPDATE_BOOT_DESC,
                    programBootDescriptor);
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException("Selfbus udpate failed.");
            }
            Thread.sleep(500);
            System.out.println(ConColors.BG_GREEN + "Firmware Update done, Restarting device now ..." + ConColors.RESET + "\n\r\n\r");
            //mc.restart(pd);
            mc.sendUpdateData(pd, UDP_RESET, new byte[] {0});  // Clean restart by application rather than lib

        } catch (final KNXException e) {
            thrown = e;
        } catch (final RuntimeException e) {
            thrown = e;
        } catch (final InterruptedException e) {
            canceled = true;
            Thread.currentThread().interrupt();
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (Throwable e) {
            e.printStackTrace();
        } finally {
            if (link != null)
                link.close();
            onCompletion(thrown, canceled);
        }
    }

    // Differential update routine
    private void doDiffFlash(UpdatableManagementClientImpl mc, Destination pd, long startAddress, byte[] binData, File oldBinFile) throws IOException, KNXDisconnectException, KNXTimeoutException, KNXRemoteException, KNXLinkClosedException, InterruptedException {
        FlashDiff differ = new FlashDiff();
        BinImage img2 = BinImage.copyFromArray(binData, startAddress);
        BinImage img1 = BinImage.readFromBin(oldBinFile.getAbsolutePath());
        differ.generateDiff(img1, img2, (outputDiffStream, crc32) -> {
            byte[] result;
            // process compressed page
            System.out.print("Sending new firmware (" + outputDiffStream.size() + " diff bytes) ");
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
                result = mc.sendUpdateData(pd, UPD_SEND_DATA_TO_DECOMPRESS, txBuf);
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
                System.out.print("Program device next page diff, CRC32 0x" + Long.toHexString(crc32) + " ... ");
            }
            result = mc.sendUpdateData(pd, UPD_PROGRAM_DECOMPRESSED_DATA, progPars);
            if (checkResult(result) != 0) {
                mc.restart(pd);
                throw new RuntimeException(
                        "Selfbus update failed.");
            }
        });
    }

    // Normal update routine, sending complete image
    // This works on sector page right now, so the complete affected flash is erased first
    private void doFullFlash(UpdatableManagementClientImpl mc, Destination pd, long startAddress, int totalLength, ByteArrayInputStream fis)
            throws IOException, KNXDisconnectException, KNXTimeoutException, KNXRemoteException, KNXLinkClosedException, InterruptedException {
        byte[] result;

        // This is the selfbus style of deleting pages, send one per telegram
        // use this for the time being
        if (true) {
            int erasePages = (totalLength / FLASH_SECTOR_SIZE) + 1;
            long startPage = startAddress / FLASH_SECTOR_SIZE;
            byte[] sector = new byte[1];
            for (int i = 0; i < erasePages; i++) {
                sector[0] = (byte) (i + startPage);
                System.out.print("Erase sector " + sector[0] + " ...");
                result = mc.sendUpdateData(pd, UPD_ERASE_PAGES, sector);
                if (checkResult(result) != 0) {
                    mc.restart(pd);
                    throw new RuntimeException("Selfbus udpate failed.");
                }
                else {
                    System.out.print(" OK");
                }
            }
        }

        // This is the knxduino way of deleting pages, sending start and amount
        // Disable for the time beeing and revert to selfbus old style
//            if (true) {
//                int erasePages = (totalLength / FLASH_PAGE_SIZE) + 1;
//                long startPage = startAddress / FLASH_PAGE_SIZE;
//                byte[] metaData = new byte[2];
//                metaData[0] = (byte) (startPage);
//                metaData[1] = (byte) (erasePages);
//                System.out.print("Erase " + erasePages + " pages starting from page " + startPage + " ...");
//                result = mc.sendUpdateData(pd, UPD_ERASE_PAGES, metaData);
//                if (checkResult(result) != 0) {
//                    mc.restart(pd);
//                    throw new RuntimeException("KNXduino udpate failed.");
//                }
//            }

        byte[] buf = new byte[11];
        int nRead = 0;
        int total = 0;
        CRC32 crc32Block = new CRC32();
        int progSize = 0;
        long progAddress = startAddress;
        boolean doProg = false;
        boolean timeoutOccured = false;
        if (true) {
            System.out.println("Sending application data (" + totalLength
                    + " bytes) ");
            while ((nRead = fis.read(buf)) != -1) {
                int txSize;
                int nDone = 0;
                while (nDone < nRead) {
                    if ((progSize + nRead) > FLASH_PAGE_SIZE) {
                        txSize = FLASH_PAGE_SIZE - progSize;
                        doProg = true;
                    } else {
                        txSize = nRead - nDone;
                    }
                    if ((total + nRead) == totalLength) {
                        doProg = true;
                    }
                    if (txSize > 0) {
                        byte[] txBuf = new byte[txSize + 1];

                        for (int i = 0; i < txSize; i++) {
                            txBuf[i + 1] = buf[i + nDone];
                        }
                        // Adresse der Daten in den ersten 2 Bytes senden
                        txBuf[0] = (byte)progSize;

                        try{
                            result = mc
                                    .sendUpdateData(pd, UPD_SEND_DATA, txBuf);

                            if (checkResult(result, false) != 0) {
                                mc.restart(pd);
                                throw new RuntimeException(
                                        "Selfbus udpate failed.");
                            }
                        }
                        // wenn eine Timeout Exception gesendet wird, ist ein Fehler bei der Datenübertragung vorgekommen
                        // daher werden die letzten Daten noch einmal neu gesendet
                        catch(KNXTimeoutException e){
                            timeoutOccured = true;
                        }

                        if(!timeoutOccured) { // wenn kein Timeout Fehler vorgekommen ist, soll die Abarbeitung fortgesetzt werden
                            crc32Block.update(txBuf, 1, txSize);
                            nDone += txSize;
                            progSize += txSize;
                            total += txSize;
                        }else{ // wenn ein Timeout passiert ist, muss der Merker wieder zurückgesetzt werden
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
                        System.out
                                .print("Program device at flash address 0x"
                                        + Long.toHexString(progAddress)
                                        + " with " + progSize
                                        + " bytes and CRC32 0x"
                                        + Long.toHexString(crc) + " ... ");
                        result = mc.sendUpdateData(pd, UPD_PROGRAM,
                                progPars);
                        if (checkResult(result) != 0) {
                            mc.restart(pd);
                            throw new RuntimeException(
                                    "Selfbus udpate failed.");
                        }
                        progAddress += progSize;
                        progSize = 0;
                        crc32Block.reset();
                    }
                }
            }
            fis.close();
            System.out.println("Wrote " + total
                    + " bytes from file to device.");
            Thread.sleep(1000);
        }
    }
}
