; $Id$
;
; MIDIbox LC SysEx Parser
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

LC_SYSEX_STATE_MYSYSEX		EQU	7
LC_SYSEX_STATE_ACTION		EQU	6

LC_SYSEX_STATE_POS_RECEIVED	EQU	5	; used by SysEx LCD write handler

LC_SYSEX_STATE_CHN_RECEIVED	EQU	5	; used by SysEx Meter handler
LC_SYSEX_STATE_MODE_RECEIVED	EQU	4	; used by SysEx Meter handler


;; --------------------------------------------------------------------------
;;  This sysex parser waits for the Mackie/LC Control Header 
;; --------------------------------------------------------------------------
LC_SYSEX_Parser
	;; store received byte in LC_SYSEX_IN
	movwf	LC_SYSEX_IN

	;; ignore realtime messages
	movlw	0xf8
	cpfslt	LC_SYSEX_IN, ACCESS
	return

	;; check sysex state
	BRA_IFSET LC_SYSEX_STATE, LC_SYSEX_STATE_MYSYSEX, ACCESS, LC_SYSEX_Handler

	movf	LC_SYSEX_STATE, W
	rcall	LC_SYSEX_SysExHeaderGet
	cpfseq	LC_SYSEX_IN, ACCESS
	rgoto LC_SYSEX_SysExCheckFailed
	incf	LC_SYSEX_STATE, F
	movf	LC_SYSEX_STATE, W
	andlw	0x07
	xorlw	0x05	; wait for 5 bytes (f0 00 00 66 <lc-id>)
	bnz	LC_SYSEX_SysExCheckOk

	;; SysEx ID received, lets go
	bsf	LC_SYSEX_STATE, LC_SYSEX_STATE_MYSYSEX
	movlw	0xf0
	andwf	LC_SYSEX_STATE, F
	call	MIOS_MPROC_MergerDisable
	rgoto	LC_SYSEX_SysExCheckOk

LC_SYSEX_SysExCheckFailed
	;; reset the sysex counter and action ID
	rcall	LC_SYSEX_ActionInvalid
LC_SYSEX_SysExCheckOk

LC_SYSEX_SysExCheck_End
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
LC_SYSEX_SysExHeaderGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x66
	movf	LC_ID, W
	return
	
	retlw	0x00		; (dummy)
	retlw	0x00		; (dummy)

;; --------------------------------------------------------------------------
;;  Action Invalid will be invoked when receiving an invalid sequence
;; --------------------------------------------------------------------------
LC_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  Action finished will be invoked when midi action is done
;; --------------------------------------------------------------------------
LC_SYSEX_ActionFinished
	clrf	LC_SYSEX_STATE
	clrf	LC_SYSEX_ACTION
	clrf	LC_MTC_CURSOR
	call	MIOS_MPROC_MergerEnable
	rgoto	LC_SYSEX_SysExCheck_End


;; --------------------------------------------------------------------------
;;  MIDI Check action: perform a sysex action
;; --------------------------------------------------------------------------
LC_SYSEX_Handler
	BRA_IFSET LC_SYSEX_STATE, LC_SYSEX_STATE_ACTION, ACCESS, LC_SYSEX_Handler_DoAction

	bsf	LC_SYSEX_STATE, LC_SYSEX_STATE_ACTION

	movff	LC_SYSEX_IN, LC_SYSEX_ACTION
	rgoto	LC_SYSEX_SysExCheck_End

	;; ---

LC_SYSEX_Handler_DoAction
	movf	LC_SYSEX_ACTION, W
	bz	LC_SYSEX_Action_Query

	movf	LC_SYSEX_ACTION, W
	xorlw	0x1a
	bz	LC_SYSEX_Action_MCQuery

	movf	LC_SYSEX_ACTION, W
	xorlw	0x02
	bz	LC_SYSEX_Action_HostReply

	movf	LC_SYSEX_ACTION, W
	xorlw	0x10
	bz	LC_SYSEX_Action_WriteMTC1

	movf	LC_SYSEX_ACTION, W
	xorlw	0x11
	bz	LC_SYSEX_Action_WriteMTC2

	movf	LC_SYSEX_ACTION, W
	xorlw	0x12
	skpnz
	rgoto	LC_SYSEX_Action_WriteLCD

	movf	LC_SYSEX_ACTION, W
	xorlw	0x13
	bz	LC_SYSEX_Action_VersionReq

	movf	LC_SYSEX_ACTION, W
	xorlw	0x20
	skpnz
	rgoto	LC_SYSEX_Action_Meter

	movf	LC_SYSEX_ACTION, W
	xorlw	0x21
	skpnz
	rgoto	LC_SYSEX_Action_GMeter

	goto	LC_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  MIDI Action: LC Query
