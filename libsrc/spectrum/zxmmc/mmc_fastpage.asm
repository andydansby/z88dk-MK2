; 
;	ZX Spectrum ZXMMC specific routines 
;	code by Luca Bisti
;	ported to z88dk by Stefano Bodrato - Feb 2010
;	
;	$Id: mmc_fastpage.asm,v 1.2 2010/03/27 18:59:25 stefano Exp $ 
;
;-----------------------------------------------------------------------------------------
; Page in the requested ZXMMC bank
;-----------------------------------------------------------------------------------------
;

	XLIB	mmc_fastpage
	
	INCLUDE "zxmmc.def"

	
mmc_fastpage:
	di
	ld a,l
	out (FASTPAGE),a
	ret
