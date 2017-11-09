; 
;	SD Card driver
;	by Stefano Bodrato, 2012
;	portions of the code are by Alessandro Poppi and Phil Ruston
;	
;
;	$Id: sd_get_cid_csd.asm,v 1.2 2012/09/11 13:09:39 stefano Exp $ 
;
;--------------------------------------------------------------------------------------------------------
; READ MMC CID subroutine. Data is stored at address (HL), command in A (READ_CID or READ_CSD).
;
; Returns error code in A (0 = no error).
; HL = 0 on success
;
;--------------------------------------------------------------------------------------------------------
;

	XLIB	sd_get_cid_csd
	
	LIB		sd_send_command_string
	LIB		sd_send_command_null_args
	LIB		sd_wait_data_token
	LIB		sd_read_bytes

	
    INCLUDE "sdcard.def"

	
sd_get_cid_csd:

	ld	(hl),0			; kill the OEM ID, just to touch the CID a little
	push hl
	call sd_send_command_null_args
	pop hl

	ld a,sd_error_bad_command_response
	ret nz	; ZF set if command response = 00	

	call sd_wait_data_token		; wait for the data token
	ld a,sd_error_data_token_timeout
	ret nz

	ld b,18				; read the card info to sector buffer (16 bytes + 2 CRC)
	call sd_read_bytes

	xor a
	ret
