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

/* $Header: /cvsroot/z88dk/z88dk/src/z80asm/modlink.c,v 1.44 2012/11/03 17:39:36 pauloscustodio Exp $ */
/* $Log: modlink.c,v $
/* Revision 1.44  2012/11/03 17:39:36  pauloscustodio
/* astyle, comments
/*
/* Revision 1.43  2012/06/14 15:01:27  pauloscustodio
/* Split safe strings from strutil.c to safestr.c
/*
/* Revision 1.42  2012/06/07 11:54:13  pauloscustodio
/* - Make mapfile static to module modlink.
/* - Remove modsrcfile, not used.
/* - GetModuleSize(): use local variable for file handle instead of objfile
/*
/* Revision 1.41  2012/05/29 21:00:35  pauloscustodio
/* BUG_0019 : z80asm closes a closed file handle, crash in Linux
/*
/* Revision 1.40  2012/05/26 18:51:10  pauloscustodio
/* CH_0012 : wrappers on OS calls to raise fatal error
/* CH_0013 : new errors interface to decouple calling code from errors.c
/*
/* Revision 1.39  2012/05/24 17:09:27  pauloscustodio
/* Unify copyright header
/*
/* Revision 1.38  2012/05/23 20:00:38  pauloscustodio
/* BUG_0017 : no error message if fails to create binary file chunk (option -c).
/* Replace ERR_FILE_OPEN by ERR_FOPEN_READ and ERR_FOPEN_WRITE.
/*
/* Revision 1.37  2012/05/20 06:39:27  pauloscustodio
/* astyle
/*
/* Revision 1.36  2012/05/20 06:02:09  pauloscustodio
/* Garbage collector
/* Added automatic garbage collection on exit and simple fence mechanism
/* to detect buffer underflow and overflow, to memalloc functions.
/* No longer needed to call init_malloc().
/* No longer need to try/catch during creation of memory structures to
/* free partially created data - all not freed data is freed atexit().
/* Renamed xfree0() to xfree().
/*
/* Revision 1.35  2012/05/20 05:31:18  pauloscustodio
/* Solve signed/unsigned mismatch warnings in symboltype, libtype: changed to char.
/*
/* Revision 1.34  2012/05/17 21:36:06  pauloscustodio
/* Remove global ASMERROR, redundant with TOTALERRORS.
/* Remove IllegalArgumentException, replace by FatalErrorException.
/*
/* Revision 1.33  2012/05/17 17:42:14  pauloscustodio
/* DefineSymbol() and DefineDefSym() defined as void, a fatal error is
/* always raised on error.
/*
/* Revision 1.32  2012/05/11 19:29:49  pauloscustodio
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
/* Revision 1.31  2012/04/05 08:39:19  stefano
/* New reserved words, ASMTAIL and ASMSIZE, referring to the finally linked program block.
/* ASMTAIL could be a good starting point to determine automatically the heap position for malloc() or to insert a stack overflow protection in programs.
/* Note the in some case, after the program packaging, there are extra critical bytes to be preserved as well.
/*
/* Revision 1.30  2011/10/14 14:46:03  pauloscustodio
/* -  BUG_0013 : defm check for MAX_CODESIZE incorrect
/*  - Remove un-necessary tests for MAX_CODESIZE; all tests are concentrated in check_space() from codearea.c.
/*
/* Revision 1.29  2011/10/07 17:53:04  pauloscustodio
/* BUG_0015 : Relocation issue - dubious addresses come out of linking
/* (reported on Tue, Sep 27, 2011 at 8:09 PM by dom)
/* - Introduced in version 1.1.8, when the CODESIZE and the codeptr were merged into the same entity.
/* - This caused the problem because CODESIZE keeps track of the start offset of each module in the sequence they will appear in the object file, and codeptr is reset to the start of the codearea for each module.
/* The effect was that all address calculations at link phase were considering
/*  a start offset of zero for all modules.
/* - Moreover, when linking modules from a libary, the modules are pulled in to the code area as they are needed, and not in the sequence they will be in the object file. The start offset was being ignored and the modules were being loaded in the incorrect order
/* - Consequence of these two issues were all linked addresses wrong.
/*
/* Revision 1.28  2011/08/21 20:37:20  pauloscustodio
/* CH_0005 : handle files as char[FILENAME_MAX] instead of strdup for every operation
/* - Factor all pathname manipulation into module file.c.
/* - Make default extensions constants.
/* - Move srcext[] and objext[] to the options.c module.
/*
/* Revision 1.27  2011/08/19 15:53:58  pauloscustodio
/* BUG_0010 : heap corruption when reaching MAXCODESIZE
/* - test for overflow of MAXCODESIZE is done before each instruction at parseline(); if only one byte is available in codearea, and a 2 byte instruction is assembled, the heap is corrupted before the exception is raised.
/* - Factored all the codearea-accessing code into a new module, checking for MAXCODESIZE on every write.
/*
/* Revision 1.26  2011/08/19 10:20:32  pauloscustodio
/* - Factored code to read/write word from file into fgetw_err/fputw_err.
/* - Renamed ReadLong/WriteLong to fgetl_err/fputl_err for symetry.
/*
/* Revision 1.25  2011/08/18 23:27:54  pauloscustodio
/* BUG_0009 : file read/write not tested for errors
/* - In case of disk full file write fails, but assembler does not detect the error
/*   and leaves back corruped object/binary files
/* - Created new exception FileIOException and ERR_FILE_IO error.
/* - Created new functions fputc_err, fgetc_err, ... to raise the exception on error.
/*
/* Revision 1.24  2011/08/18 21:46:54  pauloscustodio
/* BUG_0008 : code block of 64K is read as zero
/*
/* Revision 1.23  2011/08/15 17:12:31  pauloscustodio
/* Upgrade to Exceptions4c 2.8.9 to solve memory leak.
/*
/* Revision 1.22  2011/08/14 19:42:07  pauloscustodio
/* - LinkModules(), ModuleExpr(), CreateBinFile(), CreateLib(): throw the new exception FatalErrorException for fatal error ERR_FILE_OPEN
/*
/* Revision 1.21  2011/08/05 19:56:37  pauloscustodio
/* CH_0004 : Exception mechanism to handle fatal errors
/* Replaced all ERR_NO_MEMORY/return sequences by an exception, captured at main().
/* Replaced all the memory allocation functions malloc, calloc, ... by corresponding
/* macros xmalloc, xcalloc, ... that raise an exception if the memory cannot be allocated,
/* removing all the test code after each memory allocation.
/* Replaced all functions that allocated memory structures by the new xcalloc_struct().
/* Replaced all free() by xfree0() macro which only frees if the pointer in non-null, and
/* sets the poiter to NULL afterwards, to avoid any used of the freed memory.
/* Created try/catch sequences to clean-up partially created memory structures and rethrow the
/* exception, to cleanup memory leaks.
/* Replaced 'l' (lower case letter L) by 'len' - too easy to confuse with numeral '1'.
/*
/* Revision 1.20  2011/07/18 00:48:25  pauloscustodio
/* Initialize MS Visual Studio DEBUG build to show memory leaks on exit
/*
/* Revision 1.19  2011/07/14 01:32:08  pauloscustodio
/*     - Unified "Integer out of range" and "Out of range" errors; they are the same error.
/*     - Unified ReportIOError as ReportError(ERR_FILE_OPEN)
/*     CH_0003 : Error messages should be more informative
/*         - Added printf-args to error messages, added "Error:" prefix.
/*     BUG_0006 : sub-expressions with unbalanced parentheses type accepted, e.g. (2+3] or [2+3)
/*         - Raise ERR_UNBALANCED_PAREN instead
/*
/* Revision 1.18  2011/07/12 22:47:59  pauloscustodio
/* - Moved all error variables and error reporting code to a separate module errors.c,
/*   replaced all extern declarations of these variables by include errors.h,
/*   created symbolic constants for error codes.
/* - Added test scripts for error messages.
/*
/* Revision 1.17  2011/07/11 16:00:34  pauloscustodio
/* Moved all option variables and option handling code to a separate module options.c,
/* replaced all extern declarations of these variables by include options.h.
/* Created declarations in z80asm.h of objects defined in z80asm.c.
/*
/* Revision 1.16  2011/07/09 18:25:35  pauloscustodio
/* Log keyword in checkin comment was expanded inside Log expansion... recursive
/* Added Z80asm banner to all source files
/*
/* Revision 1.15  2011/07/09 17:36:09  pauloscustodio
/* Copied cvs log into Log history
/*
/* Revision 1.14  2011/07/09 01:46:00  pauloscustodio
/* Added Log keyword
/*
/* Revision 1.13  2011/07/09 01:20:55  pauloscustodio
/* added casts to clean up warnings
/*
/* Revision 1.12  2010/09/19 02:37:57  dom
/* 64bit issue
/*
/* Revision 1.11  2010/09/19 00:06:20  dom
/* Tweaks for compiling code generated by sdcc - it generates XREF rather than
/* LIB so treat XREF as a LIB (this possibly should be the default)
/*
/* Do some _ mapping magic when in sdcc so standard z88dk libs can be used
/* (wrong _ convention chosen a long time ago...)
/*
/* Have -forcexlib flag which treats MODULE as XLIB.
/*
/* Revision 1.10  2010/04/16 17:34:37  dom
/* Make line number an int - 32768 lines isn't big enough...
/*
/* Revision 1.9  2009/09/28 23:14:13  dom
/* Get the length of the right name.
/*
/* Revision 1.8  2009/09/03 17:42:12  dom
/* Remove endian reading kludge and make work on 64 bit
/*
/* Revision 1.7  2009/08/14 22:23:12  dom
/* clean up some compiler warnings
/*
/* Revision 1.6  2009/07/23 20:35:14  dom
/* Get the end position right here as well
/*
/* Revision 1.5  2009/07/17 22:06:48  dom
/* Experimental code to allow a LIB directive to refer to a globally exported
/* symbol from a module.
/*
/* Should provide a better way of doing the ASMDISP stuff.
/*
/* Revision 1.4  2002/11/05 11:45:56  dom
/* powerpc endian detection
/*
/* fix for 64 bit machines, sizeof(long) != 4
/*
/* Revision 1.3  2002/10/22 19:21:10  dom
/* that shouldn't have been committed (oops)
/*
/* Revision 1.2  2002/10/22 19:15:28  dom
/* Makefile changes to use $(RM) instead of rm
/*
/* Revision 1.1  2000/07/04 15:33:30  dom
/* branches:  1.1.1;
/* Initial revision
/*
/* Revision 1.1.1.1  2000/07/04 15:33:30  dom
/* First import of z88dk into the sourceforge system <gulp>
/*
/* */

