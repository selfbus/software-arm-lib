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
import java.net.InetAddress;
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

    private InetAddress knxInterface;
    private String fileName = "";
    private InetAddress localhost;
    private int localport = 0;
    private int port = KNXnetIPConnection.DEFAULT_PORT;
    private boolean nat = false;
    private String ft12 = "";
    private String tpuart = "";
    private String usbInterface = "";
    private boolean tunnelingV2 = false;
    private boolean tunnelingV1 = false;
    private boolean routing = false;
    private String medium = "tp1";

    private int userId = 1;
    private String userPassword = "";
    private String devicePassword = "";


    private IndividualAddress progDevice;
    private IndividualAddress ownAddress;
    private IndividualAddress device = null;
    private byte[] uid;
    private boolean full = false;
    private int delay = 0;

    private boolean NO_FLASH = false;
    private Level logLevel = Level.DEBUG;
    private boolean eraseFullFlash = false;
    private long dumpFlashStartAddress = -1;
    private long dumpFlashEndAddress = -1;

    private Priority priority = Priority.LOW;

    private boolean logStatistics = false;
    private int blockSize = Mcu.UPD_PROGRAM_SIZE;

    private boolean help = false;
    private boolean version = false;


    public CliOptions(final String[] args, String helpApplicationName, String helpHeader, String helpFooter,
                      IndividualAddress progDevice, IndividualAddress ownAddress) {
        this.helpApplicationName = helpApplicationName;
        this.helpHeader = helpHeader;
        this.helpFooter = helpFooter;
        this.progDevice = progDevice;
        this.ownAddress = ownAddress;

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
                .desc(String.format("KNX medium [tp1|rf] (default %s)", this.medium)).build(); ///\todo not all implemented missing [tp0|p110|p132]
        Option optProgDevice = Option.builder(OPT_SHORT_PROG_DEVICE).longOpt(OPT_LONG_PROG_DEVICE)
                .argName("x.x.x")
                .numberOfArgs(1)
                .required(false)
                .type(IndividualAddress.class)
                .desc(String.format("KNX device address in bootloader mode (default %s)", this.progDevice.toString())).build();
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
                .desc(String.format("own physical KNX address (default %s)", this.ownAddress.toString())).build();
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
                .desc(String.format("Logfile logging level [TRACE|DEBUG|INFO] (default %s)", this.logLevel.toString())).build();

        Option userId = Option.builder(null).longOpt(OPT_LONG_USER_ID)
                .argName("id")
                .numberOfArgs(1)
                .required(false)
                .type(Number.class)
                .desc(String.format("KNX IP Secure tunneling user identifier (1..127) (default %d)", this.userId)).build();
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
                .desc(String.format("KNX telegram priority (default %s)", this.priority.toString().toUpperCase())).build();

        Option blockSize = Option.builder(OPT_SHORT_BLOCKSIZE).longOpt(OPT_LONG_BLOCKSIZE)
                .argName("256|512|1024")
                .valueSeparator(' ')
                .numberOfArgs(1)
                .required(false)
                .type(Number.class)
                .desc(String.format("Block size to program (default %d bytes)", this.blockSize)).build();

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
        cliOptions.addOption(blockSize);

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
            // get the log level for log file output
            ch.qos.logback.classic.Logger root = (ch.qos.logback.classic.Logger) LoggerFactory.getLogger(Logger.ROOT_LOGGER_NAME);
            if (cmdLine.hasOption(OPT_SHORT_LOGLEVEL)) {
                String cliLogLevel = cmdLine.getOptionValue(OPT_SHORT_LOGLEVEL).toUpperCase();
                if (VALID_LOG_LEVELS.contains(cliLogLevel)) {
                    logLevel = Level.toLevel(cliLogLevel);
                    root.setLevel(logLevel);
                }
                else {
                    logger.warn(ansi().fg(RED).a("invalid {} {}, using {}").reset().toString(), OPT_LONG_LOGLEVEL, cliLogLevel, root.getLevel().toString());
                }
            }
            logger.debug("logLevel={}", root.getLevel().toString());

            if (cmdLine.hasOption(OPT_LONG_PRIORITY)) {
                try {
                    priority = Priority.get(cmdLine.getOptionValue(OPT_LONG_PRIORITY));
                }
                catch (KNXIllegalArgumentException e) {
                    logger.warn(ansi().fg(RED).a("invalid --{} {}, using {}").reset().toString(),
                            OPT_LONG_PRIORITY, cmdLine.getOptionValue(OPT_LONG_PRIORITY), priority);
                }
            }
            logger.debug("priority={}", priority.toString());

            if (cmdLine.hasOption(OPT_SHORT_HELP)) {
                help = true;
                logger.debug("help={}", true);
                return;
            }

            if (cmdLine.hasOption(OPT_SHORT_VERSION)) {
                version = true;
                logger.debug("version={}", true);
                return;
            }

            if (cmdLine.hasOption(OPT_SHORT_NO_FLASH)) {
                NO_FLASH = true;
            }
            logger.debug("NO_FLASH={}", NO_FLASH);

            if (cmdLine.hasOption(OPT_LONG_ERASEFLASH)) {
                eraseFullFlash = true;
            }
            logger.debug("eraseFlash={}", eraseFullFlash);

            if (cmdLine.hasOption(OPT_LONG_DUMPFLASH)) {
                String[] optArgs = cmdLine.getOptionValues(OPT_LONG_DUMPFLASH);
                dumpFlashStartAddress = Long.decode(optArgs[0]);
                dumpFlashEndAddress = Long.decode(optArgs[1]);
            }
            logger.debug("dumpFlashStartAddress={}", dumpFlashStartAddress);
            logger.debug("dumpFlashEndAddress={}", dumpFlashEndAddress);

            if (cmdLine.hasOption(OPT_SHORT_FULL)) {
                full = true;
            }
            logger.debug("full={}", full);

            if (cmdLine.hasOption(OPT_SHORT_TUNNEL_V2)) {
                tunnelingV2 = true;
            }
            logger.debug("tunnelingV2={}", tunnelingV2);

            if (cmdLine.hasOption(OPT_SHORT_TUNNEL_V1)) {
                tunnelingV1 = true;
            }
            logger.debug("tunneling={}", tunnelingV1);

            if (cmdLine.hasOption(OPT_SHORT_ROUTING)) {
                routing = true;
            }
            logger.debug("routing={}", routing);

            if (cmdLine.hasOption(OPT_SHORT_NAT)) {
                nat = true;
            }
            logger.debug("nat={}", nat);

            if (cmdLine.hasOption(OPT_SHORT_FILENAME)) {
                fileName = cmdLine.getOptionValue(OPT_SHORT_FILENAME);
            }
            logger.debug("fileName={}", fileName);

            if (cmdLine.hasOption(OPT_SHORT_LOCALHOST)) {
                localhost = Utils.parseHost(cmdLine.getOptionValue(OPT_SHORT_LOCALHOST));
            }
            logger.debug("localhost={}", localhost);

            if (cmdLine.hasOption(OPT_SHORT_LOCALPORT)) {
                localport = ((Number)cmdLine.getParsedOptionValue(OPT_SHORT_LOCALPORT)).intValue();
            }
            logger.debug("localport={}", localport);

            if (cmdLine.hasOption(OPT_SHORT_PORT)) {
                port = ((Number)cmdLine.getParsedOptionValue(OPT_SHORT_PORT)).intValue();
            }
            logger.debug("port={}", port);

            if (cmdLine.hasOption(OPT_SHORT_MEDIUM)) {
                medium = cmdLine.getOptionValue(OPT_SHORT_MEDIUM);
            }
            logger.debug("medium={}", medium);

            if (cmdLine.hasOption(OPT_LONG_DELAY)) {
                delay = ((Number)cmdLine.getParsedOptionValue(OPT_LONG_DELAY)).intValue();
                if ((delay < Updater.DELAY_MIN) || (delay > Updater.DELAY_MAX)) {
                    logger.warn(ansi().fg(RED).a(
                            String.format("option --%s {} is invalid (min:{}, max:{}) => set to {}", OPT_LONG_DELAY)).reset().toString(),
                            delay, Updater.DELAY_MIN, Updater.DELAY_MAX, Updater.DELAY_DEFAULT);
                    delay = Updater.DELAY_DEFAULT;    // set to DELAY_DEFAULT in case of invalid waiting time
                }
            }
            logger.debug("delay={}", delay);

            if (cmdLine.hasOption(OPT_SHORT_BLOCKSIZE)) {
                int newBlockSize = ((Number)cmdLine.getParsedOptionValue(OPT_LONG_BLOCKSIZE)).intValue();
                if (VALID_BLOCKSIZES.contains(newBlockSize)) {
                    blockSize = newBlockSize;
                }
                else {
                    logger.info(ansi().fg(YELLOW).a("--{} {} is not supported => Set --{} to default {} bytes").reset().toString(), OPT_LONG_BLOCKSIZE, newBlockSize, OPT_LONG_BLOCKSIZE, blockSize);
                }
            }
            logger.debug("{}={}", OPT_LONG_BLOCKSIZE, delay);

            if (cmdLine.hasOption(OPT_SHORT_UID)) {
               uid =  uidToByteArray(cmdLine.getOptionValue(OPT_SHORT_UID));
            }
            logger.debug("uid={}", Utils.byteArrayToHex(uid));

            if (cmdLine.hasOption(OPT_SHORT_DEVICE)) {
                device = new IndividualAddress(cmdLine.getOptionValue(OPT_SHORT_DEVICE));
            } else {
                device = null;
            }
            logger.debug("device={}", device);

            if (cmdLine.hasOption(OPT_SHORT_PROG_DEVICE)) {
                progDevice = new IndividualAddress(cmdLine.getOptionValue(OPT_SHORT_PROG_DEVICE));
            }

            logger.debug("progDevice={}", progDevice);

            if (cmdLine.hasOption(OPT_SHORT_OWN_ADDRESS)) {
                ownAddress = new IndividualAddress(cmdLine.getOptionValue(OPT_SHORT_OWN_ADDRESS));
            }
            logger.debug("ownAddress={}", ownAddress);

            if (cmdLine.hasOption(OPT_SHORT_FT12)) {
               ft12 = cmdLine.getOptionValue(OPT_SHORT_FT12);
            }
            logger.debug("ft12={}", ft12);

            if (cmdLine.hasOption(OPT_SHORT_TPUART)) {
                tpuart = cmdLine.getOptionValue(OPT_SHORT_TPUART);
            }
            logger.debug("tpuart={}", tpuart);

            if (cmdLine.hasOption(OPT_LONG_USB)) {
                usbInterface = cmdLine.getOptionValue(OPT_LONG_USB);
            }
            logger.debug("usb={}", usbInterface);

            if (cmdLine.getArgs().length > 0) {
                knxInterface = Utils.parseHost(cmdLine.getArgs()[0]);
            }
            else {
                knxInterface = null;
            }
            logger.debug("knxInterface={}", knxInterface);

            if (cmdLine.hasOption(OPT_LONG_USER_ID)) {
                logger.debug("KNX IP Secure --{} is set", OPT_LONG_USER_ID); // log only that it´s, but not the actual value
                userId = ((Number)cmdLine.getParsedOptionValue(OPT_LONG_USER_ID)).intValue();
            }

            if (cmdLine.hasOption(OPT_LONG_USER_PASSWORD)) {
                logger.debug("KNX IP Secure --{} is set", OPT_LONG_USER_PASSWORD); // log only that it´s, but not the actual value
                userPassword = cmdLine.getOptionValue(OPT_LONG_USER_PASSWORD);
            }

            if (cmdLine.hasOption(OPT_LONG_DEVICE_PASSWORD)) {
                logger.debug("KNX IP Secure --{} is set", OPT_LONG_DEVICE_PASSWORD); // log only that it´s, but not the actual value
                devicePassword = cmdLine.getOptionValue(OPT_LONG_DEVICE_PASSWORD);
            }

            if (cmdLine.hasOption(OPT_LONG_LOGSTATISTIC)) {
                logStatistics = true;
            }
            logger.debug("logStatistics={}", logStatistics);

            // some logical checks for options which exclude each other
            // differential mode and eraseflash makes no sense
            if (eraseFullFlash() && (!full())) {
                full = true;
                logger.info(ansi().fg(RED).a("--{} is set. --> switching to full flash mode").reset().toString(), OPT_LONG_ERASEFLASH);
            }

            // nat only possible with tunneling v1
            if (nat() && (!tunnelingV1())) {
                throw new CliInvalidException(String.format(ansi().fg(RED).a("Option --%s can only be used together with --%s").reset().toString(),
                        OPT_LONG_NAT, OPT_LONG_TUNNEL_V1));
            }

            // nat not allowed with tunneling v2
            if (nat() && (tunnelingV2())) {
                throw new CliInvalidException(String.format(ansi().fg(RED).a("Option --%s can not be used together with --%s").reset().toString(),
                        OPT_LONG_NAT, OPT_LONG_TUNNEL_V2));
            }

            // check IP-secure configuration
            if (!(userPassword().isEmpty()) || !(devicePassword().isEmpty())) {
                if (knxInterface() == null) {
                    throw new CliInvalidException(ansi().fg(RED).a("No IP-Interface specified for IP-secure").reset().toString());
                }
                else if (!getUsbInterface().isEmpty()) {
                    throw new CliInvalidException(ansi().fg(RED).a(String.format("IP-secure is not possible with --%s", OPT_LONG_USB)).reset().toString());
                }
                else if (!ft12().isEmpty()) {
                    throw new CliInvalidException(ansi().fg(RED).a(String.format("IP-secure is not possible with --%s", OPT_LONG_FT12)).reset().toString());
                }
                else if (!tpuart().isEmpty()) {
                    throw new CliInvalidException(ansi().fg(RED).a(String.format("IP-secure is not possible with --%s", OPT_LONG_TPUART)).reset().toString());
                }
                else if (nat()) {
                    throw new CliInvalidException(ansi().fg(RED).a(String.format("IP-secure is not possible with --%s", OPT_LONG_NAT)).reset().toString());
                }
                else if (tunnelingV1()) {
                    throw new CliInvalidException(ansi().fg(RED).a(String.format("IP-secure is not possible with --%s", OPT_LONG_TUNNEL_V1)).reset().toString());
                }
                else if (tunnelingV2()) {
                    throw new CliInvalidException(ansi().fg(RED).a(String.format("IP-secure is not possible with --%s", OPT_LONG_TUNNEL_V2)).reset().toString());
                }

                // ensure that all three IP-Secure arguments are set
                if ((userPassword().isEmpty()) || (devicePassword().isEmpty())) {
                    throw new CliInvalidException(ansi().fg(RED).a(String.format("For IP-secure --%s, --%s and --%s must be set", OPT_LONG_USER_ID,
                            OPT_LONG_USER_PASSWORD, OPT_LONG_DEVICE_PASSWORD)).reset().toString());
                }
            }

            int interfacesSet = 0;
            if (knxInterface() != null) interfacesSet++;
            if (routing()) interfacesSet++;
            if (!ft12().isEmpty()) interfacesSet++;
            if (!tpuart().isEmpty()) interfacesSet++;
            if (!getUsbInterface().isEmpty()) interfacesSet++;

            if (interfacesSet > 1) {
                throw new CliInvalidException(ansi().fg(RED).a("Only one bus interface can be used.").reset().toString());
            }
            else if (interfacesSet == 0) {
                throw new CliInvalidException(ansi().fg(RED).a("No bus interface specified.").reset().toString());
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
        logger.error(ansi().fg(RED).a(e.getMessage()).reset().toString());
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

    ///\todo move to Utils.java
    private byte[] uidToByteArray(String str) {
        String[] tokens = str.split(":");
        if (tokens.length != UPDProtocol.UID_LENGTH_USED) {
            logger.warn("ignoring --uid {}, wrong size {}, expected {}", str, tokens.length, UPDProtocol.UID_LENGTH_USED);
            return null;
        }
        byte[] uid = new byte[tokens.length];
        for (int n = 0; n < tokens.length; n++) {
            uid[n] = (byte) Integer.parseUnsignedInt(tokens[n], 16);
        }
       return uid;
    }

    public InetAddress knxInterface() {
        return knxInterface;
    }

    public InetAddress host() {
        return knxInterface;
    }

    public String fileName() {
        return fileName;
    }

    public InetAddress localhost() {
        return localhost;
    }

    public int localport() {
        return localport;
    }

    public int port() {
        return port;
    }

    public boolean nat() {
        return nat;
    }

    public String ft12() {
        return ft12;
    }

    public String tpuart() {
        return tpuart;
    }

    public boolean tunnelingV2() { return tunnelingV2; }

    public boolean tunnelingV1() { return tunnelingV1; }

    public boolean routing() {
        return routing;
    }

    public String medium() {
        return medium;
    }

    public IndividualAddress progDevice() {
        return progDevice;
    }

    public IndividualAddress device() {
        return device;
    }

    public IndividualAddress ownAddress() {
        return ownAddress;
    }

    public byte[] uid() {
        return uid;
    }

    public boolean full() {
        return full;
    }

    public int delay() {
        return delay;
    }

    public boolean NO_FLASH() {
        return NO_FLASH;
    }

    public boolean eraseFullFlash() {
        return eraseFullFlash;
    }

    public long dumpFlashStartAddress() {
        return dumpFlashStartAddress;
    }

    public long dumpFlashEndAddress() {
        return dumpFlashEndAddress;
    }

    public boolean help() {
        return help;
    }

    public boolean version() {
        return version;
    }

    public Level logLevel() {
        return logLevel;
    }

    public int userId() {
        return userId;
    }

    public String userPassword() {
        return userPassword;
    }

    public String devicePassword() {
        return devicePassword;
    }

    public Priority priority() {
        return priority;
    }

    public boolean logStatistics() {
        return logStatistics;
    }

    public int getBlockSize() {
        return blockSize;
    }

    public String getUsbInterface() {
        return usbInterface;
    }
}