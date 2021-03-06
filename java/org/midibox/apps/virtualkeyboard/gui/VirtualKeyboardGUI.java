/*
 * @(#)VirtualKeyboardGUI.java	beta8	2006/04/23
 *
 * Copyright (C) 2008    Adam King (adamjking@optusnet.com.au)
 *
 * This application is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this application; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package org.midibox.apps.virtualkeyboard.gui;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.UIManager;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;

import org.midibox.apps.virtualkeyboard.VirtualKeyboard;
import org.midibox.midi.VirtualMidiDevice;
import org.midibox.midi.gui.MidiDeviceRoutingGUI;
import org.midibox.midi.gui.MidiKeyboardControllerGUI;
import org.midibox.utils.gui.ImageLoader;

public class VirtualKeyboardGUI extends JPanel {

	private MidiKeyboardControllerGUI midiKeyboardControllerGUI;

	private MidiDeviceRoutingGUI midiDeviceRoutingGUI;

	private JMenu fileMenu;

	private boolean showConnections;

	private JCheckBoxMenuItem showConnectionsItem;

	private JMenu lookAndFeelMenu;

	private String lookAndFeel;

	protected VirtualKeyboard virtualKeyboard;

	public VirtualKeyboardGUI(VirtualKeyboard virtualKeyboard) {

		super(new GridBagLayout());

		this.virtualKeyboard = virtualKeyboard;

		lookAndFeel = UIManager.getLookAndFeel().getClass().getName();

		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridheight = 1;
		gbc.gridwidth = 1;
		gbc.fill = GridBagConstraints.BOTH;
		gbc.weightx = 1;
		gbc.weighty = 1;

		midiDeviceRoutingGUI = new MidiDeviceRoutingGUI(virtualKeyboard
				.getMidiDeviceRouting());

		midiDeviceRoutingGUI.setBorder(BorderFactory.createEtchedBorder());

		midiDeviceRoutingGUI.addMidiDeviceIcon(VirtualMidiDevice.class,
				ImageLoader.getImageIcon("virtualMidiDevice.png"));

		add(midiDeviceRoutingGUI, gbc);

		gbc.gridy++;

		gbc.fill = GridBagConstraints.NONE;
		gbc.weightx = 0;
		gbc.weighty = 0;

		midiKeyboardControllerGUI = new MidiKeyboardControllerGUI(
				virtualKeyboard.getMidiKeyboardController());

		midiKeyboardControllerGUI.setBorder(BorderFactory.createEtchedBorder());

		add(midiKeyboardControllerGUI, gbc);
	}

	public JMenuBar createMenuBar() {
		JMenuBar menuBar = new JMenuBar();

		fileMenu = new JMenu("File");

		menuBar.add(fileMenu);

		JMenu optionsMenu = new JMenu("Options");

		showConnectionsItem = new JCheckBoxMenuItem("Show Connections",
				isShowConnections());

		showConnectionsItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				setShowConnections(showConnectionsItem.isSelected());
			}
		});

		optionsMenu.add(showConnectionsItem);

		lookAndFeelMenu = new JMenu("Look & Feel");

		lookAndFeelMenu.addMenuListener(new MenuListener() {

			public void menuCanceled(MenuEvent arg0) {

			}

			public void menuDeselected(MenuEvent e) {
				lookAndFeelMenu.removeAll();
			}

			public void menuSelected(MenuEvent e) {
				buildLookAndFeel();
			}
		});

		optionsMenu.add(lookAndFeelMenu);
		menuBar.add(optionsMenu);

		return menuBar;
	}

	private void buildLookAndFeel() {

		JCheckBoxMenuItem item;

		for (int i = 0; i < UIManager.getInstalledLookAndFeels().length; i++) {
			final int l = i;
			item = new JCheckBoxMenuItem(
					UIManager.getInstalledLookAndFeels()[i].getName());
			lookAndFeelMenu.add(item);
			if (UIManager.getInstalledLookAndFeels()[i].getClassName().equals(
					lookAndFeel)) {
				item.setSelected(true);
			}
			item.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ae) {
					try {

						setLookAndFeel(UIManager.getInstalledLookAndFeels()[l]
								.getClassName());

					} catch (Exception e) {
						System.out.println(e.toString());
					}
				}
			});
		}
	}

	public void setLookAndFeel(String lookAndFeel) {

		this.lookAndFeel = lookAndFeel;

		if (!lookAndFeel
				.equals(UIManager.getLookAndFeel().getClass().getName())) {

			JOptionPane
					.showMessageDialog(
							VirtualKeyboardGUI.this,
							"The selected Look & Feel will be applied the next time you restart Virtual Keyboard",
							"ALERT", JOptionPane.ERROR_MESSAGE);
		}
	}

	public VirtualKeyboard getVirtualKeyboard() {
		return virtualKeyboard;
	}

	public boolean isShowConnections() {
		return showConnections;
	}

	public void setShowConnections(boolean showConnections) {
		this.showConnections = showConnections;
		midiDeviceRoutingGUI.setVisible(showConnections);
		revalidate();
	}

	public MidiDeviceRoutingGUI getMidiDeviceRoutingGUI() {
		return midiDeviceRoutingGUI;
	}

	public JMenu getFileMenu() {
		return fileMenu;
	}

	public String getLookAndFeel() {
		return lookAndFeel;
	}
}
