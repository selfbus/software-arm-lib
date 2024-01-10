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
import tuwien.auto.calimero.Priority;
import tuwien.auto.calimero.knxnetip.KNXnetIPConnection;
import tuwien.auto.calimero.link.medium.TPSettings;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.InetAddress;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;

/**
 * Parses command line interface (cli) options for the application
 * <p>
 * Adding a new cli option name:
 *         - declare a short and a long option
 *           private static final String OPT_SHORT_XXX = "X";
 *           private static final String OPT_LONG_XXX = "XXXXX";
 *
 *         - in constructor @ref CliOptions(.) create an instance of class Option
 *           and add it with cliOptions.addOption(yourOptionInstance)
 *
 *         - check with cmdLine.hasOption(OPT_SHORT_XXX) in method parse(.) is it set or not
 */
public class CliOptions {
    private static final Logger logger = LoggerFactory.getLogger(CliOptions.class.getName());

    private static final String OPT_SHORT_FILENAME = "f";
    private static final String OPT_LONG_FILENAME = "fileName";

    private static final String OPT_SHORT_LOCALHOST = "H";
    private static final String OPT_LONG_LOCALHOST = "localhost";
    private static final String OPT_SHORT_LOCALPORT = "P";
    private static final String OPT_LONG_LOCALPORT = "localport";
    private static final String OPT_SHORT_PORT = "p";
    private static final String OPT_LONG_PORT = "port";

    private static final String OPT_SHORT_FT12 = "s";
    private static final String OPT_LONG_FT12 = "serial";
    private static final String OPT_SHORT_TPUART = "t";
    private static final String OPT_LONG_TPUART = "tpuart";
    private static final String OPT_SHORT_MEDIUM = "m";
    private static final String OPT_LONG_MEDIUM = "medium";

    private static final String OPT_LONG_USER_ID = "user";
    private static final String OPT_LONG_USER_PASSWORD = "user-pwd";
    private static final String OPT_LONG_DEVICE_PASSWORD = "device-pwd";

    private static final String OPT_SHORT_PROG_DEVICE = "D";
    private static final String OPT_LONG_PROG_DEVICE = "progDevice";
    private static final String OPT_SHORT_DEVICE = "d";
    private static final String OPT_LONG_DEVICE = "device";
    private static final String OPT_SHORT_OWN_ADDRESS = "o";
    private static final String OPT_LONG_OWN_ADDRESS = "own";

    private static final String OPT_SHORT_APP_VERSION_PTR = "a";
    public static final String OPT_LONG_APP_VERSION_PTR = "appVersionPtr";
    private static final String OPT_SHORT_UID = "u";
    private static final String OPT_LONG_UID = "uid";

    private static final String OPT_LONG_DELAY = "delay";

    private static final String OPT_SHORT_NAT = "n";
    private static final String OPT_LONG_NAT = "nat";
    private static final String OPT_SHORT_ROUTING = "r";
    private static final String OPT_LONG_ROUTING = "routing";

    private static final String OPT_SHORT_FULL = "f1";
    private static final String OPT_LONG_FULL = "full";

    private static final String OPT_SHORT_HELP = "h";
    private static final String OPT_LONG_HELP = "help";

    private static final String OPT_SHORT_VERSION = "v";
    private static final String OPT_LONG_VERSION = "version";

    private static final String OPT_SHORT_NO_FLASH = "f0";
    public static final String OPT_LONG_NO_FLASH = "NO_FLASH";

    private static final String OPT_SHORT_LOGLEVEL = "l";
    private static final String OPT_LONG_LOGLEVEL = "logLevel";

    private static final String OPT_LONG_PRIORITY = "priority";

    private static final String OPT_LONG_ERASEFLASH = "ERASEFLASH";

    private static final String OPT_LONG_DUMPFLASH = "DUMPFLASH";

    private static final String OPT_LONG_LOGSTATISTIC = "statistic";

    private static final int PRINT_WIDTH = 100;
    private final static List<String> VALID_LOG_LEVELS = Arrays.asList("TRACE", "DEBUG", "INFO");
    private final static List<String> VALID_PRIORITIES = Arrays.asList("SYSTEM", "HIGH", "ALARM", "LOW");