;; --------------------------------------------------------------------------
LC_SYSEX_Action_Query
	;; wait until sysex footer (F7) has been received
	movlw	0xf7
	cpfseq	LC_SYSEX_IN, ACCESS
	rgoto LC_SYSEX_SysExCheck_End

	;; send SysEx header
	call	LC_SYSEX_Send_SysExHeader

	;; send Host Connect Query
	movlw	0x01
	call	MIOS_MIDI_TxBufferPut

	;; call help function which sends the LC serial number
	call	LC_SYSEX_Hlp_SendSerial

	;; send dummy password
	movlw	'A'
	call	MIOS_MIDI_TxBufferPut
	movlw	'B'
	call	MIOS_MIDI_TxBufferPut
	movlw	'C'
	call	MIOS_MIDI_TxBufferPut
	movlw	'D'
	call	MIOS_MIDI_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	call	LC_SYSEX_Send_SysExFooter

	;; finish Action
	rgoto	LC_SYSEX_ActionFinished
	
;; --------------------------------------------------------------------------
;;  MIDI Action: MC Query (undocumented)
;; --------------------------------------------------------------------------
LC_SYSEX_Action_MCQuery
	;; wait until sysex footer (F7) has been received
	movlw	0xf7
	cpfseq	LC_SYSEX_IN, ACCESS
	rgoto LC_SYSEX_SysExCheck_End

	;; send SysEx header
	call	LC_SYSEX_Send_SysExHeader

	;; send reply
	movlw	0x1b
	call	MIOS_MIDI_TxBufferPut

	;; send dummy byte (seems to work)
	movlw	0x42
	call	MIOS_MIDI_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	call	LC_SYSEX_Send_SysExFooter

	;; finish Action
	rgoto	LC_SYSEX_ActionFinished
	
;; --------------------------------------------------------------------------
;;  MIDI Action: LC Host Reply
;; --------------------------------------------------------------------------
LC_SYSEX_Action_HostReply
	;; wait until sysex footer (F7) has been received
	movlw	0xf7
	cpfseq	LC_SYSEX_IN, ACCESS
	rgoto LC_SYSEX_SysExCheck_End

	;; send SysEx header
	call	LC_SYSEX_Send_SysExHeader

	;; send Host Connect Confirmation
	movlw	0x03
	call	MIOS_MIDI_TxBufferPut

	;; call help function which sends the LC serial number
	call	LC_SYSEX_Hlp_SendSerial

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	call	LC_SYSEX_Send_SysExFooter

	;; clear character display(s)
	CALL_IFCLR LC_STAT, LC_STAT_GLCD, ACCESS, MIOS_LCD_Clear

	;; finish Action
	rgoto	LC_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;; this help function sends the pseudo serial number of the emulation: 12345678
LC_SYSEX_Hlp_SendSerial
	movlw	'1'
	movwf	TMP1
LC_SYSEX_Hlp_SendSerial_Loop
	movf	TMP1, W
	call	MIOS_MIDI_TxBufferPut
	incf	TMP1, F
	movlw	'8'
	cpfseq	TMP1, ACCESS
	rgoto LC_SYSEX_Hlp_SendSerial_Loop
	return

;; --------------------------------------------------------------------------
;;  MIDI Action: LC Version Request
;; --------------------------------------------------------------------------
LC_SYSEX_Action_VersionReq
	;; wait until sysex footer (F7) has been received
	movlw	0xf7
	cpfseq	LC_SYSEX_IN, ACCESS
	rgoto LC_SYSEX_SysExCheck_End

	;; send SysEx header
	call	LC_SYSEX_Send_SysExHeader

	;; send version number
	movlw	0x14
	call	MIOS_MIDI_TxBufferPut

	movlw	'V'
	call	MIOS_MIDI_TxBufferPut
	movlw	'1'
	call	MIOS_MIDI_TxBufferPut
	movlw	'.'
	call	MIOS_MIDI_TxBufferPut
	movlw	'4'
	call	MIOS_MIDI_TxBufferPut
	movlw	'2'
	call	MIOS_MIDI_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	call	LC_SYSEX_Send_SysExFooter

	;; finish action
	rgoto	LC_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: LC Write MTC1
;; --------------------------------------------------------------------------
LC_SYSEX_Action_WriteMTC1
	;; finish action if F7 has been received
	movf	LC_SYSEX_IN, W
	xorlw	0xf7
	bz	LC_SYSEX_Action_WriteMTC1_End

	;; get MTC cursor and continue at WriteMTC1_Cont
	movf	LC_MTC_CURSOR, W
	rgoto	LC_SYSEX_Action_WriteMTC1_Cont

