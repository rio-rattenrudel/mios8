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

package org.midibox.sidlibr;

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;

import org.midibox.sidedit.SIDSysexInfo;

public class Patch implements Receiver {
	public static Object LEAD = new Object();
	public static Object BASSLINE = new Object();
	public static Object DRUM = new Object();
	public static Object MULTI = new Object();
	private static int delay = 10; // Delay in ms between Sysex Strings
	private int patchSize;
	private boolean isEnsemble;
	protected int[] patch;
	private int WOPT = 0;
	private int maxSysExLength = 0;
	protected Receiver receiver;

	// TK: for quick & dirty initialisation
	// presets are based on
	// $MIOS_PATH/apps/synthesizers/midibox_sid_v2/src/sid_preset_*.inc
	final int[] defaultLeadPatch = new int[] { 0x4C, 0x65, 0x61, 0x64, 0x20,
			0x50, 0x61, 0x74, 0x63, 0x68, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
			0x00, 0xff, 0x00, 0x00, 0x80, 0x00, 0xff, 0x00, 0x00, 0x80, 0x00,
			0xff, 0x00, 0x00, 0x80, 0x00, 0xff, 0x00, 0x00, 0x80, 0x00, 0xff,
			0x00, 0x00, 0x80, 0x00, 0xff, 0x50, 0x00, 0x80, 0x00, 0xff, 0x00,
			0x00, 0x80, 0x00, 0xff, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
			0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00,
			0x7f, 0x00, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x02,
			0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xf0, 0x00, 0x08, 0x00, 0x00,
			0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00, 0x00, 0x14, 0x00,
			0xf0, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f,
			0x05, 0x00, 0x00, 0x14, 0x00, 0xf0, 0x00, 0x08, 0x00, 0x00, 0x40,
			0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00, 0x00, 0x04, 0x00, 0xf0,
			0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05,
			0x00, 0x00, 0x14, 0x00, 0xf0, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80,
			0x02, 0x00, 0x00, 0x1f, 0x05, 0x00, 0x00, 0x14, 0x00, 0xf0, 0x00,
			0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00,
			0x01, 0x80, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x01,
			0x80, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x01, 0x80,
			0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0, 0, 0x00, 0xc0,
			0x00, 0x30, 0xff, 0x00, 0x00, 0xff, 0x30, 0x40, 0x20, 0x00, 0x00,
			0x80, 0x80, 0x80, 0x00, 0x40, 0x00, 0x50, 0xff, 0x00, 0x00, 0xff,
			0x50, 0x10, 0x30, 0x00, 0x00, 0x80, 0x80, 0x80, 0x01, 0x00, 0x03,
			0xc0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0xc0, 0x00, 0x00,
			0x00, 0x00, 0x03, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x04,
			0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0xc0,
			0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00,
			0x00, 0x07, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
			0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x0f, 0x3f, 0x03,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0, 0, 0xcb, 0x00, 0x00, 0x1f, 0x80, 0xcb,
			0x00, 0x20, 0x3f, 0xa0, 0xcb, 0x00, 0x40, 0x5f, 0xc0, 0xcb, 0x00,
			0x60, 0x7f, 0xe0, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, };

