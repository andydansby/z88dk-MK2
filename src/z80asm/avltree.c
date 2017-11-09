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

/* $Header: /cvsroot/z88dk/z88dk/src/z80asm/avltree.c,v 1.10 2012/11/03 17:39:35 pauloscustodio Exp $ */
/* $Log: avltree.c,v $
/* Revision 1.10  2012/11/03 17:39:35  pauloscustodio
/* astyle, comments
/*
/* Revision 1.9  2012/05/24 17:09:27  pauloscustodio
/* Unify copyright header
/*
/* Revision 1.8  2012/05/20 06:02:08  pauloscustodio
/* Garbage collector
/* Added automatic garbage collection on exit and simple fence mechanism
/* to detect buffer underflow and overflow, to memalloc functions.
/* No longer needed to call init_malloc().
/* No longer need to try/catch during creation of memory structures to
/* free partially created data - all not freed data is freed atexit().
/* Renamed xfree0() to xfree().
/*
/* Revision 1.7  2012/05/11 19:29:49  pauloscustodio
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
/* Revision 1.6  2011/08/05 19:42:06  pauloscustodio
/* CH_0004 : Exception mechanism to handle fatal errors
/* Replaced all the memory allocation functions malloc, calloc, ... by corresponding
/* macros xmalloc, xcalloc, ... that raise an exception if the memory cannot be allocated,
/* removing all the test code after each memory allocation.
/* Replaced all free() by xfree0() macro which only frees if the pointer in non-null, and
/* sets the poiter to NULL afterwards, to avoid any used of the freed memory.
/*
/* Revision 1.5  2011/07/18 00:48:25  pauloscustodio
/* Initialize MS Visual Studio DEBUG build to show memory leaks on exit
/*
/* Revision 1.4  2011/07/09 18:25:35  pauloscustodio
/* Log keyword in checkin comment was expanded inside Log expansion... recursive
/* Added Z80asm banner to all source files
/*
/* Revision 1.3  2011/07/09 17:36:09  pauloscustodio
/* Copied cvs log into Log history
/*
/* Revision 1.2  2011/07/09 01:46:00  pauloscustodio
/* Added Log keyword
/*
/* Revision 1.1  2000/07/04 15:33:30  dom
/* branches:  1.1.1;
/* Initial revision
/* Revision 1.1.1.1  2000/07/04 15:33:30  dom
/* First import of z88dk into the sourceforge system <gulp>
/*
/* */

/* $History: AVLTREE.C $ */
/*  */
/* *****************  Version 4  ***************** */
/* User: Gbs          Date: 17-04-99   Time: 0:30 */
/* Updated in $/Z80asm */
/* New GNU programming style C format. Improved ANSI C coding style */
/* eliminating previous compiler warnings. New -o option. Asm sources file */
/* now parsed even though any line feed standards (CR,LF or CRLF) are */
/* used. */
/*  */
/* *****************  Version 2  ***************** */
/* User: Gbs          Date: 20-06-98   Time: 15:10 */
/* Updated in $/Z80asm */
/* SourceSafe Version History Comment Block added. */

#include "memalloc.h"   /* before any other include */

#include <stdlib.h>
#include "avltree.h"

/* Internal manipulation functions */
void rotateleft( avltree **p );
void rotateright( avltree **p );
void fixheight( avltree *p );
void balanceleft( avltree **p, short adj );
void balanceright( avltree **p, short adj );
void deletemin( avltree **p, void **data );
short difference( avltree *p );


/*
 * rotate nodes pointed to by x and x->right
 */
void
rotateleft( avltree **x )
{
    /* return *x to caller */
    avltree *y;

    if ( ( *x ) != NULL )
        if ( ( *x )->right != NULL )
        {
            y = ( *x )->right;
            ( *x )->right = y->left; /* left subtree of y becomes right subtree */
            y->left = ( *x );       /* x becomes left child of y */
            ( *x ) = y;             /* y becomes new root of whole subtree */
        }
}

/*
 * rotate nodes pointed to by x and x->left
 */
void
rotateright( avltree **x )
{
    /* return *x to caller */
    avltree *y;

    if ( ( *x ) != NULL )
        if ( ( *x )->left != NULL )
        {
            y = ( *x )->left;
            ( *x )->left = y->right; /* left subtree of y becomes right subtree */
            y->right = ( *x );      /* x becomes left child of y */
            ( *x ) = y;             /* y becomes new root of whole subtree */
        }
}

/*
 * return the difference between the heights of the left and right subtree of node n
 */
