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

Copyright (C) Paulo Custodio, 2011-2012

Keep pool of strings for all duration of the program.
Most keywords in input program are the same, no need to keep several copies
and manage strdup/free for each token.
Strings with the same contents are reused.
*/

/* $Header: /cvsroot/z88dk/z88dk/src/z80asm/strpool.c,v 1.1 2012/05/24 17:50:02 pauloscustodio Exp $ */
/* $Log: strpool.c,v $
/* Revision 1.1  2012/05/24 17:50:02  pauloscustodio
/* CH_0010 : new string pool to hold strings for all program duration
/*
/*
/* */

#include "memalloc.h"   /* before any other include */
#include "strpool.h"
#include "class.h"
#include "types.h"
#include "uthash.h"
#include "queue.h"

/*-----------------------------------------------------------------------------
*   Dummy class to destroy hash_table atexit()
*----------------------------------------------------------------------------*/
CLASS( StrPool )
END_CLASS;
DEF_CLASS( StrPool );

static void StrHash_delete_all( void );

void StrPool_init( StrPool *self )   { }
void StrPool_copy( StrPool *self )   { }
void StrPool_fini( StrPool *self )
{
    StrHash_delete_all();
}

static StrPool *str_pool = NULL;

/*-----------------------------------------------------------------------------
*   Hash table
*----------------------------------------------------------------------------*/
typedef struct StrHash
{
    char    *string;        /* xstrdup */

    UT_hash_handle hh;      /* hash table */
} StrHash;

/* hash table of all strings */
static StrHash *hash_table = NULL;

/* add one string to the hash table, if not yet there; returns address */
static char *StrHash_add( char *string )
{
    StrHash *hash;
    size_t  num_chars = strlen( string );

    /* check if string exists already */
    HASH_FIND( hh, hash_table, string, num_chars, hash );

    if ( hash )
    {
        return hash->string;    /* found */
    }

    /* add to hash */
    hash = xcalloc_struct( StrHash );
    hash->string = xstrdup( string );   /* alloc string */
    HASH_ADD_KEYPTR( hh, hash_table, hash->string, num_chars, hash );

    return hash->string;
}

/* delete all strings */
static void StrHash_delete_all( void )
{
    StrHash *hash, *tmp;

    HASH_ITER( hh, hash_table, hash, tmp )
    {
        HASH_DEL( hash_table, hash );
        xfree( hash->string );
        xfree( hash );
    }
}

/*-----------------------------------------------------------------------------
*   Public interface
*----------------------------------------------------------------------------*/
char *strpool_add( char *string )
{
    strpool_init();
    return StrHash_add( string );
}

void strpool_init( void )
{
    /* create object to destory hash on first time */
    if ( str_pool == NULL )
    {
        str_pool = OBJ_NEW( StrPool );
    }
}
