;
;       Z88dk Generic Floating Point Math Library
;
;       fraction add c ix de += (hl)
;
;       $Id: fradd.asm,v 1.2 2012/04/17 16:37:46 stefano Exp $:

		XLIB	fradd

.FRADD  LD      A,(HL)
        ADD     A,E
        LD      E,A
        INC     HL
        LD      A,(HL)
        ADC     A,D
        LD      D,A
        INC     HL
        LD      A,(HL)
        ADC     A,IXL
        LD      IXL,A
        INC     HL
        LD      A,(HL)
        ADC     A,IXH
        LD      IXH,A
        INC     HL
        LD      A,(HL)
        ADC     A,C
        LD      C,A
        RET

