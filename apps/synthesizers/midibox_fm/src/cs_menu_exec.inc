; $Id$
;
; MBFM Control Surface Menu Execute Functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


; ==========================================================================
; ==========================================================================
;  refered by cs_menu_tables.inc
; ==========================================================================
; ==========================================================================

; ==========================================================================
;  All supported EXEC_* indices
;; (max allowed number: 0x0f!)
; ==========================================================================
EXEC_NOP	EQU	0x00
EXEC_MENU	EQU	0x01
EXEC_MENU_WT	EQU	0x02
EXEC_MENU_SAVE	EQU	0x03
EXEC_SELPAR	EQU	0x04
EXEC_TOGPAR	EQU	0x05
EXEC_EDITNAME	EQU	0x06
EXEC_SAVE_IT	EQU	0x07


; ==========================================================================
;  The EXEC handler
;; IN: EXEC_xxx ID in WREG
; ==========================================================================
CS_MENU_EXEC_Handler
	JUMPTABLE_2BYTES (0x07+1)	; entries
	rgoto	CS_MENU_EXEC_NOP
	rgoto	CS_MENU_EXEC_Menu
	rgoto	CS_MENU_EXEC_Menu_WT
	rgoto	CS_MENU_EXEC_Menu_Save
	rgoto	CS_MENU_EXEC_SelPar
	rgoto	CS_MENU_EXEC_TogPar
	rgoto	CS_MENU_EXEC_EditName
	rgoto	CS_MENU_EXEC_SaveIt


; ==========================================================================
;  Dummy function, can be called if nothing has to be done
; ==========================================================================
CS_MENU_EXEC_NOP
	return

; ==========================================================================
;  Use this function to change to a menu
; ==========================================================================
CS_MENU_EXEC_Menu
	movf	CS_MENU_PARAMETER_IX, W
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu

; ==========================================================================
;  Use this function to change to the WT page
; ==========================================================================
CS_MENU_EXEC_Menu_WT
	;; update WT buffer
	clrf	CS_MENU_WT_STEP
	call	CS_MENU_MS_Update_WT_Buffer

	movf	CS_MENU_PARAMETER_IX, W
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu

; ==========================================================================
;  Use this function to change to the save patch menu
; ==========================================================================
CS_MENU_EXEC_Menu_Save
	;; copy current patch/bank number to SAVE_PATCH and SAVE_BANK and change menu
	call	CS_MENU_MS_GetInstrPatch
	movff	MBFM_PATCH, CS_MENU_SAVE_PATCH
	movff	MBFM_BANK, CS_MENU_SAVE_BANK
	movff	MBFM_DRUMSET, CS_MENU_SAVE_DRUMSET
	movff	MBFM_DBANK, CS_MENU_SAVE_DBANK
	movff	MBFM_ENSEMBLE, CS_MENU_SAVE_ENSEMBLE
	movff	MBFM_EBANK, CS_MENU_SAVE_EBANK
	movf	CS_MENU_PARAMETER_IX, W
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu


; ==========================================================================
;  Use this function to select/deselect a parameter
;  (depending on CS_STAT.CS_STAT_MODIFY_PARAMETER)
; ==========================================================================
CS_MENU_EXEC_SelPar
	;; don't select if no menu item
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return

	;; set the "modify parameter" flag
	bsf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; request a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ	; (see cs_menu.inc)

	;; copy new max value to CS_MENU_PARAMETER_MAX_L
	rcall	CS_MENU_Hlp_GetCursorPosEntry
	movlw	CS_MENU_ENTRY_MAX_OFFSET
	rcall	CS_MENU_Hlp_GetPointer

	;; read and copy
	tblrd*+
	movff	TABLAT, CS_MENU_PARAMETER_MAX_L
	;; in MIDIbox FM, the MAX_H parameter is always zero
	clrf	CS_MENU_PARAMETER_MAX_H

	;; new max value: set encoder speed
	rcall	CS_MENU_EncSpeedSet

	;; update parameter value
	goto	CS_MENU_EncParameterSet