short
difference( avltree *n )
{
    short leftheight, rightheight;

    if ( n == NULL )
    {
        return 0;
    }
    else
    {
        if ( n->left == NULL )
        {
            leftheight = -1;
        }
        else
        {
            leftheight = n->left->height;    /* get height of left subtree */
        }

        if ( n->right == NULL )
        {
            rightheight = -1;
        }
        else
        {
            rightheight = n->right->height;    /* get height of right subtree */
        }

        return ( leftheight - rightheight );
    }
}

/*
 * sets the correct height for node pointed to by n, used after insertion into subtree
 */
void
fixheight( avltree *n )
{
    short leftheight, rightheight;

    if ( n->left == NULL )
    {
        leftheight = -1;
    }
    else
    {
        leftheight = n->left->height;
    }

    if ( n->right == NULL )
    {
        rightheight = -1;
    }
    else
    {
        rightheight = n->right->height;
    }

    if ( leftheight > rightheight )
    {
        n->height = leftheight + 1;
    }
    else
    {
        n->height = rightheight + 1;
    }
}

/*
 * restores balance at n after insertion, assuming that the right subtree of n is too high
 */
void
balanceright( avltree **n, short adjust )
{
    short dif;

    dif = difference( ( *n )->right );

    if ( dif == 0 )
    {
        rotateleft( n );          /* both subtrees of right child of n have same height */
        ( ( *n )->height ) -= adjust; /* 'decrease' height of current node */
        ( ( *n )->left->height ) += adjust; /* 'increase' height of left subtree */
    }
    else
    {
        if ( dif < 0 )
        {
            rotateleft( n );      /* right subtree of right child of n is higher */
            ( *n )->left->height -= 2;
        }
        else
        {
            /* left subtree of right child of n is higher */
            rotateright( &( *n )->right ); /* pointer to n->right */
            rotateleft( n );
            ++( ( *n )->height ); /* increase height of current node */
            ( *n )->left->height -= 2;
            --( ( *n )->right->height );  /* decrease height of right subtree */
        }
    }
}


void
balanceleft( avltree **n, short adjust )
{
    short dif;

    dif = difference( ( *n )->left );

    if ( dif == 0 )
    {
        rotateright( n );         /* both subtrees of left child of n have same height */
        ( ( *n )->height ) -= adjust; /* 'decrease' height of current node */
        ( ( *n )->right->height ) += adjust; /* 'increase' height of right subtree */
    }
    else
    {
        if ( dif > 0 )
        {
            rotateright( n );     /* left subtree of left child of n is higher */
            ( *n )->right->height -= 2;
        }
        else
        {
            /* right subtree of left child of n is higher */
            rotateleft( &( *n )->left );  /* pointer to n->left */
            rotateright( n );
            ++( ( *n )->height ); /* increase height of current node */
            ( *n )->right->height -= 2;
            --( ( *n )->left->height );   /* decrease height of left subtree */
        }
    }
}


void
deletemin( avltree **n, void **dataptr )
{
    avltree *temp;
    short dif;

    if ( ( *n )->left != NULL )   /* keep going for leftmost node */
    {
        deletemin( &( *n )->left, dataptr );
    }
    else
    {
        /* leftmost node found */
        *dataptr = ( *n )->data;  /* get pointer to data */
        temp = *n;
        *n = ( *n )->right;       /* return pointer to right subtree */
        xfree( temp );           /* of leftmost node                */
    }

    if ( *n != NULL )
    {
        fixheight( *n );
        dif = difference( *n );

        if ( dif > 1 )            /* deletion caused left subtree to be too high */
        {
            balanceleft( n, -1 );
        }
        else if ( dif < -1 )      /* deletion caused right subtree to be too high */
        {
            balanceright( n, -1 );
        }
    }
}


void
delete( avltree **n, void *key, int ( *comp )( void *, void * ), void ( *deletekey )( void * ) )
{
    avltree *temp;
    void *dataptr;                /* pointer to data record of avltree node */
    short dif, cmp;

    if ( *n != NULL )
    {
        cmp = comp( key, ( *n )->data );

        if ( cmp < 0 )
        {
            delete( &( *n )->left, key, comp, deletekey );
        }
        else
        {
            if ( cmp > 0 )
            {
                delete( &( *n )->right, key, comp, deletekey );
            }

            /* node to be deleted is found */
            else
            {
                if ( ( *n )->left != NULL && ( *n )->right != NULL )
                {
                    /* node has both left & right subtrees */
                    deletemin( &( *n )->right, &dataptr );
                    deletekey( ( *n )->data );    /* release old data */
                    ( *n )->data = dataptr;       /* assign new data */
                }
                else
                {
                    temp = *n;

                    if ( ( *n )->right == NULL )
                        if ( ( *n )->left == NULL )
                        {
                            *n = NULL;    /* node has no children */
                        }
                        else
                        {
                            *n = ( *n )->left;    /* node has left child only */
                        }
                    else
                    {
                        *n = ( *n )->right;    /* node has right child only */
                    }

                    deletekey( temp->data );      /* delete node data */
                    xfree( temp ); /* delete avltree node */
                }
            }
        }

        if ( *n != NULL )
        {
            fixheight( *n );
            dif = difference( *n );

            if ( dif > 1 )        /* deletion caused left subtree to be too high */
            {
                balanceleft( n, -1 );
            }
            else if ( dif < -1 )  /* deletion caused right subtree to be too high */
            {
                balanceright( n, -1 );
            }
        }
    }
}