	final int[] defaultBasslinePatch = new int[] { 0x42, 0x61, 0x73, 0x73,
			0x6C, 0x69, 0x6E, 0x65, 0x20, 0x50, 0x61, 0x74, 0x63, 0x68, 0x20,
			0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0x00,
			0x08, 0x00, 0xff, 0x65, 0x00, 0xff, 0x00, 0xff, 0x69, 0x00, 0x60,
			0x00, 0xff, 0x6d, 0x00, 0x30, 0x00, 0xff, 0x71, 0x00, 0x20, 0x00,
			0xff, 0x00, 0x00, 0x80, 0x00, 0xff, 0x51, 0x00, 0x80, 0x00, 0xff,
			0x00, 0x00, 0x80, 0x00, 0xff, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80,
			0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
			0x00, 0x7f, 0x00, 0x17, 0x08, 0x00, 0xf0, 0x00, 0x00, 0x17, 0x08,
			0x00, 0xf0, 0x00, 0x00, 0x01, 0x04, 0x00, 0xc0, 0x00, 0x09, 0x10,
			0x00, 0x28, 0x80, 0x02, 0x28, 0x00, 0x1f, 0x05, 0x00, 0x07, 0x00,
			0x00, 0x00, 0x03, 0x80, 0x06, 0x00, 0x00, 0x80, 0x80, 0x03, 0x80,
			0x16, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0xa0, 0x04, 0x38,
			0x20, 0x10, 0x80, 0x03, 0x00, 0x0f, 0x00, 0x00, 0x28, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0xc0,
			0x00, 0x09, 0x10, 0x00, 0x28, 0x80, 0x02, 0x28, 0x00, 0x1f, 0x05,
			0x00, 0x07, 0x00, 0x00, 0x00, 0x03, 0x80, 0x06, 0x00, 0x00, 0x80,
			0x80, 0x03, 0x80, 0x16, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80,
			0xa0, 0x04, 0x38, 0x20, 0x10, 0x80, 0x03, 0x08, 0x0f, 0x00, 0x00,
			0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, };

	final int[] defaultDrumPatch = new int[] { 0x44, 0x72, 0x75, 0x6D, 0x20,
			0x4B, 0x69, 0x74, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
			0x00, 0xff, 0x00, 0x00, 0x80, 0x00, 0xff, 0x00, 0x00, 0x80, 0x00,
			0xff, 0x00, 0x00, 0x80, 0x00, 0xff, 0x00, 0x00, 0x80, 0x00, 0xff,
			0x00, 0x00, 0x80, 0x00, 0xff, 0x00, 0x00, 0x80, 0x00, 0xff, 0x00,
			0x00, 0x80, 0x00, 0xff, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
			0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x03, 0x08,
			0x7f, 0x0f, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x02,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xc9, 0x80, 0x80, 0x80, 0x80,
			0x00, 0x00, 0x00, 0x01, 0x09, 0xc9, 0x80, 0x80, 0x80, 0x80, 0x00,
			0x00, 0x00, 0x02, 0x09, 0xc9, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
			0x00, 0x03, 0x09, 0xc9, 0xff, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00,
			0x04, 0x09, 0xc9, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x05,
			0x09, 0xc9, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x06, 0x09,
			0xc9, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x07, 0x09, 0xc9,
			0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x08, 0x09, 0xc9, 0x80,
			0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x09, 0x09, 0xc9, 0x80, 0x80,
			0x80, 0x80, 0x00, 0x00, 0x00, 0x0a, 0x09, 0xc9, 0x80, 0x80, 0x80,
			0x80, 0x00, 0x00, 0x00, 0x0b, 0x09, 0xc9, 0x80, 0x80, 0x80, 0x80,
			0x00, 0x00, 0x00, 0x0c, 0x09, 0xc9, 0x80, 0x80, 0x80, 0x80, 0x00,
			0x00, 0x00, 0x0d, 0x09, 0xc9, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
			0x00, 0x0e, 0x09, 0xc9, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00,
			0x0f, 0x09, 0xc9, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x11, 0x00,
			0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x11,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x11, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x11, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x11, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x11, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x11, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, };

