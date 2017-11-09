;
;	Old School Computer Architecture - SD Card driver
;	Taken from the OSCA Bootcode by Phil Ruston 2011
;	Port by Stefano Bodrato, 2012
;
;	int sd_read_sector(struct SD_INFO descriptor, long sector, unsigned char *address);
;
;	sd_card_info and card_select must be accessible, 
;	a good place to put them is in the vars declared in the CRT0 stub
;
;	on exit: 0 if all OK or error code
;
;	$Id: sd_read_sector_callee.asm,v 1.3 2012/09/20 21:13:16 stefano Exp $
;

	XLIB	sd_read_sector_callee
	XDEF	ASMDISP_SD_READ_SECTOR_CALLEE

	XREF	sd_card_info
	XREF	card_select
	
	LIB		sd_read_sector_main
	LIB		sd_set_sector_addr_regs
	LIB		sd_send_command_current_args
	LIB		sd_wait_data_token
	LIB		sd_deselect_card

    INCLUDE "sdcard.def"
    INCLUDE "osca.def"

sd_read_sector_callee:
	pop af	; ret addr
	pop hl	; dst addr
	exx
	pop hl	; sector pos lsb
	pop de	; sector pos msb
	pop ix	; SD_INFO struct
	push af

.asmentry
						; ptr to MMC mask to be used to select port
	ld	a,(ix+1)		; or any other hw dependent reference to current slot
	ld	(card_select), a
	ld	a,(ix+2)
	ld	(sd_card_info), a
	
	scf
	call sd_set_sector_addr_regs

	ld a,CMD17			; Send CMD17 read sector command
	call sd_send_command_current_args
	ld a,sd_error_bad_command_response
	jr nz,read_end		; if ZF set command response is $00	

	call sd_wait_data_token		; wait for the data token
	ld a,sd_error_data_token_timeout
	jr nz,read_end		; ZF set if data token reeceived

;..............................................................................................	

	exx
	call sd_read_sector_main

;..............................................................................................	

read_end:
	call sd_deselect_card		; Routines always deselect card on return

	ld h,0
	ld l,a
	ret


DEFC ASMDISP_SD_READ_SECTOR_CALLEE = asmentry - sd_read_sector_callee
