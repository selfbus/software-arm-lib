package org.selfbus.updater;

import ch.qos.logback.classic.Level;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.OptionGroup;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

import org.selfbus.updater.upd.UPDProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import tuwien.auto.calimero.IndividualAddress;
import tuwien.auto.calimero.KNXFormatException;
import tuwien.auto.calimero.KNXIllegalArgumentException;
import tuwien.auto.calimero.Priority;
import tuwien.auto.calimero.knxnetip.KNXnetIPConnection;
import tuwien.auto.calimero.link.medium.KNXMediumSettings;
import tuwien.auto.calimero.link.medium.TPSettings;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;

import static org.fusesource.jansi.Ansi.*;
import static org.fusesource.jansi.Ansi.Color.*;

/**
 * Parses command line interface (cli) options for the application
 * <p>
 * Adding a new cli option name:
 *         - declare a short and a long option
 *           private static final String OPT_SHORT_XXX = "X";
 *           private static final String OPT_LONG_XXX = "XXXXX";
 *         - in constructor @ref CliOptions(.) create an instance of class Option
 *           and add it with cliOptions.addOption(yourOptionInstance)
 *         - check with cmdLine.hasOption(OPT_SHORT_XXX) in method parse(.) is it set or not
 */
public class CliOptions {
    private static final Logger logger = LoggerFactory.getLogger(CliOptions.class);

    private static final String OPT_SHORT_FILENAME = "f";
    public static final String OPT_LONG_FILENAME = "fileName";

    private static final String OPT_SHORT_LOCALHOST = "H";
    public static final String OPT_LONG_LOCALHOST = "localhost";
    private static final String OPT_SHORT_LOCALPORT = "P";
    public static final String OPT_LONG_LOCALPORT = "localport";
    private static final String OPT_SHORT_PORT = "p";
    public static final String OPT_LONG_PORT = "port";

    private static final String OPT_SHORT_FT12 = "s";
    public static final String OPT_LONG_FT12 = "serial";
    private static final String OPT_SHORT_TPUART = "t";
    public static final String OPT_LONG_TPUART = "tpuart";
    public static final String OPT_LONG_USB = "usb";
    private static final String OPT_SHORT_MEDIUM = "m";
    public static final String OPT_LONG_MEDIUM = "medium";

    public static final String OPT_LONG_USER_ID = "user";
    public static final String OPT_LONG_USER_PASSWORD = "user-pwd";
    public static final String OPT_LONG_DEVICE_PASSWORD = "device-pwd";

    private static final String OPT_SHORT_PROG_DEVICE = "D";
    public static final String OPT_LONG_PROG_DEVICE = "progDevice";
    private static final String OPT_SHORT_DEVICE = "d";
    public static final String OPT_LONG_DEVICE = "device";
    private static final String OPT_SHORT_OWN_ADDRESS = "o";
    public static final String OPT_LONG_OWN_ADDRESS = "own";

    private static final String OPT_SHORT_UID = "u";
    public static final String OPT_LONG_UID = "uid";

    public static final String OPT_LONG_DELAY = "delay";

    private static final String OPT_SHORT_TUNNEL_V2 = "t2";
    public static final String OPT_LONG_TUNNEL_V2 = "tunnelingv2";
    private static final String OPT_SHORT_TUNNEL_V1 = "t1";
    public static final String OPT_LONG_TUNNEL_V1 = "tunneling";
    private static final String OPT_SHORT_NAT = "n";
    public static final String OPT_LONG_NAT = "nat";
    private static final String OPT_SHORT_ROUTING = "r";
    public static final String OPT_LONG_ROUTING = "routing";

    private static final String OPT_SHORT_FULL = "f1";
    public static final String OPT_LONG_FULL = "full";

    private static final String OPT_SHORT_HELP = "h";
    public static final String OPT_LONG_HELP = "help";

    private static final String OPT_SHORT_VERSION = "v";
    public static final String OPT_LONG_VERSION = "version";

    private static final String OPT_SHORT_NO_FLASH = "f0";
    public static final String OPT_LONG_NO_FLASH = "NO_FLASH";

    private static final String OPT_SHORT_LOGLEVEL = "l";
    public static final String OPT_LONG_LOGLEVEL = "logLevel";

    public static final String OPT_LONG_PRIORITY = "priority";

    public static final String OPT_LONG_ERASEFLASH = "ERASEFLASH";

    public static final String OPT_LONG_DUMPFLASH = "DUMPFLASH";

    public static final String OPT_LONG_LOGSTATISTIC = "statistic";

    public static final String OPT_SHORT_BLOCKSIZE = "bs";
    public static final String OPT_LONG_BLOCKSIZE = "blocksize";

    public static final String OPT_LONG_DISCOVER = "discover";

    private final static List<Integer> VALID_BLOCKSIZES = Arrays.asList(256, 512, 1024);

    private static final int PRINT_WIDTH = 100;
    private final static List<String> VALID_LOG_LEVELS = Arrays.asList("TRACE", "DEBUG", "INFO");
    private final static int INVALID_SECURE_USER_ID = -1;

    private final Options cliOptions = new Options();
    // define parser
    CommandLine cmdLine;
    final HelpFormatter helper = new HelpFormatter();

    private String helpHeader = "";
    private String helpFooter = "";
    private String helpApplicationName = "";

    private String knxInterface ="";
    private String fileName = "";
    private String localhost = "";
    private int localPort = 0;
    private int port = KNXnetIPConnection.DEFAULT_PORT;
    private boolean natIsSet = false;
    private String ft12SerialPort = "";
    private String tpuartSerialPort = "";
    private String usbVendorIdAndProductId = "";
    private boolean tunnelingV2isSet = false;
    private boolean tunnelingV1isSet = false;
    private boolean routingIsSet = false;
    private String medium = KNXMediumSettings.getMediumString(KNXMediumSettings.MEDIUM_TP1);

    private int knxSecureUserId = INVALID_SECURE_USER_ID;
    private String knxSecureUserPassword = "";
    private String knxSecureDevicePassword = "";