	final int[] defaultMultiPatch = new int[] { 0x4D, 0x75, 0x6C, 0x74, 0x69,
			0x20, 0x50, 0x61, 0x74, 0x63, 0x68, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
			0x00, 0xff, 0x00, 0x00, 0x80, 0x00, 0xff, 0x00, 0x00, 0x80, 0x00,
			0xff, 0x00, 0x00, 0x80, 0x00, 0xff, 0x00, 0x00, 0x80, 0x00, 0xff,
			0x00, 0x00, 0x80, 0x00, 0xff, 0x00, 0x00, 0x80, 0x00, 0xff, 0x00,
			0x00, 0x80, 0x00, 0xff, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
			0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00,
			0x7f, 0x00, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x02,
			0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xfa, 0x00, 0x08, 0x00, 0x00,
			0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00, 0x08, 0x00, 0x00,
			0x79, 0x01, 0x80, 0x06, 0x00, 0x00, 0xc0, 0x80, 0x01, 0x80, 0x16,
			0x00, 0x00, 0x90, 0x80, 0x00, 0x80, 0x80, 0x80, 0x30, 0x30, 0x40,
			0x20, 0x80, 0xc7, 0x00, 0x00, 0x0f, 0xd0, 0x00, 0x04, 0x00, 0xfa,
			0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05,
			0x00, 0x08, 0x00, 0x00, 0x79, 0x01, 0x80, 0x06, 0x00, 0x00, 0xc0,
			0x80, 0x01, 0x80, 0x16, 0x00, 0x00, 0x90, 0x80, 0x00, 0x80, 0x80,
			0x80, 0x30, 0x30, 0x40, 0x20, 0x80, 0xc7, 0x00, 0x10, 0x1f, 0xd0,
			0x00, 0x04, 0x00, 0xfa, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02,
			0x00, 0x00, 0x1f, 0x05, 0x00, 0x08, 0x00, 0x00, 0x79, 0x01, 0x80,
			0x06, 0x00, 0x00, 0xc0, 0x80, 0x01, 0x80, 0x16, 0x00, 0x00, 0x90,
			0x80, 0x00, 0x80, 0x80, 0x80, 0x30, 0x30, 0x40, 0x20, 0x80, 0xc7,
			0x00, 0x20, 0x2f, 0xd0, 0x00, 0x04, 0x00, 0xfa, 0x00, 0x08, 0x00,
			0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00, 0x08, 0x00,
			0x00, 0x79, 0x01, 0x80, 0x06, 0x00, 0x00, 0xc0, 0x80, 0x01, 0x80,
			0x16, 0x00, 0x00, 0x90, 0x80, 0x00, 0x80, 0x80, 0x80, 0x30, 0x30,
			0x40, 0x20, 0x80, 0xc7, 0x00, 0x30, 0x3f, 0xd0, 0x00, 0x04, 0x00,
			0xfa, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f,
			0x05, 0x00, 0x08, 0x00, 0x00, 0x79, 0x01, 0x80, 0x06, 0x00, 0x00,
			0xc0, 0x80, 0x01, 0x80, 0x16, 0x00, 0x00, 0x90, 0x80, 0x00, 0x80,
			0x80, 0x80, 0x30, 0x30, 0x40, 0x20, 0x80, 0xc7, 0x00, 0x40, 0x4f,
			0xd0, 0x00, 0x04, 0x00, 0xfa, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80,
			0x02, 0x00, 0x00, 0x1f, 0x05, 0x00, 0x08, 0x00, 0x00, 0x79, 0x01,
			0x80, 0x06, 0x00, 0x00, 0xc0, 0x80, 0x01, 0x80, 0x16, 0x00, 0x00,
			0x90, 0x80, 0x00, 0x80, 0x80, 0x80, 0x30, 0x30, 0x40, 0x20, 0x80,
			0xc7, 0x00, 0x50, 0x5f, 0xd0, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
			0x40, };

	public Patch(Receiver receiver, int patchSize) {
		this.receiver = receiver;
		this.patchSize = patchSize;
		if (patchSize==256) {
			isEnsemble = true;
		} else {
			isEnsemble = false;
		}
		initPatch();
	}

