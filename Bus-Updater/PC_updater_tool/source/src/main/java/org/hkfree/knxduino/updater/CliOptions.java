package org.hkfree.knxduino.updater;

import ch.qos.logback.classic.Level;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.OptionGroup;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import tuwien.auto.calimero.IndividualAddress;
import tuwien.auto.calimero.KNXFormatException;
import tuwien.auto.calimero.knxnetip.KNXnetIPConnection;
import tuwien.auto.calimero.link.medium.TPSettings;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.InetAddress;
import java.util.Arrays;
import java.util.List;

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
    private static final String OPT_SHORT_PROG_DEVICE = "D";
    private static final String OPT_LONG_PROG_DEVICE = "progDevice";
    private static final String OPT_SHORT_DEVICE = "d";
    private static final String OPT_LONG_DEVICE = "device";
    private static final String OPT_SHORT_APP_VERSION_PTR = "a";
    private static final String OPT_LONG_APP_VERSION_PTR = "appVersionPtr";
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

    private static final String OPT_LONG_VERSION = "version";

    private static final String OPT_SHORT_VERBOSE = "v";
    private static final String OPT_LONG_VERBOSE = "verbose";

    private static final String OPT_SHORT_NO_FLASH = "f0";
    private static final String OPT_LONG_NO_FLASH = "NO_FLASH";

    private static final String OPT_SHORT_LOGLEVEL = "l";
    private static final String OPT_LONG_LOGLEVEL = "logLevel";

    private static final int PRINT_WIDTH = 100;
    private final static List VALID_LOG_LEVELS = Arrays.asList("TRACE", "DEBUG", "INFO");

    private final Options cliOptions = new Options();
    // define parser
    CommandLine cmdLine;
    HelpFormatter helper = new HelpFormatter();

    private final String helpHeader;
    private final String helpFooter;
    private final String helpApplicationName;

    private InetAddress knxInterface;
    private String fileName = "";
    private boolean verbose = false;
    private InetAddress localhost;
    private int localport = 0;
    private int port = KNXnetIPConnection.DEFAULT_PORT;
    private boolean nat = false;
    private String ft12 = "";
    private String tpuart = "";
    private boolean routing = false;
    private String medium = "tp1";
    private IndividualAddress progDevice;
    private IndividualAddress device = new IndividualAddress("0.0.0");
    private int appVersionPtr = 0;
    private byte[] uid;
    private boolean full = false;
    private int delay = 0;
    private boolean NO_FLASH = false;
    private Level logLevel = Level.DEBUG;

    private boolean help = false;
    private boolean version = false;


    public CliOptions(final String[] args, String helpApplicationName, String helpHeader, String helpFooter) throws ParseException, KNXFormatException {
        this.helpApplicationName = helpApplicationName;
        this.helpHeader = helpHeader;
        this.helpFooter = helpFooter;

        this.progDevice = new IndividualAddress(Updater.PHYS_ADDRESS_BOOTLOADER_AREA,
                                                Updater.PHYS_ADDRESS_BOOTLOADER_LINE,
                                                Updater.PHYS_ADDRESS_BOOTLOADER_DEVICE);

        Option nat = new Option(OPT_SHORT_NAT, OPT_LONG_NAT, false, "enable Network Address Translation (NAT)");
        Option routing = new Option(OPT_SHORT_ROUTING, OPT_LONG_ROUTING, false, "use KNXnet/IP routing (not implemented)");
        Option full = new Option(OPT_SHORT_FULL, OPT_LONG_FULL, false, "force full upload mode (disables differential mode)");
        Option help = new Option(OPT_SHORT_HELP, OPT_LONG_HELP, false, "show this help message");
        Option version = new Option(null, OPT_LONG_VERSION, false, "show tool/library version");
        Option verbose = new Option(OPT_SHORT_VERBOSE, OPT_LONG_VERBOSE, false, "enable verbose status output (not used/implemented)");
        Option NO_FLASH = new Option(OPT_SHORT_NO_FLASH, OPT_LONG_NO_FLASH, false, "for debugging use only, disable flashing firmware!");

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
                .desc("use TPUART serial communication (experimental, needs serialcom or rxtx library in java.library.path").build();
        Option medium = Option.builder(OPT_SHORT_MEDIUM).longOpt(OPT_LONG_MEDIUM)
                .argName("tp1|rf")
                .hasArg()
                .required(false)
                .type(TPSettings.class)
                .desc(String.format("KNX medium [tp1|rf] (default %s)", this.medium)).build(); ///\todo not all implemented missing [tp0|p110|p132]
        Option progDevice = Option.builder(OPT_SHORT_PROG_DEVICE).longOpt(OPT_LONG_PROG_DEVICE)
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
                .desc(String.format("send UID to unlock (default: request UID to unlock). Only the first %d bytes of UID are used", Updater.UID_LENGTH_USED)).build();
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

        // options will be shown in order as they are added to cliOptions
        cliOptions.addOption(fileName);

        // ft12 or medium, not both
        OptionGroup grpBusAccess = new OptionGroup();
        grpBusAccess.addOption(medium);
        grpBusAccess.addOption(ft12);
        grpBusAccess.addOption(tpuart);

        cliOptions.addOptionGroup(grpBusAccess);

        cliOptions.addOption(device);
        cliOptions.addOption(progDevice);

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
        cliOptions.addOption(verbose);
        cliOptions.addOption(NO_FLASH);


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
                    logger.warn("{}invalid {} {}, using {}{}", ConColors.RED, OPT_LONG_LOGLEVEL, cliLogLevel, root.getLevel().toString(), ConColors.RESET);
                }
            }
            logger.debug("logLevel={}", root.getLevel().toString());

            if (cmdLine.hasOption(OPT_SHORT_HELP)) {
                help = true;
                logger.debug("help={}", help);
                return;
            }

            if (cmdLine.hasOption(OPT_LONG_VERSION)) {
                version = true;
                logger.debug("version={}", version);
                return;
            }

            if (cmdLine.hasOption(OPT_SHORT_VERBOSE)) {
                verbose = true;
            }
            logger.debug("verbose={}", verbose);

            if (cmdLine.hasOption(OPT_SHORT_NO_FLASH)) {
                NO_FLASH = true;
                logger.warn("--NO_FLASH => only boot description block will be written");
            }
            logger.debug("NO_FLASH={}", NO_FLASH);

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
                    logger.warn("option --delay {} is invalid (min:{}, max:{}) => setting --delay {}", delay, Updater.DELAY_MIN, Updater.DELAY_MAX, Updater.DELAY_DEFAULT);
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
            } else {
                progDevice = new IndividualAddress(Updater.PHYS_ADDRESS_BOOTLOADER_AREA,
                        Updater.PHYS_ADDRESS_BOOTLOADER_LINE,
                        Updater.PHYS_ADDRESS_BOOTLOADER_DEVICE);
            }
            logger.debug("progDevice={}", progDevice);

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

        } catch (ParseException | KNXFormatException e) {
            String cliParsed = "";
            for (String arg : args) {
                cliParsed += String.format("%s ", arg);
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
        if (tokens.length != Updater.UID_LENGTH_USED) {
            logger.warn("ignoring --uid {}, wrong size {}, expected {}", str, tokens.length, Updater.UID_LENGTH_USED);
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

    public boolean verbose() {
        return verbose;
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

    public boolean help() {
        return help;
    }

    public boolean version() {
        return version;
    }

    public Level logLevel() {
        return logLevel;
    }
/* old options variant ///\todo remove before release
    public CliOptions(final String[] args) {
        Option help = new Option("h", "help", false, "show this help message");
        Option fileName = Option.builder("filename").longOpt(null)
                .argName("fileName")
                .hasArg()
                .required(true)
                .desc("Filename of hex file to program").build();
        Option version = new Option("version", null, false, "show tool/library version");
        Option verbose = new Option("v", "verbose", false, "enable verbose status output");

        Option localhost = Option.builder("localhost").longOpt(null)
                .argName("localhost")
                .hasArg()
                .required(false)
                .desc("local IP/host name").build();
        Option localport = Option.builder("localport").longOpt(null)
                .argName("localport")
                .hasArg()
                .required(false)
                .desc("local UDP port (default system assigned)").build();
        Option port = Option.builder("p").longOpt("port")
                .argName("port")
                .hasArg()
                .required(false)
                .desc("UDP port on <host> (default 3671)").build();
        Option nat = new Option("n", "nat", false, "enable Network Address Translation");
        Option ft12 = Option.builder("s").longOpt("serial")
                .argName("serial")
                .hasArg()
                .required(false)
                .desc("use FT1.2 serial communication").build();
        Option routing = new Option("routing", null, false, "use KNXnet/IP routing");
        Option medium = Option.builder("m").longOpt("medium")
                .argName("medium")
                .hasArg()
                .required(false)
                .desc("KNX medium [tp0|tp1|p110|p132|rf] (default tp1)").build();
        Option progDevice = Option.builder("progDevice").longOpt(null)
                .argName("progDevice")
                .hasArg()
                .required(false)
                .desc(String.format("KNX device address of bootloader (default %s.%s.%s)", PHYS_ADDRESS_BOOTLOADER_AREA, PHYS_ADDRESS_BOOTLOADER_LINE, PHYS_ADDRESS_BOOTLOADER_DEVICE)).build();
        Option device = Option.builder("device").longOpt(null)
                .argName("device")
                .hasArg()
                .required(false)
                .desc("KNX device address in normal operating mode (default none)").build();
        Option appVersionPtr = Option.builder("appVersionPtr").longOpt(null)
                .argName("appVersionPtr")
                .hasArg()
                .required(false)
                .desc("pointer to APP_VERSION string in new firmware file").build();

        Option uid = Option.builder("uid").longOpt(null)
                .argName("uid")
                .hasArg()
                .required(false)
                .desc("send UID to unlock (default: request UID to unlock)\nonly the first 12 bytes of UID are used").build();
        Option full = new Option("full", null, false, "force full upload mode (disables differential mode)");
        Option delay = Option.builder("delay").longOpt(null)
                .argName("delay")
                .hasArg()
                .required(false)
                .desc("delay telegrams during data transmission to reduce bus load (valid 0-500)").build();
        Option NO_FLASH = new Option("NO_FLASH", null, false, "for debugging use only, disable flashing firmware!");
        cliOptions.addOption(help);
        cliOptions.addOption(fileName);
        cliOptions.addOption(version);
        cliOptions.addOption(verbose);
        cliOptions.addOption(localhost);
        cliOptions.addOption(localport);
        cliOptions.addOption(port);
        cliOptions.addOption(nat);
        cliOptions.addOption(ft12);
        cliOptions.addOption(routing);
        cliOptions.addOption(medium);
        cliOptions.addOption(progDevice);
        cliOptions.addOption(device);
        cliOptions.addOption(appVersionPtr);
        cliOptions.addOption(uid);
        cliOptions.addOption(full);
        cliOptions.addOption(delay);
        cliOptions.addOption(NO_FLASH);
    }
*/
}