    private IndividualAddress progDevicePhysicalAddress = Updater.PHYS_ADDRESS_BOOTLOADER;
    private IndividualAddress ownPhysicalAddress = Updater.PHYS_ADDRESS_OWN;
    private IndividualAddress devicePhysicalAddress = null;
    private String uid = "";
    private boolean flashingFullModeIsSet = false;
    private int delayMs = 0;

    private boolean noFlashIsSet = false;
    private boolean eraseFullFlashIsSet = false;
    private long dumpFlashStartAddress = -1;
    private long dumpFlashEndAddress = -1;

    private Priority priority = Priority.LOW;

    private boolean logStatisticsIsSet = false;
    private int blockSize = Mcu.UPD_PROGRAM_SIZE;

    private final Level defaultLogLevel;

    private boolean discoverIsSet = false;

    private CliOptions() {
        defaultLogLevel = getLogLevel();

        Option tunnelingV2 = new Option(OPT_SHORT_TUNNEL_V2, OPT_LONG_TUNNEL_V2, false, "use KNXnet/IP tunneling v2 (TCP) (experimental)");
        Option tunnelingV1 = new Option(OPT_SHORT_TUNNEL_V1, OPT_LONG_TUNNEL_V1, false, "use KNXnet/IP tunneling v1 (UDP)");
        Option nat = new Option(OPT_SHORT_NAT, OPT_LONG_NAT, false, "enable Network Address Translation (NAT) (only available with tunneling v1)");
        Option routing = new Option(OPT_SHORT_ROUTING, OPT_LONG_ROUTING, false, "use KNXnet/IP routing/multicast (experimental)");
        Option full = new Option(OPT_SHORT_FULL, OPT_LONG_FULL, false, "force full upload mode (disables differential mode)");
        Option help = new Option(OPT_SHORT_HELP, OPT_LONG_HELP, false, "show this help message");
        Option version = new Option(OPT_SHORT_VERSION, OPT_LONG_VERSION, false, "show tool/library version");
        Option NO_FLASH = new Option(OPT_SHORT_NO_FLASH, OPT_LONG_NO_FLASH, false, "for debugging use only, disable flashing firmware!");
        Option eraseFlash = new Option(null, OPT_LONG_ERASEFLASH, false, "USE WITH CAUTION! Erases the complete flash memory including the physical KNX address and all settings of the device. Only the bootloader is not deleted.");
        Option discover = new Option(null, OPT_LONG_DISCOVER, false, "List available KNXnet/IP interfaces and USB-Interfaces");


        Option dumpFlash = Option.builder(null).longOpt(OPT_LONG_DUMPFLASH)
                .argName("start> <end")
                .valueSeparator(' ')
                .numberOfArgs(2)
                .required(false)
                .type(Number.class)
                .desc("dump a flash range in intel(R) hex to the serial port of the MCU. Works only with DEBUG version of the bootloader.").build();

        Option fileName = Option.builder(OPT_SHORT_FILENAME).longOpt(OPT_LONG_FILENAME)
                .argName("filename")
                .numberOfArgs(1)
                .required(false)
                .type(String.class)
                .desc("Filename of hex file to program").build();
        Option localhost = Option.builder(OPT_SHORT_LOCALHOST).longOpt(OPT_LONG_LOCALHOST)
                .argName("localhost")
                .numberOfArgs(1)
                .required(false)
                .type(String.class)
                .desc("local IP/host name").build();
        Option localport = Option.builder(OPT_SHORT_LOCALPORT).longOpt(OPT_LONG_LOCALPORT)
                .argName("localport")
                .numberOfArgs(1)
                .required(false)
                .type(Number.class)
                .desc("local UDP port (default system assigned)").build();
        Option port = Option.builder(OPT_SHORT_PORT).longOpt(OPT_LONG_PORT)
                .argName("port")
                .numberOfArgs(1)
                .required(false)
                .type(Number.class)
                .desc(String.format("UDP port on <KNX Interface> (default %d)", KNXnetIPConnection.DEFAULT_PORT)).build();
        Option ft12 = Option.builder(OPT_SHORT_FT12).longOpt(OPT_LONG_FT12)
                .argName("COM-port")
                .numberOfArgs(1)
                .required(false)
                .desc("use FT1.2 serial communication").build();
        Option tpuart = Option.builder(OPT_SHORT_TPUART).longOpt(OPT_LONG_TPUART)
                .argName("COM-port")
                .numberOfArgs(1)
                .required(false)
                .desc("use TPUART serial communication (experimental, needs serialcom or rxtx library in java.library.path)").build();
        Option usbInterface = Option.builder(null).longOpt(OPT_LONG_USB)
                .argName("vendorId:productId")
                .numberOfArgs(1)
                .required(false)
                .desc("use USB-Interface. Specify VendorID and ProductID e.g. 147B:5120 for the Selfbus USB-Interface (experimental)").build();
        Option medium = Option.builder(OPT_SHORT_MEDIUM).longOpt(OPT_LONG_MEDIUM)
                .argName("tp1|rf")
                .numberOfArgs(1)
                .required(false)
                .type(TPSettings.class)
                .desc(String.format("KNX medium [tp1|rf] (default %s)", getMedium())).build(); ///\todo not all implemented missing [tp0|p110|p132]
        Option optProgDevice = Option.builder(OPT_SHORT_PROG_DEVICE).longOpt(OPT_LONG_PROG_DEVICE)
                .argName("x.x.x")
                .numberOfArgs(1)
                .required(false)
                .type(IndividualAddress.class)
                .desc(String.format("KNX device address in bootloader mode (default %s)", getProgDevicePhysicalAddress().toString())).build();
        Option device = Option.builder(OPT_SHORT_DEVICE).longOpt(OPT_LONG_DEVICE)
                .argName("x.x.x")
                .numberOfArgs(1)
                .required(false)
                .type(IndividualAddress.class)
                .desc("KNX device address in normal operating mode (default none)").build();
        Option ownPhysicalAddress = Option.builder(OPT_SHORT_OWN_ADDRESS).longOpt(OPT_LONG_OWN_ADDRESS)
                .argName("x.x.x")
                .numberOfArgs(1)
                .required(false)
                .type(IndividualAddress.class)
                .desc(String.format("own physical KNX address (default %s)", getOwnPhysicalAddress().toString())).build();
        Option uid = Option.builder(OPT_SHORT_UID).longOpt(OPT_LONG_UID)
                .argName("uid")
                .numberOfArgs(1)
                .required(false)
                .desc(String.format("send UID to unlock (default: request UID to unlock). Only the first %d bytes of UID are used", UPDProtocol.UID_LENGTH_USED)).build();
        Option delay = Option.builder(null).longOpt(OPT_LONG_DELAY)
                .argName("ms")
                .numberOfArgs(1)
                .required(false)
                .type(Number.class)
                .desc(String.format("delay telegrams during data transmission to reduce bus load, valid 0-500ms, default %d", Updater.DELAY_MIN)).build();
        Option logLevel = Option.builder(OPT_SHORT_LOGLEVEL).longOpt(OPT_LONG_LOGLEVEL)
                .argName("TRACE|DEBUG|INFO")
                .numberOfArgs(1)
                .required(false)
                .type(String.class)
                .desc(String.format("Logfile logging level [TRACE|DEBUG|INFO] (default %s)", defaultLogLevel.toString())).build();

        Option userId = Option.builder(null).longOpt(OPT_LONG_USER_ID)
                .argName("id")
                .numberOfArgs(1)
                .required(false)
                .type(Number.class)
                .desc(String.format("KNX IP Secure tunneling user identifier (1..127) (default %d)", getKnxSecureUserId())).build();
        Option userPasswd = Option.builder(null).longOpt(OPT_LONG_USER_PASSWORD)
                .argName("password")
                .numberOfArgs(1)
                .required(false)
                .type(Number.class)
                .desc("KNX IP Secure tunneling user password (Commissioning password/Inbetriebnahmepasswort), quotation marks (\") in password may not work").build();
        Option devicePasswd = Option.builder(null).longOpt(OPT_LONG_DEVICE_PASSWORD)
                .argName("password")
                .numberOfArgs(1)
                .required(false)
                .type(Number.class)
                .desc("KNX IP Secure device authentication code (Authentication Code/Authentifizierungscode) quotation marks(\") in password may not work").build();

        Option knxPriority = Option.builder(null).longOpt(OPT_LONG_PRIORITY)
                .argName("SYSTEM|URGENT|NORMAL|LOW")
                .numberOfArgs(1)
                .required(false)
                .type(String.class)
                .desc(String.format("KNX telegram priority (default %s)", getPriority().toString().toUpperCase())).build();

        Option blockSizeOption = Option.builder(OPT_SHORT_BLOCKSIZE).longOpt(OPT_LONG_BLOCKSIZE)
                .argName("256|512|1024")
                .valueSeparator(' ')
                .numberOfArgs(1)
                .required(false)
                .type(Number.class)
                .desc(String.format("Block size to program (default %d bytes)", getBlockSize())).build();

        Option logStatistic = new Option(null, OPT_LONG_LOGSTATISTIC, false, "show more statistic data");

        // options will be shown in order as they are added to cliOptions
        cliOptions.addOption(fileName);

        cliOptions.addOption(medium);

        // usb, ft12 or tpuart
        OptionGroup grpBusAccess = new OptionGroup();
        grpBusAccess.addOption(ft12);
        grpBusAccess.addOption(tpuart);
        grpBusAccess.addOption(usbInterface);
        cliOptions.addOptionGroup(grpBusAccess);

        cliOptions.addOption(device);
        cliOptions.addOption(optProgDevice);
        cliOptions.addOption(ownPhysicalAddress);
        cliOptions.addOption(knxPriority);
        cliOptions.addOption(blockSizeOption);

        cliOptions.addOption(userId);
        cliOptions.addOption(userPasswd);
        cliOptions.addOption(devicePasswd);

        cliOptions.addOption(uid);
        cliOptions.addOption(full);
        cliOptions.addOption(localhost);
        cliOptions.addOption(localport);
        cliOptions.addOption(port);

        // Tunneling v1, Tunneling v2 or routing
        OptionGroup grpTunnelingOrRouting = new OptionGroup();
        grpTunnelingOrRouting.addOption(tunnelingV2);
        grpTunnelingOrRouting.addOption(tunnelingV1);
        grpTunnelingOrRouting.addOption(routing);
        cliOptions.addOptionGroup(grpTunnelingOrRouting);

        cliOptions.addOption(nat);

        // help or version, not both
        OptionGroup grpHelper = new OptionGroup();
        grpHelper.addOption(help);
        grpHelper.addOption(version);
        cliOptions.addOptionGroup(grpHelper);

        cliOptions.addOption(delay);
        cliOptions.addOption(logLevel);
        cliOptions.addOption(eraseFlash);
        cliOptions.addOption(dumpFlash);
        cliOptions.addOption(NO_FLASH);
        cliOptions.addOption(logStatistic);
        cliOptions.addOption(discover);
    }

