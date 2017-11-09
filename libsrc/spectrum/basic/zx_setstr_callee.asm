;
;	ZX Spectrum specific routines
;	by Stefano Bodrato, 29/06/2006
;	Fixed by Antonio Schifano, Dec 2008
;	
;	Copy a string to a BASIC variable
;
;	int __CALLEE__ zx_setstr_callee(char variable, char *value);
;
;	CPIR, debugged version by Antonio Schifano, 29/12/2008
;
;	$Id: zx_setstr_callee.asm,v 1.2 2008/12/31 13:58:11 stefano Exp $
;

XLIB	zx_setstr_callee
XDEF	ASMDISP_ZX_SETSTR_CALLEE

zx_setstr_callee:

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
	push	hl
	push	de

	ld	hl,($5c4b) 		; VARS
	
loop:

	ld	a,(hl)
	cp	128
	
	jr	z,store			; variable not found
	
morevar:

	cp	d
	jr	z,found

	call	$19b8			;get next variable start
	ex	de,hl
	pop	de
	push	de
	jr	loop

found:	
	call	$19b8			; get next variable start
	call	$19e8			; reclaim space (delete)
	
store:

	pop	af			; swap var name and str. ptr into stack
	pop	de
	push	af
	push	de
	xor	a
	ld	b,a
	ld	c,a
	ex	de,hl
	cpir				; scan for zero
	ex	de,hl

	ld	a,b
	cpl
	ld	b,a
	ld	a,c
	cpl
	ld	c,a			; bc=str len
	
	push	hl
	push	bc
	inc	bc
	inc	bc
	inc	bc
	call	$1655			; MAKE-ROOM
	pop	bc
	pop	hl
	
	pop	de			; get back str. ptr
	pop	af			; and var name
	ld	(hl),a
	inc	hl
	ld	(hl),c
	inc	hl
	ld	(hl),b
	ld	a,b			; handle 0 lenght strings
	or	c
	ret	z
	inc	hl
	
	ex	de,hl
	ldir
	
	ret
	

DEFC ASMDISP_ZX_SETSTR_CALLEE = asmentry - zx_setstr_callee