/* $History: MODLINK.C $ */
/*  */
/* *****************  Version 16  ***************** */
/* User: Gbs          Date: 26-01-00   Time: 22:10 */
/* Updated in $/Z80asm */
/* Expression range validation removed from 8bit unsigned (redundant). */
/*  */
/* *****************  Version 14  ***************** */
/* User: Gbs          Date: 6-06-99    Time: 20:06 */
/* Updated in $/Z80asm */
/* "PC" program counter changed to long (from unsigned short). */
/*  */
/* *****************  Version 12  ***************** */
/* User: Gbs          Date: 6-06-99    Time: 12:12 */
/* Updated in $/Z80asm */
/* Added Ascii Art "Z80asm" at top of source file. */
/*  */
/* *****************  Version 10  ***************** */
/* User: Gbs          Date: 6-06-99    Time: 11:30 */
/* Updated in $/Z80asm */
/* "config.h" included before "symbol.h" */
/*  */
/* *****************  Version 9  ***************** */
/* User: Gbs          Date: 30-05-99   Time: 1:00 */
/* Updated in $/Z80asm */
/* Redundant system include files removed. */
/* Createlib() rewritten to replace low I/O open() with fopen() and */
/* fread(). */
/*  */
/* *****************  Version 8  ***************** */
/* User: Gbs          Date: 2-05-99    Time: 18:06 */
/* Updated in $/Z80asm */
/* File IO errors now handled through new ReportIOError() function. */
/*  */
/* *****************  Version 6  ***************** */
/* User: Gbs          Date: 17-04-99   Time: 0:30 */
/* Updated in $/Z80asm */
/* New GNU programming style C format. Improved ANSI C coding style */
/* eliminating previous compiler warnings. New -o option. Asm sources file */
/* now parsed even though any line feed standards (CR,LF or CRLF) are */
/* used. */
/*  */
/* *****************  Version 5  ***************** */
/* User: Gbs          Date: 7-03-99    Time: 13:13 */
/* Updated in $/Z80asm */
/* Minor changes to keep C compiler happy. */
/*  */
/* *****************  Version 3  ***************** */
/* User: Gbs          Date: 4-09-98    Time: 0:10 */
/* Updated in $/Z80asm */
/* Various changes by Dominic Morris (ENDIAN #if). */
/*  */
/* *****************  Version 2  ***************** */
/* User: Gbs          Date: 20-06-98   Time: 15:10 */
/* Updated in $/Z80asm */
/* SourceSafe Version History Comment Block added. */