    public CliOptions(final String[] args, String helpApplicationName, String helpHeader, String helpFooter)
            throws KNXFormatException, ParseException {
        this();
        this.helpApplicationName = helpApplicationName;
        this.helpHeader = helpHeader;
        this.helpFooter = helpFooter;
        parse(args);
    }

    public CliOptions(final ArrayList<String> argsList) throws KNXFormatException, ParseException {
        this();
        this.helpApplicationName = "";
        this.helpHeader = "";
        this.helpFooter = "";
        String[] args = new String[argsList.size()];
        args = argsList.toArray(args);
        parse(args);
    }

    private void parse(final String[] args) throws ParseException, KNXFormatException {
        CommandLineParser parser = new DefaultParser();
        cmdLine = parser.parse(cliOptions, args);

        if (cmdLine.hasOption(OPT_LONG_LOGLEVEL))
            setLogLevel(cmdLine.getOptionValue(OPT_LONG_LOGLEVEL).toUpperCase());
        else
            setLogLevel(defaultLogLevel);

        if (cmdLine.hasOption(OPT_LONG_PRIORITY))
            setPriority(cmdLine.getOptionValue(OPT_LONG_PRIORITY));
        else
            setPriority("LOW");

        if (getHelpIsSet()) {
            logger.debug("{}={}", OPT_LONG_HELP, getHelpIsSet());
            return;
        }

        if (getVersionIsSet()) {
            logger.debug("{}={}", OPT_LONG_VERSION, getVersionIsSet());
            return;
        }

        setNoFlashIsSet(cmdLine.hasOption(OPT_LONG_NO_FLASH));
        setEraseFullFlashIsSet(cmdLine.hasOption(OPT_LONG_ERASEFLASH));

        if (cmdLine.hasOption(OPT_LONG_DUMPFLASH)) {
            String[] optArgs = cmdLine.getOptionValues(OPT_LONG_DUMPFLASH);
            setDumpFlashStartAddress(Long.decode(optArgs[0]));
            setDumpFlashEndAddress(Long.decode(optArgs[1]));
        }

        setFlashingFullModeIsSet(cmdLine.hasOption(OPT_LONG_FULL));
        setTunnelingV2isSet(cmdLine.hasOption(OPT_LONG_TUNNEL_V2));
        setTunnelingV1isSet(cmdLine.hasOption(OPT_LONG_TUNNEL_V1));
        setRoutingIsSet(cmdLine.hasOption(OPT_LONG_ROUTING));
        setNatIsSet(cmdLine.hasOption(OPT_LONG_NAT));
        setDiscoverIsSet(cmdLine.hasOption(OPT_LONG_DISCOVER));

        if (cmdLine.hasOption(OPT_LONG_FILENAME))
            setFileName(cmdLine.getOptionValue(OPT_LONG_FILENAME));
        else
            setFileName("");


        if (cmdLine.hasOption(OPT_LONG_LOCALHOST))
            setLocalhost(cmdLine.getOptionValue(OPT_LONG_LOCALHOST));
        else
            setLocalhost("");

        if (cmdLine.hasOption(OPT_LONG_LOCALPORT))
            setLocalPort(cmdLine.getOptionValue(OPT_LONG_LOCALPORT));
        else
            setLocalPort(0);

        if (cmdLine.hasOption(OPT_LONG_PORT))
            setPort(cmdLine.getOptionValue(OPT_LONG_PORT));
        else
            setPort(KNXnetIPConnection.DEFAULT_PORT);

        if (cmdLine.hasOption(OPT_LONG_MEDIUM))
            setMedium(cmdLine.getOptionValue(OPT_LONG_MEDIUM));
        else
            setMedium(KNXMediumSettings.getMediumString(KNXMediumSettings.MEDIUM_TP1));

        if (cmdLine.hasOption(OPT_LONG_DELAY))
            setDelayMs(cmdLine.getOptionValue(OPT_LONG_DELAY));
        else
            setDelayMs(Updater.DELAY_MIN);

        if (cmdLine.hasOption(OPT_LONG_BLOCKSIZE))
            setBlockSize(cmdLine.getOptionValue(OPT_LONG_BLOCKSIZE));
        else
            setBlockSize(Mcu.UPD_PROGRAM_SIZE);

        if (cmdLine.hasOption(OPT_LONG_UID))
            setUid(cmdLine.getOptionValue(OPT_LONG_UID));
        else
            setUid("");

        if (cmdLine.hasOption(OPT_LONG_DEVICE))
            setDevicePhysicalAddress(cmdLine.getOptionValue(OPT_LONG_DEVICE));
        else
            setDevicePhysicalAddress(null);

        if (cmdLine.hasOption(OPT_LONG_PROG_DEVICE))
            setProgDevicePhysicalAddress(cmdLine.getOptionValue(OPT_LONG_PROG_DEVICE));
        else
            setProgDevicePhysicalAddress(Updater.PHYS_ADDRESS_BOOTLOADER.toString());

        if (cmdLine.hasOption(OPT_LONG_OWN_ADDRESS))
            setOwnPhysicalAddress(cmdLine.getOptionValue(OPT_LONG_OWN_ADDRESS));
        else
            setOwnPhysicalAddress(Updater.PHYS_ADDRESS_OWN.toString());

        if (cmdLine.hasOption(OPT_LONG_FT12))
            setFt12SerialPort(cmdLine.getOptionValue(OPT_LONG_FT12));
        else
            setFt12SerialPort("");

        if (cmdLine.hasOption(OPT_LONG_TPUART))
            setTpuartSerialPort(cmdLine.getOptionValue(OPT_LONG_TPUART));
        else
            setTpuartSerialPort("");

        if (cmdLine.hasOption(OPT_LONG_USB))
            setUsbVendorIdAndProductId(cmdLine.getOptionValue(OPT_LONG_USB));
        else
            setUsbVendorIdAndProductId("");

        if (cmdLine.getArgs().length > 0)
            setKnxInterface(cmdLine.getArgs()[0]);
        else
            setKnxInterface("");

        if (cmdLine.hasOption(OPT_LONG_USER_ID))
            setKnxSecureUserId(cmdLine.getOptionValue(OPT_LONG_USER_ID));
        else
            setKnxSecureUserId(INVALID_SECURE_USER_ID);

        if (cmdLine.hasOption(OPT_LONG_USER_PASSWORD))
            setKnxSecureUserPassword(cmdLine.getOptionValue(OPT_LONG_USER_PASSWORD));
        else
            setKnxSecureUserPassword("");

        if (cmdLine.hasOption(OPT_LONG_DEVICE_PASSWORD))
            setKnxSecureDevicePassword(cmdLine.getOptionValue(OPT_LONG_DEVICE_PASSWORD));
        else
            setKnxSecureDevicePassword("");

        setLogStatisticsIsSet(cmdLine.hasOption(OPT_LONG_LOGSTATISTIC));

        validateCliParameters();
    }

