/*
     ZZZZZZZZZZZZZZZZZZZZ    8888888888888       00000000000
   ZZZZZZZZZZZZZZZZZZZZ    88888888888888888    0000000000000
                ZZZZZ      888           888  0000         0000
              ZZZZZ        88888888888888888  0000         0000
            ZZZZZ            8888888888888    0000         0000       AAAAAA         SSSSSSSSSSS   MMMM       MMMM
          ZZZZZ            88888888888888888  0000         0000      AAAAAAAA      SSSS            MMMMMM   MMMMMM
        ZZZZZ              8888         8888  0000         0000     AAAA  AAAA     SSSSSSSSSSS     MMMMMMMMMMMMMMM
      ZZZZZ                8888         8888  0000         0000    AAAAAAAAAAAA      SSSSSSSSSSS   MMMM MMMMM MMMM
    ZZZZZZZZZZZZZZZZZZZZZ  88888888888888888    0000000000000     AAAA      AAAA           SSSSS   MMMM       MMMM
  ZZZZZZZZZZZZZZZZZZZZZ      8888888888888       00000000000     AAAA        AAAA  SSSSSSSSSSS     MMMM       MMMM

Copyright (C) Gunther Strube, InterLogic 1993-99
Copyright (C) Paulo Custodio, 2011-2012
*/

