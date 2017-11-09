;
;
;       ZX Maths Routines
;
;       9/12/02 - Stefano Bodrato
;
;       $Id: dsub.asm,v 1.2 2009/06/22 21:44:17 dom Exp $
;


IF FORzx
		INCLUDE  "zxfp.def"
ELSE
		INCLUDE  "81fp.def"
ENDIF

                XLIB    dsub

                LIB	fsetup
                LIB	stkequ

.dsub
        call    fsetup
	defb	ZXFP_SUBTRACT
	defb	ZXFP_END_CALC
        jp      stkequ
