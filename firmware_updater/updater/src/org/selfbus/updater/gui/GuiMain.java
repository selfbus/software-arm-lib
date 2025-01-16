package org.selfbus.updater.gui;

import com.intellij.uiDesigner.core.GridConstraints;
import com.intellij.uiDesigner.core.GridLayoutManager;
import com.intellij.uiDesigner.core.Spacer;
import org.apache.commons.cli.ParseException;
import org.selfbus.updater.*;
import tuwien.auto.calimero.*;
import tuwien.auto.calimero.knxnetip.Discoverer;
import tuwien.auto.calimero.knxnetip.servicetype.SearchResponse;
import tuwien.auto.calimero.link.medium.KNXMediumSettings;
import tuwien.auto.calimero.serial.usb.Device;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.plaf.FontUIResource;
import javax.swing.text.StyleContext;
import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.net.InetSocketAddress;
import java.util.*;
import java.util.List;

import static java.awt.Font.PLAIN;
import static org.fusesource.jansi.Ansi.Color.*;
import static org.fusesource.jansi.Ansi.ansi;
import static org.selfbus.updater.CliOptions.*;
import static org.selfbus.updater.gui.CliConverter.argument;
import static org.selfbus.updater.gui.ConColorsToStyledDoc.DefaultBackgroundColor;
import static org.selfbus.updater.gui.ConColorsToStyledDoc.DefaultForegroundColor;

public class GuiMain extends JFrame {
    private JButton buttonLoadFile;
    private JTextField textBoxKnxGatewayIpAddr;
    private JPanel panelMain;
    private JComboBox<CalimeroSearchComboItem> comboBoxIpGateways;
    private JLabel labelIpGateway;
    private JCheckBox CheckBoxDiffFlash;
    private JTextField textFieldUid;
    private JLabel labelUid;
    private JButton buttonStartStopFlash;
    private JTextPane jLoggingPane;
    private JTextField textFieldFileName;
    private JLabel labelFileName;
    private JComboBox<ComboItem> comboBoxScenario;
    private JComboBox<String> comboBoxMedium;
    private JTextField textFieldSerial;
    private JTextField textFieldTpuart;
    private JTextField textFieldDeviceAddress;
    private JTextField textFieldBootloaderDeviceAddress;
    private JTextField textFieldPort;
    private JCheckBox natCheckBox;
    private JTextField textFieldOwnAddress;
    private JTextField textFieldDelay;
    private JCheckBox eraseCompleteFlashCheckBox;
    private JCheckBox noFlashCheckBox;
    private JLabel labelMedium;
    private JLabel labelSerial;
    private JLabel labelTpuart;
    private JLabel labelDeviceAddress;
    private JLabel labelBootloaderDeviceAddr;
    private JLabel labelOwnAddress;
    private JLabel labelDelay;
    private JLabel labelPortHint;
    private JLabel labeIIpHint;
    private JLabel labelFileNameHint;
    private JLabel labelUidHint;
    private JLabel labelFullFlashHint;
    private JLabel labelNoFlashHint;
    private JLabel labelEraseCompleteFlash;
    private JLabel labelScenarioHint;
    private JButton buttonRequestUid;
    private JCheckBox advancedSettingsCheckBox;
    private JComboBox<String> comboBoxKnxTelegramPriority;
    private JLabel labelTelegramPriority;
    private JTextField textFieldKnxSecureUser;
    private JLabel labelKnxSecureUser;
    private JLabel labelKnxSecureUserHint;
    private JTextField textFieldKnxSecureUserPwd;
    private JLabel labelKnxSecureUserPwd;
    private JLabel labelKnxSecureUserPwdHint;
    private JTextField textFieldKnxSecureDevicePwd;
    private JLabel labelKnxSecureDevicePwd;
    private JLabel labelKnxSecureDevicePwdHint;
    private JScrollPane mainScrollPane;
    private JButton reloadGatewaysButton;

    private Thread updaterThread;
    private final static Logger logger = LoggerFactory.getLogger(GuiMain.class);
    private final GuiSettings guiSettings = new GuiSettings(this);
    private static final String FILENAME_SETTINGS = "settings.json";
    private static final String LANGUAGE_RESOURCE_BUNDLE = "language/GuiMain";

    String getTranslation(String text) {
        return $$$getMessageFromBundle$$$(LANGUAGE_RESOURCE_BUNDLE, text);
    }

    private boolean getUpdaterIsRunning() {
        if (updaterThread == null) {
            return false;
        }
        return updaterThread.getState() != Thread.State.TERMINATED;
    }

    private void handleStartStopFlashAction() {
        if (getUpdaterIsRunning() && (updaterThread != null)) {
            while (updaterThread.getState() != Thread.State.TERMINATED) {
                updaterThread.interrupt();
            }
            updaterThread = null;
            updaterFinished();
            return;
        }

        jLoggingPane.setText("");
        updaterThread = new Thread(() -> {
            try {
                Thread.currentThread().setName("guiUpdater");
                CliOptions cliOptions = getCliOptions();
                displayCommandLine(cliOptions);
                final Updater updater = new Updater(cliOptions);
                updater.run();
            } catch (KNXFormatException | ParseException e) {
                logger.error("", e); // todo see logback issue https://github.com/qos-ch/logback/issues/876
                String message = String.format(getTranslation("Exception.handleStartStopFlashAction.Message"),
                        e.getMessage());
                JOptionPane.showMessageDialog(this,
                        message, getTranslation("Error"), JOptionPane.ERROR_MESSAGE);
            } finally {
                SwingUtilities.invokeLater(this::updaterFinished);
            }

        });
        updaterThread.start();
        buttonStartStopFlash.setText(getTranslation("stopFlash"));
    }

    private void updaterFinished() {
        buttonStartStopFlash.setText(getTranslation("startFlash"));
    }