/* $Header: /cvsroot/z88dk/z88dk/src/z80asm/prsident.c,v 1.35 2012/11/03 17:39:36 pauloscustodio Exp $ */
/* $Log: prsident.c,v $
/* Revision 1.35  2012/11/03 17:39:36  pauloscustodio
/* astyle, comments
/*
/* Revision 1.34  2012/06/05 22:24:47  pauloscustodio
/* BUG_0020 : Segmentation fault in ParseIdent for symbol not found with interpret OFF
/*
/* Revision 1.33  2012/05/26 18:51:10  pauloscustodio
/* CH_0012 : wrappers on OS calls to raise fatal error
/* CH_0013 : new errors interface to decouple calling code from errors.c
/*
/* Revision 1.32  2012/05/24 17:09:27  pauloscustodio
/* Unify copyright header
/*
/* Revision 1.31  2012/05/18 00:28:45  pauloscustodio
/* astyle
/*
/* Revision 1.30  2012/05/18 00:20:32  pauloscustodio
/* ParseIndent(): remove hard coded IDs of IF, ELSE, ENDIF
/* Z80ident[]: make always handling function the same name as assembler ident.
/*
/* Revision 1.29  2012/05/17 17:42:14  pauloscustodio
/* DefineSymbol() and DefineDefSym() defined as void, a fatal error is
/* always raised on error.
/*
/* Revision 1.28  2012/05/12 16:57:33  pauloscustodio
/*     BUG_0016 : RCMX000 emulation routines not assembled when LIST is ON (-l)
/*         The code "cpi" is assembled as "call rcmx_cpi" when option -RCMX000 is ON.
/*         This is implemented by calling SetTemporaryLine() to insert new code
/*         at the current input position.
/*         When LIST is ON, getasmline() remembers the input file position, reads
/*         the next line and restores the file position. It ignores the buffer
/*         set by SetTemporaryLine(), causing the assembler to skip
/*         the "call rcmx_cpi" line.
/*         Also added registry of rcmx_cpi as external library routine.
/*
/* Revision 1.27  2012/05/11 19:29:49  pauloscustodio
/* Format code with AStyle (http://astyle.sourceforge.net/) to unify brackets, spaces instead of tabs, indenting style, space padding in parentheses and operators. Options written in the makefile, target astyle.
/*         --mode=c
/*         --lineend=linux
/*         --indent=spaces=4
/*         --style=ansi --add-brackets
/*         --indent-switches --indent-classes
/*         --indent-preprocessor --convert-tabs
/*         --break-blocks
/*         --pad-oper --pad-paren-in --pad-header --unpad-paren
/*         --align-pointer=name
/*
/* Revision 1.26  2011/10/14 14:51:15  pauloscustodio
/* - Silence warnings with casts.
/*
/* Revision 1.25  2011/08/19 15:53:58  pauloscustodio
/* BUG_0010 : heap corruption when reaching MAXCODESIZE
/* - test for overflow of MAXCODESIZE is done before each instruction at parseline(); if only one byte is available in codearea, and a 2 byte instruction is assembled, the heap is corrupted before the exception is raised.
/* - Factored all the codearea-accessing code into a new module, checking for MAXCODESIZE on every write.
/*
/* Revision 1.24  2011/07/18 00:48:25  pauloscustodio
/* Initialize MS Visual Studio DEBUG build to show memory leaks on exit
/*
/* Revision 1.23  2011/07/12 22:47:59  pauloscustodio
/* - Moved all error variables and error reporting code to a separate module errors.c,
/*   replaced all extern declarations of these variables by include errors.h,
/*   created symbolic constants for error codes.
/* - Added test scripts for error messages.
/*
/* Revision 1.22  2011/07/11 16:02:04  pauloscustodio
/* Moved all option variables and option handling code to a separate module options.c,
/* replaced all extern declarations of these variables by include options.h.
/*
/* Revision 1.21  2011/07/09 18:25:35  pauloscustodio
/* Log keyword in checkin comment was expanded inside Log expansion... recursive
/* Added Z80asm banner to all source files
/*
/* Revision 1.20  2011/07/09 17:36:09  pauloscustodio
/* Copied cvs log into Log history
/*
/* Revision 1.19  2011/07/09 01:46:00  pauloscustodio
/* Added Log keyword
/*
/* Revision 1.18  2011/07/09 01:21:55  pauloscustodio
/* added DefineSymbol() declaration
/*
/* Revision 1.17  2011/02/27 11:58:46  stefano
/* Rolled back z80asm changes (I must have messed up something!!)
/* Slightly updated console output for Enterprise..
/*
/* Revision 1.16  2011/02/23 16:27:39  stefano
/* *** empty log message ***
/*
/* Revision 1.15  2010/09/19 00:06:20  dom
/* Tweaks for compiling code generated by sdcc - it generates XREF rather than
/* LIB so treat XREF as a LIB (this possibly should be the default)
/*
/* Do some _ mapping magic when in sdcc so standard z88dk libs can be used
/* (wrong _ convention chosen a long time ago...)
/*
/* Have -forcexlib flag which treats MODULE as XLIB.
/*
/* Revision 1.14  2010/04/16 17:34:37  dom
/* Make line number an int - 32768 lines isn't big enough...
/*
/* Revision 1.13  2009/10/11 00:24:38  dom
/* Define a symbol each time we come across a LINE directive - this will
/* assist the debugger somewhat
/*
/* Revision 1.12  2009/07/18 23:23:15  dom
/* clean up the code a bit more (Formatting and a fewer magic numbers)
/*
/* Revision 1.11  2009/05/28 20:33:58  dom
/* retn isn't available either on a rabbit
/*
/* Revision 1.10  2009/05/28 19:20:16  dom
/* For the RCM SLL isn't a valid opcode, neither is anything using ixh,ixl,iyh
/* or iyl.
/*
/* Revision 1.9  2009/05/28 18:49:54  dom
/* Some opcodes didn't come out right
/*
/* Revision 1.8  2007/06/17 12:07:43  dom
/* Commit the rabbit emulation code including rrd, rld
/*
/* Add a .vcproj for visual studio
/*
/* Revision 1.7  2007/02/28 11:23:24  stefano
/* New platform !
/* Rabbit Control Module 2000/3000.
/*
/* Revision 1.6  2003/10/11 15:41:04  dom
/* changes from garry
/*
/* - support for defp -> defp addr,bank
/* - square brackets can be used in expressions
/* - comma can be used in defm
/*
/* Revision 1.5  2002/05/11 20:09:38  dom
/* A patch around the appalling IF ELSE ENDIF handling of z80asm where it
/* tries to evaluate FALSE clauses and gets completely in a twist.
/*
/* These patches turn off the output to the two errors that I've seen pop
/* up in this state: Syntax error and unknown identifier. Please test this
/* one quite hard if you get a change... - it was done to allow even more
/* complicated logic in the z88 app startup to actually work - as soon as I'm
/* happy with that I'll commit it as well
/*
/* Revision 1.4  2001/02/28 18:19:24  dom
/* Fixed size of Z80ident table <grrr>
/*
/* Revision 1.3  2001/02/28 17:59:22  dom
/* Added UNDEFINE for undefining symbols, bumped version to 1.0.18
/*
/* Revision 1.2  2001/01/23 10:00:09  dom
/* Changes by x1cygnus:
/*
/* just added a harcoded macro Invoke, similar to callpkg except that the
/* instruction 'Invoke' resolves to a call by default (ti83) and to a RST if
/* the parameter -plus is specified on the command line.
/*
/* Changes by dom:
/* Added in a rudimentary default directory set up (Defined at compile time)
/* a bit kludgy and not very nice!
/*
/* Revision 1.1  2000/07/04 15:33:30  dom
/* branches:  1.1.1;
/* Initial revision
/*
/* Revision 1.1.1.1  2000/07/04 15:33:30  dom
/* First import of z88dk into the sourceforge system <gulp>
/*
/* */

