// $Id$
/*
 * MIDI program module
 *
 * ==========================================================================
 *
 *  Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>
#include <pic18fregs.h>

#include "main.h"
#include "midi.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

midi_note_t midi_note[16];

unsigned char midi_cc_chn0[128];

unsigned int midi_pitch_bender[16];

/////////////////////////////////////////////////////////////////////////////
// This function initializes the MIDI module
// it should be called from USER_Init() in main.c
/////////////////////////////////////////////////////////////////////////////
void MIDI_Init(void)
{
  unsigned char i;

  for(i=0; i<sizeof(midi_note); ++i) {
    midi_note[i].ALL = 0;
  }

  for(i=0; i<sizeof(midi_cc_chn0); ++i) {
    midi_cc_chn0[i] = 0;
  }

  for(i=0; i<sizeof(midi_pitch_bender); ++i) {
    midi_pitch_bender[i] = 0;
  }
}


/////////////////////////////////////////////////////////////////////////////
// This function is called from MPROC_NotifyReceivedEvnt to notify a
// new MIDI event
/////////////////////////////////////////////////////////////////////////////
void MIDI_Evnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2)
{
  unsigned char midi_cmd = evnt0 & 0xf0;
  unsigned char midi_chn = evnt0 & 0x0f;

  switch( midi_cmd ) {

    case 0x80: // Note Off
      evnt2 = 0x00; // force velocity to zero
    case 0x90: // Note On
      midi_note[midi_chn].NOTE = evnt1;
      midi_note[midi_chn].GATE = evnt2 > 0 ? 1 : 0;
      break;

    case 0xa0: // aftertouch
      // nothing to do
      break;

    case 0xb0: // CC value
      // only storing CC values of channel #1
      if( midi_chn == 0x00 ) {
	midi_cc_chn0[evnt1] = evnt2;
      }
      break;

    case 0xc0: // Program Change
      // nothing to do
      break;

    case 0xd0: // Poly After Touch
      // nothing to do
      break;

    case 0xe0: // Pitch Bender
      midi_pitch_bender[midi_chn] = (evnt2 << 7) | evnt1;
      break;

  }

}

