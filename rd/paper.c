/******************************************************************************/
/*                                                                            */
/*                   ---- NIH NMR Software System ----                        */
/*                         Copyright 1992 - 1997                              */
/*                             Frank Delaglio                                 */
/*                   NIH Laboratory of Chemical Physics                       */
/*                                                                            */
/*               This software is not for distribution without                */
/*                  the written permission of the author.                     */
/*                                                                            */
/******************************************************************************/

#include <string.h>
#include <stdio.h>
#ifdef _WIN
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif // _WIN

#define USE_PAPERTYPES 

#include "paper.h"

//getPaperByName: look up the given paper name.


struct PaperInfo *getPaperByName( char * name )
{
    struct PaperInfo *info;

    for( info = paperTypes; info->name; info++ )
       {
        if (!strcasecmp( info->name, name )) return( info );
       }
 
    return( (struct PaperInfo *)NULL );
}

//getPaperByCode: look up the given paper code.

struct PaperInfo *getPaperByCode( int code )
{
    struct PaperInfo *info;

    for( info = paperTypes; info->name; info++ )
       {
        if (info->code == code) return( info );
       }

    return( (struct PaperInfo *)NULL );
}

//getPaperByLoc: look up the given paper by its location.


struct PaperInfo *getPaperByLoc( int loc )
{
    struct PaperInfo *info;

    if (loc < 0 || loc > PAPER_COUNT - 1) return( (struct PaperInfo *)NULL );

    return( paperTypes + loc );
}

//showPaperInfo: display paper types.

int showPaperInfo()
{
    struct PaperInfo *info;

    (void) fprintf( stderr, "Paper Types:\n" );

    for( info = paperTypes; info->name; info++ )
       {
        (void) fprintf( stderr,
                        " Size %2s %6.2f x %6.2f %s\n",
                        info->name,
                        info->type == PAPER_US ? info->inchesX : info->cmX,
                        info->type == PAPER_US ? info->inchesY : info->cmY,
                        info->type == PAPER_US ? "inches"      : "cm" );
       }

    return( 0 );
}