/* ifdef QDOS changed to ifdef ENDIAN to sort ENDIAN djm 26/6/98 */

#include "memalloc.h"   /* before any other include */

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "options.h"
#include "symbol.h"
#include "symbols.h"
#include "z80asm.h"
#include "errors.h"
#include "file.h"
#include "codearea.h"
#include "strutil.h"
#include "safestr.h"

/* external functions */
void FreeSym( symbol *node );
void RemovePfixlist( struct expr *pfixexpr );
struct module *NewModule( void );
struct libfile *NewLibrary( void );
struct sourcefile *Newfile( struct sourcefile *curfile, char *fname );
long EvalPfixExpr( struct expr *pass2expr );
int cmpidstr( symbol *kptr, symbol *p );
int cmpidval( symbol *kptr, symbol *p );
int GetChar( FILE *fptr );
struct expr *ParseNumExpr( void );
symbol *FindSymbol( char *identifier, avltree *symbolptr );

/* local functions */
int LinkModule( char *filename, long fptr_base );
int LinkTracedModule( char *filename, long baseptr );
int LinkLibModules( char *objfilename, long fptr_base, long startnames, long endnames );
int LinkLibModule( struct libfile *library, long curmodule, char *modname );
int SearchLibfile( struct libfile *curlib, char *modname );
char *ReadName( void );
void redefinedmsg( void );
void CreateLib( void );
void SearchLibraries( char *modname );
void LinkModules( void );
void ModuleExpr( void );
void CreateBinFile( void );
void DefineOrigin( void );
void WriteMapFile( void );
void ReadNames( long nextname, long endnames );
void ReadExpr( long nextexpr, long endexpr );
void ReOrderSymbol( avltree *node, avltree **maproot, int ( *symcmp )( void *, void * ) );
void WriteMapSymbol( symbol *mapnode );
void WriteGlobal( symbol *node );
void CreateDeffile( void );
void ReleaseLinkInfo( void );
static char          *CheckIfModuleWanted( FILE *z80asmfile, long currentlibmodule, char *modname );

/* global variables */
extern FILE *listfile, *z80asmfile, *deffile, *libfile;
extern char line[], ident[];
extern char Z80objhdr[];
extern enum symbols sym, GetSym( void );
extern enum flag writeline;
extern enum flag EOL, library;
extern unsigned char PAGELEN;
extern unsigned char reloc_routine[];
extern int listfileptr;
extern struct modules *modulehdr;
extern struct liblist *libraryhdr;
extern struct module *CURRENTMODULE;
extern int PAGENR;
extern avltree *globalroot;
extern char *reloctable, *relocptr;
extern size_t sizeof_relocroutine;

struct linklist *linkhdr;
struct libfile *CURRENTLIB;
unsigned short totaladdr, curroffset, sizeof_reloctable;

/* static variables */
static FILE *mapfile;

void
ReadNames( long nextname, long endnames )
{
    char scope, symboltype;
    long value;
    symbol *foundsymbol;

    do
    {
        scope = fgetc_err( z80asmfile );
        symboltype = fgetc_err( z80asmfile );   /* type of name   */
        value = fgetl_err( z80asmfile ); /* read symbol (long) integer */
        ReadName();                       /* read symbol name */

        nextname += 1 + 1 + 4 + 1 + strlen( line );

        switch ( symboltype )
        {
            case 'A':
                symboltype = SYMADDR | SYMDEFINED;
                value += modulehdr->first->origin + CURRENTMODULE->startoffset;       /* Absolute address */
                break;

            case 'C':
                symboltype = SYMDEFINED;
                break;
        }

        switch ( scope )
        {
            case 'L':
                if ( ( foundsymbol = FindSymbol( line, CURRENTMODULE->localroot ) ) == NULL )
                {
                    foundsymbol = CreateSymbol( line, value, ( char )( symboltype | SYMLOCAL ), CURRENTMODULE );
                    insert( &CURRENTMODULE->localroot, foundsymbol, ( int ( * )( void *, void * ) ) cmpidstr );
                }
                else
                {
                    foundsymbol->symvalue = value;
                    foundsymbol->type |= symboltype | SYMLOCAL;
                    foundsymbol->owner = CURRENTMODULE;
                    redefinedmsg();
                }

                break;

            case 'G':
                if ( ( foundsymbol = FindSymbol( line, globalroot ) ) == NULL )
                {
                    foundsymbol = CreateSymbol( line, value, ( char )( symboltype | SYMXDEF ), CURRENTMODULE );
                    insert( &globalroot, foundsymbol, ( int ( * )( void *, void * ) ) cmpidstr );
                }
                else
                {
                    foundsymbol->symvalue = value;
                    foundsymbol->type |= symboltype | SYMXDEF;
                    foundsymbol->owner = CURRENTMODULE;
                    redefinedmsg();
                }

                break;

            case 'X':
                if ( ( foundsymbol = FindSymbol( line, globalroot ) ) == NULL )
                {
                    foundsymbol = CreateSymbol( line, value, ( char )( symboltype | SYMXDEF | SYMDEF ), CURRENTMODULE );
                    insert( &globalroot, foundsymbol, ( int ( * )( void *, void * ) ) cmpidstr );
                }
                else
                {
                    foundsymbol->symvalue = value;
                    foundsymbol->type |= symboltype | SYMXDEF | SYMDEF;
                    foundsymbol->owner = CURRENTMODULE;
                    redefinedmsg();
                }

                break;
        }
    }
    while ( nextname < endnames );
}



