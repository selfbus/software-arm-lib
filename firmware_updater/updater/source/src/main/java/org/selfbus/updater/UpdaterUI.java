package org.selfbus.updater;

import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.UIManager;
import javax.swing.filechooser.FileNameExtensionFilter;

import java.awt.Color;
import java.awt.SystemColor;
import java.awt.CardLayout;
import javax.swing.JButton;
import javax.swing.JFileChooser;

import java.awt.event.ActionListener;
import java.io.File;
import java.awt.event.ActionEvent;
import javax.swing.JLabel;
import java.awt.Font;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;

public class UpdaterUI {
	private static final String DEFAULT_DIRECTORY = "D://busupdater//configs";
	
	private JFrame frmSelfbusFirmwareUpdater;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
					UpdaterUI window = new UpdaterUI();
					window.frmSelfbusFirmwareUpdater.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public UpdaterUI() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frmSelfbusFirmwareUpdater = new JFrame();
		frmSelfbusFirmwareUpdater.setTitle("Selfbus Firmware Updater v%s");
		frmSelfbusFirmwareUpdater.setBounds(100, 100, 640, 480);
		frmSelfbusFirmwareUpdater.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frmSelfbusFirmwareUpdater.getContentPane().setLayout(null);
		
		JPanel topPanel = new JPanel();
		topPanel.setBounds(0, 0, 624, 33);
		topPanel.setBackground(Color.PINK);
		
		JButton btn1 = new JButton("1");
		topPanel.add(btn1);
		
		JButton btn2 = new JButton("2");
		topPanel.add(btn2);
		
		JButton btn3 = new JButton("3");
		topPanel.add(btn3);
		frmSelfbusFirmwareUpdater.getContentPane().add(topPanel);
		
		JPanel bottomPanel = new JPanel();
		bottomPanel.setBounds(0, 32, 624, 409);
		bottomPanel.setBackground(SystemColor.info);
		bottomPanel.setLayout(new CardLayout(0, 0));
		frmSelfbusFirmwareUpdater.getContentPane().add(bottomPanel);
		
		JPanel panel1 = new JPanel();
		panel1.setBackground(new Color(255, 102, 0));
		bottomPanel.add(panel1, "name_10216420690100");
		
		JLabel lbl1 = new JLabel("1");
		panel1.add(lbl1);
		
		JPanel panel2 = new JPanel();
		panel2.setBackground(new Color(255, 255, 0));
		bottomPanel.add(panel2, "name_10238040635700");
		
		JLabel lbl2 = new JLabel("2");
		panel2.add(lbl2);
		
		JPanel panel3 = new JPanel();
		panel3.setBackground(new Color(51, 204, 51));
		bottomPanel.add(panel3, "name_10239975245300");
		
		JLabel lbl3 = new JLabel("3");
		lbl3.setFont(new Font("Dialog", Font.BOLD, 60));
		panel3.add(lbl3);
		
		
		// ACTIONS
		btn1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
		panel1.setVisible(true);
		panel2.setVisible(false);
		panel3.setVisible(false);
		lbl1.setText(selectFirmwareFile(DEFAULT_DIRECTORY));
			}
		});
		
		btn2.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				panel1.setVisible(false);
				panel2.setVisible(true);
				panel3.setVisible(false);
			}
		});
		
		btn3.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				panel1.setVisible(false);
				panel2.setVisible(false);
				panel3.setVisible(true);
			}
		});
		
		frmSelfbusFirmwareUpdater.setTitle(String.format(frmSelfbusFirmwareUpdater.getTitle(), ToolInfo.getVersion()));
		frmSelfbusFirmwareUpdater.setLocationRelativeTo(null);
	}
	
    private String selectFirmwareFile(String initialDirectory) {
        JFileChooser fileDlg = new JFileChooser();
        fileDlg.setCurrentDirectory(new File(initialDirectory));
        fileDlg.setFileFilter(new FileNameExtensionFilter("Firmware Files (*.hex)", "hex"));
        int result = fileDlg.showOpenDialog(null);
        if (result != JFileChooser.APPROVE_OPTION) {
            return "";
        }
        return fileDlg.getSelectedFile().getAbsolutePath();
    }
}
