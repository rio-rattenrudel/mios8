; $Id$
;
; MIOS Custom LCD Driver Example for PCD8544 based displays
;
; please find the LCD driver in $MIOS_PATH/modules/app_lcd/pcd8544/app_lcd.inc
;
; ==========================================================================
;
;  Copyright (C) 2003  Thorsten Klose (tk@midibox.org)
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
#include <mios_mt_table.inc>	; use dummy table located in $MIOS_PATH/include/asm
#include <mios_enc_table.inc>	; use dummy table located in $MIOS_PATH/include/asm

;; ---[ Custom LCD driver ]---
#include <app_lcd.inc>		; located in $MIOS_PATH/modules/app_lcd/pcd8544


;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init

	;; select LCD type #7 (custom LCD driver, hooks in app_lcd module will be used)
	movlw	0x01			; here you could forward an additional parameter
	movwf	MIOS_PARAMETER1
	movlw	0x00			; here you could forward an additional parameter
	movwf	MIOS_PARAMETER2
	movlw	0x07
	call	MIOS_LCD_TypeSet

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
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
	;; show demo screen
	goto	GLCD_DemoScreen

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	return


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
;;  This function is called by MIOS when an button has been toggled
;;  Input:
;;     o Button number in WREG and MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
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


; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function shows a nice demo screen on the GLCD
;; --------------------------------------------------------------------------
GLCD_DemoScreen

	call	MIOS_LCD_Clear

	

	;; write text, use special print function which has been
	;; improved for more than 4 lines

	;; --- CS0
	TABLE_ADDR MIOS_GLCD_FONT	; use default font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS0_0	; "PCD 8544"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR TEXT_WELCOME_CS0_1	; "powered by"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR GLCD_FONT_BIG	; use big font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS0_2	; "MIOS"
	rcall	USER_LCD_PrintXString


	;; --- CS1
	TABLE_ADDR MIOS_GLCD_FONT	; use default font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS1_0	; "PCD 8544"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR TEXT_WELCOME_CS1_1	; "powered by"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR GLCD_FONT_BIG	; use big font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS1_2	; "MIOS"
	rcall	USER_LCD_PrintXString


	;; --- CS2
	TABLE_ADDR MIOS_GLCD_FONT	; use default font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS2_0	; "PCD 8544"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR TEXT_WELCOME_CS2_1	; "powered by"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR GLCD_FONT_BIG	; use big font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS2_2	; "MIOS"
	rcall	USER_LCD_PrintXString


	;; --- CS3
	TABLE_ADDR MIOS_GLCD_FONT	; use default font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS3_0	; "PCD 8544"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR TEXT_WELCOME_CS3_1	; "powered by"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR GLCD_FONT_BIG	; use big font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS3_2	; "MIOS"
	rcall	USER_LCD_PrintXString


	;; --- CS4
	TABLE_ADDR MIOS_GLCD_FONT	; use default font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS4_0	; "PCD 8544"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR TEXT_WELCOME_CS4_1	; "powered by"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR GLCD_FONT_BIG	; use big font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS4_2	; "MIOS"
	rcall	USER_LCD_PrintXString


	;; --- CS5
	TABLE_ADDR MIOS_GLCD_FONT	; use default font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS5_0	; "PCD 8544"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR TEXT_WELCOME_CS5_1	; "powered by"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR GLCD_FONT_BIG	; use big font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS5_2	; "MIOS"
	rcall	USER_LCD_PrintXString


	;; --- CS6
	TABLE_ADDR MIOS_GLCD_FONT	; use default font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS6_0	; "PCD 8544"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR TEXT_WELCOME_CS6_1	; "powered by"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR GLCD_FONT_BIG	; use big font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS6_2	; "MIOS"
	rcall	USER_LCD_PrintXString


	;; --- CS7
	TABLE_ADDR MIOS_GLCD_FONT	; use default font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS7_0	; "PCD 8544"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR TEXT_WELCOME_CS7_1	; "powered by"
	rcall	USER_LCD_PrintXString
	TABLE_ADDR GLCD_FONT_BIG	; use big font
	call	MIOS_GLCD_FontInit
	TABLE_ADDR TEXT_WELCOME_CS7_2	; "MIOS"
	rcall	USER_LCD_PrintXString


	return

	;; this format is not compatible with MIOS_LCD_PrintString
	;; use USER_LCD_PrintXString instead

	;; -----------          X        Y   len   string
TEXT_WELCOME_CS0_0	db   0*84+ 0,  0*6+0, 14, "   PCD 8544   "
TEXT_WELCOME_CS0_1	db   0*84+ 0,  0*6+2, 14, "  powered by  "
TEXT_WELCOME_CS0_2	db   0*84+12,  0*6+3,  4, "MIOS"

TEXT_WELCOME_CS1_0	db   1*84+ 0,  0*6+0, 14, "  PCD8544 #2  "
TEXT_WELCOME_CS1_1	db   1*84+ 0,  0*6+2, 14, "  powered by  "
TEXT_WELCOME_CS1_2	db   1*84+12,  0*6+3,  4, "MIOS"

TEXT_WELCOME_CS2_0	db   2*84+ 0,  0*6+0, 14, "  PCD8544 #3  "
TEXT_WELCOME_CS2_1	db   2*84+ 0,  0*6+2, 14, "  powered by  "
TEXT_WELCOME_CS2_2	db   2*84+12,  0*6+3,  4, "MIOS"

TEXT_WELCOME_CS3_0	db   0*84+ 0,  1*6+0, 14, "  PCD8544 #4  "
TEXT_WELCOME_CS3_1	db   0*84+ 0,  1*6+2, 14, "  powered by  "
TEXT_WELCOME_CS3_2	db   0*84+12,  1*6+3,  4, "MIOS"

TEXT_WELCOME_CS4_0	db   1*84+ 0,  1*6+0, 14, "  PCD8544 #5  "
TEXT_WELCOME_CS4_1	db   1*84+ 0,  1*6+2, 14, "  powered by  "
TEXT_WELCOME_CS4_2	db   1*84+12,  1*6+3,  4, "MIOS"

TEXT_WELCOME_CS5_0	db   2*84+ 0,  1*6+0, 14, "  PCD8544 #6  "
TEXT_WELCOME_CS5_1	db   2*84+ 0,  1*6+2, 14, "  powered by  "
TEXT_WELCOME_CS5_2	db   2*84+12,  1*6+3,  4, "MIOS"

TEXT_WELCOME_CS6_0	db   0*84+ 0,  2*6+0, 14, "  PCD8544 #7  "
TEXT_WELCOME_CS6_1	db   0*84+ 0,  2*6+2, 14, "  powered by  "
TEXT_WELCOME_CS6_2	db   0*84+12,  2*6+3,  4, "MIOS"

TEXT_WELCOME_CS7_0	db   1*84+ 0,  2*6+0, 14, "  PCD8544 #8  "
TEXT_WELCOME_CS7_1	db   1*84+ 0,  2*6+2, 14, "  powered by  "
TEXT_WELCOME_CS7_2	db   1*84+12,  2*6+3,  4, "MIOS"


USER_LCD_PrintXString
	;; load X and Y position from table
	tblrd*+
	movf	TABLAT, W
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER1

	;; set graphical cursor
	call	MIOS_GLCD_GCursorSet

	;; get length
	tblrd*+
	movf	TABLAT, W

	;; print string and exit
	goto	MIOS_LCD_PrintPreconfString

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Including fonts from $MIOS_PATH/modules/glcd_font/
;; --------------------------------------------------------------------------
#include <glcd_font_big.inc>

	END