/* $History: PRSIDENT.C $ */
/*  */
/* *****************  Version 14  ***************** */
/* User: Gbs          Date: 30-01-00   Time: 12:51 */
/* Updated in $/Z80asm */
/* Bug fix from v1.0.14 where IF, ELSE & ENDIF id's were wrong in  */
/* ParseIdent () due to new CALL_PKG pseudo macro. */
/*  */
/* *****************  Version 13  ***************** */
/* User: Gbs          Date: 30-09-99   Time: 22:39 */
/* Updated in $/Z80asm */
/* CALL_PKG hard coded macro implemented for Garry Lancaster's Package */
/* System. */
/*  */
/* *****************  Version 11  ***************** */
/* User: Gbs          Date: 6-06-99    Time: 20:06 */
/* Updated in $/Z80asm */
/* "PC" program counter changed to long (from unsigned short). */
/*  */
/* *****************  Version 9  ***************** */
/* User: Gbs          Date: 6-06-99    Time: 12:13 */
/* Updated in $/Z80asm */
/* Added Ascii Art "Z80asm" at top of source file. */
/*  */
/* *****************  Version 7  ***************** */
/* User: Gbs          Date: 6-06-99    Time: 11:30 */
/* Updated in $/Z80asm */
/* "config.h" included before "symbol.h" */
/*  */
/* *****************  Version 5  ***************** */
/* User: Gbs          Date: 17-04-99   Time: 0:30 */
/* Updated in $/Z80asm */
/* New GNU programming style C format. Improved ANSI C coding style */
/* eliminating previous compiler warnings. New -o option. Asm sources file */
/* now parsed even though any line feed standards (CR,LF or CRLF) are */
/* used. */
/*  */
/* *****************  Version 4  ***************** */
/* User: Gbs          Date: 7-03-99    Time: 13:13 */
/* Updated in $/Z80asm */
/* Minor changes to keep C compiler happy. */
/*  */
/* *****************  Version 2  ***************** */
/* User: Gbs          Date: 20-06-98   Time: 15:04 */
/* Updated in $/Z80asm */
/* SUB, AND, OR, XOR and CP instruction improved with alternative syntax: */
/* "<instr> [A,]xxx", allowing for specification of accumulator. This */
/* makes all accumulator related instructions equal in syntax and removes */
/* ambiguity. */

#include "memalloc.h"   /* before any other include */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "z80asm.h"
#include "symbol.h"
#include "symbols.h"
#include "options.h"
#include "errors.h"
#include "codearea.h"
#include "options.h"
#include "types.h"

/* external functions */
void Skipline( FILE *fptr );
void Subroutine_addr( int opc0, int opc );
void JP_instr( int opc0, int opc );
void PushPop_instr( int opcode );
void RotShift_instr( int opcode );
void BitTest_instr( int opcode );
void ArithLog8_instr( int opcode );
void DeclModuleName( void );
void DEFINE( void );
void ifstatement( enum flag interpret );
void DEFVARS( void ), DEFS( void ), ORG( void ), INCLUDE( void ), BINARY( void ), CALL_OZ( void ), OZ( void ), CALL_PKG( void ), FPP( void );
void ADC( void ), ADD( void ), DEC( void ), IM( void ), IN( void ), INC( void ), INVOKE( void );
void JR( void ), LD( void ), OUT( void ), RET( void ), SBC( void );
void DEFB( void ), DEFC( void ), DEFM( void ), DEFW( void ), DEFL( void ), DEFP( void );
void RST( void ), DEFGROUP( void );
long GetConstant( char * );
int CheckRegister8( void );
void UNDEFINE( void );


