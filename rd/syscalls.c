     
/* syscalls: some system-dependant calls grouped together for easier porting.
 ***/

#include <stdio.h>
#include <stdlib.h>
#ifdef LINUX
// Include 1
#include <unistd.h>
#include <regex.h>

// Include 2
#include <sys/types.h>
#include <sys/time.h>
#endif // LINUX

#include <sys/types.h>

#ifdef SOLARIS
#define const
#endif

#ifdef _WIN
#include <windows.h>
#endif // _WIN

#ifdef WIN95
#include <winsock.h>
#endif

// #ifdef MAC_OSX

#ifdef WINXP
#include <sys/types.h>
#include <limits.h>
#include <regex.h>
#endif

#include "memory.h"
#include "prec.h"

#if defined (HP) || defined (WINNT) || defined (SOLARIS) || defined (MAC_OSX)
#include <regex.h>
#endif

#define USE_USLEEP

#if defined (WINNT) || defined (WIN95)
#define SELECT_USLEEP
#endif

#ifdef HP
#undef USE_USLEEP
#endif

#ifdef SGI
#define SELECT_USLEEP
#endif

#ifdef SOLARIS
#include <libgen.h>
#define SELECT_USLEEP
#endif

#ifdef CONVEX
#undef USE_USLEEP
#endif

#ifdef CRAY
#undef USE_USLEEP
#endif

#define FDNULL (fd_set *) NULL

int re_exec2( char *s );

#ifdef ALPHA
int matherr()
{
   return( 0 );
}
#endif

     
/* Windows95 substitute for wait().
 ***/

#ifdef WIN95

int wait( i )

   int i;
{
    return( 0 );
}

#endif

     
/* uSleep: stop execution for given number of microseconds.
 ***/

int uSleep( uSecDelay )

   unsigned uSecDelay;
{
#ifndef USE_USLEEP 
    uSecDelay /= 1000000;
#endif

#ifdef USE_USLEEP
    #ifdef _WIN
    if (uSecDelay) (void) Sleep( uSecDelay * 1000 );
    #endif // _WIN
    #ifdef LINUX
    if (uSecDelay) (void) usleep( uSecDelay );
    #endif // LINUX
#else
    #ifdef _WIN
    if (uSecDelay) (void) Sleep( uSecDelay / 1000 );
    #endif // _WIN
    #ifdef LINUX
    if (uSecDelay) (void) sleep( uSecDelay );
    #endif // LINUX
#endif

    return( 0 );
}

     
/* usleep: SYSV replacement for BSD usleep function.
 ***/

#ifdef SELECT_USLEEP

int usleep( delay )

   unsigned delay;
{
    int    status;
    struct timeval timeOut;

    timeOut.tv_sec  = delay/1000000;
    timeOut.tv_usec = delay - timeOut.tv_sec*1000000;

    status = select( 0, FDNULL, FDNULL, FDNULL, &timeOut );

    return( status );
}
#endif

     
/* psignal: print a discription of a caught signal.
 ***/

#if defined(HP) || defined(WINXP)
int psignal( sig, str )

   int  sig;
   char str;
{
    (void) fprintf( stderr, "%s: signal %d\n", str, sig );
    return( 0 ); 
}
#endif

     
/* getwd: return current directory.
 ***/

#ifdef HP
char *getwd( path )

   char *path;
{
    (void) getcwd( path, NAMELEN );
    return( path );
}
#endif

#ifdef SOLARIS
char *getwd( path )

   char *path;
{
    (void) getcwd( path, NAMELEN );
    return( path );
}
#endif

#ifdef WINXP
char *getcwd( path, n )
   char   *path;
   size_t n;
{
   static char sBuff[PATH_MAX+1];
   char *sPtr;

   if (path)
      {
       sPtr = path;
       (void) getwd( sPtr );
      } 
   else
      {
       if (!(sPtr = charAlloc( "getcwd", PATH_MAX + 1 ))) sPtr = sBuff;
       (void) getwd( sPtr );
      }

   return( sPtr );
}
#endif

     
/* vfork: fork a process.
 ***/