; ==========================================================================
;  Use this function to select/deselect a parameter and to toggle it
;  (usefull for on/off switches)
; ==========================================================================
CS_MENU_EXEC_TogPar
	;; don't toggle if no menu item
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return

	;; select parameter
	rcall	CS_MENU_EXEC_SelPar

	;; toggle it
	btg	CS_MENU_PARAMETER_L, 0

	;; transfer to register
	rgoto	CS_MENU_ParameterUpdate

; ==========================================================================
;  Enter the "edit name" function
; ==========================================================================
CS_MENU_EXEC_EditName
	;; clear name position
	clrf	CS_MENU_NAME_POS
	;; enable "edit name" mode
	bsf	CS_STAT, CS_STAT_MODIFY_NAME
	;; enter parameter mode
	goto	CS_MENU_EXEC_SelPar

; ==========================================================================
;  Use this function to save the current patch to CS_MENU_SAVE_PATCH
; ==========================================================================
CS_MENU_EXEC_SaveIt
	;; save patch/drumset/ensemble
	movf	CS_MENU_SAVE_TYPE, W
	bz	CS_MENU_EXEC_SavePat_P
	decf	CS_MENU_SAVE_TYPE, W
	bz	CS_MENU_EXEC_SavePat_D
CS_MENU_EXEC_SavePat_E
	call	CS_MENU_SaveEnsemble
	rgoto	CS_MENU_EXEC_SavePat_Cont
CS_MENU_EXEC_SavePat_D
	call	CS_MENU_SaveDrumset
	rgoto	CS_MENU_EXEC_SavePat_Cont
CS_MENU_EXEC_SavePat_P
	call	CS_MENU_SavePatch
	;;	rgoto	CS_MENU_EXEC_SavePat_Cont

CS_MENU_EXEC_SavePat_Cont
	;; reset menu
	bcf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	movlw	CS_MENU_SAVE
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu


; ==========================================================================
;  This help function is used to change to another menu if
;  "parameter modify" mode not active
;  if currently a parameter is modified, change to parameter select
;  mode instead
;  IN: new menu ID in WREG
; ==========================================================================
CS_MENU_EXEC_Hlp_ChangeMenu
	BRA_IFCLR CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_EXEC_Hlp_ChangeMenu_Now

	;; we are in "modify parameter" mode, clear flag and update screen
	;; the user has to press the exec button once more to leave the menu
	bcf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	bcf	CS_STAT, CS_STAT_MODIFY_NAME
	;; request update of display
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; set encoder speed
	rcall	CS_MENU_EncSpeedSet
	;; update parameter
	rcall	CS_MENU_EncParameterSet
	;; update parameter max value and exit
	rgoto	CS_MENU_EncMaxSet

CS_MENU_EXEC_Hlp_ChangeMenu_Now
	movwf	CS_MENU
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; set encoder speed and exit
	rgoto	CS_MENU_EncSpeedSet



; ==========================================================================
;  directly called from cs_menu_tables.inc
;  Use this function to change to the main page
; ==========================================================================
CS_MENU_EXEC_GoToMain
	movlw	CS_MENU_MAIN
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu


; ==========================================================================
;  directly called from cs_menu_tables.inc
;  Use this function to change to the root menu
; ==========================================================================
CS_MENU_EXEC_GoToRoot
	movlw	CS_MENU_ROOT
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu

; ==========================================================================
;  directly called from cs_menu_tables.inc
;  This function first writes to EEPROM before it changes to the root menu
; ==========================================================================
CS_MENU_EXEC_GoToRootWT
	;; force write to EEPROM
	call	CS_MENU_MS_Update_WT_EEPROM
	rgoto	CS_MENU_EXEC_GoToRoot

; ==========================================================================
;  directly called from cs_menu_tables.inc
;  This function first stores the AOUT mapping before it changes to the root menu
; ==========================================================================
CS_MENU_EXEC_GoToRootAOUT
	;; write AOUT mapping into EEPROM
	call	MBFM_BANK_StoreA
	rgoto	CS_MENU_EXEC_GoToRoot

; ==========================================================================
;  directly called from cs_menu_tables.inc
;  Use this function to change to the Drum menu
; ==========================================================================
CS_MENU_EXEC_GoToDRUM
	movff	MBFM_DRUMSET, CS_MENU_PARAMETER_L

	movlw	CS_MENU_DRUM
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu
