package org.selfbus.updater.gui;

import com.intellij.uiDesigner.core.GridConstraints;
import com.intellij.uiDesigner.core.GridLayoutManager;
import com.intellij.uiDesigner.core.Spacer;
import io.calimero.KNXFormatException;
import io.calimero.KNXIllegalArgumentException;
import io.calimero.knxnetip.Discoverer;
import io.calimero.knxnetip.servicetype.SearchResponse;

import org.apache.commons.cli.ParseException;
import org.selfbus.updater.CliOptions;
import org.selfbus.updater.DiscoverKnxInterfaces;
import org.selfbus.updater.ToolInfo;
import org.selfbus.updater.Updater;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.Method;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Objects;
import java.util.Properties;
import java.util.ResourceBundle;


public class GuiMain extends JFrame {
    private JButton buttonLoadFile;
    private JTextField textBoxKnxGatewayIpAddr;
    private JPanel panelMain;
    private JComboBox<ComboItem> comboBoxIpGateways;
    private JLabel labelIpGateway;
    private JCheckBox CheckBoxFullFlash;
    private JTextField textFieldUid;
    private JLabel labelUid;
    private JButton buttonStartStopFlash;
    private JTextPane jLoggingPane;
    private JTextField textFieldFileName;
    private JLabel labelFileName;
    private JScrollPane scrollPaneConsole;
    private JComboBox<String> comboBoxScenario;
    private JComboBox<String> comboBoxMedium;
    private JTextField textFieldSerial;
    private JTextField textFieldTpuart;
    private JTextField textFieldDeviceAddress;
    private JTextField textFieldBootloaderDeviceAddress;
    private JTextField textFieldPort;
    private JCheckBox natCheckBox;
    private JTextField textFieldOwnAddress;
    private JTextField textFieldDelay;
    private JTextField textFieldTimeout;
    private JCheckBox eraseCompleteFlashCheckBox;
    private JCheckBox noFlashCheckBox;
    private JLabel labelMedium;
    private JLabel labelSerial;
    private JLabel labelTpuart;
    private JLabel labelDeviceAddress;
    private JLabel labelBootloaderDeviceAddr;
    private JLabel labelOwnAddress;
    private JLabel labelDelay;
    private JLabel labelTimeout;
    private JLabel labelPortHint;
    private JLabel labeIIpHint;
    private JLabel labelFileNameHint;
    private JLabel labelUidHint;
    private JLabel labelFullFlashHint;
    private JLabel labelNoFlashHint;
    private JLabel labelEraseCompleteFlash;
    private JLabel labelScenarioHint;
    private JButton buttonRequestUid;
    private InetAddress knxGatewayIpAddr;

    private CliOptions cliOptions;

    private Thread updaterThread;

    public static GuiMain guiMainInstance;

    private final static Logger logger = LoggerFactory.getLogger(GuiMain.class.getName());

    private static final Properties userProperties = new Properties();

    private static Locale locale;

