; FRAM Module
;
; ==========================================================================
;
;  Copyright 2009 Matthias Mächler (maechler@mm-computing.ch / thismaechler@gmx.ch)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

; If set to 1, mios_iic functions will be used (@400kHZ)
#ifndef FRAM_MIOS_IIC
#define FRAM_MIOS_IIC 0
#endif

; If set to 1, multiplexing will be enabled, bits 4 and 5 of the device address
; will be used to multiplex (bit 5 MSB, bit 4 LSB)
#ifndef FRAM_MULTIPLEX_ENABLE
#define FRAM_MULTIPLEX_ENABLE 0
#endif

#if FRAM_MULTIPLEX_ENABLE==1

; Multiplexer MSB Port/Pin
#ifndef FRAM_MULTIPLEX_PORT_MSB
#define FRAM_MULTIPLEX_PORT_MSB PORTC
#define FRAM_MULTIPLEX_TRIS_MSB TRISC
#define FRAM_MULTIPLEX_PIN_MSB 5
#endif

; Multiplexer LSB Port/Pin
#ifndef FRAM_MULTIPLEX_PORT_LSB
#define FRAM_MULTIPLEX_PORT_LSB PORTC
#define FRAM_MULTIPLEX_TRIS_LSB TRISC
#define FRAM_MULTIPLEX_PIN_LSB 4
#endif

#endif


;includes 1MHZ IIC driver if needed. maps functions either from mios_iic or fram_iic
#if FRAM_MIOS_IIC==0
#include "fram_iic.inc"
#define IIC_Start FRAM_IIC_Start
#define IIC_Stop FRAM_IIC_Stop
#define IIC_AckSend FRAM_IIC_AckSend
#define IIC_NakSend FRAM_IIC_NakSend
#define IIC_ByteReceive FRAM_IIC_ByteReceive
#define IIC_ByteSend FRAM_IIC_ByteSend
#else
#define IIC_Start MIOS_IIC_Start
#define IIC_Stop MIOS_IIC_Stop
#define IIC_AckSend MIOS_IIC_AckSend
#define IIC_NakSend MIOS_IIC_NakSend
#define IIC_ByteReceive MIOS_IIC_ByteReceive
#define IIC_ByteSend MIOS_IIC_ByteSend
#endif


#define slave_id 0xA0 ;high nibble is the slave ID (b1010)

;; --------------------------------------------------------------------------
;; -- Always start a transaction with FRAM_Start and end it with FRAM_Stop. 
;; -- If you share a port with other IIC - devices (FRAM_MIOS_IIC==1), be sure
;; -- to always call FRAM_Stop before using other IIC related functions!
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_Begin 
;;  DESCRIPTION: Begins a read or write transaction, selects chip and memory
;;  start address. After this call, subsequent reads/writes according to the
;;  selected mode (r/w) can be performed.
;;  If you share the IIC port (FRAM_MIOS_IIC==1), you have to call FRAM_End
;;  before you call some other IIC related function!
;;
;;  IN:  chip select address and r/w mode in WREG 
;;       (<5:4>multiplex <3:1>chip select <0:0>mode, 1 for read, 0 for write
;;       Chip select LSB selects one of two sectors for FM24C512, so you can
;;       look at a FM24C512 like it were two FM24C256 (see also readme).
;;
;;       memory address in MIOS_PARAMETER1 (LSB) and MIOS_PARAMETER2 (MSB)
;;       0x0000 - 0x7FFF or 0x0000 - 0x1FFF, see README
;;
;;  OUT: WREG  0x00 on error (NAK received), else 0xFF
;;
;;  USES: FRAM_REG, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0) 
;; --------------------------------------------------------------------------
_FRAM_Begin
FRAM_Begin
	SET_BSR FRAM_REG
	movwf FRAM_REG,BANKED ;move addr/mode to FRAM_REG
	bcf FRAM_REG,7,BANKED ;clear first-byte-read flag
	
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_End 
;;  DESCRIPTION: Ends a read or write transaction. Releases the IIC port (stop).
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: FRAM_REG
;; --------------------------------------------------------------------------
_FRAM_End
FRAM_End
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_ReadByte 
;;  DESCRIPTION: Reads a byte from FRAM. The internal address latch of the FRAM
;;  will be increased by one.
;; 
;;  IN:   -
;;  OUT:  read byte in WREG
;;  USES: FRAM_REG, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
_FRAM_ReadByte
FRAM_ReadByte
	return 
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_WriteByte 
;;  DESCRIPTION: Writes one byte to the FRAM. The internal address latch of the
;;  FRAM will be increased by one.
;; 
;;  IN:  Byte to write in WREG
;;  OUT:  WREG 0x00 on error, else 0xFF
;;  USES:  FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
_FRAM_WriteByte
FRAM_WriteByte
	return 
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_ReadBuf 
;;  DESCRIPTION: Reads size bytes from FRAM. The internal address latch of 
;;  the FRAM will be increased by size.
;; 
;;  IN:  buffer size in WREG, pointer to buffer in FSR1
;;  OUT:  -
;;  USES: FRAM_REG, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
_FRAM_ReadBuf
FRAM_ReadBuf
	return 
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_WriteBuf 
;;  DESCRIPTION: Writes size bytes to FRAM. The internal address latch of 
;;  the FRAM will be increased by size.
;; 
;;  IN:  buffer size in WREG, pointer to buffer in FSR1
;;  OUT:  0x00 on error, else 0xff
;;  USES: FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
_FRAM_WriteBuf
FRAM_WriteBuf
	return