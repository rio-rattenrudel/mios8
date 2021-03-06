/*
 * @(#)SIDV2librarian.java	beta1	2008/01/21
 *
 * Copyright (C) 2008    Rutger Vlek (rutgervlek@hotmail.com)
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

package org.midibox.sidlibr.gui;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.EventObject;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.KeyStroke;
import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.TableModel;

import org.midibox.sidlibr.Bank;
import org.midibox.sidlibr.SIDLibController;

public class BankTable extends JPanel implements TableModelListener,
		ListSelectionListener, Observer, MouseListener, ActionListener,
		KeyListener {
	private SIDLibController sidLibController;
	JTable table;
	JPopupMenu popupMenu;
	JMenuItem m1, m2, m3, m4, m5;
	int bankNumber;
	int selectedBeforeDrag = 0;

	public BankTable(SIDLibController sidLibController, int bankNumber) {
		this.sidLibController = sidLibController;
		this.bankNumber = bankNumber;
		sidLibController.addObserver(this);

		createPopupMenu();

		table = new JTable(new BankTableModel(sidLibController
				.getBank(bankNumber)));
		table.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
		table.getModel().addTableModelListener(this);
		table.addMouseListener(this);
		table.addKeyListener(this);
		table.getSelectionModel().addListSelectionListener(this);
		table.getColumnModel().getColumn(1)
				.setCellEditor(new PatchNameEditor());
		table.getColumnModel().getColumn(0).setPreferredWidth(30);
		if (table.getColumnModel().getColumnCount() == 3) {
			table.getColumnModel().getColumn(1).setPreferredWidth(170);
			table.getColumnModel().getColumn(2).setPreferredWidth(70);
		} else {
			table.getColumnModel().getColumn(1).setPreferredWidth(240);
		}

		table.setPreferredScrollableViewportSize(new Dimension(table
				.getPreferredSize().width, Math
				.round(table.getPreferredSize().height / 4)));

		JScrollPane scrollPane = new JScrollPane(table);
		scrollPane.setPreferredSize(new Dimension(
				table.getPreferredSize().width, Math.round(table
						.getPreferredSize().height / 4)));

		add(scrollPane);
		resetSelection();
	}

	public void resetSelection() {
		table.getSelectionModel().setSelectionInterval(0, 0);
	}

	public void tableChanged(TableModelEvent e) {
		int row = e.getFirstRow();
		int column = e.getColumn();
		TableModel model = (TableModel) e.getSource();
		Object data = model.getValueAt(row, column);
		if ((column == 1)
				&& !(sidLibController.getBank(bankNumber).getPatchAt(row)
						.getPatchName().equals((String) data))) {
			sidLibController.getBank(bankNumber).getPatchAt(row).setPatchName(
					(String) data);
		}
	}

	public void update(Observable observable, Object object) {
		if (object == "Data changed") {
			Bank b = sidLibController.getBank(bankNumber);
			TableModel m = table.getModel();
			if (!b.isEnsembleBank()) {
				for (int c = 0; c < b.bankSize; c++) {
					m.setValueAt(b.getPatchAt(c).getPatchName(), c, 1);
					m.setValueAt(b.getPatchAt(c).getEngineStr(), c, 2);
				}
			}
		}
		if (object == "Rename") {
			table.editCellAt(table.getSelectedRow(), 1, new EventObject(this));
		}
	}

	public int[] getSelectedRows() {
		return table.getSelectedRows();
	}

	private void maybeShowPopup(MouseEvent e) {

		if (e.getButton() == MouseEvent.BUTTON3) {

			JTable source = (JTable) e.getSource();

			int index = source.rowAtPoint(e.getPoint());

			int[] selectedRows = source.getSelectedRows();

			boolean rowSelected = false;

			for (int r = 0; r < selectedRows.length; r++) {
				if (selectedRows[r] == index) {
					rowSelected = true;
					break;
				}
			}

			if (!rowSelected) {
				source.setRowSelectionInterval(index, index);
			}
		}

		if (e.isPopupTrigger()) {
			if (sidLibController.getBank(bankNumber).isEnsembleBank()) {
				m1.setEnabled(false);
				m2.setEnabled(false);
				m3.setEnabled(false);
				m4.setEnabled(false);
				m5.setEnabled(true);
			} else {
				m1.setEnabled(true);
				m2.setEnabled(true);
				m3.setEnabled(true);
				m4.setEnabled(true);
				m5.setEnabled(false);
			}
			popupMenu.show(e.getComponent(), e.getX(), e.getY());
		}
	}

	public void mousePressed(MouseEvent e) {
		if (e.getButton() == MouseEvent.BUTTON1 && e.isControlDown()) {
			// selectedBeforeDrag = table.getSelectedRow();
		}

		maybeShowPopup(e);
	}

	public void mouseReleased(MouseEvent e) {
		if (e.getButton() == MouseEvent.BUTTON1 && e.isControlDown()) {
			if (table.getSelectedRow() != selectedBeforeDrag) {
				// sidLibController.editSwap(selectedBeforeDrag,
				// table.getSelectedRow());
			}
		}
		maybeShowPopup(e);
	}

	public void mouseEntered(MouseEvent e) {
	}

	public void mouseExited(MouseEvent e) {
	}

	public void mouseClicked(MouseEvent e) {
		if ((e.getButton() == MouseEvent.BUTTON1) && (e.getClickCount() == 2)) {
			sidLibController.editCurrentPatch();
		}
	}

	private void createPopupMenu() {
		popupMenu = new JPopupMenu();
		JMenu submenu;
		JMenuItem menuItem;

		menuItem = new JMenuItem("Edit", KeyEvent.VK_E);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_E,
				ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Edit");
		menuItem.addActionListener(this);
		popupMenu.add(menuItem);

		menuItem = new JMenuItem("Rename", KeyEvent.VK_R);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_R,
				ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Rename");
		menuItem.addActionListener(this);
		popupMenu.add(menuItem);

		popupMenu.addSeparator();

		menuItem = new JMenuItem("Cut", KeyEvent.VK_U);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X,
				ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Cut");
		menuItem.addActionListener(this);
		popupMenu.add(menuItem);

		menuItem = new JMenuItem("Copy", KeyEvent.VK_C);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C,
				ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Copy");
		menuItem.addActionListener(this);
		popupMenu.add(menuItem);

		menuItem = new JMenuItem("Paste", KeyEvent.VK_P);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V,
				ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Paste");
		menuItem.addActionListener(this);
		popupMenu.add(menuItem);

		popupMenu.addSeparator();

		submenu = new JMenu("Init current patch");
		submenu.setFont(submenu.getFont().deriveFont(Font.PLAIN));
		submenu.setMnemonic(KeyEvent.VK_I);
		popupMenu.add(submenu);

		m1 = new JMenuItem("LEAD engine", KeyEvent.VK_P);
		m1.setFont(m1.getFont().deriveFont(Font.PLAIN));
		m1.setActionCommand("Init LEAD patch");
		m1.addActionListener(this);
		submenu.add(m1);

		m2 = new JMenuItem("BASSLINE engine", KeyEvent.VK_P);
		m2.setFont(m2.getFont().deriveFont(Font.PLAIN));
		m2.setActionCommand("Init BASSLINE patch");
		m2.addActionListener(this);
		submenu.add(m2);

		m3 = new JMenuItem("DRUM engine", KeyEvent.VK_P);
		m3.setFont(m3.getFont().deriveFont(Font.PLAIN));
		m3.setActionCommand("Init DRUM patch");
		m3.addActionListener(this);
		submenu.add(m3);

		m4 = new JMenuItem("MULTI engine", KeyEvent.VK_P);
		m4.setFont(m4.getFont().deriveFont(Font.PLAIN));
		m4.setActionCommand("Init MULTI patch");
		m4.addActionListener(this);
		submenu.add(m4);

		m5 = new JMenuItem("ENSEMBLE", KeyEvent.VK_S);
		m5.setFont(m5.getFont().deriveFont(Font.PLAIN));
		m5.setActionCommand("Init ensemble");
		m5.addActionListener(this);
		submenu.add(m5);

		menuItem = new JMenuItem("Init current bank", KeyEvent.VK_N);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setActionCommand("Init current bank");
		menuItem.addActionListener(this);
		popupMenu.add(menuItem);

		popupMenu.addSeparator();

		submenu = new JMenu("Transmit");
		submenu.setFont(submenu.getFont().deriveFont(Font.PLAIN));
		submenu.setMnemonic(KeyEvent.VK_I);
		popupMenu.add(submenu);

		// Transmit menu items:
		menuItem = new JMenuItem("Patch to buffer", KeyEvent.VK_B);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setActionCommand("Transmit patch to buffer");
		menuItem.addActionListener(this);
		submenu.add(menuItem);

		menuItem = new JMenuItem("Patch to memory", KeyEvent.VK_M);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setActionCommand("Transmit patch to memory");
		menuItem.addActionListener(this);
		submenu.add(menuItem);

		submenu.addSeparator();

		menuItem = new JMenuItem("Bank", KeyEvent.VK_A);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setActionCommand("Transmit bank to memory");
		menuItem.addActionListener(this);
		submenu.add(menuItem);

		submenu = new JMenu("Receive");
		submenu.setFont(submenu.getFont().deriveFont(Font.PLAIN));
		submenu.setMnemonic(KeyEvent.VK_I);
		popupMenu.add(submenu);

		menuItem = new JMenuItem("Patch from buffer", KeyEvent.VK_B);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setActionCommand("Receive patch from buffer");
		menuItem.addActionListener(this);
		submenu.add(menuItem);

		menuItem = new JMenuItem("Patch from memory", KeyEvent.VK_M);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setActionCommand("Receive patch from memory");
		menuItem.addActionListener(this);
		submenu.add(menuItem);

		submenu.addSeparator();

		menuItem = new JMenuItem("Bank", KeyEvent.VK_A);
		menuItem.setFont(menuItem.getFont().deriveFont(Font.PLAIN));
		menuItem.setActionCommand("Receive bank from memory");
		menuItem.addActionListener(this);
		submenu.add(menuItem);
	}

	public void actionPerformed(ActionEvent ae) {
		sidLibController.actionPerformed(ae);
	}

	public void valueChanged(ListSelectionEvent event) {
		int[] patchNumber = table.getSelectedRows();
		if (patchNumber.length == 0) {
			resetSelection();
			patchNumber = new int[] { 0 };
		}
		sidLibController.setCurrentPatchNumber(patchNumber);
	}

	public void keyPressed(KeyEvent e) {
		if ((e.getKeyCode() == KeyEvent.VK_E)
				&& (e.getModifiers() == ActionEvent.CTRL_MASK)) {
			sidLibController.editCurrentPatch();
		} else if ((e.getKeyCode() == KeyEvent.VK_R)
				&& (e.getModifiers() == ActionEvent.CTRL_MASK)) {
			sidLibController.editRename();
		} else if ((e.getKeyCode() == KeyEvent.VK_X)
				&& (e.getModifiers() == ActionEvent.CTRL_MASK)) {
			sidLibController.editCut();
		} else if ((e.getKeyCode() == KeyEvent.VK_C)
				&& (e.getModifiers() == ActionEvent.CTRL_MASK)) {
			sidLibController.editCopy();
		} else if ((e.getKeyCode() == KeyEvent.VK_V)
				&& (e.getModifiers() == ActionEvent.CTRL_MASK)) {
			sidLibController.editPaste();
		}
	}

	public void keyReleased(KeyEvent e) {

	}

	public void keyTyped(KeyEvent e) {

	}

}