void
redefinedmsg( void )
{
    printf( "Symbol <%s> redefined in module '%s'\n", line, CURRENTMODULE->mname );
}



void
ReadExpr( long nextexpr, long endexpr )
{
    char type;
    long constant, i, fptr;
    struct expr *postfixexpr;
    size_t patchptr, offsetptr;

    do
    {
        type = fgetc_err( z80asmfile );
        offsetptr = fgetw_err( z80asmfile );

        /* assembler PC     as absolute address */
        set_PC( modulehdr->first->origin + CURRENTMODULE->startoffset + offsetptr );

        FindSymbol( ASSEMBLERPC, globalroot )->symvalue = get_PC();

        i = fgetc_err( z80asmfile ); /* get length of infix expression */
        fptr = ftell( z80asmfile );       /* file pointer is at start of expression */
        fgets( line, i + 1, z80asmfile ); /* read string for error reference */
        fseek( z80asmfile, fptr, SEEK_SET );      /* reset file pointer to start of expression */
        nextexpr += 1 + 1 + 1 + 1 + i + 1;


        EOL = OFF;                /* reset end of line parsing flag - a line is to be parsed... */

        GetSym();

        if ( ( postfixexpr = ParseNumExpr() ) != NULL )
        {
            /* parse numerical expression */
            if ( postfixexpr->rangetype & NOTEVALUABLE )
            {
                error_at( CURRENTFILE->fname, 0, ERR_NOT_DEFINED_EXPR, line );
            }
            else
            {
                constant = EvalPfixExpr( postfixexpr );
                patchptr = CURRENTMODULE->startoffset + offsetptr;        /* index to memory buffer */

                switch ( type )
                {
                    case 'U':
                        patch_byte( &patchptr, ( unsigned char ) constant );
                        break;

                    case 'S':
                        if ( ( constant >= -128 ) && ( constant <= 255 ) )
                        {
                            patch_byte( &patchptr, ( unsigned char ) constant );    /* opcode is stored, now store signed 8bit value */
                        }
                        else
                        {
                            error_at( CURRENTFILE->fname, 0, ERR_INT_RANGE_EXPR, constant, line );
                        }

                        break;

                    case 'C':
                        if ( ( constant >= -32768 ) && ( constant <= 65535 ) )
                        {
                            patch_word( &patchptr, constant );
                        }
                        else
                        {
                            error_at( CURRENTFILE->fname, 0, ERR_INT_RANGE_EXPR, constant, line );
                        }

                        if ( autorelocate )
                            if ( postfixexpr->rangetype & SYMADDR )
                            {
                                /* Expression contains relocatable address */
                                constant = get_PC() - curroffset;

                                if ( ( constant >= 0 ) && ( constant <= 255 ) )
                                {
                                    *relocptr++ = ( unsigned char ) constant;
                                    sizeof_reloctable++;
                                }
                                else
                                {
                                    *relocptr++ = 0;
                                    *relocptr++ = ( unsigned short )( get_PC() - curroffset ) % 256U;
                                    *relocptr++ = ( unsigned short )( get_PC() - curroffset ) / 256U;
                                    sizeof_reloctable += 3;
                                }

                                totaladdr++;
                                curroffset = ( unsigned short )get_PC();
                            }

                        break;

                    case 'L':
                        if ( constant >= LONG_MIN && constant <= LONG_MAX )
                        {
                            patch_long( &patchptr, constant );
                        }
                        else
                        {
                            error_at( CURRENTFILE->fname, 0, ERR_INT_RANGE_EXPR, constant, line );
                        }

                        break;
                }
            }

            RemovePfixlist( postfixexpr );
        }
        else
        {
            error_at( CURRENTFILE->fname, 0, ERR_EXPR, line );
        }
    }
    while ( nextexpr < endexpr );
}



