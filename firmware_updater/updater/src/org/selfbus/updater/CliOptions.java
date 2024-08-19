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
import tuwien.auto.calimero.link.medium.TPSettings;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Arrays;
import java.util.List;

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
    private final static List<Integer> VALID_BLOCKSIZES = Arrays.asList(256, 512, 1024);

    private static final int PRINT_WIDTH = 100;
    private final static List<String> VALID_LOG_LEVELS = Arrays.asList("TRACE", "DEBUG", "INFO");

    private final Options cliOptions = new Options();
    // define parser
    CommandLine cmdLine;
    final HelpFormatter helper = new HelpFormatter();

    private final String helpHeader;
    private final String helpFooter;
    private final String helpApplicationName;

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
    private String medium = "tp1";

    private int knxSecureUserId = 1;
    private String knxSecureUserPassword = "";
    private String knxSecureDevicePassword = "";


    private IndividualAddress progDevicePhysicalAddress;
    private IndividualAddress ownPhysicalAddress;
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

    public CliOptions(final String[] args, String helpApplicationName, String helpHeader, String helpFooter,
                      IndividualAddress progDevicePhysicalAddress, IndividualAddress ownAddress) {
        this.helpApplicationName = helpApplicationName;
        this.helpHeader = helpHeader;
        this.helpFooter = helpFooter;
        setProgDevicePhysicalAddress(progDevicePhysicalAddress); //todo check why we have this as a method parameter and why is it set here?
        setOwnPhysicalAddress(ownAddress); //todo check why we have this as a method parameter and why is it set here?

        Option tunnelingV2 = new Option(OPT_SHORT_TUNNEL_V2, OPT_LONG_TUNNEL_V2, false, "use KNXnet/IP tunneling v2 (TCP) (experimental)");
        Option tunnelingV1 = new Option(OPT_SHORT_TUNNEL_V1, OPT_LONG_TUNNEL_V1, false, "use KNXnet/IP tunneling v1 (UDP)");
        Option nat = new Option(OPT_SHORT_NAT, OPT_LONG_NAT, false, "enable Network Address Translation (NAT) (only available with tunneling v1)");
        Option routing = new Option(OPT_SHORT_ROUTING, OPT_LONG_ROUTING, false, "use KNXnet/IP routing/multicast (experimental)");
        Option full = new Option(OPT_SHORT_FULL, OPT_LONG_FULL, false, "force full upload mode (disables differential mode)");
        Option help = new Option(OPT_SHORT_HELP, OPT_LONG_HELP, false, "show this help message");
        Option version = new Option(OPT_SHORT_VERSION, OPT_LONG_VERSION, false, "show tool/library version");
        Option NO_FLASH = new Option(OPT_SHORT_NO_FLASH, OPT_LONG_NO_FLASH, false, "for debugging use only, disable flashing firmware!");
        Option eraseFlash = new Option(null, OPT_LONG_ERASEFLASH, false, "USE WITH CAUTION! Erases the complete flash memory including the physical KNX address and all settings of the device. Only the bootloader is not deleted.");

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
                .desc(String.format("Logfile logging level [TRACE|DEBUG|INFO] (default %s)", getLogLevel().toString())).build();

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

        // usb, ft12 or tpuart, not both
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


        parse(args);
    }

    private void parse(final String[] args) {
        CommandLineParser parser = new DefaultParser();
        try {
            cmdLine = parser.parse(cliOptions, args);
            if (cmdLine.hasOption(OPT_SHORT_LOGLEVEL)) {
                String cliLogLevel = cmdLine.getOptionValue(OPT_SHORT_LOGLEVEL).toUpperCase();
                if (VALID_LOG_LEVELS.contains(cliLogLevel)) {
                    setLogLevel(Level.toLevel(cliLogLevel));
                }
                else {
                    logger.warn("{}invalid {} {}, using {}{}", ansi().fg(RED), OPT_LONG_LOGLEVEL, cliLogLevel,
                            getLogLevel().toString(), ansi().reset());
                }
            }
            logger.debug("logLevel={}", getLogLevel());

            if (cmdLine.hasOption(OPT_LONG_PRIORITY)) {
                try {
                    setPriority(Priority.get(cmdLine.getOptionValue(OPT_LONG_PRIORITY))); //todo simplify
                }
                catch (KNXIllegalArgumentException e) {
                    logger.warn("{}invalid --{} {}, using {}{}",ansi().fg(RED),  OPT_LONG_PRIORITY,
                            cmdLine.getOptionValue(OPT_LONG_PRIORITY), getPriority(), ansi().reset());
                }
            }
            logger.debug("priority={}", getPriority().toString());

            if (getHelpIsSet()) {
                logger.debug("help={}", getHelpIsSet());
                return;
            }

            if (getVersionIsSet()) {
                logger.debug("version={}", getVersionIsSet());
                return;
            }

            setNoFlashIsSet(cmdLine.hasOption(OPT_SHORT_NO_FLASH));
            logger.debug("NO_FLASH={}", getNoFlashIsSet());

            setEraseFullFlashIsSet(cmdLine.hasOption(OPT_LONG_ERASEFLASH));
            logger.debug("eraseFlash={}", getEraseFullFlashIsSet());

            if (cmdLine.hasOption(OPT_LONG_DUMPFLASH)) {
                String[] optArgs = cmdLine.getOptionValues(OPT_LONG_DUMPFLASH);
                setDumpFlashStartAddress(Long.decode(optArgs[0]));
                setDumpFlashEndAddress(Long.decode(optArgs[1]));
            }
            logger.debug("dumpFlashStartAddress={}", getDumpFlashStartAddress());
            logger.debug("dumpFlashEndAddress={}", getDumpFlashEndAddress());

            setFlashingFullModeIsSet(cmdLine.hasOption(OPT_SHORT_FULL));
            logger.debug("full={}", getFlashingFullModeIsSet());

            setTunnelingV2isSet(cmdLine.hasOption(OPT_SHORT_TUNNEL_V2));
            logger.debug("tunnelingV2={}", getTunnelingV2isSet());

            setTunnelingV1isSet(cmdLine.hasOption(OPT_SHORT_TUNNEL_V1));
            logger.debug("tunneling={}", getTunnelingV2isSet());

            setRoutingIsSet(cmdLine.hasOption(OPT_SHORT_ROUTING));
            logger.debug("routing={}", getRoutingIsSet());

            setNatIsSet(cmdLine.hasOption(OPT_SHORT_NAT));
            logger.debug("nat={}", getNatIsSet());

            if (cmdLine.hasOption(OPT_SHORT_FILENAME)) {
                setFileName(cmdLine.getOptionValue(OPT_SHORT_FILENAME));
            }
            logger.debug("fileName={}", getFileName());

            if (cmdLine.hasOption(OPT_SHORT_LOCALHOST)) {
                setLocalhost(cmdLine.getOptionValue(OPT_SHORT_LOCALHOST));
            }
            logger.debug("localhost={}", getLocalhost());

            if (cmdLine.hasOption(OPT_SHORT_LOCALPORT)) {
                setLocalPort(((Number)cmdLine.getParsedOptionValue(OPT_SHORT_LOCALPORT)).intValue());
            }
            logger.debug("localport={}", getLocalPort());

            if (cmdLine.hasOption(OPT_SHORT_PORT)) {
                setPort(((Number)cmdLine.getParsedOptionValue(OPT_SHORT_PORT)).intValue());
            }
            logger.debug("port={}", getPort());

            if (cmdLine.hasOption(OPT_SHORT_MEDIUM)) {
                setMedium(cmdLine.getOptionValue(OPT_SHORT_MEDIUM));
            }
            logger.debug("medium={}", getMedium());

            if (cmdLine.hasOption(OPT_LONG_DELAY)) {
                setDelayMs(((Number)cmdLine.getParsedOptionValue(OPT_LONG_DELAY)).intValue());
            }
            logger.debug("delay={}", getDelayMs());

            if (cmdLine.hasOption(OPT_SHORT_BLOCKSIZE)) {
                int newBlockSize = ((Number)cmdLine.getParsedOptionValue(OPT_LONG_BLOCKSIZE)).intValue();
                if (VALID_BLOCKSIZES.contains(newBlockSize)) {
                    setBlockSize(newBlockSize);
                }
                else {
                    logger.info("{}--{} {} is not supported => Set --{} to default {} bytes{}", ansi().fg(YELLOW),
                            OPT_LONG_BLOCKSIZE, newBlockSize, OPT_LONG_BLOCKSIZE, getBlockSize(), ansi().reset());
                }
            }
            logger.debug("{}={}", OPT_LONG_BLOCKSIZE, getBlockSize());

            if (cmdLine.hasOption(OPT_SHORT_UID)) {
               setUid(cmdLine.getOptionValue(OPT_SHORT_UID));
            }
            logger.debug("uid={}", getUid());

            if (cmdLine.hasOption(OPT_SHORT_DEVICE)) {
                setDevicePhysicalAddress(new IndividualAddress(cmdLine.getOptionValue(OPT_SHORT_DEVICE)));
            } else {
                setDevicePhysicalAddress(null);
            }
            logger.debug("device={}", getDevicePhysicalAddress());

            if (cmdLine.hasOption(OPT_SHORT_PROG_DEVICE)) {
                setProgDevicePhysicalAddress(new IndividualAddress(cmdLine.getOptionValue(OPT_SHORT_PROG_DEVICE)));
            }
            logger.debug("progDevice={}", getProgDevicePhysicalAddress());

            if (cmdLine.hasOption(OPT_SHORT_OWN_ADDRESS)) {
                setOwnPhysicalAddress(new IndividualAddress(cmdLine.getOptionValue(OPT_SHORT_OWN_ADDRESS)));
            }
            logger.debug("ownAddress={}", getOwnPhysicalAddress());

            if (cmdLine.hasOption(OPT_SHORT_FT12)) {
               setFt12SerialPort(cmdLine.getOptionValue(OPT_SHORT_FT12));
            }
            logger.debug("ft12={}", getFt12SerialPort());

            if (cmdLine.hasOption(OPT_SHORT_TPUART)) {
                setTpuartSerialPort(cmdLine.getOptionValue(OPT_SHORT_TPUART));
            }
            logger.debug("tpuart={}", getTpuartSerialPort());

            if (cmdLine.hasOption(OPT_LONG_USB)) {
                setUsbVendorIdAndProductId(cmdLine.getOptionValue(OPT_LONG_USB));
            }
            logger.debug("usb={}", getUsbVendorIdAndProductId());

            if (cmdLine.getArgs().length > 0) {
                setKnxInterface(cmdLine.getArgs()[0]);
            }
            else {
                setKnxInterface(null);
            }
            logger.debug("knxInterface={}", getKnxInterface());

            if (cmdLine.hasOption(OPT_LONG_USER_ID)) {
                logger.debug("KNX IP Secure --{} is set", OPT_LONG_USER_ID); // log only that it´s, but not the actual value
                setKnxSecureUserId(((Number)cmdLine.getParsedOptionValue(OPT_LONG_USER_ID)).intValue());
            }

            if (cmdLine.hasOption(OPT_LONG_USER_PASSWORD)) {
                logger.debug("KNX IP Secure --{} is set", OPT_LONG_USER_PASSWORD); // log only that it´s, but not the actual value
                setKnxSecureUserPassword(cmdLine.getOptionValue(OPT_LONG_USER_PASSWORD));
            }

            if (cmdLine.hasOption(OPT_LONG_DEVICE_PASSWORD)) {
                logger.debug("KNX IP Secure --{} is set", OPT_LONG_DEVICE_PASSWORD); // log only that it´s, but not the actual value
                setKnxSecureDevicePassword(cmdLine.getOptionValue(OPT_LONG_DEVICE_PASSWORD));
            }

            setLogStatisticsIsSet(cmdLine.hasOption(OPT_LONG_LOGSTATISTIC));
            logger.debug("logStatistics={}", getLogStatisticsIsSet());

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
                if (getKnxInterface().isEmpty()) {
                    throw new CliInvalidException(String.format("%sNo IP-Interface specified for IP-secure%s",
                            ansi().fg(RED), ansi().reset()));
                }
                else if (!getUsbVendorIdAndProductId().isEmpty()) {
                    throw new CliInvalidException(String.format("%sIP-secure is not possible with --%s%s",
                            ansi().fg(RED), OPT_LONG_USB, ansi().reset()));
                }
                else if (!getFt12SerialPort().isEmpty()) {
                    throw new CliInvalidException(String.format("%sIP-secure is not possible with --%s%s",
                            ansi().fg(RED), OPT_LONG_FT12, ansi().reset()));
                }
                else if (!getTpuartSerialPort().isEmpty()) {
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
            if (!getKnxInterface().isEmpty()) interfacesSet++;
            if (getRoutingIsSet()) interfacesSet++;
            if (!getFt12SerialPort().isEmpty()) interfacesSet++;
            if (!getTpuartSerialPort().isEmpty()) interfacesSet++;
            if (!getUsbVendorIdAndProductId().isEmpty()) interfacesSet++;

            if (interfacesSet > 1) {
                throw new CliInvalidException(String.format("%sOnly one bus interface can be used.%s",
                        ansi().fg(RED), ansi().reset()));
            }
            else if (interfacesSet == 0) {
                throw new CliInvalidException(String.format("%sNo bus interface specified.%s",
                        ansi().fg(RED), ansi().reset()));
            }
        }
        catch (CliInvalidException e) {
            logCliException(e, args, false);
            System.exit(0);
        }
        catch (ParseException | KNXFormatException e) {
            logCliException(e, args, true);
            System.exit(0);
        }
    }

    private void logCliException(Exception e, String[] args, boolean verbose) {
        String cliParsed = argsToString(args);
        System.out.printf("Invalid command line parameters: '%s'%s", cliParsed,
                System.lineSeparator()); // don't log possible IP-secure parameters
        logger.debug("Invalid command line parameters:");
        logger.error("{}{}{}", ansi().fg(RED), e.getMessage(), ansi().reset());
        logger.error("For more information about the usage start with --{}", OPT_LONG_HELP);
        if (verbose) {
            logger.error("", e);
        }
    }

    private String argsToString(final String[] args) {
        StringBuilder result = new StringBuilder();
        for (String arg : args) {
            result.append(String.format("%s ", arg));
        }
        return result.toString().trim();
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

    public String getKnxInterface() {
        return knxInterface;
    }

    private void setKnxInterface(String knxInterface) {
        this.knxInterface = knxInterface;
    }

    public String getFileName() {
        return fileName;
    }

    private void setFileName(String fileName) {
        this.fileName = fileName;
    }

    public String getLocalhost() {
        return localhost;
    }

    private void setLocalhost(String localhost) {
        this.localhost = localhost;
    }

    public int getLocalPort() {
        return localPort;
    }

    private void setLocalPort(int localPort) {
        this.localPort = localPort;
    }

    public int getPort() {
        return port;
    }

    private void setPort(int port) {
        this.port = port;
    }

    public boolean getNatIsSet() {
        return natIsSet;
    }

    private void setNatIsSet(boolean natIsSet) {
        this.natIsSet = natIsSet;
    }

    public String getFt12SerialPort() {
        return ft12SerialPort;
    }

    private void setFt12SerialPort(String ft12SerialPort) {
        this.ft12SerialPort = ft12SerialPort;
    }

    public String getTpuartSerialPort() {
        return tpuartSerialPort;
    }

    private void setTpuartSerialPort(String tpuartSerialPort) {
        this.tpuartSerialPort = tpuartSerialPort;
    }

    public boolean getTunnelingV2isSet() {
        return tunnelingV2isSet;
    }

    private void setTunnelingV2isSet(boolean tunnelingV2isSet) {
        this.tunnelingV2isSet = tunnelingV2isSet;
    }

    public boolean getTunnelingV1isSet() {
        return tunnelingV1isSet;
    }

    private void setTunnelingV1isSet(boolean tunnelingV1isSet) {
        this.tunnelingV1isSet = tunnelingV1isSet;
    }

    public boolean getRoutingIsSet() {
        return routingIsSet;
    }

    private void setRoutingIsSet(boolean routingIsSet) {
        this.routingIsSet = routingIsSet;
    }

    public String getMedium() {
        return medium;
    }

    private void setMedium(String medium) {
        this.medium = medium;
    }

    public IndividualAddress getProgDevicePhysicalAddress() {
        return progDevicePhysicalAddress;
    }

    private void setProgDevicePhysicalAddress(IndividualAddress progDevicePhysicalAddress){
        this.progDevicePhysicalAddress = progDevicePhysicalAddress;
    }

    public IndividualAddress getDevicePhysicalAddress() {
        return devicePhysicalAddress;
    }

    private void setDevicePhysicalAddress(IndividualAddress devicePhysicalAddress) {
        this.devicePhysicalAddress = devicePhysicalAddress;
    }

    public IndividualAddress getOwnPhysicalAddress() {
        return ownPhysicalAddress;
    }

    private void setOwnPhysicalAddress(IndividualAddress ownPhysicalAddress) {
        this.ownPhysicalAddress = ownPhysicalAddress;
    }

    public String getUid() {
        return uid;
    }

    private void setUid(String uidString) {
        this.uid = uidString;
    }

    public boolean getFlashingFullModeIsSet() {
        return flashingFullModeIsSet;
    }

    private void setFlashingFullModeIsSet(boolean flashingFullModeIsSet) {
        this.flashingFullModeIsSet = flashingFullModeIsSet;
    }

    public int getDelayMs() {
        return delayMs;
    }

    private void setDelayMs(int delayMs) {
        if ((delayMs < Updater.DELAY_MIN) || (delayMs > Updater.DELAY_MAX)) {
            logger.warn("{}option --{} {} is invalid (min:{}, max:{}) => set to {}{}",
                    ansi().fg(RED), OPT_LONG_DELAY, delayMs, Updater.DELAY_MIN,
                    Updater.DELAY_MAX, Updater.DELAY_DEFAULT, ansi().reset());
            delayMs = Updater.DELAY_DEFAULT;    // set to DELAY_DEFAULT in case of invalid waiting time
        }
        this.delayMs = delayMs;
    }

    public boolean getNoFlashIsSet() {
        return noFlashIsSet;
    }

    private void setNoFlashIsSet(boolean noFlashIsSet) {
        this.noFlashIsSet = noFlashIsSet;
    }

    public boolean getEraseFullFlashIsSet() {
        return eraseFullFlashIsSet;
    }

    private void setEraseFullFlashIsSet(boolean eraseFullFlashIsSet) {
        this.eraseFullFlashIsSet = eraseFullFlashIsSet;
    }

    public long getDumpFlashStartAddress() {
        return dumpFlashStartAddress;
    }

    private void setDumpFlashStartAddress(long dumpFlashStartAddress) {
        this.dumpFlashStartAddress = dumpFlashStartAddress;
    }

    public long getDumpFlashEndAddress() {
        return dumpFlashEndAddress;
    }

    private void setDumpFlashEndAddress(long dumpFlashEndAddress) {
        this.dumpFlashEndAddress = dumpFlashEndAddress;
    }

    public boolean getHelpIsSet() {
        return cmdLine.hasOption(OPT_SHORT_HELP);
    }

    public boolean getVersionIsSet() {
        return cmdLine.hasOption(OPT_SHORT_VERSION);
    }

    public Level getLogLevel() {
        ch.qos.logback.classic.Logger root = (ch.qos.logback.classic.Logger) LoggerFactory.getLogger(Logger.ROOT_LOGGER_NAME);
        return root.getLevel();
    }

    private void setLogLevel(Level newLevel) {
        ch.qos.logback.classic.Logger root = (ch.qos.logback.classic.Logger) LoggerFactory.getLogger(Logger.ROOT_LOGGER_NAME);
        root.setLevel(newLevel);
    }

    public int getKnxSecureUserId() {
        return knxSecureUserId;
    }

    private void setKnxSecureUserId(int knxSecureUserId) {
        this.knxSecureUserId = knxSecureUserId;
    }

    public String getKnxSecureUserPassword() {
        return knxSecureUserPassword;
    }

    private void setKnxSecureUserPassword(String knxSecureUserPassword) {
        this.knxSecureUserPassword = knxSecureUserPassword;
    }

    public String getKnxSecureDevicePassword() {
        return knxSecureDevicePassword;
    }

    private void setKnxSecureDevicePassword(String knxSecureDevicePassword) {
        this.knxSecureDevicePassword = knxSecureDevicePassword;
    }

    public Priority getPriority() {
        return priority;
    }

    private void setPriority(Priority priority) {
        this.priority = priority;
    }

    public boolean getLogStatisticsIsSet() {
        return logStatisticsIsSet;
    }

    private void setLogStatisticsIsSet(boolean logStatisticsIsSet) {
        this.logStatisticsIsSet = logStatisticsIsSet;
    }

    public int getBlockSize() {
        return blockSize;
    }

    private void setBlockSize(int blockSize) {
        this.blockSize = blockSize;
    }

    public String getUsbVendorIdAndProductId() {
        return usbVendorIdAndProductId;
    }

    public void setUsbVendorIdAndProductId(String usbVendorIdAndProductId) {
        this.usbVendorIdAndProductId = usbVendorIdAndProductId;
    }
}