/* local functions */
void ParseIdent( enum flag interpret );
void AND( void ), BIT( void ), CALL( void ), CCF( void ), CP( void ), CPD( void );
void CPDR( void ), CPI( void ), CPIR( void ), CPL( void ), DAA( void );
void DI( void ), DJNZ( void );
void EI( void ), EX( void ), EXX( void ), HALT( void );
void IND( void );
void INDR( void ), INI( void ), INIR( void ), JP( void );
void LDD( void ), LDDR( void );
void LDI( void ), LDIR( void ), NEG( void ), NOP( void ), OR( void ), OTDR( void ), OTIR( void );
void OUTD( void ), OUTI( void ), POP( void ), PUSH( void ), RES( void );
void RETI( void ), RETN( void );
void RL( void ), RLA( void ), RLC( void ), RLCA( void ), RLD( void ), RR( void ), RRA( void ), RRC( void );
void RRCA( void ), RRD( void );
void SCF( void ), SET( void ), SLA( void ), SLL( void ), SRA( void );
void SRL( void ), SUB( void ), XOR( void );
void XREF( void ), XDEF( void ), LSTON( void ), LSTOFF( void );
void LIB( void ), XLIB( void );
void IF( void ), ELSE( void ), ENDIF( void );
void MODULE( void );
void LINE( void );
void SetTemporaryLine( char *line );

/* global variables */
extern FILE *z80asmfile;
extern enum symbols sym, GetSym( void );
extern enum flag writeline, EOL;
extern char ident[], line[];
extern struct module *CURRENTMODULE;


typedef void ( *ptrfunc )( void ); /* ptr to function returning void */
typedef int ( *fptr )( const void *, const void * );

struct Z80sym
{
    char *z80mnem;
    ptrfunc z80func;
};


#define DEF_ENTRY(func)     { #func, func }
struct Z80sym Z80ident[] =
{
    DEF_ENTRY( ADC ),                  /* 0 */
    DEF_ENTRY( ADD ),
    DEF_ENTRY( AND ),
    DEF_ENTRY( BINARY ),
    DEF_ENTRY( BIT ),
    DEF_ENTRY( CALL ),                /* 5 */
    DEF_ENTRY( CALL_OZ ),
    DEF_ENTRY( CALL_PKG ),
    DEF_ENTRY( CCF ),
    DEF_ENTRY( CP ),
    DEF_ENTRY( CPD ),                  /* 10 */
    DEF_ENTRY( CPDR ),
    DEF_ENTRY( CPI ),
    DEF_ENTRY( CPIR ),
    DEF_ENTRY( CPL ),
    DEF_ENTRY( DAA ),                  /* 15 */
    DEF_ENTRY( DEC ),
    DEF_ENTRY( DEFB ),
    DEF_ENTRY( DEFC ),
    DEF_ENTRY( DEFGROUP ),
    DEF_ENTRY( DEFINE ),            /* 20 */
    DEF_ENTRY( DEFL ),
    DEF_ENTRY( DEFM ),
    DEF_ENTRY( DEFP ),
    DEF_ENTRY( DEFS ),
    DEF_ENTRY( DEFVARS ),          /* 25 */
    DEF_ENTRY( DEFW ),
    DEF_ENTRY( DI ),
    DEF_ENTRY( DJNZ ),
    DEF_ENTRY( EI ),
    DEF_ENTRY( ELSE ),            /* 30 */
    DEF_ENTRY( ENDIF ),
    DEF_ENTRY( EX ),
    DEF_ENTRY( EXX ),
    DEF_ENTRY( FPP ),
    DEF_ENTRY( HALT ),                /* 35 */
    DEF_ENTRY( IF ),
    DEF_ENTRY( IM ),
    DEF_ENTRY( IN ),
    DEF_ENTRY( INC ),
    DEF_ENTRY( INCLUDE ),      /* 40 */
    DEF_ENTRY( IND ),
    DEF_ENTRY( INDR ),
    DEF_ENTRY( INI ),
    DEF_ENTRY( INIR ),
    DEF_ENTRY( INVOKE ),            /* 45 */
    DEF_ENTRY( JP ),
    DEF_ENTRY( JR ),
    DEF_ENTRY( LD ),
    DEF_ENTRY( LDD ),
    DEF_ENTRY( LDDR ),                /* 50 */
    DEF_ENTRY( LDI ),
    DEF_ENTRY( LDIR ),
    DEF_ENTRY( LIB ),
    DEF_ENTRY( LINE ),
    DEF_ENTRY( LSTOFF ),        /* 55 */
    DEF_ENTRY( LSTON ),
    DEF_ENTRY( MODULE ),
    DEF_ENTRY( NEG ),
    DEF_ENTRY( NOP ),
    DEF_ENTRY( OR ),                    /* 60 */
    DEF_ENTRY( ORG ),
    DEF_ENTRY( OTDR ),
    DEF_ENTRY( OTIR ),
    DEF_ENTRY( OUT ),
    DEF_ENTRY( OUTD ),                /* 65 */
    DEF_ENTRY( OUTI ),
    DEF_ENTRY( OZ ),
    DEF_ENTRY( POP ),
    DEF_ENTRY( PUSH ),
    DEF_ENTRY( RES ),                  /* 70 */
    DEF_ENTRY( RET ),
    DEF_ENTRY( RETI ),
    DEF_ENTRY( RETN ),
    DEF_ENTRY( RL ),
    DEF_ENTRY( RLA ),                  /* 75 */
    DEF_ENTRY( RLC ),
    DEF_ENTRY( RLCA ),
    DEF_ENTRY( RLD ),
    DEF_ENTRY( RR ),
    DEF_ENTRY( RRA ),                  /* 80 */
    DEF_ENTRY( RRC ),
    DEF_ENTRY( RRCA ),
    DEF_ENTRY( RRD ),
    DEF_ENTRY( RST ),
    DEF_ENTRY( SBC ),                  /* 85 */
    DEF_ENTRY( SCF ),
    DEF_ENTRY( SET ),
    DEF_ENTRY( SLA ),
    DEF_ENTRY( SLL ),
    DEF_ENTRY( SRA ),                  /* 90 */
    DEF_ENTRY( SRL ),
    DEF_ENTRY( SUB ),
    DEF_ENTRY( UNDEFINE ),
    DEF_ENTRY( XDEF ),
    DEF_ENTRY( XLIB ),  /* 95 */
    DEF_ENTRY( XOR ),
    DEF_ENTRY( XREF )
};
#undef DEF_ENTRY