    private void validateCliParameters() throws CliInvalidException {
        // some logical checks for options which exclude each other
        // differential mode and eraseflash makes no sense
        if (getEraseFullFlashIsSet() && (!getFlashingFullModeIsSet())) {
            setFlashingFullModeIsSet(true);
            logger.info("{}--{} is set. --> switching to full flash mode{}", ansi().fg(RED),
                    OPT_LONG_ERASEFLASH, ansi().reset());
        }

        // nat only possible with tunneling v1
        if (getNatIsSet() && (!getTunnelingV1isSet())) {
            throw new CliInvalidException(String.format("%sOption --%s can only be used together with --%s%s",
                    ansi().fg(RED), OPT_LONG_NAT, OPT_LONG_TUNNEL_V1, ansi().reset()));
        }

        // nat not allowed with tunneling v2
        if (getNatIsSet() && (getTunnelingV2isSet())) {
            throw new CliInvalidException(String.format("%sOption --%s can not be used together with --%s%s",
                    ansi().fg(RED), OPT_LONG_NAT, OPT_LONG_TUNNEL_V2, ansi().reset()));
        }

        // check IP-secure configuration
        if (!(getKnxSecureUserPassword().isEmpty()) || !(getKnxSecureDevicePassword().isEmpty())) {
            if (getKnxInterface().isBlank()) {
                throw new CliInvalidException(String.format("%sNo IP-Interface specified for IP-secure%s",
                        ansi().fg(RED), ansi().reset()));
            }
            else if (!getUsbVendorIdAndProductId().isBlank()) {
                throw new CliInvalidException(String.format("%sIP-secure is not possible with --%s%s",
                        ansi().fg(RED), OPT_LONG_USB, ansi().reset()));
            }
            else if (!getFt12SerialPort().isBlank()) {
                throw new CliInvalidException(String.format("%sIP-secure is not possible with --%s%s",
                        ansi().fg(RED), OPT_LONG_FT12, ansi().reset()));
            }
            else if (!getTpuartSerialPort().isBlank()) {
                throw new CliInvalidException(String.format("%sIP-secure is not possible with --%s%s",
                        ansi().fg(RED), OPT_LONG_TPUART, ansi().reset()));
            }
            else if (getNatIsSet()) {
                throw new CliInvalidException(String.format("%sIP-secure is not possible with --%s%s",
                        ansi().fg(RED), OPT_LONG_NAT, ansi().reset()));
            }
            else if (getTunnelingV1isSet()) {
                throw new CliInvalidException(String.format("%sIP-secure is not possible with --%s%s",
                        ansi().fg(RED), OPT_LONG_TUNNEL_V1, ansi().reset()));
            }
            else if (getTunnelingV2isSet()) {
                throw new CliInvalidException(String.format("%sIP-secure is not possible with --%s%s",
                        ansi().fg(RED), OPT_LONG_TUNNEL_V2, ansi().reset()));
            }

            // ensure that all three IP-Secure arguments are set
            if ((getKnxSecureUserPassword().isEmpty()) || (getKnxSecureDevicePassword().isEmpty())) {
                throw new CliInvalidException(String.format("%sFor IP-secure --%s, --%s and --%s must be set%s",
                        ansi().fg(RED), OPT_LONG_USER_ID, OPT_LONG_USER_PASSWORD, OPT_LONG_DEVICE_PASSWORD,
                        ansi().reset()));
            }
        }

        int interfacesSet = 0;
        if (!getKnxInterface().isBlank()) interfacesSet++;
        if (getRoutingIsSet()) interfacesSet++;
        if (!getFt12SerialPort().isBlank()) interfacesSet++;
        if (!getTpuartSerialPort().isBlank()) interfacesSet++;
        if (!getUsbVendorIdAndProductId().isBlank()) interfacesSet++;

        if (interfacesSet > 1) {
            throw new CliInvalidException("Only one bus interface can be used.");
        }
        else if ((interfacesSet == 0) && (!getDiscoverIsSet())) {
            throw new CliInvalidException("No bus interface specified.");
        }
    }