#ifdef SGI
#ifndef XVIEW3
pid_t vfork()
{
    return( fork() );
}
#endif
#endif

#ifdef IBM 
pid_t vfork()
{
    return( fork() );
}
#endif

#ifdef HP 
#include <sys/resource.h>
int getdtablesize()
{
   return( RLIMIT_NOFILE );
}
#endif

#ifdef WINNT
double erf( x )

   double x;
{
   return( (float)0.0 );
}
#endif

     
/* re_comp: compile a regular expression.
 * re_exec: interpret string according to last compiled regular expression.
 ***/

#if defined (SOLARIS)

static char *regInfo = (char *)NULL;
static int  regValid = 0;

char *re_comp( reStr )

   char *reStr;
{
    if (regValid)
       {
#ifndef LINT_CHECK
        (void) regfree( &regInfo );
#endif
        regValid = 0;
       }

    regInfo = regcmp( reStr, (char *)NULL );

    if (!regInfo)
       {
        regValid = 0;
        return( "error" );
       }

    regValid = 1;

    return( (char *)NULL );
}

int re_exec( subjStr )

   char *subjStr;
{
    char *str;
    int status;

    if (!regValid) return( -1 );

    str = regex( regInfo, subjStr );

    if (!str) return( 0 );

    return( 1 );
}

#endif

#if defined (HP) || defined (WINNT) || defined (MAC_OSX) || defined(WINXP)

char *re_comp( reStr )

   char *reStr;
{
    char *sPtr;
    char *re_comp2();

    sPtr = re_comp2( reStr );

    return( sPtr ); 
}

int re_exec( subjStr )

   char *subjStr;
{
    int status;

    status = re_exec2( subjStr );

    return( status );
}
#endif

#if defined (HP) || defined (WINNT) || defined (SOLARIS) || defined (MAC_OSX) || defined(WINXP)

static regex_t regInfo2;
static int     regValid2 = 0;

char *re_comp2( reStr )

   char *reStr;
{
    if (regValid2)
       {
        (void) regfree( &regInfo2 );
        regValid2 = 0;
       }

    if (regcomp( &regInfo2, reStr, REG_NOSUB ))
       {
        regValid2 = 0;
        return( "error" );
       }

    regValid2 = 1;

    return( (char *)NULL );
}

int re_exec2( char *subjStr )
{
    int status;

    if (!regValid2) return( -1 );

    status = regexec( &regInfo2, subjStr, 0, NULL, (int)0 );

    if (!status) return( 1 );

    return( 0 );
}

#endif

     
/* bZero: replacement for BSD bzero.
 ***/

#if defined (SOLARIS) || defined (WIN95)

void bZero( sp, len )

   register char *sp;
   int      len;
{
    if (!sp)     return;
    if (len < 0) return;

    while( len-- ) *sp++ = '\0';
}

     
/* bZero: replacement for BSD bzero.
 ***/

void bCopy( s1, s2, len )

   register char *s1, *s2;
   int len;
{
    if (!s1) return;
    if (!s2) return;

    if (len < 0) return;

    while( len-- ) *s2++ = *s1++;
}

#endif

#if defined (WINNT) || defined (LINUX)
int _cleanup()
{
   return( 0 );
}
#endif

#ifdef WINXP
int svc_fdset()
{
   return( 0 );
}

int svc_getreqset()
{
   return( 0 );
}
#endif

     
/* Atof2: substitute for atof versions which fail with integer text.
 ***/

double Atof2( str )

   char *str;
{
    double d;

    (void) sscanf( str, "%lf", &d );
    return( d );
}

     
/* Dummy MAIN for F77 libraries.
 ***/

int MAIN__()
{
   return( 0 );
}

     
/* Bottom.
 ***/
