/*
 * @(#)MidiParameterControlSlider.java	beta8	2006/04/23
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

package org.midibox.midi.gui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.util.Observable;

import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.midibox.midi.MidiParameterControl;

public class MidiParameterControlSlider extends MidiParameterControlGUI
		implements ChangeListener, MouseWheelListener {

	public static float mouseWheelResolution = 0.1f;

	private JSlider slider;

	public MidiParameterControlSlider(MidiParameterControl midiParameter,
			JSlider slider, boolean showLabel, String labelLocation,
			boolean valueBelow, boolean showValue) {
		super(midiParameter, showLabel, labelLocation, valueBelow, showValue);
		this.slider = slider;

		slider.setMinimum(midiParameter.getMidiMinValue());
		slider.setMaximum(midiParameter.getMidiMaxValue());

		JPanel panel = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
		panel.setOpaque(false);
		panel.add(slider);

		add(panel, BorderLayout.CENTER);
		slider.addChangeListener(this);
		slider.addMouseWheelListener(this);
		slider.addMouseListener(this);

		updateGraphics();
	}

	public void mouseWheelMoved(MouseWheelEvent mwe) {
		slider
				.setValue((int) (slider.getValue() - ((mwe.getWheelRotation() * Math
						.max(mouseWheelResolution
								* (slider.getMaximum() - slider.getMinimum()),
								1)))));
	}

	public void stateChanged(ChangeEvent ce) {

		if (update) {

			update = false;

			midiParameterControl.setMidiValue(slider.getValue(), true);

			update = true;
		}
	}

	public void updateGraphics() {

		super.updateGraphics();

		if (update) {

			update = false;

			slider.setValue(midiParameterControl.getMidiValue());

			update = true;
		}
	}

	public void update(Observable observable, Object object) {
		super.update(observable, object);

		if (object == midiParameterControl.HIGH_RESOLUTION) {

			slider.setMinimum(midiParameterControl.getMidiMinValue());
			slider.setMaximum(midiParameterControl.getMidiMaxValue());
		}
	}
}