int
idcmp( const char *idptr, const struct Z80sym *symptr )
{
    return strcmp( idptr, symptr->z80mnem );
}


struct Z80sym *SearchId( void )
{
    struct Z80sym *foundsym;

    foundsym = ( struct Z80sym * ) bsearch( ident, Z80ident, NUM_ELEMS( Z80ident ), sizeof( struct Z80sym ),
                                            ( fptr ) idcmp );
    return foundsym;
}



void
ParseIdent( enum flag interpret )
{
    struct Z80sym *foundsym;

    foundsym = SearchId();

    if ( foundsym == NULL )
    {
        if ( interpret == ON )      /* only issue error message if interpreting */
        {
            error( ERR_UNKNOWN_IDENT );
        }
    }
    else
    {
        if ( foundsym->z80func == IF )
        {
            if ( interpret == OFF )
            {
                Skipline( z80asmfile );    /* skip current line until EOL */
            }

            ifstatement( interpret );
        }
        else if ( foundsym->z80func == ELSE ||
                  foundsym->z80func == ENDIF )
        {
            ( foundsym->z80func )();
            Skipline( z80asmfile );
        }
        else
        {
            if ( interpret == ON )
            {
                ( foundsym->z80func )();
            }

            Skipline( z80asmfile );               /* skip current line until EOL */
        }
    }
}



void
LSTON( void )
{
    if ( listing_CPY == ON )
    {
        listing = ON;             /* switch listing ON again... */
        writeline = OFF;          /* but don't write this line in listing file */
        line[0] = '\0';
    }
}



void
LSTOFF( void )
{
    if ( listing_CPY == ON )
    {
        listing = writeline = OFF;        /* but don't write this line in listing file */
        line[0] = '\0';
    }
}

/* Function for Line number in C source */

