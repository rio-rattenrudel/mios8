/*
 * @(#)Knob.java	beta8	2006/04/23
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
 *
 ****************************************************************************
 * Based on com.dreamfabric.DKnob.java
 * (c) 2000 by Joakim Eriksson
 ****************************************************************************
 */

package org.midibox.utils.gui;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.geom.Arc2D;

import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.UIManager;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.EventListenerList;

public class Knob extends JComponent implements MouseListener,
		MouseMotionListener, KeyListener, FocusListener {

	public final static int LINEAR = 0;

	public final static int ROUND = 1;

	public static int mouseDragType = LINEAR;

	private Icon icon;

	private final static float START = 225;

	private final static float LENGTH = 270;

	private final static float START_ANG = (START / 360) * (float) Math.PI * 2;

	private final static float LENGTH_ANG = (LENGTH / 360) * (float) Math.PI
			* 2;

	private final static float LENGTH_ANG_DIV10 = (float) (LENGTH_ANG / 10.01);

	private final static float MULTIP = 180 / (float) Math.PI;

	private final static Color FOCUS_COLOR = UIManager.getColor("Slider.focus");

	private float DRAG_SPEED;

	private float CLICK_SPEED;

	private static Dimension PREF_SIZE;

	private int middle;

	private ChangeEvent changeEvent = null;

	private EventListenerList listenerList = new EventListenerList();

	private Arc2D hitArc = new Arc2D.Float(Arc2D.PIE);

	private float ang = START_ANG;

	private float minValue = 0f;

	private float maxValue = 1.0f;

	private float value;

	private int dragpos = -1;

	private float startVal;

	private double lastAng;

	private Color tickColor;

	public Knob(ImageIcon icon) {

		this.icon = icon;

		PREF_SIZE = new Dimension(icon.getIconHeight() + 8, icon
				.getIconHeight() + 8);
		DRAG_SPEED = 0.0075F;// 0.01F;
		CLICK_SPEED = 0.01F;

		hitArc.setAngleStart(235); // Degrees ??? Radians???
		hitArc.setFrame(4, 4, icon.getIconHeight(), icon.getIconHeight());
		middle = getHeight() / 2;

		addMouseListener(this);
		addMouseMotionListener(this);
		addKeyListener(this);
		addFocusListener(this);
	}

	public boolean isFocusable() {
		return true;
	}

	private void incValue() {
		setValue(value + CLICK_SPEED);
	}

	private void decValue() {
		setValue(value - CLICK_SPEED);
	}

	public float getMaxValue() {
		return maxValue;
	}

	public void setMaxValue(float maxValue) {
		this.maxValue = maxValue;
	}

	public float getMinValue() {
		return minValue;
	}

	public void setMinValue(float minValue) {
		this.minValue = minValue;
	}

	public float getValue() {
		return value;
	}

	public void setValue(float val) {
		if (val < 0)
			val = 0;
		if (val > 1)
			val = 1;
		float oldVal = this.value;
		this.value = val;
		ang = START_ANG - LENGTH_ANG * val;
		repaint();
		if (oldVal != this.value) {
			fireChangeEvent();
		}
	}

	public void addChangeListener(ChangeListener cl) {
		listenerList.add(ChangeListener.class, cl);
	}

	public void removeChangeListener(ChangeListener cl) {
		listenerList.remove(ChangeListener.class, cl);
	}

	public Dimension getMinimumSize() {
		return PREF_SIZE;
	}

	public Dimension getPreferredSize() {
		return PREF_SIZE;
	}

	public void setTickColor(Color tickColor) {
		this.tickColor = tickColor;
		repaint();
	}

	protected void fireChangeEvent() {
		Object[] listeners = listenerList.getListenerList();
		for (int i = listeners.length - 2; i >= 0; i -= 2) {
			if (listeners[i] == ChangeListener.class) {
				if (changeEvent == null)
					changeEvent = new ChangeEvent(this);
				((ChangeListener) listeners[i + 1]).stateChanged(changeEvent);
			}
		}
	}

	public void paint(Graphics g) {

		if (hasFocus()) {
			paintFocus(g);
		}

		paintTicks(g);
		paintKnob(g);
	}

	public void paintFocus(Graphics g) {

		g.setColor(FOCUS_COLOR);
		Graphics2D g2d = (Graphics2D) g;

		g2d.setStroke(new BasicStroke(1f, BasicStroke.CAP_BUTT,
				BasicStroke.JOIN_ROUND, 1f, new float[] { 1f }, 0f));

		int width = getWidth();
		int height = getHeight();

		g2d.drawLine(0, 0, width, 0);
		g2d.drawLine(width - 1, 0, width - 1, height);
		g2d.drawLine(0, height - 1, width, height - 1);
		g2d.drawLine(0, 0, 0, height);

		g2d.setStroke(new BasicStroke());
		g2d.setColor(Color.BLACK);
	}

	public void paintTicks(Graphics g) {
		g.setColor(tickColor);

		int offsetX = ((getWidth()) - icon.getIconHeight()) / 2;
		int offsetY = ((getHeight()) - icon.getIconHeight()) / 2;

		for (float a2 = START_ANG; a2 >= START_ANG - LENGTH_ANG; a2 = a2
				- LENGTH_ANG_DIV10) {
			int x = offsetX + (icon.getIconHeight() / 2)
					+ (int) ((2 + (icon.getIconHeight() / 2)) * Math.cos(a2));
			int y = offsetY + (icon.getIconHeight() / 2)
					- (int) ((2 + (icon.getIconHeight() / 2)) * Math.sin(a2));
			g.drawLine(x, y, x, y);
		}
	}

	public void paintKnob(Graphics g) {

		Graphics2D g2d = (Graphics2D) g;

		int offsetX = ((getWidth()) - icon.getIconHeight()) / 2;
		int offsetY = ((getHeight()) - icon.getIconHeight()) / 2;
		int noImages = (icon.getIconWidth() / icon.getIconHeight()) - 1;

		if (icon != null) {

			g2d.clipRect(offsetX, offsetY, icon.getIconHeight(), icon
					.getIconHeight());
			offsetX -= icon.getIconHeight() * ((int) (value * noImages));
			icon.paintIcon(this, g2d, offsetX, offsetY);
			g2d.dispose();
		}
	}

	public void mousePressed(MouseEvent me) {

		dragpos = me.getX() - me.getY();
		startVal = value;

		// Fix last angle
		int xpos = middle - me.getX();
		int ypos = middle - me.getY();
		lastAng = Math.atan2(xpos, ypos);
		requestFocus();
	}

	public void mouseReleased(MouseEvent me) {
	}

	public void mouseClicked(MouseEvent me) {
		hitArc.setAngleExtent(-(LENGTH + 20));
		if (hitArc.contains(me.getX(), me.getY())) {
			hitArc.setAngleExtent(MULTIP * (ang - START_ANG) - 10);
			if (hitArc.contains(me.getX(), me.getY())) {
				decValue();
			} else
				incValue();
		}
	}

	public void mouseEntered(MouseEvent me) {
	}

	public void mouseExited(MouseEvent me) {
	}

	public void mouseMoved(MouseEvent me) {
	}

	public void mouseDragged(MouseEvent me) {
		if (mouseDragType == LINEAR) {
			float f = DRAG_SPEED * (float) ((me.getX() - me.getY()) - dragpos);
			setValue(startVal + f);
		}

		else if (mouseDragType == ROUND) {
			int xpos = middle - me.getX();
			int ypos = middle - me.getY();
			double ang = Math.atan2(xpos, ypos);
			double diff = lastAng - ang;

			setValue((float) (value + (diff / LENGTH_ANG)));

			lastAng = ang;
		}
	}

	public void keyPressed(KeyEvent e) {
		int k = e.getKeyCode();
		if (k == KeyEvent.VK_RIGHT)
			incValue();
		else if (k == KeyEvent.VK_LEFT)
			decValue();
	}

	public void keyReleased(KeyEvent e) {
	}

	public void keyTyped(KeyEvent e) {
	}

	public void focusGained(FocusEvent e) {
		repaint();
	}

	public void focusLost(FocusEvent e) {
		repaint();
	}
}