;; --------------------------------------------------------------------------
;;  MIDI Action: LC Write MTC2
;; --------------------------------------------------------------------------
LC_SYSEX_Action_WriteMTC2
	;; finish action if F7 has been received
	movf	LC_SYSEX_IN, W
	xorlw	0xf7
	bz	LC_SYSEX_Action_WriteMTC2_End

	;; check if graphical LCD is connected - if not, skip this routine
	call	MIOS_LCD_TypeGet
	bz	LC_SYSEX_Action_WriteMTC2_End

	;; get MTC cursor, add 10
	movf	LC_MTC_CURSOR, W
	addlw	0x0a

LC_SYSEX_Action_WriteMTC1_Cont
	;; call LC_DISPLAY_Print_MTC/STATUS_Digit
	;; expects: digit position in MIOS_PARAMETER2
	;; digit value in MIOS_PARAMETER3
	movff	LC_MTC_CURSOR, MIOS_PARAMETER2
	movff	LC_SYSEX_IN, MIOS_PARAMETER3
	;; branch depending on digit position
	movlw	0x0a
	cpfslt	MIOS_PARAMETER2, ACCESS
	rgoto LC_SYSEX_Action_WriteMTC1_STATUS
LC_SYSEX_Action_WriteMTC1_MTC
	call	LC_DISPLAY_Print_MTC_Digit
	rgoto	LC_SYSEX_Action_WriteMTC1_Cont2
LC_SYSEX_Action_WriteMTC1_STATUS
	;; status digit 0 or 1
	movlw	-0x0a
	addwf	MIOS_PARAMETER2, F
	call	LC_DISPLAY_Print_STATUS_Digit
	;; 	rgoto	LC_SYSEX_Action_WriteMTC1_Cont2

LC_SYSEX_Action_WriteMTC1_Cont2
	;; increment cursor
	incf	LC_MTC_CURSOR, F

	;; and leave handler
	rgoto	LC_SYSEX_SysExCheck_End


LC_SYSEX_Action_WriteMTC1_End
LC_SYSEX_Action_WriteMTC2_End
	;; send footer if merger enabled to ensure proper MIDI protocol
	movlw	0x00
	rcall	LC_SYSEX_Send_SysExFooter

	;; finish action
	rgoto	LC_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: LC Write LCD
;; --------------------------------------------------------------------------
LC_SYSEX_Action_WriteLCD
	;; finish action if F7 has been received
	movf	LC_SYSEX_IN, W
	xorlw	0xf7
	bz	LC_SYSEX_Action_WriteLCD_End

	;; if cursor position has been received, branch to character output routine
	BRA_IFSET LC_SYSEX_STATE, LC_SYSEX_STATE_POS_RECEIVED, ACCESS, LC_SYSEX_Action_WriteLCD_Next

	;; first byte: receive cursor position
	;; notify this in LC_SYSEX_STATE, so that the routine branches to the output
	;; routine with the next received byte
	bsf	LC_SYSEX_STATE, LC_SYSEX_STATE_POS_RECEIVED

	;; calc new LC_CURSOR position
	movf	LC_SYSEX_IN, W
	sublw	0x38-1
	movf	LC_SYSEX_IN, W
	skpc
	addlw	0x08
	movwf	LC_CURSOR

	;; update cursor pos
	call	LC_DISPLAY_Msg_CursorUpdate
	
	;; and leave handler
	rgoto	LC_SYSEX_SysExCheck_End

	;; next step: print the characters
LC_SYSEX_Action_WriteLCD_Next
	;; save message in MSG_HOST[...]
	lfsr	FSR0, MSG_HOST_BEGIN
	movf	LC_CURSOR, W
	andlw	0x7f
	addwf	FSR0L, F
	movff	LC_SYSEX_IN, INDF0

	movf	LC_SYSEX_IN, W
	call	LC_DISPLAY_Msg_PrintChar

	;; leave handler
	rgoto	LC_SYSEX_SysExCheck_End

	;; this routine will be reached if sysex footer (F7) has been received
LC_SYSEX_Action_WriteLCD_End
	;; send footer if merger enabled to ensure proper MIDI protocol
	movlw	0x00
	rcall	LC_SYSEX_Send_SysExFooter

	;; finish action
	rgoto	LC_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  Set meter mode
;; --------------------------------------------------------------------------
LC_SYSEX_Action_Meter
	;; finish action if F7 has been received
	movf	LC_SYSEX_IN, W
	xorlw	0xf7
	bz	LC_SYSEX_Action_Meter_End

	;; branch if meter channel has been received
	BRA_IFSET LC_SYSEX_STATE, LC_SYSEX_STATE_CHN_RECEIVED, ACCESS, LC_SYSEX_Action_Meter_Next1

	;; first byte: meter channel
	;; notify this in LC_SYSEX_STATE, so that the routine branches to the output
	;; routine with the next received byte
	bsf	LC_SYSEX_STATE, LC_SYSEX_STATE_CHN_RECEIVED

	;; store channel in LC_METER_CHANNEL
	movff	LC_SYSEX_IN, LC_METER_CHANNEL
	
	;; and leave handler
	rgoto	LC_SYSEX_SysExCheck_End

	;; next step: get meter mode