void LINE( void )
{
    char    err;
    char    name[128];
    GetSym();

    clineno = GetConstant( &err );

    if ( clinemode )
    {
        set_error_line( clineno );
    }

    line[0] = '\0';
    snprintf( name, sizeof( name ), "__C_LINE_%d", clineno );
    DefineSymbol( name, get_PC(), SYMADDR | SYMTOUCHED );
}



/* dummy function - not used */
void
IF( void )
{
}




void
ELSE( void )
{
    sym = elsestatm;
    writeline = OFF;              /* but don't write this line in listing file */
}




void
ENDIF( void )
{
    sym = endifstatm;
    writeline = OFF;              /* but don't write this line in listing file */
}



void
XDEF( void )
{
    do
    {
        if ( GetSym() == name )
        {
            DeclSymGlobal( ident, 0 );
        }
        else
        {
            error( ERR_SYNTAX );
            return;
        }
    }
    while ( GetSym() == comma );

    if ( sym != newline )
    {
        error( ERR_SYNTAX );
    }
}



void
XLIB( void )
{
    if ( GetSym() == name )
    {
        DeclModuleName();         /* XLIB name is implicit MODULE name */
        DeclSymGlobal( ident, SYMDEF );
    }
    else
    {
        error( ERR_SYNTAX );
        return;
    }
}



void
XREF( void )
{
    if ( sdcc_hacks == ON )
    {
        LIB();
        return;
    }

    do
    {
        if ( GetSym() == name )
        {
            DeclSymExtern( ident, 0 );    /* Define symbol as extern */
        }
        else
        {
            error( ERR_SYNTAX );
            return;
        }
    }
    while ( GetSym() == comma );

    if ( sym != newline )
    {
        error( ERR_SYNTAX );
    }
}



void
LIB( void )
{

    do
    {
        if ( GetSym() == name )
        {
            DeclSymExtern( ident, SYMDEF );     /* Define symbol as extern LIB reference */
        }
        else
        {
            error( ERR_SYNTAX );
            return;
        }
    }
    while ( GetSym() == comma );

    if ( sym != newline )
    {
        error( ERR_SYNTAX );
    }
}



void
MODULE( void )
{
    if ( force_xlib == ON )
    {
        XLIB();
    }
    else
    {
        GetSym();
        DeclModuleName();
    }
}


void
NOP( void )
{
    append_byte( 0x00 );
    inc_PC( 1 );
}



void
HALT( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0x76 );
    inc_PC( 1 );
}



void
LDI( void )
{
    append_byte( 0xED );
    append_byte( 0xA0 );
    inc_PC( 2 );
}



void
LDIR( void )
{
    append_byte( 0xED );
    append_byte( 0xB0 );
    inc_PC( 2 );
}



void
LDD( void )
{
    append_byte( 0xED );
    append_byte( 0xA8 );
    inc_PC( 2 );
}



void
LDDR( void )
{
    append_byte( 0xED );
    append_byte( 0xB8 );
    inc_PC( 2 );
}



void
CPI( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        SetTemporaryLine( "\nlib rcmx_cpi\ncall rcmx_cpi\n" );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xA1 );
    inc_PC( 2 );
}



void
CPIR( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        SetTemporaryLine( "\nlib rcmx_cpir\ncall rcmx_cpir\n" );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xB1 );
    inc_PC( 2 );
}



void
CPD( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        SetTemporaryLine( "\nlib rcmx_cpd\ncall rcmx_cpd\n" );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xA9 );
    inc_PC( 2 );
}



void
CPDR( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        SetTemporaryLine( "\nlib rcmx_cpdr\ncall rcmx_cpdr\n" );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xB9 );
    inc_PC( 2 );
}



void
IND( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xAA );
    inc_PC( 2 );
}



void
INDR( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xBA );
    inc_PC( 2 );
}



void
INI( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xA2 );
    inc_PC( 2 );
}



void
INIR( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xB2 );
    inc_PC( 2 );
}



void
OUTI( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xA3 );
    inc_PC( 2 );
}



void
OUTD( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xAB );
    inc_PC( 2 );
}



void
OTIR( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xB3 );
    inc_PC( 2 );
}



void
OTDR( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xED );
    append_byte( 0xBB );
    inc_PC( 2 );
}




/*
 * Allow specification of "<instr> [A,]xxx"
 * in SUB, AND, OR, XOR, CP instructions
 */