    public String reconstructCommandLine() {
        String builder = "";

        if (!getKnxInterface().isBlank())
            builder += getKnxInterface();

        if (!getFileName().isBlank())
            builder += String.format(" --%s %s", OPT_LONG_FILENAME, getFileName());

        if (!getLocalhost().isBlank())
            builder += String.format(" --%s %s", OPT_LONG_LOCALHOST, getLocalhost());

        if (getLocalPort() != 0)
            builder += String.format(" --%s %s", OPT_LONG_LOCALPORT, getLocalPort());

        if (getPort() != KNXnetIPConnection.DEFAULT_PORT)
            builder += String.format(" --%s %s", OPT_LONG_PORT, getPort());

        if (!getFt12SerialPort().isBlank())
            builder += String.format(" --%s %s", OPT_LONG_FT12, getFt12SerialPort());

        if (!getTpuartSerialPort().isBlank())
            builder += String.format(" --%s %s", OPT_LONG_TPUART, getTpuartSerialPort());

        if (!getUsbVendorIdAndProductId().isBlank())
            builder += String.format(" --%s %s", OPT_LONG_USB, getUsbVendorIdAndProductId());

        if (!Objects.equals(getMedium(), KNXMediumSettings.getMediumString(KNXMediumSettings.MEDIUM_TP1)) && (!getMedium().isBlank()))
            builder += String.format(" --%s %s", OPT_LONG_MEDIUM, getMedium());

        if (getKnxSecureUserId() >= 0)
            builder += String.format(" --%s ***", OPT_LONG_USER_ID);

        if (!getKnxSecureUserPassword().isBlank())
            builder += String.format(" --%s ***", OPT_LONG_USER_PASSWORD);

        if (!getKnxSecureDevicePassword().isBlank())
            builder += String.format(" --%s ***", OPT_LONG_DEVICE_PASSWORD);

        if (!getProgDevicePhysicalAddress().toString().equals(Updater.PHYS_ADDRESS_BOOTLOADER.toString()))
            builder += String.format(" --%s %s", OPT_LONG_PROG_DEVICE, getProgDevicePhysicalAddress());

        if (getDevicePhysicalAddress() != null)
            builder += String.format(" --%s %s", OPT_LONG_DEVICE, getDevicePhysicalAddress());

        if (!getOwnPhysicalAddress().toString().equals(Updater.PHYS_ADDRESS_OWN.toString()))
            builder += String.format(" --%s %s", OPT_LONG_OWN_ADDRESS, getOwnPhysicalAddress());

        if (!getUid().isBlank())
            builder += String.format(" --%s %s", OPT_LONG_UID, getUid());

        if (getDelayMs() > 0)
            builder += String.format(" --%s %d", OPT_LONG_DELAY, getDelayMs());

        if (getTunnelingV2isSet())
            builder += String.format(" --%s", OPT_LONG_TUNNEL_V2);

        if (getTunnelingV1isSet())
            builder += String.format(" --%s", OPT_LONG_TUNNEL_V1);

        if (getNatIsSet())
            builder += String.format(" --%s", OPT_LONG_NAT);

        if (getRoutingIsSet())
            builder += String.format(" --%s", OPT_LONG_ROUTING);

        if (getFlashingFullModeIsSet())
            builder += String.format(" --%s", OPT_LONG_FULL);

        if (getHelpIsSet())
            builder += String.format(" --%s", OPT_LONG_HELP);

        if (getVersionIsSet())
            builder += String.format(" --%s", OPT_LONG_VERSION);

        if (getNoFlashIsSet())
            builder += String.format(" --%s", OPT_LONG_NO_FLASH);

        if (getLogLevel() != defaultLogLevel)
            builder += String.format(" --%s %s", OPT_LONG_LOGLEVEL, getLogLevel());

        if (getPriority() != Priority.LOW)
            builder += String.format(" --%s %s", OPT_LONG_PRIORITY, getPriority());

        if (getEraseFullFlashIsSet())
            builder += String.format(" --%s", OPT_LONG_ERASEFLASH);

        if (getDumpFlashStartAddress() >= 0)
            builder += String.format(" --%s 0x%08X 0x%08X", OPT_LONG_DUMPFLASH,
                    getDumpFlashStartAddress(), getDumpFlashEndAddress());

        if (getLogStatisticsIsSet())
            builder += String.format(" --%s", OPT_LONG_LOGSTATISTIC);

        if (getBlockSize() != Mcu.UPD_PROGRAM_SIZE)
            builder += String.format(" --%s %s", OPT_LONG_BLOCKSIZE, getBlockSize());

        return builder.trim();
    }