    private final Options cliOptions = new Options();
    // define parser
    CommandLine cmdLine;
    HelpFormatter helper = new HelpFormatter();

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
    private boolean routing = false;
    private String medium = "tp1";

    private int userId = 1;
    private String userPassword = "";
    private String devicePassword = "";


    private IndividualAddress progDevice;
    private IndividualAddress ownAddress;
    private IndividualAddress device = null;
    private int appVersionPtr = 0;
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

    private boolean help = false;
    private boolean version = false;


    public CliOptions(final String[] args, String helpApplicationName, String helpHeader, String helpFooter,
                      IndividualAddress progDevice, IndividualAddress ownAddress)
            throws ParseException, KNXFormatException {
        this.helpApplicationName = helpApplicationName;
        this.helpHeader = helpHeader;
        this.helpFooter = helpFooter;
        this.progDevice = progDevice;
        this.ownAddress = ownAddress;

        Option nat = new Option(OPT_SHORT_NAT, OPT_LONG_NAT, false, "enable Network Address Translation (NAT)");
        Option routing = new Option(OPT_SHORT_ROUTING, OPT_LONG_ROUTING, false, "use KNXnet/IP routing (not implemented)");
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
                .hasArg()
                .required(false) ///\todo .required(true) leads to an exception with --help or --version
                .type(String.class)
                .desc("Filename of hex file to program").build();
        Option localhost = Option.builder(OPT_SHORT_LOCALHOST).longOpt(OPT_LONG_LOCALHOST)
                .argName("localhost")
                .hasArg()
                .required(false)
                .type(String.class)
                .desc("local IP/host name").build();
        Option localport = Option.builder(OPT_SHORT_LOCALPORT).longOpt(OPT_LONG_LOCALPORT)
                .argName("localport")
                .hasArg()
                .required(false)
                .type(Number.class)
                .desc("local UDP port (default system assigned)").build();
        Option port = Option.builder(OPT_SHORT_PORT).longOpt(OPT_LONG_PORT)
                .argName("port")
                .hasArg()
                .required(false)
                .type(Number.class)
                .desc(String.format("UDP port on <KNX Interface> (default %d)", KNXnetIPConnection.DEFAULT_PORT)).build();
        Option ft12 = Option.builder(OPT_SHORT_FT12).longOpt(OPT_LONG_FT12)
                .argName("COM-port")
                .hasArg()
                .required(false)
                .desc("use FT1.2 serial communication").build();
        Option tpuart = Option.builder(OPT_SHORT_TPUART).longOpt(OPT_LONG_TPUART)
                .argName("COM-port")
                .hasArg()
                .required(false)
                .desc("use TPUART serial communication (experimental, needs serialcom or rxtx library in java.library.path)").build();
        Option medium = Option.builder(OPT_SHORT_MEDIUM).longOpt(OPT_LONG_MEDIUM)
                .argName("tp1|rf")
                .hasArg()
                .required(false)
                .type(TPSettings.class)
                .desc(String.format("KNX medium [tp1|rf] (default %s)", this.medium)).build(); ///\todo not all implemented missing [tp0|p110|p132]
        Option optProgDevice = Option.builder(OPT_SHORT_PROG_DEVICE).longOpt(OPT_LONG_PROG_DEVICE)
                .argName("x.x.x")
                .hasArg()
                .required(false)
                .type(IndividualAddress.class)
                .desc(String.format("KNX device address in bootloader mode (default %s)", this.progDevice.toString())).build();
        Option device = Option.builder(OPT_SHORT_DEVICE).longOpt(OPT_LONG_DEVICE)
                .argName("x.x.x")
                .hasArg()
                .required(false)
                .type(IndividualAddress.class)
                .desc("KNX device address in normal operating mode (default none)").build();
        Option ownPhysicalAddress = Option.builder(OPT_SHORT_OWN_ADDRESS).longOpt(OPT_LONG_OWN_ADDRESS)
                .argName("x.x.x")
                .hasArg()
                .required(false)
                .type(IndividualAddress.class)
                .desc(String.format("own physical KNX address (default %s)", this.ownAddress.toString())).build();
        Option appVersionPtr = Option.builder(OPT_SHORT_APP_VERSION_PTR).longOpt(OPT_LONG_APP_VERSION_PTR)
                .argName("address")
                .hasArg()
                .required(false)
                .type(String.class)
                .desc("pointer address to APP_VERSION string in new firmware file").build();
        Option uid = Option.builder(OPT_SHORT_UID).longOpt(OPT_LONG_UID)
                .argName("uid")
                .hasArg()
                .required(false)
                .desc(String.format("send UID to unlock (default: request UID to unlock). Only the first %d bytes of UID are used", UPDProtocol.UID_LENGTH_USED)).build();
        Option delay = Option.builder(null).longOpt(OPT_LONG_DELAY)
                .argName("ms")
                .hasArg()
                .required(false)
                .type(Number.class)
                .desc(String.format("delay telegrams during data transmission to reduce bus load, valid 0-500ms, default %d", Updater.DELAY_MIN)).build();
        Option logLevel = Option.builder(OPT_SHORT_LOGLEVEL).longOpt(OPT_LONG_LOGLEVEL)
                .argName("TRACE|DEBUG|INFO")
                .hasArg()
                .required(false)
                .type(String.class)
                .desc(String.format("Logfile logging level [TRACE|DEBUG|INFO] (default %s)", this.logLevel.toString())).build();

        Option userId = Option.builder(null).longOpt(OPT_LONG_USER_ID)
                .argName("id")
                .hasArg()
                .required(false)
                .type(Number.class)
                .desc(String.format("KNX IP Secure tunneling user identifier (1..127) (default %d)", this.userId)).build();
        Option userPasswd = Option.builder(null).longOpt(OPT_LONG_USER_PASSWORD)
                .argName("password")
                .hasArg()
                .required(false)
                .type(Number.class)
                .desc("KNX IP Secure tunneling user password (Commissioning password/Inbetriebnahmepasswort), quotation marks (\") in password may not work").build();
        Option devicePasswd = Option.builder(null).longOpt(OPT_LONG_DEVICE_PASSWORD)
                .argName("password")
                .hasArg()
                .required(false)
                .type(Number.class)
                .desc("KNX IP Secure device authentication code (Authentication Code/Authentifizierungscode) quotation marks(\") in password may not work").build();

        Option knxPriority = Option.builder(null).longOpt(OPT_LONG_PRIORITY)
                .argName("SYSTEM|ALARM|HIGH|LOW")
                .hasArg()
                .required(false)
                .type(String.class)
                .desc(String.format("KNX telegram priority (default %s)", this.priority.toString().toUpperCase())).build();

        Option logStatistic = new Option(null, OPT_LONG_LOGSTATISTIC, false, "show more statistic data");

        // options will be shown in order as they are added to cliOptions
        cliOptions.addOption(fileName);

        // ft12 or medium, not both
        OptionGroup grpBusAccess = new OptionGroup();
        grpBusAccess.addOption(medium);
        grpBusAccess.addOption(ft12);
        grpBusAccess.addOption(tpuart);

        cliOptions.addOptionGroup(grpBusAccess);

        cliOptions.addOption(device);
        cliOptions.addOption(optProgDevice);
        cliOptions.addOption(ownPhysicalAddress);
        cliOptions.addOption(knxPriority);

        cliOptions.addOption(userId);
        cliOptions.addOption(userPasswd);
        cliOptions.addOption(devicePasswd);

        cliOptions.addOption(uid);
        cliOptions.addOption(full);
        cliOptions.addOption(localhost);
        cliOptions.addOption(localport);
        cliOptions.addOption(port);
        cliOptions.addOption(nat);
        cliOptions.addOption(routing);
        cliOptions.addOption(appVersionPtr);

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
            int i = 0;
            String cliCensored = "";
            while (i < args.length) {
                String testCli = args[i].toLowerCase(Locale.ROOT);
                if ((testCli.contains(OPT_LONG_USER_ID.toLowerCase(Locale.ROOT))) ||
                    (testCli.contains(OPT_LONG_USER_PASSWORD.toLowerCase(Locale.ROOT))) ||
                    (testCli.contains(OPT_LONG_DEVICE_PASSWORD.toLowerCase(Locale.ROOT)))) {
                    cliCensored += " " + args[i] + " (censored)";
                    i++;
                }
                else {
                    cliCensored += " " + args[i];
                }
                i++;
            }
            logger.debug("cli: {}", cliCensored.trim());

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
                    logger.warn("{}invalid {} {}, using {}{}", ConColors.RED, OPT_LONG_LOGLEVEL, cliLogLevel, root.getLevel().toString(), ConColors.RESET);
                }
            }
            logger.debug("logLevel={}", root.getLevel().toString());

            if (cmdLine.hasOption(OPT_LONG_PRIORITY)) {
                String cliPriority = cmdLine.getOptionValue(OPT_LONG_PRIORITY).toUpperCase();
                if (VALID_PRIORITIES.contains(cliPriority)) {
                    priority = Priority.valueOf(cliPriority);
                }
                else {
                    logger.warn("{}invalid {} {}, using {}{}", ConColors.RED, OPT_LONG_LOGLEVEL, cliPriority, priority, ConColors.RESET);
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

            if (fileName.length() <= 0) {
                throw new ParseException("No fileName specified.");
            }

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
                    logger.warn("{}option --delay {} is invalid (min:{}, max:{}) => setting --delay {}{}", ConColors.RED, delay, Updater.DELAY_MIN, Updater.DELAY_MAX, Updater.DELAY_DEFAULT, ConColors.RESET);
                    delay = Updater.DELAY_DEFAULT;    // set to DELAY_DEFAULT in case of invalid waiting time
                }
            }
            logger.debug("delay={}", delay);

            if (cmdLine.hasOption(OPT_SHORT_UID)) {
               uid =  uidToByteArray(cmdLine.getOptionValue(OPT_SHORT_UID));
            }
            logger.debug("uid={}", Utils.byteArrayToHex(uid));

            if (cmdLine.hasOption(OPT_SHORT_APP_VERSION_PTR)) {
                appVersionPtr = Integer.decode(cmdLine.getOptionValue(OPT_SHORT_APP_VERSION_PTR));

            }
            logger.debug("appVersionPtr={}", appVersionPtr);

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

            if ((ft12.length() == 0) && (tpuart.length() == 0)) {
                // no ft12 or tpuart => get the <KNX Interface>
                if (cmdLine.getArgs().length <= 0) {
                    throw new ParseException("No <KNX Interface>, ft12 or tpuart specified.");
                } else {
                    knxInterface = Utils.parseHost(cmdLine.getArgs()[0]);
                }
            }
            logger.debug("knxInterface={}", knxInterface);

            if (cmdLine.hasOption(OPT_LONG_USER_ID)) {
                logger.debug("KNX IP Secure userId is set"); // don't log knx secure ip specific options
                userId = ((Number)cmdLine.getParsedOptionValue(OPT_LONG_USER_ID)).intValue();
            }

            if (cmdLine.hasOption(OPT_LONG_USER_PASSWORD)) {
                logger.debug("KNX IP Secure userPassword is set"); // don't log knx secure ip specific options
                userPassword = cmdLine.getOptionValue(OPT_LONG_USER_PASSWORD);
            }

            if (cmdLine.hasOption(OPT_LONG_DEVICE_PASSWORD)) {
                logger.debug("KNX IP Secure devicePassword is set"); // don't log knx secure ip specific options
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
                logger.info("{}--{} is set. --> switching to full flash mode{}", ConColors.RED, OPT_LONG_ERASEFLASH, ConColors.RESET);
            }


        } catch (ParseException | KNXFormatException e) {
            StringBuilder cliParsed = new StringBuilder();
            for (String arg : args) {
                cliParsed.append(String.format("%s ", arg));
            }
            logger.error("Invalid command line parameters:");
            logger.error("{}", cliParsed);
            logger.error("{}{}{}", ConColors.RED, e.getMessage(), ConColors.RESET);
            logger.error("For more information about the usage start with --{}", OPT_LONG_HELP);
            logger.error("", e);
            System.exit(0);
        }
    }

    public String helpToString() {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        helper.setWidth(PRINT_WIDTH);
        helper.setOptionComparator(null);
        helper.printHelp(pw, helper.getWidth(), helpApplicationName + " <KNX Interface>",
                "\n" + helpHeader + ":\n", cliOptions, helper.getLeftPadding(),
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

    public int appVersionPtr() {
        return appVersionPtr;
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

    public boolean logStatistics() {return logStatistics;};
}