void
LinkModules( void )
{
    char fheader[9];
    size_t origin;
    struct module *lastobjmodule;
    symtable = listing = OFF;
    linkhdr = NULL;

    if ( verbose )
    {
        puts( "linking module(s)...\nPass1..." );
    }

    if ( autorelocate == ON )
    {
        reloctable = ( char * ) xmalloc( 32768U );
        relocptr = reloctable;
        relocptr += 4;  /* point at first offset to store */
        totaladdr = 0;
        sizeof_reloctable = 0;  /* relocation table, still 0 elements .. */
        curroffset = 0;
    }
    else
    {
        reloctable = NULL;
    }

    try
    {
        CURRENTMODULE = modulehdr->first;       /* begin with first module */
        lastobjmodule = modulehdr->last;        /* remember this last module, further modules are libraries */

        path_replace_ext( errfilename, CURRENTFILE->fname, FILEEXT_ERR );
        /* overwrite '.asm' extension with '.err' */

        /* open error file */
        open_error_file( errfilename );

        set_PC( 0 );
        DefineDefSym( ASSEMBLERPC, get_PC(), 0, &globalroot );  /* Create standard 'ASMPC' identifier */

        do                                      /* link machine code & read symbols in all modules */
        {
            set_error_null();
            set_error_module( CURRENTMODULE->mname );

            if ( library )
            {
                CURRENTLIB = libraryhdr->firstlib;      /* begin library search  from first library for each
                                                        * module */
                CURRENTLIB->nextobjfile = 8;            /* point at first library module (past header) */
            }

            CURRENTFILE->line = 0;              /* no line references on errors during link processing */

            if ( !clinemode )
            {
                set_error_line( CURRENTFILE->line );    /* error location */
            }

            path_replace_ext( objfilename, CURRENTFILE->fname, objext );
            /* overwrite '.asm' extension with * '.obj' */

            /* open relocatable file for reading */
            z80asmfile = fopen_err( objfilename, "rb" );           /* CH_0012 */
            freadc_err( fheader, 8U, z80asmfile );             /* read first 6 chars from file into array */
            fheader[8] = '\0';

            /* compare header of file */
            if ( strcmp( fheader, Z80objhdr ) != 0 )
            {
                error( ERR_NOT_OBJ_FILE, objfilename );  /* not a object     file */
                fclose( z80asmfile );
                z80asmfile = NULL;
                break;
            }

            origin = fgetw_err( z80asmfile );

            if ( modulehdr->first == CURRENTMODULE )            /* origin of first module */
            {
                if ( autorelocate )
                {
                    CURRENTMODULE->origin = 0;    /* ORG 0 on auto relocation */
                }
                else
                {
                    if ( deforigin )
                    {
                        CURRENTMODULE->origin = EXPLICIT_ORIGIN;    /* use origin from command line    */
                    }
                    else
                    {
                        CURRENTMODULE->origin = origin;

                        if ( CURRENTMODULE->origin == 65535U )
                        {
                            DefineOrigin();    /* Define origin of first module from the keyboard */
                        }
                    }
                }

                if ( verbose == ON )
                {
                    printf( "ORG address for code is %04lX\n", CURRENTMODULE->origin );
                }
            }

            fclose( z80asmfile );
            z80asmfile = NULL;

            LinkModule( objfilename, 0 );       /* link code & read name definitions */

            CURRENTMODULE = CURRENTMODULE->nextmodule;  /* get next module, if any */

        }
        while ( CURRENTMODULE != lastobjmodule->nextmodule );

        /* parse only object modules, not added library modules */

        set_error_null();

        DefineDefSym( "ASMSIZE", get_codesize(), 0, &globalroot );
        DefineDefSym( "ASMTAIL", ( size_t )( modulehdr->first->origin + get_codesize() ), 0, &globalroot );

        if ( verbose == ON )
        {
            printf( "Code size of linked modules is %d bytes\n", ( int )get_codesize() );
        }

        if ( ! get_num_errors() )
        {
            ModuleExpr();               /*  Evaluate expressions in  all modules */
        }

    }

    finally
    {
        set_error_null();

        ReleaseLinkInfo();              /* Release module link information */

        close_error_file();
    }
}




int
LinkModule( char *filename, long fptr_base )
{
    long fptr_namedecl, fptr_modname, fptr_modcode, fptr_libnmdecl;
    size_t size;
    int flag = 0;

    /* open object file for reading */
    z80asmfile = fopen_err( filename, "rb" );           /* CH_0012 */
    fseek( z80asmfile, fptr_base + 10U, SEEK_SET );

    fptr_modname = fgetl_err( z80asmfile );              /* get file pointer to module name */
    fgetl_err( z80asmfile );                     /* get file pointer to expression declarations */
    fptr_namedecl = fgetl_err( z80asmfile );     /* get file pointer to name declarations */
    fptr_libnmdecl = fgetl_err( z80asmfile );    /* get file pointer to library name declarations */
    fptr_modcode = fgetl_err( z80asmfile );              /* get file pointer to module code */

    if ( fptr_modcode != -1 )
    {
        fseek( z80asmfile, fptr_base + fptr_modcode, SEEK_SET );  /* set file pointer to module code */

        size = fgetw_err( z80asmfile );

        /* BUG_0008 : fix size, if a zero was written, the moudule is actually 64K */
        if ( size == 0 )
        {
            size = 0x10000;
        }

        /* read module code at startoffset of the module */
        /* BUG_0015: was reading at current position in code area, swaping order of modules */
        fread_codearea_offset( z80asmfile, CURRENTMODULE->startoffset, size );

        /* BUG_0015 : was no updating codesize */
        if ( CURRENTMODULE->startoffset == get_codesize() )
        {
            inc_codesize( size );    /* a new module has been added */
        }

    }

    if ( fptr_namedecl != -1 )
    {
        fseek( z80asmfile, fptr_base + fptr_namedecl, SEEK_SET );  /* set file pointer to point at name
                                                                 * declarations */

        if ( fptr_libnmdecl != -1 )
        {
            ReadNames( fptr_namedecl, fptr_libnmdecl );    /* Read symbols until library declarations */
        }
        else
        {
            ReadNames( fptr_namedecl, fptr_modname );    /* Read symbol suntil module name */
        }
    }

    fclose( z80asmfile );
    z80asmfile = NULL;

    if ( fptr_libnmdecl != -1 )
    {
        if ( library )
        {
            /* search in libraries, if present */
            flag = LinkLibModules( filename, fptr_base, fptr_libnmdecl, fptr_modname );   /* link library modules */

            if ( !flag )
            {
                return 0;
            }
        }
    }

    return LinkTracedModule( filename, fptr_base );       /* Remember module for pass2 */
}




int
LinkLibModules( char *filename, long fptr_base, long nextname, long endnames )
{
    long len;
    char *modname;

    do
    {
        /* open object file for reading */
        z80asmfile = fopen_err( filename, "rb" );           /* CH_0012 */
        fseek( z80asmfile, fptr_base + nextname, SEEK_SET );       /* set file pointer to point at library name
                                                             * declarations */
        ReadName();               /* read library reference name */
        fclose( z80asmfile );
        z80asmfile = NULL;

        len = strlen( line );
        nextname += 1 + len;      /* remember module pointer to next name in this   object module */

        if ( FindSymbol( line, globalroot ) == NULL )
        {
            modname = xstrdup( line );

            try
            {
                SearchLibraries( modname );       /* search name in libraries */
            }

            finally
            {
                xfree( modname ); /* remove copy of module name */
            }
        }
    }
    while ( nextname < endnames );

    return 1;
}




void
SearchLibraries( char *modname )
{

    int i;


    for ( i = 0; i < 2; i++ )
    {
        /* Libraries searched in max. 2 passes */
        while ( CURRENTLIB != NULL )
        {
            if ( SearchLibfile( CURRENTLIB, modname ) )
            {
                return;
            }

            CURRENTLIB = CURRENTLIB->nextlib;

            if ( CURRENTLIB != NULL )
                if ( CURRENTLIB->nextobjfile != 8 )
                {
                    CURRENTLIB->nextobjfile = 8;    /* search at start of next lib */
                }
        }

        /* last library read ... */
        CURRENTLIB = libraryhdr->firstlib;        /* start at the beginning of the first module */
        CURRENTLIB->nextobjfile = 8;              /* in the first library */
    }
}