    public String helpToString() {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        helper.setWidth(PRINT_WIDTH);
        helper.setOptionComparator(null);
        helper.printHelp(pw, helper.getWidth(), helpApplicationName + " <KNX Interface>",
                System.lineSeparator() + helpHeader + ":" + System.lineSeparator(), cliOptions, helper.getLeftPadding(),
                helper.getDescPadding(), helpFooter, true);
        pw.flush();
        return sw.toString();
    }

    private String nonNullString(String s) {
        return s != null ? s : "";
    }

    public String getKnxInterface() {
        return nonNullString(knxInterface);
    }

    private void setKnxInterface(String knxInterface) {
        this.knxInterface = knxInterface;
        logger.debug("knxInterface={}", getKnxInterface());
    }

    public String getFileName() {
        return fileName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
        logger.debug("{}={}", OPT_LONG_FILENAME, getFileName());
    }

    public String getLocalhost() {
        return localhost;
    }

    private void setLocalhost(String localhost) {
        this.localhost = localhost;
        logger.debug("{}={}", OPT_LONG_LOCALHOST, getLocalhost());
    }

    public int getLocalPort() {
        return localPort;
    }

    private void setLocalPort(int localPort) {
        if ((localPort >= 0) && (localPort <= 65535)) {
            this.localPort = localPort;
            logger.debug("{}={}", OPT_LONG_LOCALPORT, getLocalPort());
        }
        else {
            this.localPort = 0;
            logger.warn("{}option --{} {} is invalid (min:{}, max:{}) => set to {}{}",
                    ansi().fg(RED), OPT_LONG_LOCALPORT, localPort, 0, 65535, this.localPort, ansi().reset());
        }
    }

    private void setLocalPort(String localPort) {
        try {
            if (localPort == null || localPort.isBlank())
                setLocalPort(KNXnetIPConnection.DEFAULT_PORT);
            else
                setLocalPort(Integer.parseInt(localPort));
        }
        catch (NumberFormatException e) {
            setLocalPort(0);
            logger.warn("{}option --{} {} is invalid => set to default {}{}",
                    ansi().fg(RED), OPT_LONG_LOCALPORT, localPort, getLocalPort(), ansi().reset());
        }
    }

    public int getPort() {
        return port;
    }

    private void setPort(int port) {
        if ((port >= 1) && (port <= 65535)) {
            this.port = port;
            logger.debug("{}={}", OPT_LONG_PORT, getPort());
        }
        else {
            this.port = KNXnetIPConnection.DEFAULT_PORT;
            logger.warn("{}option --{} {} is invalid (min:{}, max:{}) => set to {}{}",
                    ansi().fg(RED), OPT_LONG_PORT, port, 1, 65535, this.port, ansi().reset());
        }
    }

    private void setPort(String port) {
        try {
            if (port == null || port.isBlank())
                setPort(KNXnetIPConnection.DEFAULT_PORT);
            else
                setPort(Integer.parseInt(port));
        }
        catch (NumberFormatException e) {
            setPort(KNXnetIPConnection.DEFAULT_PORT);
            logger.warn("{}option --{} {} is invalid => set to default {}{}",
                    ansi().fg(RED), OPT_LONG_PORT, port, getPort(), ansi().reset());
        }
    }

    public boolean getNatIsSet() {
        return natIsSet;
    }

    public void setNatIsSet(boolean natIsSet) {
        this.natIsSet = natIsSet;
        logger.debug("{}={}", OPT_LONG_NAT, getNatIsSet());
    }

    public String getFt12SerialPort() {
        return nonNullString(ft12SerialPort);
    }

    private void setFt12SerialPort(String ft12SerialPort) {
        this.ft12SerialPort = ft12SerialPort;
        logger.debug("{}={}", OPT_LONG_FT12, getFt12SerialPort());
    }

    public String getTpuartSerialPort() {
        return nonNullString(tpuartSerialPort);
    }

    private void setTpuartSerialPort(String tpuartSerialPort) {
        this.tpuartSerialPort = tpuartSerialPort;
        logger.debug("{}={}", OPT_LONG_TPUART, getTpuartSerialPort());
    }

    public boolean getTunnelingV2isSet() {
        return tunnelingV2isSet;
    }

    public void setTunnelingV2isSet(boolean tunnelingV2isSet) {
        this.tunnelingV2isSet = tunnelingV2isSet;
        logger.debug("{}={}", OPT_LONG_TUNNEL_V2, getTunnelingV2isSet());
    }

    public boolean getTunnelingV1isSet() {
        return tunnelingV1isSet;
    }