    private void handleLoadFileAction() {
        JFileChooser fc = new JFileChooser();
        String fileName = textFieldFileName.getText();

        File initialDirectory = null;
        if (fileName != null && !fileName.isEmpty()) {
            // try to set to the directory of the last .hex firmware
            initialDirectory = new File(fileName).getParentFile();
            if (!initialDirectory.exists() || !initialDirectory.isDirectory()) {
                initialDirectory = null;
            }
        }

        if (initialDirectory == null) {
            // set to current working directory
            String currentDirectory = System.getProperty("user.dir");
            initialDirectory = new File(currentDirectory);
        }

        fc.setCurrentDirectory(initialDirectory);

        fc.setFileFilter(new FileNameExtensionFilter(getTranslation("loadFile.firmwareFilterDescription"), "hex"));
        int result = fc.showOpenDialog(panelMain);

        if (result == JFileChooser.APPROVE_OPTION) {
            if (fc.getSelectedFile().exists()) {
                textFieldFileName.setText(fc.getSelectedFile().toString());
            }
        }
    }

    private void handleRequestUidAction() {
        jLoggingPane.setText("");
        updaterThread = new Thread(() -> {
            Thread.currentThread().setName("uidRequest");
            String uid = "";
            try {
                CliOptions cliOptions = getCliOptions();
                cliOptions.setFileName("");
                cliOptions.setUid("");
                displayCommandLine(cliOptions);
                final Updater upd = new Updater(cliOptions);
                SwingUtilities.invokeLater(this::beforeUidRequest);
                uid = upd.requestUid();
            } catch (ParseException | UpdaterException | KNXException e) {
                logger.error("", e); // todo see logback issue https://github.com/qos-ch/logback/issues/876
                JOptionPane.showMessageDialog(this,
                        String.format(getTranslation("Exception.requestUidAction.Message"), e),
                        getTranslation("Error"), JOptionPane.ERROR_MESSAGE);
            } finally {
                String finalUid = uid;
                SwingUtilities.invokeLater(() -> onRequestUidFinished(finalUid));
            }
        });
        updaterThread.start();
    }

    private void setUIDComponentsEnabled(boolean enabled) {
        comboBoxScenario.setEnabled(enabled);
        buttonRequestUid.setEnabled(enabled);
        buttonStartStopFlash.setEnabled(enabled);
        textFieldUid.setEnabled(enabled);
    }

    private void beforeUidRequest() {
        textFieldUid.setText("");
        setUIDComponentsEnabled(false);
    }

    private void onRequestUidFinished(String uid) {
        textFieldUid.setText(uid);
        setUIDComponentsEnabled(true);
    }

    private void handleReloadGatewaysAction() {
        new Thread(this::loadKnxIpInterfacesAndFillComboBox).start();
    }

    public GuiMain() {
        //todo load language from FILENAME_SETTINGS and set it here.
        // Has to be done before calling $$$setupUI$$$();
        //Locale.setDefault(Locale.ENGLISH); // for tests
        //Locale.setDefault(Locale.ROOT);    // default language for tests
        $$$setupUI$$$();

        // Set uncaught exception handler to show a dialog
        Thread.setDefaultUncaughtExceptionHandler(new GuiUncaughtExceptionHandler(this));

        buttonLoadFile.addActionListener(actionEvent -> handleLoadFileAction());
        buttonStartStopFlash.addActionListener(actionEvent -> handleStartStopFlashAction());
        buttonRequestUid.addActionListener(actionEvent -> handleRequestUidAction());
        comboBoxScenario.addActionListener(actionEvent -> setGuiElementsVisibility());
        advancedSettingsCheckBox.addActionListener(actionEvent -> setGuiElementsVisibility());
        reloadGatewaysButton.addActionListener(actionEvent -> handleReloadGatewaysAction());
        comboBoxIpGateways.addActionListener(comboBoxIpGatewaysActionListener);
        startUpdaterGui();
    }

