;
;
;       ZX Maths Routines
;
;       8/12/02 - Stefano Bodrato
;
;       $Id: ddiv.asm,v 1.2 2009/06/22 21:44:17 dom Exp $
;


IF FORzx
		INCLUDE  "zxfp.def"
ELSE
		INCLUDE  "81fp.def"
ENDIF

                XLIB    ddiv

                LIB	fsetup
                LIB	stkequ

.ddiv
        call    fsetup
	defb	ZXFP_DIVISION
	defb	ZXFP_END_CALC
        jp      stkequ