    public void setTunnelingV1isSet(boolean tunnelingV1isSet) {
        this.tunnelingV1isSet = tunnelingV1isSet;
        logger.debug("{}={}", OPT_LONG_TUNNEL_V1, getTunnelingV2isSet());
    }

    public boolean getRoutingIsSet() {
        return routingIsSet;
    }

    private void setRoutingIsSet(boolean routingIsSet) {
        this.routingIsSet = routingIsSet;
        logger.debug("{}={}", OPT_LONG_ROUTING, getRoutingIsSet());
    }

    public String getMedium() {
        return medium;
    }

    private void setMedium(String medium) {
        this.medium = medium;
        logger.debug("{}={}", OPT_LONG_MEDIUM, getMedium());
    }

    public IndividualAddress getProgDevicePhysicalAddress() {
        return progDevicePhysicalAddress;
    }

    private void setProgDevicePhysicalAddress(String progDevicePhysicalAddress) throws KNXFormatException {
        if ((progDevicePhysicalAddress != null) && (!progDevicePhysicalAddress.isBlank())) {
            this.progDevicePhysicalAddress = new IndividualAddress(progDevicePhysicalAddress);
        }
        else {
            this.progDevicePhysicalAddress = Updater.PHYS_ADDRESS_BOOTLOADER;
        }
        logger.debug("{}={}", OPT_LONG_PROG_DEVICE, getProgDevicePhysicalAddress());
    }

    public IndividualAddress getDevicePhysicalAddress() {
        return devicePhysicalAddress;
    }

    private void setDevicePhysicalAddress(String devicePhysicalAddress) throws KNXFormatException {
        if ((devicePhysicalAddress != null) && (!devicePhysicalAddress.isBlank())) {
            this.devicePhysicalAddress = new IndividualAddress(devicePhysicalAddress);
        }
        else {
            this.devicePhysicalAddress = null;
        }
        logger.debug("{}={}", OPT_LONG_DEVICE, getDevicePhysicalAddress());
    }

    public IndividualAddress getOwnPhysicalAddress() {
        return ownPhysicalAddress;
    }

    private void setOwnPhysicalAddress(String ownPhysicalAddress) throws KNXFormatException {
        if ((ownPhysicalAddress != null) && (!ownPhysicalAddress.isBlank())) {
            this.ownPhysicalAddress = new IndividualAddress(ownPhysicalAddress);
        }
        else {
            this.ownPhysicalAddress = Updater.PHYS_ADDRESS_OWN;
        }
        logger.debug("{}={}", OPT_LONG_OWN_ADDRESS, getOwnPhysicalAddress());
    }

    public String getUid() {
        return uid;
    }

    public void setUid(String uidString) {
        this.uid = uidString;
        logger.debug("{}={}", OPT_LONG_UID, getUid());
    }

    public boolean getFlashingFullModeIsSet() {
        return flashingFullModeIsSet;
    }

    private void setFlashingFullModeIsSet(boolean flashingFullModeIsSet) {
        this.flashingFullModeIsSet = flashingFullModeIsSet;
        logger.debug("{}={}", OPT_LONG_FULL, getFlashingFullModeIsSet());
    }

    public int getDelayMs() {
        return delayMs;
    }

    private void setDelayMs(int delayMs) {
        if ((delayMs < Updater.DELAY_MIN) || (delayMs > Updater.DELAY_MAX)) {
            logger.warn("{}option --{} {} is invalid (min:{}, max:{}) => set to {}{}",
                    ansi().fg(RED), OPT_LONG_DELAY, delayMs, Updater.DELAY_MIN,
                    Updater.DELAY_MAX, Updater.DELAY_MIN, ansi().reset());
            delayMs = Updater.DELAY_MIN; // set to DELAY_MIN in case of invalid waiting time
        }
        this.delayMs = delayMs;
        logger.debug("{}={}", OPT_LONG_DELAY, getDelayMs());
    }

    private void setDelayMs(String delayMs) {
        try {
            if (delayMs == null || delayMs.isBlank())
                setDelayMs(Updater.DELAY_MIN);
            else
                setDelayMs(Integer.parseInt(delayMs));
        }
        catch (NumberFormatException e) {
            setDelayMs(Updater.DELAY_MIN);
            logger.warn("{}option --{} {} is invalid => set to default {}{}",
                    ansi().fg(RED), OPT_LONG_DELAY, delayMs, getDelayMs(), ansi().reset());
        }
    }

    public boolean getNoFlashIsSet() {
        return noFlashIsSet;
    }

    private void setNoFlashIsSet(boolean noFlashIsSet) {
        this.noFlashIsSet = noFlashIsSet;
        logger.debug("{}={}", OPT_LONG_NO_FLASH, getNoFlashIsSet());
    }

    public boolean getEraseFullFlashIsSet() {
        return eraseFullFlashIsSet;
    }

    private void setEraseFullFlashIsSet(boolean eraseFullFlashIsSet) {
        this.eraseFullFlashIsSet = eraseFullFlashIsSet;
        logger.debug("{}={}", OPT_LONG_ERASEFLASH, getEraseFullFlashIsSet());
    }

    public long getDumpFlashStartAddress() {
        return dumpFlashStartAddress;
    }

    private void setDumpFlashStartAddress(long dumpFlashStartAddress) {
        this.dumpFlashStartAddress = dumpFlashStartAddress;
        logger.debug("dumpFlashStartAddress={}", getDumpFlashStartAddress());
    }

    public long getDumpFlashEndAddress() {
        return dumpFlashEndAddress;
    }

    private void setDumpFlashEndAddress(long dumpFlashEndAddress) {
        this.dumpFlashEndAddress = dumpFlashEndAddress;
        logger.debug("dumpFlashEndAddress={}", getDumpFlashEndAddress());
    }

    public boolean getHelpIsSet() {
        if (cmdLine == null)
            return false;
        else
            return cmdLine.hasOption(OPT_LONG_HELP);
    }

    public boolean getVersionIsSet() {
        if (cmdLine == null)
            return false;
        else
            return cmdLine.hasOption(OPT_LONG_VERSION);
    }

    public Level getLogLevel() {
        ch.qos.logback.classic.Logger root = (ch.qos.logback.classic.Logger) LoggerFactory.getLogger(Logger.ROOT_LOGGER_NAME);
        return root.getLevel();
    }