LC_SYSEX_Action_Meter_Next1
	;; branch if meter mode has been received
	BRA_IFSET LC_SYSEX_STATE, LC_SYSEX_STATE_MODE_RECEIVED, ACCESS, LC_SYSEX_Action_Meter_Next2

	;; second byte: meter mode
	;; notify this in LC_SYSEX_STATE, so that the routine branches to the output
	;; routine with the next received byte
	bsf	LC_SYSEX_STATE, LC_SYSEX_STATE_MODE_RECEIVED

	;; don't copy to METER_MODE if meter number >= 0x08
	movlw	0x08
	cpfslt	LC_METER_CHANNEL, ACCESS
	rgoto LC_SYSEX_Action_Meter_Next1_Skip

	;; copy mode to METER_MODE[METER_CHANNEL]
	lfsr	FSR0, METER_MODE_BEGIN
	movf	LC_METER_CHANNEL, W
	andlw	0x07		; just ensure that we don't overwrite other variables
	addwf	FSR0L, F
	movff	LC_SYSEX_IN, INDF0

	;; and leave handler
LC_SYSEX_Action_Meter_Next1_Skip
	rgoto	LC_SYSEX_SysExCheck_End

	;; last step: do nothing, just wait for F7
LC_SYSEX_Action_Meter_Next2
	rgoto	LC_SYSEX_SysExCheck_End

	;; this routine will be reached if sysex footer (F7) has been received
LC_SYSEX_Action_Meter_End
	;; send footer if merger enabled to ensure proper MIDI protocol
	movlw	0x00
	rcall	LC_SYSEX_Send_SysExFooter

	;; finish action
	rgoto	LC_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  Set global meter mode
;; --------------------------------------------------------------------------
LC_SYSEX_Action_GMeter
	;; finish action if F7 has been received
	movf	LC_SYSEX_IN, W
	xorlw	0xf7
	bz	LC_SYSEX_Action_GMeter_End

	;; branch if meter mode has been received
	BRA_IFSET LC_SYSEX_STATE, LC_SYSEX_STATE_MODE_RECEIVED, ACCESS, LC_SYSEX_Action_GMeter_Next1

	;; first byte: global meter mode
	;; notify this in LC_SYSEX_STATE, so that the routine branches to the output
	;; routine with the next received byte
	bsf	LC_SYSEX_STATE, LC_SYSEX_STATE_MODE_RECEIVED

	;; store global meter mode in LC_channel in LC_METER_GLOBAL
	movff	LC_SYSEX_IN, LC_METER_GLOBAL
	
	;; and leave handler
	rgoto	LC_SYSEX_SysExCheck_End

	;; last step: do nothing, just wait for F7
LC_SYSEX_Action_GMeter_Next1
	rgoto	LC_SYSEX_SysExCheck_End

	;; this routine will be reached if sysex footer (F7) has been received
LC_SYSEX_Action_GMeter_End
	;; send footer if merger enabled to ensure proper MIDI protocol
	movlw	0x00
	rcall	LC_SYSEX_Send_SysExFooter

	;; finish action
	rgoto	LC_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  Send LC SysEx Header (Util function)
;; --------------------------------------------------------------------------
LC_SYSEX_Send_SysExHeader
	;; if LC_SYSEX_SYXSTATE > 0, check merger flag to allow propper sysex merging
	BRA_IFCLR LC_SYSEX_STATE, LC_SYSEX_STATE_MYSYSEX, ACCESS, LC_SYSEX_Send_SysExHeader_Skp
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	LC_SYSEX_Send_SysExHeader_Skp
	movlw	0x04		; send only LC ID
	movwf	TMP1
	rgoto	LC_SYSEX_Send_SysExHeaderLoop
LC_SYSEX_Send_SysExHeader_Skp

	clrf	TMP1
LC_SYSEX_Send_SysExHeaderLoop
	movf	TMP1, W
	rcall	LC_SYSEX_SysExHeaderGet
	call	MIOS_MIDI_TxBufferPut
	incf	TMP1, F
	movlw	0x05
	cpfseq	TMP1, ACCESS
	rgoto LC_SYSEX_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
LC_SYSEX_Send_SysExFooter
	;; if WREG[0]=1: send F7 regardless of the merger state
	BRA_IFSET WREG, 0, ACCESS, LC_SYSEX_Send_SysExFooter_Forced
	;; send footer only if merger has been enabled
	;; to ensure a proper MIDI protocol
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	skpnz
	return

LC_SYSEX_Send_SysExFooter_Forced
	movlw	0xf7
	goto	MIOS_MIDI_TxBufferPut

