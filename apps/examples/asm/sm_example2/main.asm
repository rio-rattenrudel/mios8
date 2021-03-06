; $Id$
;
; Scan Matrix Example #2
; 
; ==========================================================================
;
;  Copyright 2004 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ---[ MIOS header file ]---
#include <mios.h>

;; ---[ useful macros ]---
#include <macros.h>

;; ---[ vectors to MIOS functions (never change!) ]---
#include <mios_vectors.inc>

;; ---[ user hooks (never change!) ]---
#include <user_vectors.inc>

;; ==========================================================================
;;  General Application Settings
;; ==========================================================================

;; ---[ variables used by application ]---
#include "app_defines.h"

; ==========================================================================

;; ---[ configuration table for MIDI processor and rotary encoders ]---
#include <mios_mt_table.inc>
#include <mios_enc_table.inc>

;; ---[ Custom LCD driver ]---
#include <app_lcd.inc>

;; ---[ Scan Matrix functions ]---
#include "sm_fast.inc"

;; ---[ simplifies the handling with MIDI events ]---
#include "midi_evnt.inc"

;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; initialize the scan matrix driver
	call	SM_Init

	;; initialize the timer function for the debounce counters
	;; we want to setup the timer with a frequency of 1kHz = 1 mS
        ;; prescaler 1:1 should be used
        ;; calculate the required number of clocks for this period:
        ;; clocks = period / 100 nS = 1 mS / 100 nS = 10000
        ;; calculate low and high byte:
        ;;    low byte  = period & 0xff
        ;;    high byte = period >> 8
        ;; therefore:
        movlw   10000 & 0xff
        movwf   MIOS_PARAMETER1
        movlw   10000 >> 8
        movwf   MIOS_PARAMETER2
        movlw   0x00
        call	MIOS_TIMER_Init
        ;; now the USER_Timer function is called every mS!

	;; initialize the shift registers
	movlw	0			; DONT USE THE MIOS SRIO DRIVER!!!
	call	MIOS_SRIO_NumberSet

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; call the scan matrix button handler
	call	SM_ButtonHandler
	
	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
	;; call the debounce function
	call	SM_DebounceTimer
	
	;; increment the display timer so we don't update display 
	;; more than once every 256 mSec
	incfsz	SM_DISPLAY_TIMER, F
	return
	
	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_TIME	;; allow update to occur	
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a debug command has been received
;;  via SysEx
;;  Input:
;;     o WREG, MIOS_PARAMETER1, MIOS_PARAMETER2, MIOS_PARAMETER3 like
;;       specified in the debug command
;;  Output:
;;     o return values WREG, MIOS_PARAMETER1, MIOS_PARAMETER2, MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_DebugTrigger
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the display content should be 
;;  initialized. Thats the case during startup and after a temporary message
;;  has been printed on the screen
;; --------------------------------------------------------------------------
USER_DISPLAY_Init
	call	MIOS_LCD_Clear

	TABLE_ADDR TEXT_WELCOME_0
	call	MIOS_LCD_PrintString
	call	MIOS_LCD_PrintString

	return

TEXT_WELCOME_0	STRING 16, 0x00, "sm_example2_v2.1"
TEXT_WELCOME_1	STRING 16, 0x40, " 8 OUT x 8 IN   "

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick

	;; update display no more than every 256 mSec to minimize the CPU load
	btfss	MB_STAT, MB_STAT_DISPLAY_UPDATE_TIME 
	return
	;; update display only when requested to minimize the CPU load
	btfss	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ 
	return
	
	;; update display
	;; clear request and display timer
	bcf		MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ
	bcf		MB_STAT, MB_STAT_DISPLAY_UPDATE_TIME
	clrf	SM_DISPLAY_TIMER

	;; and print message on screen depending on button status
	BRA_IFCLR	SM_BUTTON_VALUE, 0, ACCESS, USER_DISPLAY_BtnPressed
	
USER_DISPLAY_BtnDepressed
	TABLE_ADDR STR_BUTTON_DEPRESSED
	rgoto	USER_DISPLAY_Cont
	
USER_DISPLAY_BtnPressed
	TABLE_ADDR STR_BUTTON_PRESSED
	;; 	rgoto	USER_DISPLAY_Cont
	
USER_DISPLAY_Cont
	call	MIOS_LCD_PrintString

	movlw	0x03
	call	MIOS_LCD_CursorSet
	movf	SM_LAST_COLUMN, W
	call	MIOS_LCD_PrintHex2
			
	movlw	0x09
	call	MIOS_LCD_CursorSet
	movf	SM_BUTTON_ROW, W
	call	MIOS_LCD_PrintHex2
	
	;; second line: print related MIDI event
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; W: MIDI Event
	movf	MIDI_EVNT0, W
	bz	MIDISM_DISPLAY_Handler_NoInEvnt		; branch if no MIDI event
	
MIDISM_DISPLAY_Handler_InEvnt				; display MIDI event on LCD line 2
	call	MIDI_EVNT_Print
	rgoto	MIDISM_DISPLAY_Handler_InEvntCont
	
MIDISM_DISPLAY_Handler_NoInEvnt				; clear LCD line 2 if no MIDI event
	TABLE_ADDR STR_NO_MIDI_EVENT
	call	MIOS_LCD_PrintString
	;; rgoto	MIDISM_DISPLAY_Handler_InEvntCont
	
MIDISM_DISPLAY_Handler_InEvntCont

	return

STR_BUTTON_DEPRESSED STRING 16, 0x00, "T:x   N:x   Off "
STR_BUTTON_PRESSED   STRING 16, 0x00, "T:x   N:x   On  "
STR_NO_MIDI_EVENT	 STRING 16, 0x40, "                "


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has been received
;;  which has been specified in the CONFIG_MIDI_IN table
;;  Input:
;;     o number of entry in WREG
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyFoundEvent
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has not been completly
;;  received within 2 seconds
;; --------------------------------------------------------------------------
USER_MPROC_NotifyTimeout
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the transfer of a MIDI byte. 
;;  It can be used to monitor the Tx activity or to do any other actions
;;  (e.g. to switch a pin for multiplexed MIDI Outs) before the byte will 
;;  be sent.
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  Input:
;;     o transmitted byte in WREG
;; --------------------------------------------------------------------------
USER_MIDI_NotifyTx
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received.
;;  It can be used to monitor the Rx activity or to do any action - e.g.
;;  to react on realtime events like MIDI clock (0xf8) with a minimum latency
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  Input:
;;     o received byte in WREG
;; --------------------------------------------------------------------------
USER_MIDI_NotifyRx
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a button has been toggled
;;  Input:
;;     o Button number in WREG and MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V);;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
USER_DIN_NotifyToggle
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an encoder has been moved
;;  Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
USER_ENC_NotifyChange
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the shift register are loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Prepare
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS after the shift register have been loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Finish
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_AIN_NotifyChange
	return

	END