int
SearchLibfile( struct libfile *curlib, char *modname )
{
    long currentlibmodule, modulesize;
    int ret;
    char *mname;

    z80asmfile = fopen_err( curlib->libfilename, "rb" );           /* CH_0012 */

    while ( curlib->nextobjfile != -1 )
    {
        /* search name in all available library modules */
        do
        {
            /* point at first available module in library */
            fseek( z80asmfile, curlib->nextobjfile, SEEK_SET );   /* point at beginning of a module */
            currentlibmodule = curlib->nextobjfile;
            curlib->nextobjfile = fgetl_err( z80asmfile ); /* get file pointer to next module in library */
            modulesize = fgetl_err( z80asmfile ); /* get size of current module */
        }
        while ( modulesize == 0 && curlib->nextobjfile != -1 );

        if ( modulesize != 0 )
        {
            if ( ( mname = CheckIfModuleWanted( z80asmfile, currentlibmodule, modname ) ) != NULL )
            {
                try
                {
                    fclose( z80asmfile );
                    z80asmfile = NULL;
                    ret =  LinkLibModule( curlib, currentlibmodule + 4 + 4, mname );
                }

                finally
                {
                    xfree( mname );
                }
                return ret;
            }
            else if ( sdcc_hacks == ON && modname[0] == '_' && ( mname = CheckIfModuleWanted( z80asmfile, currentlibmodule, modname + 1 ) ) != NULL )
            {
                try
                {
                    fclose( z80asmfile );
                    z80asmfile = NULL;
                    ret =  LinkLibModule( curlib, currentlibmodule + 4 + 4, mname );
                }

                finally
                {
                    xfree( mname );
                }
                return ret;
            }
        }
    }

    fclose( z80asmfile );
    z80asmfile = NULL;
    return 0;
}


/** \brief Check to see if a library module is required
 *
 *  \param z80asmfile - File to read from
 *  \param currentlibmodule - Current offset in file
 *  \param modname - Module/symbol to search for

 */
static char *
CheckIfModuleWanted( FILE *z80asmfile, long currentlibmodule, char *modname )
{
    long fptr_mname, fptr_expr, fptr_name, fptr_libname;
    char *mname;
    char *name;
    enum flag found = OFF;


    /* found module name? */
    fseek( z80asmfile, currentlibmodule + 4 + 4 + 8 + 2, SEEK_SET );     /* point at module name  file
                                                                        * pointer */
    fptr_mname = fgetl_err( z80asmfile );      /* get module name file  pointer   */
    fptr_expr = fgetl_err( z80asmfile );
    fptr_name = fgetl_err( z80asmfile );
    fptr_libname = fgetl_err( z80asmfile );
    fseek( z80asmfile, currentlibmodule + 4 + 4 + fptr_mname, SEEK_SET );       /* point at module name  */
    mname = xstrdup( ReadName() );                      /* read module name */

    if ( strcmp( mname, modname ) == 0 )
    {
        found = ON;
    }
    else
    {
        /* We didn't find the module name, lets have a look through the exported symbol list */
        if ( fptr_name != 0 )
        {
            long end = fptr_libname;
            long red = 0;

            if ( fptr_libname == -1 )
            {
                end = fptr_mname;
            }

            /* Move to the name section */
            fseek( z80asmfile, currentlibmodule + 4 + 4 + fptr_name, SEEK_SET );
            red = fptr_name;

            while ( ! found && red < end )
            {
                char scope, type;
                long temp;

                scope = fgetc_err( z80asmfile );
                red++;
                type = fgetc_err( z80asmfile );
                red++;
                temp = fgetl_err( z80asmfile );
                red += 4;
                name = ReadName();
                red += strlen( name );
                red++; /* Length byte */

                if ( ( scope == 'X' || scope == 'G' ) && strcmp( name, modname ) == 0 )
                {
                    found = ON;
                }
            }
        }
    }

    if ( !found )
    {
        xfree( mname );
        mname = NULL;
    }

    return mname;
}

int
LinkLibModule( struct libfile *library, long curmodule, char *modname )
{
    struct module *tmpmodule;
    int flag;

    tmpmodule = CURRENTMODULE;  /* remember current module */

    CURRENTMODULE = NewModule();
    CURRENTMODULE->mname = xstrdup( modname );  /* get a copy of module name */
    /* create new module for library */
    CURRENTFILE = Newfile( NULL, library->libfilename ); /* filename for 'module' */

    if ( verbose )
    {
        printf( "Linking library module <%s>\n", modname );
    }

    flag = LinkModule( library->libfilename, curmodule );       /* link   module & read names */

    CURRENTMODULE = tmpmodule;  /* restore previous current module */
    return flag;
}



char *
ReadName( void )
{
    size_t strlength;

    strlength = fgetc_err( z80asmfile );
    freadc_err( line, strlength, z80asmfile ); /* read   name */
    line[strlength] = '\0';

    return line;
}