    public GuiMain() {
        buttonLoadFile.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                JFileChooser fc = new JFileChooser();
                String propFile = null;
                
                if(new File("settings.xml").exists()) {
                    try {
                        userProperties.loadFromXML(new FileInputStream("settings.xml"));

                    } catch (IOException ex) {
                        throw new RuntimeException(ex);
                    }
                    propFile = userProperties.getProperty("hexFilePath");
                }

                if (propFile != null && !propFile.isEmpty()) {
                    fc.setCurrentDirectory(new File(propFile).getParentFile());
                }
                fc.setFileFilter(new FileNameExtensionFilter("HEX", "hex"));
                int result = fc.showOpenDialog(panelMain);

                if (result == JFileChooser.APPROVE_OPTION) {
                    if (fc.getSelectedFile().exists()) {
                        textFieldFileName.setText(fc.getSelectedFile().toString());
                        storeFlashFilePath(fc.getSelectedFile());
                    }
                }
            }
        });

        // if there was a manual change of file path
        textFieldFileName.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                userProperties.setProperty("hexFilePath", textFieldFileName.getText());
                storeFlashFilePath(new File(textFieldFileName.getText()));
            }
        });

        comboBoxIpGateways.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if ((ComboItem) comboBoxIpGateways.getSelectedItem() == null) return;
                InetSocketAddress addr = ((ComboItem) comboBoxIpGateways.getSelectedItem()).value.remoteEndpoint();
                textBoxKnxGatewayIpAddr.setText(addr.getAddress().getHostAddress());
                textFieldPort.setText(String.valueOf(addr.getPort()));
            }
        });

        buttonStartStopFlash.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                ResourceBundle bundle = ResourceBundle.getBundle("GuiTranslation");

                if (Objects.equals(buttonStartStopFlash.getText(), bundle.getString("startFlash"))) {
                    jLoggingPane.setText("");

                    updaterThread = new Thread() {
                        public void run() {
                            try {
                                setCliOptions();
                            } catch (KNXFormatException | ParseException ex) {
                                throw new RuntimeException(ex);
                            }

                            try {
                                final Updater d = new Updater(cliOptions);

                                //final ShutdownHandler sh = new ShutdownHandler().register();
                                d.run();
                                SwingUtilities.invokeLater(() -> {
                                    guiMainInstance.buttonStartStopFlash.setText(bundle.getString("startFlash"));
                                });
                                //sh.unregister();
                            } catch (final Throwable t) {
                                logger.error("parsing options ", t);
                            } finally {
                                logger.info("\n\n");
                                logger.debug("main exit");
                            }
                        }
                    };
                    updaterThread.start();

                    buttonStartStopFlash.setText(bundle.getString("stopFlash"));
                } else {
                    updaterThread.interrupt(); //.stop() is deprecated


                    buttonStartStopFlash.setText(bundle.getString("startFlash"));
                }
            }
        });
        comboBoxScenario.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                setGuiElementsVisibility();
            }
        });
        buttonRequestUid.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                updaterThread = new Thread() {
                    public void run() {
                        try {
                            if (textFieldFileName.getText().isEmpty()) {
                                textFieldFileName.setText("dummy.hex");
                            }
                            setCliOptions();
                            if (Objects.equals(textFieldFileName.getText(), "dummy.hex")) {
                                textFieldFileName.setText("");
                            }
                        } catch (KNXFormatException | ParseException ex) {
                            throw new RuntimeException(ex);
                        }

                        try {
                            final Updater d = new Updater(cliOptions);

                            String uid = d.requestUid();

                            SwingUtilities.invokeLater(() -> {
                                guiMainInstance.textFieldUid.setText(uid);
                            });

                        } catch (final Throwable t) {
                            logger.error("parsing options ", t);
                        } finally {
                            logger.info("\n\n");
                            logger.debug("main exit");
                        }
                    }
                };
                updaterThread.start();
            }
        });
    }

    private void storeFlashFilePath(File flashFile) {
        if (flashFile.exists()) {
            userProperties.setProperty("hexFilePath", flashFile.toString());
            try {
                userProperties.storeToXML(new FileOutputStream("settings.xml"), "");
            } catch (IOException ex) {
                throw new RuntimeException(ex);
            }
        }
    }

    private void setCliOptions() throws KNXFormatException, ParseException {
        ArrayList<String> argsList = new ArrayList<>();

        argsList.add(((ComboItem) Objects.requireNonNull(comboBoxIpGateways.getSelectedItem())).value.remoteEndpoint().getAddress().getHostAddress());

        argsList.add("-f" + textFieldFileName.getText());
        if (textFieldUid.isVisible() && !Objects.equals(textFieldUid.getText(), ""))
            argsList.add("-u" + textFieldUid.getText());
        if (comboBoxMedium.isVisible()) argsList.add("-m" + comboBoxMedium.getSelectedItem());
        if (textFieldSerial.isVisible()) argsList.add("-s" + textFieldSerial.getText());
        if (textFieldTpuart.isVisible()) argsList.add("-t" + textFieldTpuart.getText());
        if (textFieldDeviceAddress.isVisible()) argsList.add("-d" + textFieldDeviceAddress.getText());
        if (textFieldBootloaderDeviceAddress.isVisible())
            argsList.add("-D" + textFieldBootloaderDeviceAddress.getText());
        if (textFieldOwnAddress.isVisible()) argsList.add("-o" + textFieldOwnAddress.getText());
        if (CheckBoxFullFlash.isVisible() && CheckBoxFullFlash.isSelected()) argsList.add("-f1");
        if (natCheckBox.isVisible() && natCheckBox.isSelected()) argsList.add("-n");
        if (textFieldDelay.isVisible()) argsList.add("--delay" + textFieldDelay.getText());
        if (textFieldTimeout.isVisible()) argsList.add("--timeout" + textFieldTimeout.getText());
        if (eraseCompleteFlashCheckBox.isVisible() && eraseCompleteFlashCheckBox.isSelected())
            argsList.add("--ERASEFLASH");
        if (noFlashCheckBox.isVisible() && noFlashCheckBox.isSelected()) argsList.add("-f0");

        String[] args = new String[argsList.size()];
        args = argsList.toArray(args);

        try {
            // read in user-supplied command line options
            this.cliOptions = new CliOptions(args, String.format("SB_updater-%s-all.jar", ToolInfo.getVersion()),
                    "Selfbus KNX-Firmware update tool options", "", Updater.PHYS_ADDRESS_BOOTLOADER, Updater.PHYS_ADDRESS_OWN);
        } catch (final KNXIllegalArgumentException | KNXFormatException | ParseException e) {
            throw e;
        } catch (final RuntimeException e) {
            throw new KNXIllegalArgumentException(e.getMessage(), e);
        }
    }


    public static void startSwingGui() {
        SwingUtilities.invokeLater(() -> {
            guiMainInstance = new GuiMain();
            guiMainInstance.startUpdaterGui();
        });
    }

    public void startUpdaterGui() {
        this.setContentPane(this.panelMain);
        this.setTitle("Selfbus Updater");
        //t.setBounds(600,200,200,200);
        this.setSize(1000, 800);
        this.setVisible(true);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        new Thread() {
            public void run() {
                DiscoverKnxInterfaces.getAllInterfaces().forEach(r ->
                        comboBoxIpGateways.addItem(new ComboItem(r.response().getDevice().getName() +
                                " (" + r.response().getControlEndpoint().getAddress().getHostAddress() + ")", r)));
            }
        }.start();

        ListAppender.addLog4j2TextPaneAppender(this.jLoggingPane);

        fillScenarios();
        fillMediumComboBox();
    }

    private void fillScenarios() {
        if (comboBoxScenario == null) return;

        ResourceBundle bundle = ResourceBundle.getBundle("GuiTranslation");
        String test = bundle.getString("appDevice");

        comboBoxScenario.addItem(bundle.getString("newDevice"));
        comboBoxScenario.addItem(bundle.getString("appDevice"));
        comboBoxScenario.addItem(bundle.getString("requestUid"));
        comboBoxScenario.addItem(bundle.getString("allOptions"));
    }

    private void fillMediumComboBox() {
        comboBoxMedium.addItem("tp1");
        comboBoxMedium.addItem("rf");
    }

    private void createUIComponents() {
        // TODO: place custom component creation code here
        comboBoxScenario = new JComboBox<String>();
    }

    public static class ComboItem {
        private final String key;
        Discoverer.Result<SearchResponse> value;

        public ComboItem(String key, Discoverer.Result<SearchResponse> value) {
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

    private void setGuiElementsVisibility() {
        ResourceBundle bundle = ResourceBundle.getBundle("GuiTranslation");

        final String selectedScenario = Objects.requireNonNull(comboBoxScenario.getSelectedItem()).toString();

        if (selectedScenario.equals(bundle.getString("newDevice"))) {
            labelScenarioHint.setText(bundle.getString("newDeviceHint"));

            buttonLoadFile.setVisible(true);
            textFieldFileName.setVisible(true);
            labelFileName.setVisible(true);
            labelFileNameHint.setVisible(true);

            buttonRequestUid.setVisible(false);

            labelUid.setVisible(false);
            textFieldUid.setVisible(false);
            labelUidHint.setVisible(false);

            labelMedium.setVisible(false);
            comboBoxMedium.setVisible(false);

            labelSerial.setVisible(false);
            textFieldSerial.setVisible(false);

            labelTpuart.setVisible(false);
            textFieldTpuart.setVisible(false);

            labelDeviceAddress.setVisible(false);
            textFieldDeviceAddress.setVisible(false);

            labelBootloaderDeviceAddr.setVisible(false);
            textFieldBootloaderDeviceAddress.setVisible(false);

            labelOwnAddress.setVisible(false);
            textFieldOwnAddress.setVisible(false);

            labelDelay.setVisible(false);
            textFieldDelay.setVisible(false);

            labelTimeout.setVisible(false);
            textFieldTimeout.setVisible(false);

            CheckBoxFullFlash.setVisible(true);
            labelFullFlashHint.setVisible(true);

            natCheckBox.setVisible(false);

            eraseCompleteFlashCheckBox.setVisible(false);
            labelEraseCompleteFlash.setVisible(false);

            noFlashCheckBox.setVisible(false);
            labelNoFlashHint.setVisible(false);

        } else if (selectedScenario.equals(bundle.getString("appDevice"))) {
            labelScenarioHint.setText(bundle.getString("appDeviceHint"));

            buttonLoadFile.setVisible(true);
            textFieldFileName.setVisible(true);
            labelFileName.setVisible(true);
            labelFileNameHint.setVisible(true);

            buttonRequestUid.setVisible(false);

            labelUid.setVisible(true);
            textFieldUid.setVisible(true);
            labelUidHint.setVisible(true);

            labelMedium.setVisible(false);
            comboBoxMedium.setVisible(false);

            labelSerial.setVisible(false);
            textFieldSerial.setVisible(false);

            labelTpuart.setVisible(false);
            textFieldTpuart.setVisible(false);

            labelDeviceAddress.setVisible(false);
            textFieldDeviceAddress.setVisible(false);

            labelBootloaderDeviceAddr.setVisible(false);
            textFieldBootloaderDeviceAddress.setVisible(false);

            labelOwnAddress.setVisible(false);
            textFieldOwnAddress.setVisible(false);

            labelDelay.setVisible(false);
            textFieldDelay.setVisible(false);

            labelTimeout.setVisible(false);
            textFieldTimeout.setVisible(false);

            CheckBoxFullFlash.setVisible(true);
            labelFullFlashHint.setVisible(true);

            natCheckBox.setVisible(false);

            eraseCompleteFlashCheckBox.setVisible(false);
            labelEraseCompleteFlash.setVisible(false);

            noFlashCheckBox.setVisible(false);
            labelNoFlashHint.setVisible(false);

        } else if (selectedScenario.equals(bundle.getString("requestUid"))) {
            labelScenarioHint.setText(bundle.getString("requestUid"));

            buttonLoadFile.setVisible(false);
            textFieldFileName.setVisible(false);
            labelFileName.setVisible(false);
            labelFileNameHint.setVisible(false);

            buttonRequestUid.setVisible(true);

            labelUid.setVisible(true);
            textFieldUid.setVisible(true);
            labelUidHint.setVisible(true);

            labelMedium.setVisible(false);
            comboBoxMedium.setVisible(false);

            labelSerial.setVisible(false);
            textFieldSerial.setVisible(false);

            labelTpuart.setVisible(false);
            textFieldTpuart.setVisible(false);

            labelDeviceAddress.setVisible(false);
            textFieldDeviceAddress.setVisible(false);

            labelBootloaderDeviceAddr.setVisible(false);
            textFieldBootloaderDeviceAddress.setVisible(false);

            labelOwnAddress.setVisible(false);
            textFieldOwnAddress.setVisible(false);

            labelDelay.setVisible(false);
            textFieldDelay.setVisible(false);

            labelTimeout.setVisible(false);
            textFieldTimeout.setVisible(false);

            CheckBoxFullFlash.setVisible(false);
            labelFullFlashHint.setVisible(false);

            natCheckBox.setVisible(false);

            eraseCompleteFlashCheckBox.setVisible(false);
            labelEraseCompleteFlash.setVisible(false);

            noFlashCheckBox.setVisible(false);
            labelNoFlashHint.setVisible(false);

        } else if (selectedScenario.equals(bundle.getString("allOptions"))) {
            labelScenarioHint.setText(bundle.getString("allOptionsHint"));

            buttonLoadFile.setVisible(true);
            textFieldFileName.setVisible(true);
            labelFileName.setVisible(true);
            labelFileNameHint.setVisible(true);

            buttonRequestUid.setVisible(true);

            labelUid.setVisible(true);
            textFieldUid.setVisible(true);
            labelUidHint.setVisible(true);

            labelMedium.setVisible(true);
            comboBoxMedium.setVisible(true);

            labelSerial.setVisible(true);
            textFieldSerial.setVisible(true);

            labelTpuart.setVisible(true);
            textFieldTpuart.setVisible(true);

            labelDeviceAddress.setVisible(true);
            textFieldDeviceAddress.setVisible(true);

            labelBootloaderDeviceAddr.setVisible(true);
            textFieldBootloaderDeviceAddress.setVisible(true);

            labelOwnAddress.setVisible(true);
            textFieldOwnAddress.setVisible(true);

            labelDelay.setVisible(true);
            textFieldDelay.setVisible(true);

            labelTimeout.setVisible(true);
            textFieldTimeout.setVisible(true);

            CheckBoxFullFlash.setVisible(true);
            labelFullFlashHint.setVisible(true);

            natCheckBox.setVisible(true);

            eraseCompleteFlashCheckBox.setVisible(true);
            labelEraseCompleteFlash.setVisible(true);

            noFlashCheckBox.setVisible(true);
            labelNoFlashHint.setVisible(true);
        }
    }

    {
// GUI initializer generated by IntelliJ IDEA GUI Designer
// >>> IMPORTANT!! <<<
// DO NOT EDIT OR ADD ANY CODE HERE!
        $$$setupUI$$$();
    }

    /**
     * Method generated by IntelliJ IDEA GUI Designer
     * >>> IMPORTANT!! <<<
     * DO NOT edit this method OR call it in your code!
     *
     * @noinspection ALL
     */
    private void $$$setupUI$$$() {
        panelMain = new JPanel();
        panelMain.setLayout(new GridLayoutManager(22, 5, new Insets(0, 0, 0, 0), -1, -1));
        panelMain.setAutoscrolls(true);
        comboBoxIpGateways = new JComboBox();
        panelMain.add(comboBoxIpGateways, new GridConstraints(0, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelIpGateway = new JLabel();
        this.$$$loadLabelText$$$(labelIpGateway, this.$$$getMessageFromBundle$$$("GuiTranslation", "selectKnxIpGateway"));
        panelMain.add(labelIpGateway, new GridConstraints(0, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        final Spacer spacer1 = new Spacer();
        panelMain.add(spacer1, new GridConstraints(0, 4, 1, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, 1, null, new Dimension(5, -1), null, 0, false));
        buttonLoadFile = new JButton();
        this.$$$loadButtonText$$$(buttonLoadFile, this.$$$getMessageFromBundle$$$("GuiTranslation", "loadFile"));
        panelMain.add(buttonLoadFile, new GridConstraints(4, 2, 1, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        final JLabel label1 = new JLabel();
        this.$$$loadLabelText$$$(label1, this.$$$getMessageFromBundle$$$("GuiTranslation", "ipAddress"));
        panelMain.add(label1, new GridConstraints(1, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textBoxKnxGatewayIpAddr = new JTextField();
        panelMain.add(textBoxKnxGatewayIpAddr, new GridConstraints(1, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        textFieldUid = new JTextField();
        panelMain.add(textFieldUid, new GridConstraints(7, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelUid = new JLabel();
        this.$$$loadLabelText$$$(labelUid, this.$$$getMessageFromBundle$$$("GuiTranslation", "uid"));
        panelMain.add(labelUid, new GridConstraints(7, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        final Spacer spacer2 = new Spacer();
        panelMain.add(spacer2, new GridConstraints(0, 0, 1, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, 1, null, new Dimension(5, -1), null, 0, false));
        buttonStartStopFlash = new JButton();
        this.$$$loadButtonText$$$(buttonStartStopFlash, this.$$$getMessageFromBundle$$$("GuiTranslation", "startFlash"));
        panelMain.add(buttonStartStopFlash, new GridConstraints(20, 2, 1, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldFileName = new JTextField();
        panelMain.add(textFieldFileName, new GridConstraints(5, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelFileName = new JLabel();
        this.$$$loadLabelText$$$(labelFileName, this.$$$getMessageFromBundle$$$("GuiTranslation", "fileName"));
        panelMain.add(labelFileName, new GridConstraints(5, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        scrollPaneConsole = new JScrollPane();
        panelMain.add(scrollPaneConsole, new GridConstraints(21, 0, 1, 5, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_BOTH, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_WANT_GROW, null, null, null, 0, false));
        jLoggingPane = new JTextPane();
        jLoggingPane.setEditable(false);
        scrollPaneConsole.setViewportView(jLoggingPane);
        final JLabel label2 = new JLabel();
        this.$$$loadLabelText$$$(label2, this.$$$getMessageFromBundle$$$("GuiTranslation", "scenario"));
        panelMain.add(label2, new GridConstraints(3, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelMedium = new JLabel();
        this.$$$loadLabelText$$$(labelMedium, this.$$$getMessageFromBundle$$$("GuiTranslation", "medium"));
        panelMain.add(labelMedium, new GridConstraints(8, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        comboBoxMedium = new JComboBox();
        panelMain.add(comboBoxMedium, new GridConstraints(8, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelSerial = new JLabel();
        this.$$$loadLabelText$$$(labelSerial, this.$$$getMessageFromBundle$$$("GuiTranslation", "serial"));
        panelMain.add(labelSerial, new GridConstraints(9, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldSerial = new JTextField();
        textFieldSerial.setText("");
        panelMain.add(textFieldSerial, new GridConstraints(9, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelTpuart = new JLabel();
        this.$$$loadLabelText$$$(labelTpuart, this.$$$getMessageFromBundle$$$("GuiTranslation", "tpuart"));
        panelMain.add(labelTpuart, new GridConstraints(10, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelDeviceAddress = new JLabel();
        this.$$$loadLabelText$$$(labelDeviceAddress, this.$$$getMessageFromBundle$$$("GuiTranslation", "knxDeviceAddr"));
        panelMain.add(labelDeviceAddress, new GridConstraints(11, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldTpuart = new JTextField();
        panelMain.add(textFieldTpuart, new GridConstraints(10, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        textFieldDeviceAddress = new JTextField();
        panelMain.add(textFieldDeviceAddress, new GridConstraints(11, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelBootloaderDeviceAddr = new JLabel();
        this.$$$loadLabelText$$$(labelBootloaderDeviceAddr, this.$$$getMessageFromBundle$$$("GuiTranslation", "knxProgDeviceAddr"));
        panelMain.add(labelBootloaderDeviceAddr, new GridConstraints(12, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldBootloaderDeviceAddress = new JTextField();
        panelMain.add(textFieldBootloaderDeviceAddress, new GridConstraints(12, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelOwnAddress = new JLabel();
        this.$$$loadLabelText$$$(labelOwnAddress, this.$$$getMessageFromBundle$$$("GuiTranslation", "knxOwnAddress"));
        panelMain.add(labelOwnAddress, new GridConstraints(13, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        final JLabel label3 = new JLabel();
        this.$$$loadLabelText$$$(label3, this.$$$getMessageFromBundle$$$("GuiTranslation", "port"));
        panelMain.add(label3, new GridConstraints(2, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldPort = new JTextField();
        panelMain.add(textFieldPort, new GridConstraints(2, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        textFieldOwnAddress = new JTextField();
        panelMain.add(textFieldOwnAddress, new GridConstraints(13, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelDelay = new JLabel();
        this.$$$loadLabelText$$$(labelDelay, this.$$$getMessageFromBundle$$$("GuiTranslation", "knxMessageDelay"));
        panelMain.add(labelDelay, new GridConstraints(14, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        textFieldDelay = new JTextField();
        panelMain.add(textFieldDelay, new GridConstraints(14, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        textFieldTimeout = new JTextField();
        panelMain.add(textFieldTimeout, new GridConstraints(15, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_WANT_GROW, GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        labelTimeout = new JLabel();
        this.$$$loadLabelText$$$(labelTimeout, this.$$$getMessageFromBundle$$$("GuiTranslation", "knxTimeout"));
        panelMain.add(labelTimeout, new GridConstraints(15, 1, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        eraseCompleteFlashCheckBox = new JCheckBox();
        this.$$$loadButtonText$$$(eraseCompleteFlashCheckBox, this.$$$getMessageFromBundle$$$("GuiTranslation", "eraseFlash"));
        panelMain.add(eraseCompleteFlashCheckBox, new GridConstraints(18, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        noFlashCheckBox = new JCheckBox();
        this.$$$loadButtonText$$$(noFlashCheckBox, this.$$$getMessageFromBundle$$$("GuiTranslation", "noFlash"));
        panelMain.add(noFlashCheckBox, new GridConstraints(19, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        CheckBoxFullFlash = new JCheckBox();
        this.$$$loadButtonText$$$(CheckBoxFullFlash, this.$$$getMessageFromBundle$$$("GuiTranslation", "fullFlash"));
        panelMain.add(CheckBoxFullFlash, new GridConstraints(16, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        natCheckBox = new JCheckBox();
        this.$$$loadButtonText$$$(natCheckBox, this.$$$getMessageFromBundle$$$("GuiTranslation", "useNat"));
        panelMain.add(natCheckBox, new GridConstraints(17, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelPortHint = new JLabel();
        this.$$$loadLabelText$$$(labelPortHint, this.$$$getMessageFromBundle$$$("GuiTranslation", "portHint"));
        panelMain.add(labelPortHint, new GridConstraints(2, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelFileNameHint = new JLabel();
        this.$$$loadLabelText$$$(labelFileNameHint, this.$$$getMessageFromBundle$$$("GuiTranslation", "fileNameHint"));
        panelMain.add(labelFileNameHint, new GridConstraints(5, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelUidHint = new JLabel();
        this.$$$loadLabelText$$$(labelUidHint, this.$$$getMessageFromBundle$$$("GuiTranslation", "uidHint"));
        panelMain.add(labelUidHint, new GridConstraints(7, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelFullFlashHint = new JLabel();
        this.$$$loadLabelText$$$(labelFullFlashHint, this.$$$getMessageFromBundle$$$("GuiTranslation", "fullFlashHint"));
        panelMain.add(labelFullFlashHint, new GridConstraints(16, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelNoFlashHint = new JLabel();
        this.$$$loadLabelText$$$(labelNoFlashHint, this.$$$getMessageFromBundle$$$("GuiTranslation", "noFlashHint"));
        panelMain.add(labelNoFlashHint, new GridConstraints(19, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelEraseCompleteFlash = new JLabel();
        this.$$$loadLabelText$$$(labelEraseCompleteFlash, this.$$$getMessageFromBundle$$$("GuiTranslation", "eraseCompleteFlashHint"));
        panelMain.add(labelEraseCompleteFlash, new GridConstraints(18, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labeIIpHint = new JLabel();
        this.$$$loadLabelText$$$(labeIIpHint, this.$$$getMessageFromBundle$$$("GuiTranslation", "ipAddrHint"));
        panelMain.add(labeIIpHint, new GridConstraints(1, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        labelScenarioHint = new JLabel();
        labelScenarioHint.setText("Label");
        panelMain.add(labelScenarioHint, new GridConstraints(3, 3, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_NONE, GridConstraints.SIZEPOLICY_FIXED, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        comboBoxScenario = new JComboBox();
        final DefaultComboBoxModel defaultComboBoxModel1 = new DefaultComboBoxModel();
        comboBoxScenario.setModel(defaultComboBoxModel1);
        panelMain.add(comboBoxScenario, new GridConstraints(3, 2, 1, 1, GridConstraints.ANCHOR_WEST, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        buttonRequestUid = new JButton();
        this.$$$loadButtonText$$$(buttonRequestUid, this.$$$getMessageFromBundle$$$("GuiTranslation", "requestUid"));
        panelMain.add(buttonRequestUid, new GridConstraints(6, 2, 1, 1, GridConstraints.ANCHOR_CENTER, GridConstraints.FILL_HORIZONTAL, GridConstraints.SIZEPOLICY_CAN_SHRINK | GridConstraints.SIZEPOLICY_CAN_GROW, GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
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