void
ExtAccumulator( int opcode )
{
    long fptr;

    fptr = ftell( z80asmfile );

    if ( GetSym() == name )
    {
        if ( CheckRegister8() == 7 )
        {
            if ( GetSym() == comma )
            {
                /* <instr> A, ... */
                ArithLog8_instr( opcode );

                return;
            }
        }
    }

    /* reparse and code generate (if possible) */
    sym = nil;
    EOL = OFF;

    fseek( z80asmfile, fptr, SEEK_SET );
    ArithLog8_instr( opcode );
}



void
CP( void )
{
    ExtAccumulator( 7 );
}




void
AND( void )
{
    ExtAccumulator( 4 );
}



void
OR( void )
{
    ExtAccumulator( 6 );
}



void
XOR( void )
{
    ExtAccumulator( 5 );
}


void
SUB( void )
{
    ExtAccumulator( 2 );
}




void
SET( void )
{
    BitTest_instr( 192 );
}



void
RES( void )
{
    BitTest_instr( 128 );
}



void
BIT( void )
{
    BitTest_instr( 64 );
}



void
RLC( void )
{
    RotShift_instr( 0 );
}



void
RRC( void )
{
    RotShift_instr( 1 );
}



void
RL( void )
{
    RotShift_instr( 2 );
}



void
RR( void )
{
    RotShift_instr( 3 );
}



void
SLA( void )
{
    RotShift_instr( 4 );
}



void
SRA( void )
{
    RotShift_instr( 5 );
}



void
SLL( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    RotShift_instr( 6 );
}



void
SRL( void )
{
    RotShift_instr( 7 );
}



void
CPL( void )
{
    append_byte( 0x2F );
    inc_PC( 1 );
}



void
RLA( void )
{
    append_byte( 0x17 );
    inc_PC( 1 );
}



void
RRA( void )
{
    append_byte( 0x1F );
    inc_PC( 1 );
}



void
RRCA( void )
{
    append_byte( 0x0F );
    inc_PC( 1 );
}



void
RLCA( void )
{
    append_byte( 0x07 );
    inc_PC( 1 );
}



void
EXX( void )
{
    append_byte( 0xD9 );
    inc_PC( 1 );
}



void
PUSH( void )
{
    PushPop_instr( 197 );
}



void
POP( void )
{
    PushPop_instr( 193 );
}




void
RETI( void )
{
    append_byte( 0xED );
    append_byte( 0x4D );
    inc_PC( 2 );
}



void
RETN( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xED );
    append_byte( 0x45 );
    inc_PC( 2 );
}



void
RLD( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        SetTemporaryLine( "\nlib rcmx_rld\ncall rcmx_rld\n" );
        return;
    }

    append_byte( 0xED );
    append_byte( 0x6F );
    inc_PC( 2 );
}



void
RRD( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        SetTemporaryLine( "\nlib rcmx_rrd\ncall rcmx_rrd\n" );
        return;
    }

    append_byte( 0xED );
    append_byte( 0x67 );
    inc_PC( 2 );
}



void
NEG( void )
{
    append_byte( 0xED );
    append_byte( 0x44 );
    inc_PC( 2 );
}



void
CALL( void )
{
    Subroutine_addr( 205, 196 );
}



void
JP( void )
{
    JP_instr( 195, 194 );
}



void
CCF( void )
{
    append_byte( 0x3F );
    inc_PC( 1 );
}



void
SCF( void )
{
    append_byte( 0x37 );
    inc_PC( 1 );
}



void
DI( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xF3 );
    inc_PC( 1 );
}



void
EI( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0xFB );
    inc_PC( 1 );
}



void
DAA( void )
{
    if ( ( cpu_type & CPU_RABBIT ) )
    {
        error( ERR_ILLEGAL_IDENT );
        return;
    }

    append_byte( 0x27 );
    inc_PC( 1 );
}

/*
 * Local Variables:
 *  indent-tabs-mode:nil
 *  require-final-newline:t
 *  c-basic-offset: 2
 *  eval: (c-set-offset 'case-label 0)
 *  eval: (c-set-offset 'substatement-open 2)
 *  eval: (c-set-offset 'access-label 0)
 *  eval: (c-set-offset 'class-open 2)
 *  eval: (c-set-offset 'class-close 2)
 * End:
 */



