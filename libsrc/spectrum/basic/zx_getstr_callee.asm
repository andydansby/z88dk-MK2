; 
;	ZX Spectrum specific routines 
;	by Stefano Bodrato, 22/06/2006 
;	Fixed by Antonio Schifano, Dec 2008
;
;	Copy a variable from basic 
;
;	int __CALLEE__ zx_getstr_callee(char variable, char *value); 
;
;	Debugged version by Antonio Schifano, 29/12/2008
;
;	$Id: zx_getstr_callee.asm,v 1.2 2008/12/31 13:58:11 stefano Exp $ 
;

XLIB	zx_getstr_callee
XDEF	ASMDISP_ZX_GETSTR_CALLEE

zx_getstr_callee:

	pop	bc
	pop	hl
	pop	de
	push	bc

; enter : hl = char *value
;          e = char variable

.asmentry

	ld	a,e
	and	95
	
	ld	d,a
	push	hl			; save destination
	
	ld	hl,($5c4b) 		; VARS

loop:	ld	a,(hl)
	cp	128
	jr	z,notfound		;  n.b. z => nc

	cp	d
	jr	z,found

	push	de
	call	$19b8			;get next variable start
	ex	de,hl
	pop	de
	jr	loop

found:	
	inc	hl
	ld	c,(hl)
	ld	a,c
	inc	hl
	ld	b,(hl)
	or	b
	inc	hl

	pop	de
	jr	z,zerolen
	ldir
zerolen:
	xor	a
	ld	(de),a
	ld	h,a
	ld	l,a
	ret
	
notfound:
	pop	hl
	ld	hl,-1
	ret

DEFC ASMDISP_ZX_GETSTR_CALLEE = asmentry - zx_getstr_callee

