; $Id: glcd_font_big.asm 69 2008-02-01 00:20:18Z tk $
;
; Wrapper for GLCD fonts
; To be used for relocatable programs
;
; ==========================================================================

	radix	dec

#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================
; Export Label
; ==========================================================================
	global	_SPACE_FONT
	global	_MIOS_FONT
	

; ==========================================================================
; Start code section and include font
; ==========================================================================
GLCD_FONT CODE
_SPACE_FONT
_MIOS_FONT
#include "space_font.inc"
	END