void
ModuleExpr( void )
{
    long fptr_namedecl, fptr_modname, fptr_exprdecl, fptr_libnmdecl;
    long fptr_base;
    struct linkedmod *curlink;

    if ( verbose )
    {
        puts( "Pass2..." );
    }

    curlink = linkhdr->firstlink;

    do
    {
        CURRENTMODULE = curlink->moduleinfo;
        fptr_base = curlink->modulestart;

        set_error_null();
        set_error_module( CURRENTMODULE->mname );


        /* open relocatable file for reading */
        z80asmfile = fopen_err( curlink->objfilename, "rb" );           /* CH_0012 */
        fseek( z80asmfile, fptr_base + 10, SEEK_SET );        /* point at module name  pointer   */
        fptr_modname = fgetl_err( z80asmfile );              /* get file pointer to module name */
        fptr_exprdecl = fgetl_err( z80asmfile );     /* get file pointer to expression declarations */
        fptr_namedecl = fgetl_err( z80asmfile );     /* get file pointer to name declarations */
        fptr_libnmdecl = fgetl_err( z80asmfile );    /* get file pointer to library name declarations */

        if ( fptr_exprdecl != -1 )
        {
            fseek( z80asmfile, fptr_base + fptr_exprdecl, SEEK_SET );

            if ( fptr_namedecl != -1 )
            {
                ReadExpr( fptr_exprdecl, fptr_namedecl );
            }    /* Evaluate until beginning of name

                                                         * declarations     */
            else if ( fptr_libnmdecl != -1 )
            {
                ReadExpr( fptr_exprdecl, fptr_libnmdecl );
            }   /* Evaluate until beginning of library

                                                         * reference declarations */
            else
            {
                ReadExpr( fptr_exprdecl, fptr_modname );    /* Evaluate until beginning of module name */
            }
        }

        fclose( z80asmfile );
        z80asmfile = NULL;

        curlink = curlink->nextlink;
    }
    while ( curlink != NULL );

    set_error_null();
}



void
CreateBinFile( void )
{
    char binfilenumber = '0';
    FILE *binaryfile;
    size_t codesize, codeblock, offset;
    SSTR_DEFINE( filename, FILENAME_MAX );

    if ( expl_binflnm == ON )
        /* use predined output filename from command line */
    {
        sstr_set( filename, binfilename );
    }
    else
    {
        /* create output filename, based on project filename */
        /* get source filename from first module */
        sstr_set( filename, modulehdr->first->cfile->fname );
        path_remove_ext( sstr_data( filename ) );
        sstr_sync_len( filename );

        if ( codesegment == ON && get_codesize() > 16384 )
        {
            sstr_cat( filename, FILEEXT_SEGBIN );   /* add '.bn0' extension */
        }
        else
        {
            sstr_cat( filename, FILEEXT_BIN );      /* add '.bin' extension */
        }
    }

    /* binary output to srcfilename.bin */
    binaryfile = fopen_err( sstr_data( filename ), "wb" );         /* CH_0012 */

    if ( autorelocate == ON && totaladdr != 0 )
    {
        fwritec_err( reloc_routine, sizeof_relocroutine, binaryfile );   /* relocate routine */
        *( reloctable + 0 ) = ( unsigned short ) totaladdr % 256U;
        *( reloctable + 1 ) = ( unsigned short ) totaladdr / 256U;    /* total of relocation elements */
        *( reloctable + 2 ) = ( unsigned short ) sizeof_reloctable % 256U;
        *( reloctable + 3 ) = ( unsigned short ) sizeof_reloctable / 256U; /* total size of relocation table elements */

        fwritec_err( reloctable, sizeof_reloctable + 4, binaryfile );    /* write relocation table, inclusive 4 byte header */
        printf( "Relocation header is %d bytes.\n", ( int )( sizeof_relocroutine + sizeof_reloctable + 4 ) );
        fwrite_codearea( binaryfile );                                /* write code as one big chunk */
        fclose( binaryfile );
        binaryfile = NULL;
    }
    else if ( codesegment == ON && get_codesize() > 16384 )
    {
        fclose( binaryfile );
        binaryfile = NULL;
        offset = 0;
        codesize = get_codesize();

        do
        {
            codeblock = ( codesize > 16384U ) ? 16384U : codesize;
            codesize -= codeblock;
            sstr_data( filename )[sstr_len( filename ) - 1] = binfilenumber++; /* path code file with number */
            binaryfile = fopen_err( sstr_data( filename ), "wb" );         /* CH_0012 */
            fwrite_codearea_chunk( binaryfile, offset, codeblock ); /* code in 16K chunks */
            fclose( binaryfile );
            binaryfile = NULL;

            offset += codeblock;
        }
        while ( codesize );
    }
    else
    {
        fwrite_codearea( binaryfile );                                /* write code as one big chunk */
        fclose( binaryfile );
        binaryfile = NULL;
    }

    if ( verbose )
    {
        puts( "Code generation completed." );
    }
}



void
CreateLib( void )
{
    long Codesize;
    FILE *objectfile = NULL;
    long fptr;
    char *filebuffer = NULL, *fname = NULL;

    if ( verbose )
    {
        puts( "Creating library..." );
    }

    CURRENTMODULE = modulehdr->first;

    path_replace_ext( errfilename, libfilename, FILEEXT_ERR );  /* overwrite '.lib' extension with '.err' */

    try
    {
        open_error_file( errfilename );

        do
        {
            set_error_null();
            set_error_module( CURRENTMODULE->mname );

            fname = CURRENTFILE->fname;
            strcpy( fname + strlen( fname ) - 4, objext );      /* overwrite '_asm' extension with '_obj' */

            objectfile = fopen_err( CURRENTFILE->fname, "rb" );           /* CH_0012 */
            fseek( objectfile, 0L, SEEK_END );  /* file pointer to end of file */
            Codesize = ftell( objectfile );
            fseek( objectfile, 0L, SEEK_SET );  /* file pointer to start of file */

            filebuffer = ( char * ) xmalloc( ( size_t ) Codesize );
            freadc_err( filebuffer, Codesize, objectfile ); /* load object file */
            fclose( objectfile );
            objectfile = NULL;

            if ( memcmp( filebuffer, Z80objhdr, 8U ) != 0 )
            {
                error( ERR_NOT_OBJ_FILE, CURRENTFILE->fname );
                break;
            }

            if ( verbose )
            {
                printf( "<%s> module at %04lX.\n", CURRENTFILE->fname, ftell( libfile ) );
            }

            if ( CURRENTMODULE->nextmodule == NULL )
            {
                fputl_err( -1, libfile );    /* this is the last module */
            }
            else
            {
                fptr = ftell( libfile ) + 4 + 4;
                fputl_err( fptr + Codesize, libfile ); /* file pointer to next module */
            }

            fputl_err( Codesize, libfile );    /* size of this module */
            fwritec_err( filebuffer, ( size_t ) Codesize, libfile ); /* write module to library */
            xfree( filebuffer );

            CURRENTMODULE = CURRENTMODULE->nextmodule;
        }
        while ( CURRENTMODULE != NULL );
    }

    finally
    {
        set_error_null();
        close_error_file();

        if ( objectfile )
        {
            fclose( objectfile );
            objectfile = NULL;
        }

        if ( filebuffer )
        {
            xfree( filebuffer );
        }
    }
}