void
deleteall( avltree **p, void ( *deldata )( void * ) )
{
    if ( *p != NULL )
    {
        deleteall( &( *p )->left, deldata );
        deleteall( &( *p )->right, deldata );

        deldata( ( *p )->data );
        xfree( *p );
        *p = NULL;
    }
}


/*
 * insert identifier in the subtree rooted at p
 */
void
insert( avltree **p, void *newdata, int ( *comp )( void *, void * ) )
{
    int cmp, dif;

    if ( *p == NULL )
    {
        *p = ( avltree * ) xmalloc( sizeof( avltree ) );

        if ( *p != NULL )
        {
            ( *p )->height = 0;
            ( *p )->data = newdata; /* new data linked to avltree node */
            ( *p )->left = NULL;  /* initialized to no subtrees */
            ( *p )->right = NULL;
        }
    }
    else
    {
        cmp = comp( newdata, ( *p )->data );

        if ( cmp <= 0 )
        {
            insert( &( *p )->left, newdata, comp );    /* put it in left subtree of p */
        }
        else if ( cmp > 0 )
        {
            insert( &( *p )->right, newdata, comp );    /* put it in right subtree of p */
        }

        fixheight( *p );          /* may have to adjust height if subtree grew */
        dif = difference( *p );

        if ( dif > 1 )            /* insertion caused left subtree to be too high */
        {
            balanceleft( p, 1 );
        }
        else if ( dif < -1 )      /* right subtree is too high */
        {
            balanceright( p, 1 );
        }
    }
}


/*
 * find identifier in the avltree rooted at p
 */
void *
find( avltree *p, void *key, int ( *comp )( void *, void * ) )
{
    int cmp;

    if ( p == NULL )
    {
        return NULL;
    }
    else
    {
        if ( ( cmp = comp( key, p->data ) ) == 0 )
        {
            return p->data;
        }
        else
        {
            if ( cmp < 0 )
            {
                return find( p->left, key, comp );    /* search left subtree of p */
            }
            else
            {
                return find( p->right, key, comp );    /* search right subtree of p */
            }
        }
    }
}


/*
 * interface function to move source avltree into destination avl-tree
 * source avltree will be empty, when completed.
 */
void
move( avltree **p, avltree **newroot, int ( *symcmp )( void *, void * ) )
{
    if ( *p != NULL )
    {
        move( &( *p )->left, newroot, symcmp );
        move( &( *p )->right, newroot, symcmp );

        insert( newroot, ( *p )->data, symcmp );  /* insert node data by symcmp order */
        xfree( *p );             /* release avl-node */
        *p = NULL;
    }
}


/*
 * interface function to copy source avltree into destination avl-tree
 */
void
copy( avltree *p, avltree **newroot, int ( *symcmp )( void *, void * ), void * ( *create )( void * ) )
{
    void *sym;

    if ( p != NULL )
    {
        copy( p->left, newroot, symcmp, create );
        copy( p->right, newroot, symcmp, create );
        sym = create( p->data );  /* create a copy of data */

        if ( sym != NULL )
        {
            insert( newroot, sym, symcmp );    /* insert node data by symcmp order */
        }
    }
}



/*
 * interface function to re-order avltree
 */
void *
reorder( avltree *p, int ( *symcmp )( void *, void * ) )
{
    avltree *newroot = NULL;

    move( &p, &newroot, symcmp ); /* re-order avl-tree */
    return newroot;               /* return pointer to new root */
}


/*
 * interface function to traverse the avltree in inorder (left, node, right)
 * and perform appropriate action for each data node.
 */
void
inorder( avltree *p, void ( *action )( void * ) )
{
    if ( p != NULL )
    {
        inorder( p->left, action );
        action( p->data );
        inorder( p->right, action );
    }
}


/*
 * interface function to traverse the avltree in preorder (node, left, right)
 * and perform appropriate action for each data node.
 */
void
preorder( avltree *p, void ( *action )( void * ) )
{
    if ( p != NULL )
    {
        action( p->data );
        inorder( p->left, action );
        inorder( p->right, action );
    }
}