    private void setLogLevel(Level logLevel) {
        ch.qos.logback.classic.Logger root = (ch.qos.logback.classic.Logger) LoggerFactory.getLogger(Logger.ROOT_LOGGER_NAME);
        root.setLevel(logLevel);
        logger.debug("{}={}", OPT_LONG_LOGLEVEL, getLogLevel());
    }

    private void setLogLevel(String newLevel) {
        newLevel = newLevel.toUpperCase();
        if (VALID_LOG_LEVELS.contains(newLevel)) {
            setLogLevel(Level.toLevel(newLevel));
        }
        else {
            logger.warn("{}invalid {} {}, using {}{}", ansi().fg(RED), OPT_LONG_LOGLEVEL, newLevel,
                    getLogLevel().toString(), ansi().reset());
        }
    }

    public int getKnxSecureUserId() {
        return knxSecureUserId;
    }

    private void setKnxSecureUserId(int knxSecureUserId) {
        if (knxSecureUserId == INVALID_SECURE_USER_ID) {
            this.knxSecureUserId = INVALID_SECURE_USER_ID;
            logger.debug("{}=", OPT_LONG_USER_ID);
            return;
        }

        if (knxSecureUserId >= 1 && knxSecureUserId <= 127) {
            logger.debug("{}=***", OPT_LONG_USER_ID); // log only that it´s, but not the actual value
            this.knxSecureUserId = knxSecureUserId;
        }
        else {
            logger.warn("{}option --{} *** is invalid => set to default{}",
                    ansi().fg(RED), OPT_LONG_USER_ID, ansi().reset());
            this.knxSecureUserId = INVALID_SECURE_USER_ID;
            logger.debug("{}=", OPT_LONG_USER_ID);
        }
    }

    private void setKnxSecureUserId(String knxSecureUserId) {
        try {
            if (knxSecureUserId == null || knxSecureUserId.isBlank())
                setKnxSecureUserId(INVALID_SECURE_USER_ID);
            else
                setKnxSecureUserId(Integer.parseInt(knxSecureUserId));
        }
        catch (NumberFormatException e) {
            setKnxSecureUserId(INVALID_SECURE_USER_ID);
            logger.warn("{}option --{} *** is invalid => set to default{}",
                    ansi().fg(RED), OPT_LONG_USER_ID, ansi().reset());
        }
    }

    public String getKnxSecureUserPassword() {
        return knxSecureUserPassword;
    }

    private void setKnxSecureUserPassword(String knxSecureUserPassword) {
        this.knxSecureUserPassword = knxSecureUserPassword;
        if (!nonNullString(this.knxSecureUserPassword).isBlank())
            logger.debug("{}=****", OPT_LONG_USER_PASSWORD); // log only that it´s, but not the actual value
        else
            logger.debug("{}=", OPT_LONG_USER_PASSWORD);
    }

    public String getKnxSecureDevicePassword() {
        return knxSecureDevicePassword;
    }

    private void setKnxSecureDevicePassword(String knxSecureDevicePassword) {
        this.knxSecureDevicePassword = knxSecureDevicePassword;
        if (!nonNullString(this.knxSecureDevicePassword).isBlank())
            logger.debug("{}=*****", OPT_LONG_DEVICE_PASSWORD); // log only that it´s, but not the actual value
        else
            logger.debug("{}=", OPT_LONG_DEVICE_PASSWORD);
    }

    public Priority getPriority() {
        return priority;
    }

    private void setPriority(String priority) {
        try {
            this.priority = Priority.get(priority);
            logger.debug("{}={}", OPT_LONG_PRIORITY, getPriority().toString());
        }
        catch (KNXIllegalArgumentException e) {
            logger.warn("{}invalid --{} {}, using {}{}",ansi().fg(RED),  OPT_LONG_PRIORITY,
                    priority, getPriority(), ansi().reset());
        }
    }

    public boolean getLogStatisticsIsSet() {
        return logStatisticsIsSet;
    }

    private void setLogStatisticsIsSet(boolean logStatisticsIsSet) {
        this.logStatisticsIsSet = logStatisticsIsSet;
        logger.debug("{}={}", OPT_LONG_LOGSTATISTIC , getLogStatisticsIsSet());
    }

    public int getBlockSize() {
        return blockSize;
    }

    private void setBlockSize(int blockSize) {
        if (VALID_BLOCKSIZES.contains(blockSize)) {
            this.blockSize = blockSize;
        }
        else {
            this.blockSize = Mcu.UPD_PROGRAM_SIZE;
            logger.info("{}--{} {} is not supported => Set --{} to default {} bytes{}", ansi().fg(YELLOW),
                    OPT_LONG_BLOCKSIZE, blockSize, OPT_LONG_BLOCKSIZE, getBlockSize(), ansi().reset());
        }
        logger.debug("{}={}", OPT_LONG_BLOCKSIZE, getBlockSize());
    }

    private void setBlockSize(String blockSize) {
        try {
            if (blockSize == null || blockSize.isBlank())
                setBlockSize(Mcu.UPD_PROGRAM_SIZE);
            else
                setBlockSize(Integer.parseInt(blockSize));
        }
        catch (NumberFormatException e) {
            setBlockSize(Mcu.UPD_PROGRAM_SIZE);
            logger.warn("{}option --{} {} is invalid => set to default {}{}",
                    ansi().fg(RED), OPT_LONG_BLOCKSIZE, blockSize, getBlockSize(), ansi().reset());
        }
    }

    public String getUsbVendorIdAndProductId() {
        return nonNullString(usbVendorIdAndProductId);
    }

    private void setUsbVendorIdAndProductId(String usbVendorIdAndProductId) {
        this.usbVendorIdAndProductId = usbVendorIdAndProductId;
        logger.debug("{}={}", OPT_LONG_USB, getUsbVendorIdAndProductId());
    }

    public boolean getDiscoverIsSet() {
        return discoverIsSet;
    }

    public void setDiscoverIsSet(boolean discoverIsSet) {
        this.discoverIsSet = discoverIsSet;
        logger.debug("{}={}", OPT_LONG_DISCOVER, this.discoverIsSet);
    }
}