int
LinkTracedModule( char *filename, long baseptr )
{
    struct linkedmod *newm;
    char *fname;

    if ( linkhdr == NULL )
    {
        linkhdr = xcalloc_struct( struct linklist );
        linkhdr->firstlink = NULL;
        linkhdr->lastlink = NULL;       /* Library header initialised */
    }

    fname = xstrdup( filename );        /* get a copy module file name */

    newm = xcalloc_struct( struct linkedmod );
    newm->nextlink = NULL;
    newm->objfilename = fname;
    newm->modulestart = baseptr;
    newm->moduleinfo = CURRENTMODULE;   /* pointer to current (active) module structure   */

    if ( linkhdr->firstlink == NULL )
    {
        linkhdr->firstlink = newm;
        linkhdr->lastlink = newm;       /* First module trace information */
    }
    else
    {
        linkhdr->lastlink->nextlink = newm;     /* current/last linked module points now at new current */
        linkhdr->lastlink = newm;               /* pointer to current linked module updated */
    }

    return 1;
}




void
DefineOrigin( void )
{
    printf( "ORG not yet defined!\nPlease enter as hexadecimal: " );
    scanf( "%lx", &modulehdr->first->origin );
}



void
CreateDeffile( void )
{
    char *globaldefname;

    /* use first module filename to create global definition file */
    globaldefname = xstrdup( modulehdr->first->cfile->fname );

    try
    {
        strcpy( globaldefname + strlen( globaldefname ) - 4, FILEEXT_DEF );       /* overwrite '_asm' extension with
                                                                            * '_def' */

        /* Create DEFC file with global label declarations */
        deffile = fopen_err( globaldefname, "w" );           /* CH_0012 */
    }

    finally
    {
        xfree( globaldefname );
    }
}


void
WriteMapFile( void )
{
    avltree *maproot = NULL, *newmaproot = NULL;
    struct module *cmodule;
    char *mapfilename;

    cmodule = modulehdr->first; /* begin with first module */

    mapfilename = xstrdup( cmodule->cfile->fname );
    strcpy( mapfilename + strlen( mapfilename ) - 4, FILEEXT_MAP );     /* overwrite '_asm' extension with '_map' */

    try
    {
        /* Create MAP file */
        mapfile = fopen_err( mapfilename, "w" );           /* CH_0012 */

        if ( verbose )
        {
            puts( "Creating map..." );
        }

        do
        {
            move( &cmodule->localroot, &maproot, ( int ( * )( void *, void * ) ) cmpidstr ); /* move all  local address symbols alphabetically */
            cmodule = cmodule->nextmodule;  /* alphabetically */
        }
        while ( cmodule != NULL );

        move( &globalroot, &maproot, ( int ( * )( void *, void * ) ) cmpidstr ); /* move all global address symbols alphabetically */

        if ( maproot == NULL )
        {
            fputs( "None.\n", mapfile );
        }
        else
        {
            inorder( maproot, ( void ( * )( void * ) ) WriteMapSymbol ); /* Write map symbols alphabetically */
            move( &maproot, &newmaproot, ( int ( * )( void *, void * ) ) cmpidval ); /* then re-order symbols numerically */
            fputs( "\n\n", mapfile );

            inorder( newmaproot, ( void ( * )( void * ) ) WriteMapSymbol ); /* Write map symbols numerically */
            deleteall( &newmaproot, ( void ( * )( void * ) ) FreeSym ); /* then release all map symbols */
        }

        fclose( mapfile );
    }

    finally
    {
        xfree( mapfilename );
    }
}



void
WriteMapSymbol( symbol *mapnode )
{
    int tabulators, space;

    if ( mapnode->type & SYMADDR )
    {
        fprintf( mapfile, "%s", mapnode->symname );
        space = COLUMN_WIDTH - strlen( mapnode->symname );

        for ( tabulators = space % TAB_DIST ? space / TAB_DIST + 1 : space / TAB_DIST;
                tabulators > 0; tabulators-- )
        {
            fputc_err( '\t', mapfile );
        }

        if ( autorelocate )
        {
            fprintf( mapfile, "= %04lX, ", sizeof_relocroutine + sizeof_reloctable + 4 + mapnode->symvalue );
        }
        else
        {
            fprintf( mapfile, "= %04lX, ", mapnode->symvalue );
        }

        if ( mapnode->type & SYMLOCAL )
        {
            fputc_err( 'L', mapfile );
        }
        else
        {
            fputc_err( 'G', mapfile );
        }

        fprintf( mapfile, ": %s\n", mapnode->owner->mname );
    }
}



void
WriteGlobal( symbol *node )
{
    int tabulators, space;

    if ( ( node->type & SYMTOUCHED ) && ( node->type & SYMADDR ) &&
            ( node->type & SYMXDEF ) && !( node->type & SYMDEF ) )
    {
        /* Write only global definitions - not library routines     */
        fprintf( deffile, "DEFC %s", node->symname );

        space = COLUMN_WIDTH - 5 - strlen( node->symname );

        for ( tabulators = space % TAB_DIST ? space / TAB_DIST + 1 : space / TAB_DIST; tabulators > 0; tabulators-- )
        {
            fputc_err( '\t', deffile );
        }

        fprintf( deffile, "= $%04lX; ", node->symvalue + modulehdr->first->origin + CURRENTMODULE->startoffset );
        fprintf( deffile, "Module %s\n", node->owner->mname );
    }
}



void
ReleaseLinkInfo( void )
{
    struct linkedmod *m, *n;

    if ( linkhdr == NULL )
    {
        return;
    }

    m = linkhdr->firstlink;

    while ( m != NULL )               /* move test to start in case list is empty */
    {
        if ( m->objfilename != NULL )
        {
            xfree( m->objfilename );
        }

        n = m->nextlink;
        xfree( m );
        m = n;
    }

    xfree( linkhdr );

    linkhdr = NULL;
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

