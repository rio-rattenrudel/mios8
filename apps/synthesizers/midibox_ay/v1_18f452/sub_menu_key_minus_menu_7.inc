;; sub_menu_key_minus_menu_7.inc

;; -------------------[SWITCH VELOCITY TO ABC + NOISE PERIOD OFF]-------------
;; jumped in here from sub_menu_key_minus.inc

MINUS_KEY_SUBROUTINE_AT_MAIN_MENU7
	
	;; 1) CHECK WHICH ITEM IS 
	;; IN EDITING MODE "*"
	;; VIA AY_LCD_COURSER_POS_MAIN_MENU_VELOCITY_CONFIG
	;;
	;; 2) INC ITEM 

	;; --[ 1) CHECK ITEM]-------

	;; CHECK LOCATION VIA EDITING MODE "*"
	;; AND JUMP TO THE EDITING MODE ITEM
	movff AY_LCD_COURSER_POS_MAIN_MENU_VELOCITY_CONFIG, MIOS_PARAMETER1
	movf  MIOS_PARAMETER1, W
	JUMPTABLE_4BYTES 5 ;; [ITEM 1 to 4 JUMP, ITEM 0 = NOT VALID EXIT HERE]
		goto LABEL_MAIN_MENUE_SWITCH					;; POS0 - EXIT HERE TO MAIN MENU
		goto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_SET_VELOCITY_TO_A	;; POS1 - SWITCH OFF VELOCITY TO TONE A
		goto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_SET_VELOCITY_TO_B	;; POS2 - SWITCH OFF VELOCITY TO TONE B
		goto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_SET_VELOCITY_TO_C	;; POS1 - SWITCH OFF VELOCITY TO TONE C
		goto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_SET_VELOCITY_TO_NOISE_PERIOD ;; POS4 - SWITCH OFF VELOCITY NOISE PERIOD


LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_SET_VELOCITY_TO_A	
	;; ITEM POS1 - SET VELOCITY TO TONE A (MIDI VELOCITY DATA TO VOLUME OF TONE A)
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT0_TONE_A_ON_OFF

		;; WORK IS DONE BACK TO MAIN MENU
		goto LABEL_MAIN_MENUE_SWITCH


LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_SET_VELOCITY_TO_B	
	;; POS2 - SET VELOCITY TO TONE B
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT1_TONE_B_ON_OFF

		;; WORK IS DONE BACK TO MAIN MENU
		goto LABEL_MAIN_MENUE_SWITCH



LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_SET_VELOCITY_TO_C
	;; POS1 - SET VELOCITY TO TONE C
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT2_TONE_C_ON_OFF
		;; WORK IS DONE BACK TO MAIN MENU
		goto LABEL_MAIN_MENUE_SWITCH

;; ---- [VELOCITY TO NOISE PERIOD - SWITCH VELOCITY NOISE PERIOD TO TONE C to B to A]-------

LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_SET_VELOCITY_TO_NOISE_PERIOD
;; MEMO
;; VELOCITY => NOISE PERIOD:
;; VELOCITY 	      => NOISE PERID is OFF	==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=0, BIT5=0,BIT6=0,BIT7=0
;; VELOCITY of TONE A => NOISE PERIOD 		==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=1, BIT5=1,BIT6=0,BIT7=0 
;; VELOCITY of TONE B => NOISE PERIOD 		==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=1, BIT5=0,BIT6=1,BIT7=0 
;; VELOCITY of TONE C => NOISE PERIOD 		==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=1, BIT5=0,BIT6=0,BIT7=1 
	;; CHECK IF BIT3 is SET
	;; IS VELOCITY TO NOISE PERIOD = ON?
	btfss AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT3_VELOCITY_TO_NOISE_PERIOD_ON_OFF
		;; NO, BIT3 IS NOT SET
		;; SO EXIT  
		rgoto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_VELOCITY_IS_OFF
	
		;; YES - BIT3 IS SET - CHECK IF NOISE PERID VELOCITY @ TONE A IS SET
		rgoto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_CHK_VELOCITY_NOISE_CH_A

LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_VELOCITY_IS_OFF
			;; Nothing to do anymore
			;; exit here
			goto LABEL_MAIN_MENUE_SWITCH

LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_CHK_VELOCITY_NOISE_CH_A
		;; VELOCITY TO NOISE PERID = ON
		;; CHECK IF BIT5 [TONE A] is SET
		btfss AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT5_VELOCITY_A_TO_NOISE_PERIOD_ON_OFF
			;; NO, BIT5 IS NOT SET - CHECK IF VELOCITY @ TONE B BIT6 IS SET
			rgoto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_CHK_VELOCITY_NOISE_CH_B
	
			;; YES - BIT5 IS SET, SO SWITCH OFF VELOCITY @ NOISE RATE
			rgoto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_VELOCITY_TO_NOISE_OFF


LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_VELOCITY_TO_NOISE_OFF
	;; VELOCITY TONE A to NOISE PERID was SET
	;; BUT THERE HAD BEEN A [-] KEY EVENT SO SWITCH OFF
	;; VELOCITY to NOISE PERIOD
;; MEMO:
;; VELOCITY => NOISE PERIOD:
;; VELOCITY 	      => NOISE PERID is OFF	==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=0, BIT5=0,BIT6=0,BIT7=0
;; VELOCITY of TONE A => NOISE PERIOD 		==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=1, BIT5=1,BIT6=0,BIT7=0 
;; VELOCITY of TONE B => NOISE PERIOD 		==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=1, BIT5=0,BIT6=1,BIT7=0 
;; VELOCITY of TONE C => NOISE PERIOD 		==> AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF: BIT3=1, BIT5=0,BIT6=0,BIT7=1 
	;; SWITCH OFF VELOCITY @ NOISE RATE
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT3_VELOCITY_TO_NOISE_PERIOD_ON_OFF
	;; SWITCH OFF VELOCITY TONE A
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT5_VELOCITY_A_TO_NOISE_PERIOD_ON_OFF
	;; SWITCH OFF VELOCITY TONE B
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT6_VELOCITY_B_TO_NOISE_PERIOD_ON_OFF
	;; SWITCH OFF VELOCITY TONE C
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT7_VELOCITY_C_TO_NOISE_PERIOD_ON_OFF
		;; WORK IS DONE BACK TO MAIN MENU
		goto LABEL_MAIN_MENUE_SWITCH


LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_CHK_VELOCITY_NOISE_CH_B
	;; CHECK IF BIT6 [TONE B] is SET
	btfss AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT6_VELOCITY_B_TO_NOISE_PERIOD_ON_OFF
		;; NO, BIT6 IS NOT SET - ITS MUST BE TONE C 
		;; MUST BE BIT 7 - TONE C 
		rgoto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_VELOCITY_IS_NOISE_CH_B
	
		;; YES - BIT6 IS SET, SO IT IS TONE B @ NOISE PERIOD
		;; TONE A MUST BE VELOCITY FOR NOISE PERIOD 
		rgoto LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_SET_VELOCITY_TO_NOISE_CH_A

LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_SET_VELOCITY_TO_NOISE_CH_A
	;; SET VELOCITY TONE B TO NOISE PERIOD
	bsf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT5_VELOCITY_A_TO_NOISE_PERIOD_ON_OFF  ;; SWITCH ON VELOCITY TONE A
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT6_VELOCITY_B_TO_NOISE_PERIOD_ON_OFF  ;; SWITCH OFF VELOCITY TONE B
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT7_VELOCITY_C_TO_NOISE_PERIOD_ON_OFF  ;; SWITCH OFF VELOCITY TONE C
		;; WORK IS DONE BACK TO MAIN MENU
		goto LABEL_MAIN_MENUE_SWITCH

LABEL_AY_SUB_MENU_MINUS_MAIN_MENU7_VELOCITY_IS_NOISE_CH_B
	;; SET VELOCITY TONE B TO NOISE PERIOD
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT5_VELOCITY_A_TO_NOISE_PERIOD_ON_OFF  ;; SWITCH OFF VELOCITY TONE A
	bsf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT6_VELOCITY_B_TO_NOISE_PERIOD_ON_OFF  ;; SWITCH ON  VELOCITY TONE B
	bcf	AY_VELOCITY_TO_ABC_NOISE_PERIOD_ON_OFF, BIT7_VELOCITY_C_TO_NOISE_PERIOD_ON_OFF  ;; SWITCH OFF VELOCITY TONE C
		;; WORK IS DONE BACK TO MAIN MENU
		goto LABEL_MAIN_MENUE_SWITCH