	private void initPatch() {
		patch = new int[patchSize];
		if (isEnsemble) {
			for (int c = 0; c < patchSize; c++) {
				patch[c] = 0;
			}
		} else {
			// TK: initialize patch depending on engine
			// (quick & dirty initialisation - presets are based on
			// $MIOS_PATH/apps/synthesizers/midibox_sid_v2/src/sid_preset_*.inc)
			// Btw.: I've no idea, why the two nibbles of defaultLeadPatch[] bytes
			// are swapped - please enlight me ;)
			switch (getParameter(16, 0, 2)) {
			case 0:
				for (int i = 0; i < patchSize; ++i) {
					patch[i] = ((defaultLeadPatch[i] & 0x0f) << 4)
							| ((defaultLeadPatch[i] & 0xf0) >> 4);
				}
				setPatchName("Lead Patch      ");
				break;
			case 1:
				for (int i = 0; i < patchSize; ++i) {
					patch[i] = ((defaultBasslinePatch[i] & 0x0f) << 4)
							| ((defaultBasslinePatch[i] & 0xf0) >> 4);
				}
				setPatchName("Bassline Patch  ");
				break;
			case 2:
				for (int i = 0; i < patchSize; ++i) {
					patch[i] = ((defaultDrumPatch[i] & 0x0f) << 4)
							| ((defaultDrumPatch[i] & 0xf0) >> 4);
				}
				setPatchName("Drum Kit        ");
				break;
			case 3:
				for (int i = 0; i < patchSize; ++i) {
					patch[i] = ((defaultMultiPatch[i] & 0x0f) << 4)
							| ((defaultMultiPatch[i] & 0xf0) >> 4);
				}
				setPatchName("Multi Patch     ");
				break;
			}
		}
	}

	public void setPatchName(String s) {
		for (int i = 0; i < 16; i++) {
			int value = ((int) s.charAt(i)) & 0xFF;
			patch[i] = nibbleSwap(value);
		}
	}

	public String getPatchName() {
		String s = new String("");
		for (int i = 0; i < 16; i++) {
			s = s + (char) nibbleSwap(patch[i]);
		}
		return s;
	}

	public Object getEngine() {
		Object engine = new Object();
		switch (getParameter(16, 0, 2)) {
		case 0:
			engine = LEAD;
			break;
		case 1:
			engine = BASSLINE;
			break;
		case 2:
			engine = DRUM;
			break;
		case 3:
			engine = MULTI;
			break;
		default:
			System.out.println("Unknown engine!");
			break;
		}
		return engine;
	}

	public String getEngineStr() {
		String engine = "";
		switch (getParameter(16, 0, 2)) {
		case 0:
			engine = "LEAD";
			break;
		case 1:
			engine = "BASSLINE";
			break;
		case 2:
			engine = "DRUM";
			break;
		case 3:
			engine = "MULTI";
			break;
		default:
			System.out.println("Unknown engine!");
			break;
		}
		return engine;
	}

	public String getSysexString() {
		String s = "";
		int sum = 0;
		for (int i = 0; i < patch.length; i++) {
			String newStr = calcValue(nibbleSwap(patch[i]), 1);
			sum = sum + Integer.parseInt(newStr.substring(1, 2), 16);
			sum = sum + Integer.parseInt(newStr.substring(3, 4), 16);
			s = s + newStr;
		}
		String checksum = Integer.toHexString(-sum & 0x7F);
		if (checksum.length() == 1) {
			checksum = "0" + checksum;
		}
		return (s + checksum);
	}
		
	public String parsePatch(String d) {
		String status;
		int dsi = 20; // Index where real patch data starts
		int checksum = 0;
		for (int i = 0; i < patchSize; i++) {
			String s1 = d.substring((4 * i) + dsi + 1, (4 * i) + dsi + 2);
			String s2 = d.substring((4 * i) + dsi + 3, (4 * i) + dsi + 4);
			checksum = checksum + Integer.parseInt(s1, 16);
			checksum = checksum + Integer.parseInt(s2, 16);
			int temp = Integer.parseInt(s1 + s2, 16);
			patch[i] = temp;
		}
		int chk = Integer.parseInt(d.substring((4 * patchSize) + dsi, (4 * patchSize) + dsi	+ 2), 16);
		if (chk != (-checksum & 0x7F)) {
			status = "checksum error";
		} else {
			status = "succesful";
		}
		return status;
	}

	public void setPatch(int[] p) {
		patch = p;
	}

	public int[] getPatch() {
		return patch;
	}
	
	public void setEngine(Object object) {
		if (object == LEAD) {
			setParameter(16, 0, 0, 2, true);
		} else if (object == BASSLINE) {
			setParameter(16, 1, 0, 2, true);
		} else if (object == DRUM) {
			setParameter(16, 2, 0, 2, true);
		} else if (object == MULTI) {
			setParameter(16, 3, 0, 2, true);
		}
		
		// TK: patch has to be re-initialized after engine change
		initPatch();
	}

	public int getParameter(int address, int start_bit, int resolution) {
		int value = 0;
		int absres = Math.abs(resolution);

		// resolution < 8 bit: gets value of bit length 'resolution' starting on
		// the start_bit
		if (absres < 8) {
			int bitmask = 0;
			for (int j = 0; j < absres; j++) { // Iterate over all bits
				bitmask = bitmask | (int) Math.pow(2, (start_bit + j));
			}
			value = (bitmask & nibbleSwap(patch[address])) >> start_bit;
		}

		// 8 bit values
		if (absres == 8) {
			value = nibbleSwap(patch[address]);
		}

		// 12 bit values
		if (absres == 12) {
			int temp1 = nibbleSwap(patch[address]);
			int temp2 = nibbleSwap(patch[address + 1] & 0xF0);
			value = (temp2 << 8) + temp1;
		}

		// 16 bit values
		if (absres == 16) {
			int temp1 = nibbleSwap(patch[address]);
			int temp2 = nibbleSwap(patch[address + 1]);
			value = (temp2 << 8) + temp1;
		}

		// Shift positive integers down to make them bipolar (if required)
		if (resolution < 0) {
			int midimin = -((int) Math.ceil((Math.pow(2, absres) - 1) / 2));
			value = value + midimin;
		}

		return value;
	}

	public void setParameter(int address, int value, int start_bit,
			int resolution, boolean forward) {
		int absres = Math.abs(resolution);

		// Shift all bipolar values up to make them fit into a positive integer
		if (resolution < 0) {
			int midimin = -((int) Math.ceil((Math.pow(2, absres) - 1) / 2));
			value = value - midimin;
		}

		// resolution < 8 bit: sets value of bit length 'resolution' starting on
		// the start_bit
		if (absres < 8) {
			int temp = nibbleSwap(patch[address]); // Retrieve data

			for (int j = 0; j < absres; j++) { // Iterate over all bits
				int this_bit = start_bit + j;
				int bitmask0 = (int) Math.pow(2, j); // Bitmask for checking if
				// current bit needs to
				// change
				int bitmask1 = (int) Math.pow(2, (this_bit)); // Bitmask for
				// turning bits
				// on
				int bitmask2 = (~bitmask1) & 0xFF; // Bitmask for turning bits
				// off
				if ((value & bitmask0) == 0) {
					temp = (temp & bitmask2) & 0xFF; // Turn bit off
				}
				if ((value & bitmask0) != 0) {
					temp = (temp | bitmask1) & 0xFF; // Turn bit on
				}
			}
			patch[address] = nibbleSwap(temp); // Put back new data
			if (forward) {
				sysexSend(address, temp, 1);
			}
		}

		// 8 bit values
		if (absres == 8) {
			patch[address] = nibbleSwap(value & 0xFF);
			if (forward) {
				sysexSend(address, (value & 0xFF), 1);
			}
		}

		// 12 bit values
		if (absres == 12) {
			int temp = nibbleSwap(patch[address + 1]); // Retrieve data
			temp = (temp & 0xFF00);
			patch[address] = nibbleSwap(value & 0xFF);
			patch[address + 1] = nibbleSwap(temp + ((value & 0xF00) >> 8));
			if (forward) {
				sysexSend(address, ((temp << 4) + (value & 0xFFF)), 2);
			}
		}

		// 16 bit values
		if (absres == 16) {
			patch[address] = nibbleSwap(value & 0xFF);
			patch[address + 1] = nibbleSwap((value & 0xFF00) >> 8);
			if (forward) {
				sysexSend(address, (value & 0xFFFF), 2);
			}
		}
	}

