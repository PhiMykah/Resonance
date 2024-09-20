

#ifdef IBM
#include <sys/select.h>
#endif

#ifdef SOLARIS
#include <limits.h>
#undef  DTABLESIZE
#define DTABLESIZE OPEN_MAX
#endif

#ifdef WIN95 
#include <limits.h>
#undef  DTABLESIZE
#define DTABLESIZE 256
#endif

#ifdef WINNT
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/select.h>
#undef  DTABLESIZE
#define DTABLESIZE FD_SETSIZE
#endif

#ifdef HP 
#include <sys/stat.h>
#undef  DTABLESIZE
#define DTABLESIZE (size_t) 256
#endif

#ifdef LINUX
#include <sys/stat.h>
#define DTABLESIZE getdtablesize()
#endif

#ifdef WINXP 
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef _WIN
#undef  DTABLESIZE
#define DTABLESIZE 256
#endif // _WIN

#ifdef CONVEX
#include <sys/stat.h>
#endif

#ifdef ALPHA
#include <sys/stat.h>
#endif

#ifdef SGI
#include <sys/stat.h>
#endif

#ifdef SOLARIS
#include <sys/stat.h>
#endif

#ifdef CRAY
#include <sys/stat.h>
#endif

static  fd_set readfds, writefds;

#ifndef MAC_OSX_NEW
void    bZero(), bzero();
#endif

#define FDNULL (fd_set *) NULL
#define TONULL (struct timeval *) NULL

#if defined (SOLARIS) || defined (WIN95)
#define FD_ZERO2(p) bZero((char *)(p), (int)sizeof(*(p)))
#endif

#ifdef _WIN
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)  
#define FD_ZERO2(p) bzero((VOID *)(p), (int)sizeof(*(p)))
#endif // _WIN
#ifdef LINUX
#define FD_ZERO2(p) bzero((VOID *)(p), (int)sizeof(*(p)))
#endif // LINUX
