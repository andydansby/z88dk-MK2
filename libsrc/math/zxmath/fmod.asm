;
;
;       ZX Maths Routines
;
;       9/12/02 - Stefano Bodrato
;
;       $Id: fmod.asm,v 1.2 2009/06/22 21:44:17 dom Exp $
;


;double fmod(n,m)
;Number in FA..


IF FORzx
		INCLUDE  "zxfp.def"
ELSE
		INCLUDE  "81fp.def"
ENDIF

                XLIB    fmod

                LIB	fsetup
                LIB	stkequ

.fmod
        call    fsetup
	defb	ZXFP_N_MOD_M
	defb	ZXFP_END_CALC
        jp      stkequ