	private static int nibbleSwap(int inInt) {
		int nibble0 = (((byte) inInt) << 4) & 0xf0;
		int nibble1 = (((byte) inInt) >>> 4) & 0x0f;
		return (nibble0 | nibble1);
	}

	public void setStereoLink(boolean b) {
		if (b) {
			WOPT = WOPT | 0x01;
		} else {
			WOPT = WOPT & 0x02;
		}
	}

	public void setOscillatorLink(boolean b) {
		if (b) {
			WOPT = WOPT | 0x02;
		} else {
			WOPT = WOPT & 0x01;
		}
	}
	
	public boolean isEnsemble() {
		return isEnsemble;
	}

	public void sysexSend(int addr, int value, int bytes) {
		SysexMessage sysexMessage = new SysexMessage();
		String strMessage;
		if (isEnsemble) {
			strMessage = SIDSysexInfo.editEnsembleParameterSysex;			
		} else {
			strMessage = SIDSysexInfo.editPatchParameterSysex;
			strMessage = strMessage.replace("<wopt>", "0" + Integer.toHexString(WOPT));			
		}		
		strMessage = strMessage.replace("<device>", "00");
		strMessage = strMessage.replace("<address>", calcAddr(addr));
		strMessage = strMessage.replace("<value>", calcValue(value, bytes));
		
		// When the length of the Sysex String is shorter than any previous strings, pad the end with F7 bytes to make it identical in size
		// This is yet another workaround for the freakin' Java Sysex bug!
		if (strMessage.length() > maxSysExLength) {
			maxSysExLength = strMessage.length();
		} else {
			while (strMessage.length() < maxSysExLength) {
				strMessage = strMessage + "F7";
			}				
		}
		
		int nLengthInBytes = strMessage.length() / 2;
		byte[] abMessage = new byte[nLengthInBytes];
		for (int i = 0; i < nLengthInBytes; i++) {
			abMessage[i] = (byte) Integer.parseInt(strMessage.substring(i * 2, i * 2 + 2), 16);
		}
		//System.out.println(strMessage);
		try {
			sysexMessage.setMessage(abMessage, abMessage.length);
		} catch (Exception e) {
			System.out.println("exception!");
			System.out.println(e);
		}
		receiver.send(sysexMessage, -1);

		try {
			Thread.currentThread().sleep(delay);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

	}

	protected String calcAddr(int addr) {
		String str0 = Integer.toHexString(addr & 0x00F);
		String str1 = Integer.toHexString((addr & 0x070) >> 4);
		String str2 = Integer.toHexString((addr & 0x180) >> 7);
		return ("0" + str2 + str1 + str0);
	}

	protected String calcValue(int val, int byt) {
		String s = "";
		if (byt == 1) {
			String s0 = "0";
			String s1 = Integer.toHexString(val & 0x0F);
			String s2 = Integer.toHexString((val & 0xF0) >> 4);
			s = (s0 + s1 + s0 + s2);
		} else if (byt == 2) {
			String s0 = "0";
			String s1 = Integer.toHexString(val & 0x000F);
			String s2 = Integer.toHexString((val & 0x00F0) >> 4);
			String s3 = Integer.toHexString((val & 0x0F00) >> 8);
			String s4 = Integer.toHexString((val & 0xF000) >> 12);
			s = (s0 + s1 + s0 + s2 + s0 + s3 + s0 + s4);
		}
		return s;
	}

	public void close() {
	}

	public Receiver getReceiver() {
		return receiver;
	}

	public void send(MidiMessage message, long lTimeStamp) {

	}

	public Patch clone() {
		Patch p = new Patch(receiver, patchSize);
		p.setPatch(patch.clone());
		return p;
	}
}