    // externe Definition des ActionListeners, weil dieser vor der Füllung der ComboBox gelöscht und anschließend
    // wieder eingefügt werden muss
    public final ActionListener comboBoxIpGatewaysActionListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            if (comboBoxIpGateways.getSelectedItem() == null) return;
            Discoverer.Result<SearchResponse> value = ((CalimeroSearchComboItem) comboBoxIpGateways.getSelectedItem()).value;
            if (value == null) return;
            InetSocketAddress addr = value.remoteEndpoint();
            textBoxKnxGatewayIpAddr.setText(addr.getAddress().getHostAddress());
            textFieldPort.setText(String.valueOf(addr.getPort()));
        }
    };

    private void setComponentNames() {
        setName("MainFrame");
        comboBoxIpGateways.setName("comboBoxIpGateways");
        advancedSettingsCheckBox.setName("AdvancedSettings");
        textBoxKnxGatewayIpAddr.setName("GatewayIpAddr");
        textFieldPort.setName("GatewayPort");
        natCheckBox.setName("UseNat");
        textFieldTpuart.setName("TpUart");
        textFieldSerial.setName("Serial");
        comboBoxMedium.setName("Medium");
        comboBoxScenario.setName("Scenario");
        textFieldFileName.setName("FlashFilePath");
        textFieldUid.setName("Uid");
        CheckBoxDiffFlash.setName("DiffFlash");
        eraseCompleteFlashCheckBox.setName("EraseCompleteFlash");
        noFlashCheckBox.setName("NoFlash");
        textFieldBootloaderDeviceAddress.setName("BootloaderDeviceAddress");
        textFieldDeviceAddress.setName("DeviceAddress");
        textFieldOwnAddress.setName("OwnAddress");
        textFieldDelay.setName("DelayMs");
        comboBoxKnxTelegramPriority.setName("TelegramPriority");
        textFieldKnxSecureUser.setName("SecureUser");
        textFieldKnxSecureUserPwd.setName("SecureUserPassword");
        textFieldKnxSecureDevicePwd.setName("SecureDevicePassword");
    }

    @SuppressWarnings("SameParameterValue")
    private void saveAllParameters(String fileName) {
        try {
            guiSettings.writeComponentSettings(fileName);
        } catch (IOException e) {
            logger.error("", e); // todo see logback issue https://github.com/qos-ch/logback/issues/876
            JOptionPane.showMessageDialog(this,
                    String.format(getTranslation("IOException.savingSettings.Message"), fileName, e.getMessage()),
                    this.getTranslation("Warning"), JOptionPane.WARNING_MESSAGE);
        }
    }

    @SuppressWarnings("SameParameterValue")
    private boolean loadAllParameters(String fileName) {
        try {
            setComponentNames();
            guiSettings.readComponentsSettings(fileName);
        } catch (IOException e) {
            logger.error("", e); // todo see logback issue https://github.com/qos-ch/logback/issues/876
            JOptionPane.showMessageDialog(this,
                    String.format(getTranslation("IOException.loadingSettings.Message"), fileName, e.getMessage()),
                    this.getTranslation("Warning"), JOptionPane.WARNING_MESSAGE);
            return false;
        }
        setGuiElementsVisibility();
        return true;
    }

    private CliOptions getCliOptions() throws KNXFormatException, ParseException {
        ArrayList<String> argsList = new ArrayList<>();

        argsList.add(argument("", textBoxKnxGatewayIpAddr));
        argsList.add(argument(OPT_LONG_FILENAME, textFieldFileName));
        //argsList.add(argument(OPT_LONG_LOCALHOST, textField*)); // todo add gui textfield --localhost
        //argsList.add(argument(OPT_LONG_LOCALPORT, textField*)); // todo add gui textfield --localport
        argsList.add(argument(OPT_LONG_PORT, textFieldPort));

        argsList.add(argument(OPT_LONG_FT12, textFieldSerial));
        argsList.add(argument(OPT_LONG_TPUART, textFieldTpuart));
        //argsList.add(argument(OPT_LONG_USB, textField*)); // todo add gui textfield --usb

        argsList.add(argument(OPT_LONG_MEDIUM, comboBoxMedium));

        argsList.add(argument(OPT_LONG_USER_ID, textFieldKnxSecureUser));
        argsList.add(argument(OPT_LONG_USER_PASSWORD, textFieldKnxSecureUserPwd));
        argsList.add(argument(OPT_LONG_DEVICE_PASSWORD, textFieldKnxSecureDevicePwd));

        argsList.add(argument(OPT_LONG_PROG_DEVICE, textFieldBootloaderDeviceAddress));

        if (textFieldDeviceAddress.isVisible())
            argsList.add(argument(OPT_LONG_DEVICE, textFieldDeviceAddress));

        if (textFieldOwnAddress.isVisible())
            argsList.add(argument(OPT_LONG_OWN_ADDRESS, textFieldOwnAddress));

        if (textFieldUid.isVisible())
            argsList.add(argument(OPT_LONG_UID, textFieldUid));

        argsList.add(argument(OPT_LONG_DELAY, textFieldDelay));
/*
        if (checkBox*.isVisible() && checkBox*.isSelected()) // todo add gui checkbox --tunnelingv2
            argsList.add(argument(OPT_LONG_TUNNEL_V2));
        if (checkBox*.isVisible() && checkBox*.isSelected()) // todo add gui checkbox --tunneling
            argsList.add(argument(OPT_LONG_TUNNEL_V1));
*/
        if (natCheckBox.isVisible() && natCheckBox.isSelected())
            argsList.add(argument(OPT_LONG_NAT));

        // todo add gui OPT_LONG_ROUTING (--routing)

        if ((CheckBoxDiffFlash.isVisible() && !CheckBoxDiffFlash.isSelected()) || !CheckBoxDiffFlash.isVisible())
            argsList.add(argument(OPT_LONG_FULL));

        if (noFlashCheckBox.isVisible() && noFlashCheckBox.isSelected())
            argsList.add(argument(OPT_LONG_NO_FLASH));

        // argsList.add(argument(OPT_LONG_LOGLEVEL, comboBox*)); // todo add gui combobox --logLevel

        argsList.add(argument(OPT_LONG_PRIORITY, comboBoxKnxTelegramPriority));

        if (eraseCompleteFlashCheckBox.isVisible() && eraseCompleteFlashCheckBox.isSelected())
            argsList.add(argument(OPT_LONG_ERASEFLASH));

        // argsList.add(argument(OPT_LONG_DUMPFLASH, textField*)); // todo add gui textfield --DUMPFLASH startaddress endaddress
/*
        if (checkBox*.isVisible() && checkBox*.isSelected()) // todo add gui checkbox --statistic
            argsList.add(argument(OPT_LONG_LOGSTATISTIC));
*/
        // argsList.add(argument(OPT_LONG_BLOCKSIZE, comboBox*)); // todo add gui combobox --blocksize 256, 512, 1024

/*
        if (checkBox*.isVisible() && checkBox*.isSelected()) // todo add gui checkbox --discover
        argsList.add(argument(OPT_LONG_DISCOVER));
 */
        // argsList.add(argument(OPT_LONG_RECONNECT, textFieldReconnect)); // todo add gui textfield --reconnect
        // todo add gui button for --discover

        return new CliOptions(argsList);
    }

    private void displayCommandLine(CliOptions cliOptions) {
        logger.info("{}java -jar {} {}{}", ansi().fg(YELLOW), ToolInfo.getToolJarName(),
                cliOptions.reconstructCommandLine(), ansi().reset());
    }

    public static void startSwingGui() {
        new GuiMain();
    }

    private void startUpdaterGui() {
        setContentPane(panelMain);
        setTitle(ToolInfo.getToolAndVersion());
        jLoggingPane.setFont(new Font(Font.MONOSPACED, PLAIN, 12));
        jLoggingPane.setBackground(DefaultBackgroundColor);
        jLoggingPane.setForeground(DefaultForegroundColor);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        TextAppender textAppender = ListTextAppenders.searchAppender();
        if (textAppender != null) {
            textAppender.setTextPane(this.jLoggingPane);
        }
        initGuiElementsVisibility();
        fillScenarios();
        fillMediumComboBox();
        fillTelegramPriorityComboBox();

        setMinimumSize(new Dimension(800, 600));
        if (!loadAllParameters(FILENAME_SETTINGS)) {
            setLocationRelativeTo(null);
        }
        int windowExtendedState = getExtendedState();
        if ((windowExtendedState & Frame.ICONIFIED) == Frame.ICONIFIED) {
            windowExtendedState &= ~Frame.ICONIFIED;
        }
        setExtendedState(windowExtendedState);

        mainScrollPane.getVerticalScrollBar().setUnitIncrement(10);

        this.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                saveAllParameters(FILENAME_SETTINGS);
            }
        });
        setVisible(true);
        if (textBoxKnxGatewayIpAddr.getText().isEmpty()) {
            new Thread(this::loadKnxIpInterfacesAndFillComboBox).start();
        }
    }

    private void loadKnxIpInterfacesAndFillComboBox() {
        reloadGatewaysButton.setEnabled(false);
        // ActionListener löschen und nach dem Füllen der ComboBox wieder hinzufügen
        // damit das Füllen kein Event auslöst
        comboBoxIpGateways.removeActionListener(comboBoxIpGatewaysActionListener);

        comboBoxIpGateways.removeAllItems();

        comboBoxIpGateways.addItem(new CalimeroSearchComboItem(getTranslation("selectInterface"), null));

        List<Discoverer.Result<SearchResponse>> result;
        try {
            result = DiscoverKnxInterfaces.getAllnetIPInterfaces();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt(); // Restore interrupt status
            logger.info("Interrupted while discovering IP interfaces", e);  // todo see logback issue https://github.com/qos-ch/logback/issues/876
            return;
        }

        for (Discoverer.Result<SearchResponse> r : result) {
            SearchResponse sr = r.response();
            String ifName = sr.getDevice().getName();
            String ifHostAddress = sr.getControlEndpoint().endpoint().getAddress().getHostAddress();
            IndividualAddress ifPhysAddress = sr.getDevice().getAddress();
            int ifPort = sr.getControlEndpoint().endpoint().getPort();
            int ifTunnelVersion;
            if (sr.v2())
                ifTunnelVersion = 2;
            else
                ifTunnelVersion = 1;

            String ipInterfaceText = String.format("%s (%s:%d) v%d %s", ifName, ifHostAddress, ifPort, ifTunnelVersion,
                    ifPhysAddress.toString());
            logger.debug("Found IP interface: {}", ipInterfaceText);
            CalimeroSearchComboItem comboBoxItem = new CalimeroSearchComboItem(ipInterfaceText, r);
            comboBoxIpGateways.addItem(comboBoxItem);
        }

        Set<Device> usbInterfaces;
        try {
            usbInterfaces = DiscoverKnxInterfaces.getUsbInterfaces();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt(); // Restore interrupt status
            logger.info("Interrupted while discovering USB interfaces", e);  // todo see logback issue https://github.com/qos-ch/logback/issues/876
            return;
        }
        for (final var d : usbInterfaces) {
            final String vp = String.format("%04x:%04x", d.vendorId(), d.productId());
            logger.info("Found USB Interface: {} {} S/N {} VID/PID {}",
                    d.manufacturer(), d.product(), d.serialNumber(), vp);
        }

        comboBoxIpGateways.addActionListener(comboBoxIpGatewaysActionListener);
        reloadGatewaysButton.setEnabled(true);
    }

    private void fillScenarios() {
        if (comboBoxScenario == null) return;
        comboBoxScenario.addItem(new ComboItem(getTranslation(GuiObjsVisOpts.NEWDEV.getVisOption()), GuiObjsVisOpts.NEWDEV));
        comboBoxScenario.addItem(new ComboItem(getTranslation(GuiObjsVisOpts.APPDEV.getVisOption()), GuiObjsVisOpts.APPDEV));
        comboBoxScenario.addItem(new ComboItem(getTranslation(GuiObjsVisOpts.REQUID.getVisOption()), GuiObjsVisOpts.REQUID));
    }

    private void fillMediumComboBox() {
        comboBoxMedium.addItem(KNXMediumSettings.getMediumString(KNXMediumSettings.MEDIUM_TP1));
        comboBoxMedium.addItem(KNXMediumSettings.getMediumString(KNXMediumSettings.MEDIUM_RF));
    }

    private void fillTelegramPriorityComboBox() {
        if (comboBoxKnxTelegramPriority == null) return;

        comboBoxKnxTelegramPriority.addItem(Priority.LOW.toString());
        comboBoxKnxTelegramPriority.addItem(Priority.NORMAL.toString());
        comboBoxKnxTelegramPriority.addItem(Priority.URGENT.toString());
        comboBoxKnxTelegramPriority.addItem(Priority.SYSTEM.toString());
    }

    private void setFrameImages() {
        String resourceTemplate = "/frame_images/selfbus_logo_%sx%s.png";
        String[] resolutions = {"16", "24", "32", "48", "256"};
        List<Image> frameImageList = new ArrayList<>();

        for (String resolution : resolutions) {
            String resourceName = String.format(resourceTemplate, resolution, resolution);
            InputStream imageStream = this.getClass().getResourceAsStream(resourceName);
            if (imageStream == null) {
                logger.warn("getResourceAsStream({}) failed", resourceName);
                continue;
            }

            try {
                Image image = ImageIO.read(imageStream);
                frameImageList.add(image);
                logger.debug("Added {} to frameImageList", resourceName);
            } catch (IOException e) {
                logger.warn("Could not add {} to frameImageList {}", resourceName, Arrays.toString(e.getStackTrace()));
            }
        }

        if (!frameImageList.isEmpty()) {
            this.setIconImages(frameImageList);
        }
    }

    private void createUIComponents() {
        // TODO: place custom component creation code here
        comboBoxIpGateways = new JComboBox<>();
        comboBoxMedium = new JComboBox<>();
        comboBoxScenario = new JComboBox<>();
        comboBoxKnxTelegramPriority = new JComboBox<>();
        setFrameImages();
    }

    public static class ComboItem {
        private final String key;
        final Object value;

        public ComboItem(String key, Object value) {
            this.key = key;
            this.value = value;
        }

        @Override
        public String toString() {
            return key;
        }

        public String getKey() {
            return key;
        }

        public Object getValue() {
            return value;
        }
    }

    public static class CalimeroSearchComboItem {
        private final String key;
        final Discoverer.Result<SearchResponse> value;

        public CalimeroSearchComboItem(String key, Discoverer.Result<SearchResponse> value) {
            this.key = key;
            this.value = value;
        }

        @Override
        public String toString() {
            return key;
        }

        public String getKey() {
            return key;
        }

        public Discoverer.Result<SearchResponse> getValue() {
            return value;
        }
    }

    enum GuiObjsVisOpts {
        // der String ist die Referenz für die Translations
        NEWDEV("newDevice"),
        APPDEV("appDevice"),
        REQUID("requestUid"),
        ADVSET("advancedSettings");

        private final String visOption;

        GuiObjsVisOpts(String VisOption) {
            this.visOption = VisOption;
        }

        public String getVisOption() {
            return visOption;
        }
    }

    private static final Map<Object, List<GuiObjsVisOpts>> GuiObjectsMap = new HashMap<>();

    private void initGuiElementsVisibility() {
        // hier wird definiert, in welchem Zustand der GUI welche Elemente sichtbar sein sollen

        GuiObjectsMap.put(buttonLoadFile, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV));
        GuiObjectsMap.put(textFieldFileName, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV));
        GuiObjectsMap.put(labelFileName, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV));
        GuiObjectsMap.put(labelFileNameHint, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV));

        GuiObjectsMap.put(buttonRequestUid, List.of(GuiObjsVisOpts.REQUID));

        GuiObjectsMap.put(labelUid, Arrays.asList(GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID));
        GuiObjectsMap.put(textFieldUid, Arrays.asList(GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID));
        GuiObjectsMap.put(labelUidHint, Arrays.asList(GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID));

        GuiObjectsMap.put(labelMedium, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(comboBoxMedium, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(labelSerial, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(textFieldSerial, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(labelTpuart, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(textFieldTpuart, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(labelDeviceAddress, List.of(GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID));
        GuiObjectsMap.put(textFieldDeviceAddress, List.of(GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID));

        GuiObjectsMap.put(labelBootloaderDeviceAddr, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(textFieldBootloaderDeviceAddress, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(labelOwnAddress, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(textFieldOwnAddress, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(labelDelay, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(textFieldDelay, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(CheckBoxDiffFlash, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(labelFullFlashHint, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(natCheckBox, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(eraseCompleteFlashCheckBox, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(labelEraseCompleteFlash, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(noFlashCheckBox, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(labelNoFlashHint, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(buttonStartStopFlash, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV));

        GuiObjectsMap.put(labelTelegramPriority, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(comboBoxKnxTelegramPriority, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(labelKnxSecureUser, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(textFieldKnxSecureUser, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(labelKnxSecureUserHint, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(labelKnxSecureUserPwd, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(textFieldKnxSecureUserPwd, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(labelKnxSecureUserPwdHint, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));

        GuiObjectsMap.put(labelKnxSecureDevicePwd, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(textFieldKnxSecureDevicePwd, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
        GuiObjectsMap.put(labelKnxSecureDevicePwdHint, Arrays.asList(GuiObjsVisOpts.NEWDEV, GuiObjsVisOpts.APPDEV, GuiObjsVisOpts.REQUID, GuiObjsVisOpts.ADVSET));
    }

    private void setGuiElementsVisibility() {

        final GuiObjsVisOpts selectedScenario = (GuiObjsVisOpts) ((ComboItem) Objects.requireNonNull(comboBoxScenario.getSelectedItem())).getValue();

        switch (selectedScenario) {
            case NEWDEV:
                labelScenarioHint.setText(getTranslation("newDeviceHint"));
                break;
            case APPDEV:
                labelScenarioHint.setText(getTranslation("appDeviceHint"));
                break;
            case REQUID:
                labelScenarioHint.setText(getTranslation("requestUidHint"));
                break;
        }

        for (var guiObjectEntry : GuiObjectsMap.entrySet()) {
            boolean isInScenario = guiObjectEntry.getValue().contains(selectedScenario);
            if (isInScenario && guiObjectEntry.getValue().contains(GuiObjsVisOpts.ADVSET)) {
                ((JComponent) guiObjectEntry.getKey()).setVisible(advancedSettingsCheckBox.isSelected());
            } else {
                ((JComponent) guiObjectEntry.getKey()).setVisible(isInScenario);
            }
        }
    }


    /**
     * Method generated by IntelliJ IDEA GUI Designer
     * >>> IMPORTANT!! <<<
     * DO NOT edit this method OR call it in your code!
     *
     * @noinspection ALL
     */
    private void $$$setupUI$$$() {
        createUIComponents();
        panelMain = new JPanel();
        panelMain.setLayout(new GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        panelMain.setAutoscrolls(true);
        panelMain.setMinimumSize(new Dimension(400, 300));
        advancedSettingsCheckBox = new JCheckBox();
        advancedSettingsCheckBox.setSelected(false);
        this.$$$loadButtonText$$$(advancedSettingsCheckBox, this.$$$getMessageFromBundle$$$("language/GuiMain", "advancedSettings"));
        panelMain.add(advancedSettingsCheckBox, new GridConstraints(0, 0, 1, 1, GridConstraints.ANCHOR_EAST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        mainScrollPane = new JScrollPane();
        panelMain.add(mainScrollPane, new GridConstraints(1, 0, 1, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_BOTH, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_WANT_GROW, null, null, null, 0, false));
        final JPanel panel1 = new JPanel();
        panel1.setLayout(new GridLayoutManager(31, 5, new Insets(0, 0, 0, 0), -1, -1));
        mainScrollPane.setViewportView(panel1);
        final Spacer spacer1 = new Spacer();
        panel1.add(spacer1, new GridConstraints(0, 4, 30, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, 1, null, new Dimension(5, -1), null, 0, false));
        final Spacer spacer2 = new Spacer();
        panel1.add(spacer2, new GridConstraints(0, 0, 30, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, 1, null, new Dimension(5, -1), null, 0, false));
        final JLabel label1 = new JLabel();
        Font label1Font = this.$$$getFont$$$(null, Font.BOLD, 14, label1.getFont());
        if (label1Font != null) label1.setFont(label1Font);
        this.$$$loadLabelText$$$(label1, this.$$$getMessageFromBundle$$$("language/GuiMain", "KnxGatewayConnectionSettings"));
        panel1.add(label1, new GridConstraints(0, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelIpGateway = new JLabel();
        this.$$$loadLabelText$$$(labelIpGateway, this.$$$getMessageFromBundle$$$("language/GuiMain", "selectKnxIpGateway"));
        panel1.add(labelIpGateway, new GridConstraints(1, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        panel1.add(comboBoxIpGateways, new GridConstraints(1, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        final JLabel label2 = new JLabel();
        this.$$$loadLabelText$$$(label2, this.$$$getMessageFromBundle$$$("language/GuiMain", "ipAddress"));
        panel1.add(label2, new GridConstraints(2, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textBoxKnxGatewayIpAddr = new JTextField();
        panel1.add(textBoxKnxGatewayIpAddr, new GridConstraints(2, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labeIIpHint = new JLabel();
        this.$$$loadLabelText$$$(labeIIpHint, this.$$$getMessageFromBundle$$$("language/GuiMain", "ipAddrHint"));
        panel1.add(labeIIpHint, new GridConstraints(2, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldPort = new JTextField();
        panel1.add(textFieldPort, new GridConstraints(3, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        final JLabel label3 = new JLabel();
        this.$$$loadLabelText$$$(label3, this.$$$getMessageFromBundle$$$("language/GuiMain", "port"));
        panel1.add(label3, new GridConstraints(3, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelPortHint = new JLabel();
        this.$$$loadLabelText$$$(labelPortHint, this.$$$getMessageFromBundle$$$("language/GuiMain", "portHint"));
        panel1.add(labelPortHint, new GridConstraints(3, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        natCheckBox = new JCheckBox();
        this.$$$loadButtonText$$$(natCheckBox, this.$$$getMessageFromBundle$$$("language/GuiMain", "useNat"));
        panel1.add(natCheckBox, new GridConstraints(4, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldTpuart = new JTextField();
        panel1.add(textFieldTpuart, new GridConstraints(5, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelTpuart = new JLabel();
        this.$$$loadLabelText$$$(labelTpuart, this.$$$getMessageFromBundle$$$("language/GuiMain", "tpuart"));
        panel1.add(labelTpuart, new GridConstraints(5, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldSerial = new JTextField();
        textFieldSerial.setText("");
        panel1.add(textFieldSerial, new GridConstraints(6, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelSerial = new JLabel();
        this.$$$loadLabelText$$$(labelSerial, this.$$$getMessageFromBundle$$$("language/GuiMain", "serial"));
        panel1.add(labelSerial, new GridConstraints(6, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelMedium = new JLabel();
        this.$$$loadLabelText$$$(labelMedium, this.$$$getMessageFromBundle$$$("language/GuiMain", "medium"));
        panel1.add(labelMedium, new GridConstraints(7, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        final JLabel label4 = new JLabel();
        Font label4Font = this.$$$getFont$$$(null, Font.BOLD, 14, label4.getFont());
        if (label4Font != null) label4.setFont(label4Font);
        this.$$$loadLabelText$$$(label4, this.$$$getMessageFromBundle$$$("language/GuiMain", "UpdaterSettings"));
        panel1.add(label4, new GridConstraints(9, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        final JSeparator separator1 = new JSeparator();
        panel1.add(separator1, new GridConstraints(8, 1, 1, 3, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_BOTH, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, new Dimension(-1, 1), 0, false));
        final JLabel label5 = new JLabel();
        this.$$$loadLabelText$$$(label5, this.$$$getMessageFromBundle$$$("language/GuiMain", "scenario"));
        panel1.add(label5, new GridConstraints(10, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelScenarioHint = new JLabel();
        this.$$$loadLabelText$$$(labelScenarioHint, this.$$$getMessageFromBundle$$$("language/GuiMain", "labelScenarioHint"));
        panel1.add(labelScenarioHint, new GridConstraints(10, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        buttonLoadFile = new JButton();
        this.$$$loadButtonText$$$(buttonLoadFile, this.$$$getMessageFromBundle$$$("language/GuiMain", "loadFile"));
        panel1.add(buttonLoadFile, new GridConstraints(11, 2, 1, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldFileName = new JTextField();
        panel1.add(textFieldFileName, new GridConstraints(12, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelFileName = new JLabel();
        this.$$$loadLabelText$$$(labelFileName, this.$$$getMessageFromBundle$$$("language/GuiMain", "fileName"));
        panel1.add(labelFileName, new GridConstraints(12, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelFileNameHint = new JLabel();
        this.$$$loadLabelText$$$(labelFileNameHint, this.$$$getMessageFromBundle$$$("language/GuiMain", "fileNameHint"));
        panel1.add(labelFileNameHint, new GridConstraints(12, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        buttonRequestUid = new JButton();
        this.$$$loadButtonText$$$(buttonRequestUid, this.$$$getMessageFromBundle$$$("language/GuiMain", "requestUid"));
        panel1.add(buttonRequestUid, new GridConstraints(13, 2, 1, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldUid = new JTextField();
        panel1.add(textFieldUid, new GridConstraints(14, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelUid = new JLabel();
        this.$$$loadLabelText$$$(labelUid, this.$$$getMessageFromBundle$$$("language/GuiMain", "uid"));
        panel1.add(labelUid, new GridConstraints(14, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelUidHint = new JLabel();
        this.$$$loadLabelText$$$(labelUidHint, this.$$$getMessageFromBundle$$$("language/GuiMain", "uidHint"));
        panel1.add(labelUidHint, new GridConstraints(14, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        CheckBoxDiffFlash = new JCheckBox();
        this.$$$loadButtonText$$$(CheckBoxDiffFlash, this.$$$getMessageFromBundle$$$("language/GuiMain", "diffFlash"));
        panel1.add(CheckBoxDiffFlash, new GridConstraints(15, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelFullFlashHint = new JLabel();
        this.$$$loadLabelText$$$(labelFullFlashHint, this.$$$getMessageFromBundle$$$("language/GuiMain", "diffFlashHint"));
        panel1.add(labelFullFlashHint, new GridConstraints(15, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        eraseCompleteFlashCheckBox = new JCheckBox();
        this.$$$loadButtonText$$$(eraseCompleteFlashCheckBox, this.$$$getMessageFromBundle$$$("language/GuiMain", "eraseFlash"));
        panel1.add(eraseCompleteFlashCheckBox, new GridConstraints(16, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelEraseCompleteFlash = new JLabel();
        this.$$$loadLabelText$$$(labelEraseCompleteFlash, this.$$$getMessageFromBundle$$$("language/GuiMain", "eraseCompleteFlashHint"));
        panel1.add(labelEraseCompleteFlash, new GridConstraints(16, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        noFlashCheckBox = new JCheckBox();
        this.$$$loadButtonText$$$(noFlashCheckBox, this.$$$getMessageFromBundle$$$("language/GuiMain", "noFlash"));
        panel1.add(noFlashCheckBox, new GridConstraints(17, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelNoFlashHint = new JLabel();
        this.$$$loadLabelText$$$(labelNoFlashHint, this.$$$getMessageFromBundle$$$("language/GuiMain", "noFlashHint"));
        panel1.add(labelNoFlashHint, new GridConstraints(17, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        final JLabel label6 = new JLabel();
        Font label6Font = this.$$$getFont$$$(null, Font.BOLD, 14, label6.getFont());
        if (label6Font != null) label6.setFont(label6Font);
        this.$$$loadLabelText$$$(label6, this.$$$getMessageFromBundle$$$("language/GuiMain", "KnxBusSettings"));
        panel1.add(label6, new GridConstraints(19, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelBootloaderDeviceAddr = new JLabel();
        this.$$$loadLabelText$$$(labelBootloaderDeviceAddr, this.$$$getMessageFromBundle$$$("language/GuiMain", "knxProgDeviceAddr"));
        panel1.add(labelBootloaderDeviceAddr, new GridConstraints(20, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelDeviceAddress = new JLabel();
        this.$$$loadLabelText$$$(labelDeviceAddress, this.$$$getMessageFromBundle$$$("language/GuiMain", "knxDeviceAddr"));
        panel1.add(labelDeviceAddress, new GridConstraints(21, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelOwnAddress = new JLabel();
        this.$$$loadLabelText$$$(labelOwnAddress, this.$$$getMessageFromBundle$$$("language/GuiMain", "knxOwnAddress"));
        panel1.add(labelOwnAddress, new GridConstraints(22, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldOwnAddress = new JTextField();
        panel1.add(textFieldOwnAddress, new GridConstraints(22, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        textFieldDelay = new JTextField();
        panel1.add(textFieldDelay, new GridConstraints(23, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelDelay = new JLabel();
        this.$$$loadLabelText$$$(labelDelay, this.$$$getMessageFromBundle$$$("language/GuiMain", "knxMessageDelay"));
        panel1.add(labelDelay, new GridConstraints(23, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        panel1.add(comboBoxKnxTelegramPriority, new GridConstraints(25, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelTelegramPriority = new JLabel();
        this.$$$loadLabelText$$$(labelTelegramPriority, this.$$$getMessageFromBundle$$$("language/GuiMain", "messagePriority"));
        panel1.add(labelTelegramPriority, new GridConstraints(25, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelKnxSecureUser = new JLabel();
        this.$$$loadLabelText$$$(labelKnxSecureUser, this.$$$getMessageFromBundle$$$("language/GuiMain", "knxSecureUser"));
        panel1.add(labelKnxSecureUser, new GridConstraints(26, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldKnxSecureUser = new JTextField();
        panel1.add(textFieldKnxSecureUser, new GridConstraints(26, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelKnxSecureUserHint = new JLabel();
        this.$$$loadLabelText$$$(labelKnxSecureUserHint, this.$$$getMessageFromBundle$$$("language/GuiMain", "knxSecureUserHint"));
        panel1.add(labelKnxSecureUserHint, new GridConstraints(26, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelKnxSecureUserPwd = new JLabel();
        this.$$$loadLabelText$$$(labelKnxSecureUserPwd, this.$$$getMessageFromBundle$$$("language/GuiMain", "knxSecureUserPwd"));
        panel1.add(labelKnxSecureUserPwd, new GridConstraints(27, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelKnxSecureUserPwdHint = new JLabel();
        this.$$$loadLabelText$$$(labelKnxSecureUserPwdHint, this.$$$getMessageFromBundle$$$("language/GuiMain", "knxSecureUserPwdHint"));
        panel1.add(labelKnxSecureUserPwdHint, new GridConstraints(27, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldKnxSecureUserPwd = new JTextField();
        panel1.add(textFieldKnxSecureUserPwd, new GridConstraints(27, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelKnxSecureDevicePwd = new JLabel();
        this.$$$loadLabelText$$$(labelKnxSecureDevicePwd, this.$$$getMessageFromBundle$$$("language/GuiMain", "knxSecureDevicePwd"));
        panel1.add(labelKnxSecureDevicePwd, new GridConstraints(28, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldKnxSecureDevicePwd = new JTextField();
        panel1.add(textFieldKnxSecureDevicePwd, new GridConstraints(28, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelKnxSecureDevicePwdHint = new JLabel();
        this.$$$loadLabelText$$$(labelKnxSecureDevicePwdHint, this.$$$getMessageFromBundle$$$("language/GuiMain", "knxSecureDevicePwdHint"));
        panel1.add(labelKnxSecureDevicePwdHint, new GridConstraints(28, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        buttonStartStopFlash = new JButton();
        this.$$$loadButtonText$$$(buttonStartStopFlash, this.$$$getMessageFromBundle$$$("language/GuiMain", "startFlash"));
        panel1.add(buttonStartStopFlash, new GridConstraints(29, 2, 1, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        final JScrollPane scrollPane1 = new JScrollPane();
        panel1.add(scrollPane1, new GridConstraints(30, 0, 1, 5, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_BOTH, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_WANT_GROW, new Dimension(-1, 200), new Dimension(-1, 200), null, 0, false));
        jLoggingPane = new JTextPane();
        jLoggingPane.setEditable(false);
        scrollPane1.setViewportView(jLoggingPane);
        final JSeparator separator2 = new JSeparator();
        panel1.add(separator2, new GridConstraints(18, 1, 1, 3, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_BOTH, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, new Dimension(-1, 1), 0, false));
        textFieldBootloaderDeviceAddress = new JTextField();
        panel1.add(textFieldBootloaderDeviceAddress, new GridConstraints(20, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        textFieldDeviceAddress = new JTextField();
        panel1.add(textFieldDeviceAddress, new GridConstraints(21, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        panel1.add(comboBoxMedium, new GridConstraints(7, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        panel1.add(comboBoxScenario, new GridConstraints(10, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        reloadGatewaysButton = new JButton();
        this.$$$loadButtonText$$$(reloadGatewaysButton, this.$$$getMessageFromBundle$$$("language/GuiMain", "reloadKnxIpGateways"));
        panel1.add(reloadGatewaysButton, new GridConstraints(1, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelIpGateway.setLabelFor(comboBoxIpGateways);
        label2.setLabelFor(textBoxKnxGatewayIpAddr);
        label3.setLabelFor(textFieldPort);
        labelTpuart.setLabelFor(textFieldTpuart);
        labelSerial.setLabelFor(textFieldSerial);
        labelMedium.setLabelFor(comboBoxMedium);
        label5.setLabelFor(comboBoxScenario);
        labelFileName.setLabelFor(textFieldFileName);
        labelUid.setLabelFor(textFieldUid);
        labelBootloaderDeviceAddr.setLabelFor(textFieldBootloaderDeviceAddress);
        labelDeviceAddress.setLabelFor(textFieldDeviceAddress);
        labelOwnAddress.setLabelFor(textFieldOwnAddress);
        labelDelay.setLabelFor(textFieldDelay);
        labelTelegramPriority.setLabelFor(comboBoxKnxTelegramPriority);
        labelKnxSecureUser.setLabelFor(textFieldKnxSecureUser);
        labelKnxSecureUserPwd.setLabelFor(textFieldKnxSecureUserPwd);
        labelKnxSecureDevicePwd.setLabelFor(textFieldKnxSecureDevicePwd);
    }

    /**
     * @noinspection ALL
     */
    private Font $$$getFont$$$(String fontName, int style, int size, Font currentFont) {
        if (currentFont == null) return null;
        String resultName;
        if (fontName == null) {
            resultName = currentFont.getName();
        } else {
            Font testFont = new Font(fontName, Font.PLAIN, 10);
            if (testFont.canDisplay('a') && testFont.canDisplay('1')) {
                resultName = fontName;
            } else {
                resultName = currentFont.getName();
            }
        }
        Font font = new Font(resultName, style >= 0 ? style : currentFont.getStyle(), size >= 0 ? size : currentFont.getSize());
        boolean isMac = System.getProperty("os.name", "").toLowerCase(Locale.ENGLISH).startsWith("mac");
        Font fontWithFallback = isMac ? new Font(font.getFamily(), font.getStyle(), font.getSize()) : new StyleContext().getFont(font.getFamily(), font.getStyle(), font.getSize());
        return fontWithFallback instanceof FontUIResource ? fontWithFallback : new FontUIResource(fontWithFallback);
    }

    private static Method $$$cachedGetBundleMethod$$$ = null;

    private String $$$getMessageFromBundle$$$(String path, String key) {
        ResourceBundle bundle;
        try {
            Class<?> thisClass = this.getClass();
            if ($$$cachedGetBundleMethod$$$ == null) {
                Class<?> dynamicBundleClass = thisClass.getClassLoader().loadClass("com.intellij.DynamicBundle");
                $$$cachedGetBundleMethod$$$ = dynamicBundleClass.getMethod("getBundle", String.class, Class.class);
            }
            bundle = (ResourceBundle) $$$cachedGetBundleMethod$$$.invoke(null, path, thisClass);
        } catch (Exception e) {
            bundle = ResourceBundle.getBundle(path);
        }
        return bundle.getString(key);
    }

    /**
     * @noinspection ALL
     */
    private void $$$loadLabelText$$$(JLabel component, String text) {
        StringBuffer result = new StringBuffer();
        boolean haveMnemonic = false;
        char mnemonic = '\0';
        int mnemonicIndex = -1;
        for (int i = 0; i < text.length(); i++) {
            if (text.charAt(i) == '&') {
                i++;
                if (i == text.length()) break;
                if (!haveMnemonic && text.charAt(i) != '&') {
                    haveMnemonic = true;
                    mnemonic = text.charAt(i);
                    mnemonicIndex = result.length();
                }
            }
            result.append(text.charAt(i));
        }
        component.setText(result.toString());
        if (haveMnemonic) {
            component.setDisplayedMnemonic(mnemonic);
            component.setDisplayedMnemonicIndex(mnemonicIndex);
        }
    }

    /**
     * @noinspection ALL
     */
    private void $$$loadButtonText$$$(AbstractButton component, String text) {
        StringBuffer result = new StringBuffer();
        boolean haveMnemonic = false;
        char mnemonic = '\0';
        int mnemonicIndex = -1;
        for (int i = 0; i < text.length(); i++) {
            if (text.charAt(i) == '&') {
                i++;
                if (i == text.length()) break;
                if (!haveMnemonic && text.charAt(i) != '&') {
                    haveMnemonic = true;
                    mnemonic = text.charAt(i);
                    mnemonicIndex = result.length();
                }
            }
            result.append(text.charAt(i));
        }
        component.setText(result.toString());
        if (haveMnemonic) {
            component.setMnemonic(mnemonic);
            component.setDisplayedMnemonicIndex(mnemonicIndex);
        }
    }

    /**
     * @noinspection ALL
     */
    public JComponent $$$getRootComponent$$$() {
        return panelMain;
    }

}
