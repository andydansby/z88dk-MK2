;
;
;       ZX Maths Routines
;
;       9/12/02 - Stefano Bodrato
;
;       $Id: floor.asm,v 1.2 2009/06/22 21:44:17 dom Exp $
;


;double floor(double)  
;Number in FA..


IF FORzx
		INCLUDE  "zxfp.def"
ELSE
		INCLUDE  "81fp.def"
ENDIF

                XLIB    floor

                LIB	fsetup1
                LIB	stkequ

.floor
        call    fsetup1
	defb	ZXFP_INT
	defb	ZXFP_END_CALC
        jp      stkequ
