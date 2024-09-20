
/*
 * NMRPipe: NMRDrawNX contour drawing functions.
 *  Adapted from "A Contour Drawing Subroutine"
 *  Paul D. Bourke, BYTE June 1987.
 *  frank.delaglio@nist.gov (c) 1995-2020
 */

#define ICLIP( M, LO, HI ) if (M < LO) M = LO; if (M > HI) M = HI
#define ISWAP( I, J )      itemp = I; I = J; J = itemp;

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "memory.h"
#include "conrecnx.h"

static int genGridNX( float *grid, float gStart, float gEnd, int gCount );
 
static int firstTime = 1;

static int im[] = {0, 1, 1, 0};
static int jm[] = {0, 0, 1, 1};

static int cases[] = {0, 0, 8, 0, 2, 5, 7, 6, 9,
                      0, 3, 4, 1, 3, 1, 4, 3, 0,
                      9, 6, 7, 5, 2, 0, 8, 0, 0 };

#define XI(Q1,Q2) (h[Q1]*xh[Q2]-h[Q2]*xh[Q1])/(h[Q1]-h[Q2])
#define YI(Q1,Q2) (h[Q1]*yh[Q2]-h[Q2]*yh[Q1])/(h[Q1]-h[Q2])

static int casetab[3][3][3];

int conrecNX( float **d,         /* Array of pointers for data to render d[jub][iub].  */
              float *z,          /* Array of contour heights, ascending order.         */
              float *x,          /* Array of contour X grid sample locations.          */
              float *y,          /* Array of contour Y grid sample locations.          */
              int   iub,         /* Count of contour X grid sample locations.          */
              int   jub,         /* Count of contour Y grid sample locations.          */
              int   nc,          /* Number of contour levels.                          */
              int   (*drawVec)( float rx1, float ry1, float rxn, float ryn, int ii ),
              int (*stopFn)() )   /* Exit test: stopFn();                               */
{
    float xh[5], yh[5], h[5], x1, y1, x2, y2, dmin, dmax;
    int   ish[5], i, j, k, l, m1, m2, m3, m;

     
/* Initialize:
 ***/

    if (firstTime)
       {
        l = 0;

        for( k = 0; k < 3; k++ )
           {
            for( j = 0; j < 3; j++ )
               {
                for( i = 0; i < 3; i++ )
                   {
                    casetab[k][j][i] = cases[l++];
                   }
               }
           }

        firstTime = 0;
       }

     
/* Check input parameters:
 ***/

    if (iub < 2 || jub < 2) return( 1 );

    for( k = 0; k < nc - 1; k++ )
       {
        if (z[k] > z[k+1]) return( 2 );
       }

     
/*  Scan each cell of 4 points:
 *     Find highest and lowest point in cell.
 *     Test if any contours pass through cell.
 *       For all contours through cell:
 *         Find contour orientation relative to cell corners.
 *         Interpolate positions of contour vector through the cell.
 *         Draw the contour vector.
 ***/

    for( j = jub - 2; j >= 0; j-- )
       {
        if (stopFn && stopFn()) return( 1 );

        for( i = 0; i < iub - 1; i++ )
           {
            dmin = d[j][i]     < d[j+1][i] ? d[j][i]     : d[j+1][i];
            dmin = d[j][i+1]   < dmin      ? d[j][i+1]   : dmin;
            dmin = d[j+1][i+1] < dmin      ? d[j+1][i+1] : dmin;

            dmax = d[j][i]     > d[j+1][i] ? d[j][i]     : d[j+1][i];
            dmax = d[j][i+1]   > dmax      ? d[j][i+1]   : dmax;
            dmax = d[j+1][i+1] > dmax      ? d[j+1][i+1] : dmax;

            if (!(dmax < z[0] || dmin > z[nc-1]))
               {
                for( k = 0; k < nc; k++ )
                   {
                    if (!(z[k] < dmin || z[k] > dmax))
                       {
                        for( m = 4; m >= 0; m-- )
                           {
                            if (m)
                               {
                                h[m]  = d[j+jm[m-1]][i+im[m-1]] - z[k];
                                xh[m] = x[i+im[m-1]];
                                yh[m] = y[j+jm[m-1]];
                               }
                            else
                               {
                                h[0]  = 0.25*(h[1]+h[2]+h[3]+h[4]);
                                xh[0] = 0.50*(x[i] + x[i+1]);
                                yh[0] = 0.50*(y[j] + y[j+1]);
                               }

                            if (h[m] > 0)
                               ish[m] = 2;
                            else if (h[m] < 0)
                               ish[m] = 0;
                            else
                               ish[m] = 1;
                           }

                        for( m = 1; m <= 4; m++ )
                           {
                            m1 = m; m2 = 0; m3 = m + 1;

                            if (m3 == 5) m3 = 1;

                            switch( casetab[ish[m1]][ish[m2]][ish[m3]] )
                               {
                                case 0:
                                   break;
                                case 1:
                                   x1 = xh[m1];
                                   y1 = yh[m1]; 
                                   x2 = xh[m2];
                                   y2 = yh[m2];
                                   (void) drawVec( x1, y1, x2, y2, k );
                                   break;
                                case 2:
                                   x1 = xh[m2];
                                   y1 = yh[m2]; 
                                   x2 = xh[m3];
                                   y2 = yh[m3];
                                   (void) drawVec( x1, y1, x2, y2, k );
                                   break;
                                case 3:
                                   x1 = xh[m3];
                                   y1 = yh[m3]; 
                                   x2 = xh[m1];
                                   y2 = yh[m1];
                                   (void) drawVec( x1, y1, x2, y2, k );
                                   break;
                                case 4:
                                   x1 = xh[m1];
                                   y1 = yh[m1]; 
                                   x2 = XI( m3, m2 );
                                   y2 = YI( m3, m2 );
                                   (void) drawVec( x1, y1, x2, y2, k );
                                   break;
                                case 5:
                                   x1 = xh[m2];
                                   y1 = yh[m2]; 
                                   x2 = XI( m1, m3 );
                                   y2 = YI( m1, m3 );
                                   (void) drawVec( x1, y1, x2, y2, k );
                                   break;
                                case 6:
                                   x1 = xh[m3];
                                   y1 = yh[m3]; 
                                   x2 = XI( m2, m1 );
                                   y2 = YI( m2, m1 );
                                   (void) drawVec( x1, y1, x2, y2, k );
                                   break;
                                case 7:
                                   x1 = XI( m2, m1 );
                                   y1 = YI( m2, m1 );
                                   x2 = XI( m3, m2 );
                                   y2 = YI( m3, m2 );
                                   (void) drawVec( x1, y1, x2, y2, k );
                                   break;
                                case 8:
                                   x1 = XI( m3, m2 );
                                   y1 = YI( m3, m2 );
                                   x2 = XI( m1, m3 );
                                   y2 = YI( m1, m3 );
                                   (void) drawVec( x1, y1, x2, y2, k );
                                   break;
                                case 9:
                                   x1 = XI( m1, m3 );
                                   y1 = YI( m1, m3 );
                                   x2 = XI( m2, m1 );
                                   y2 = YI( m2, m1 );
                                   (void) drawVec( x1, y1, x2, y2, k ); 
                                   break;
                                default:
                                   break;
                               }
                           }
                       }
                   }
               }
           }
       }

    return( 0 );
}

int conrecNXInit( float *matrix,
                  float graphXStart, float graphXEnd,
                  float graphYStart, float graphYEnd,
                  int   dataXStart,  int   dataXEnd,
                  int   dataYStart,  int   dataYEnd,
                  float ***dPtr, float **xPtr, float **yPtr,
                  int dataXSize, int dataYSize )
{
    int   itemp;

     
/* Clip data coords:
 ***/

    ICLIP( dataXStart, 1, dataXSize );
    ICLIP( dataYStart, 1, dataYSize );
    ICLIP( dataXEnd,   1, dataXSize );
    ICLIP( dataYEnd,   1, dataYSize );

    if (dataXStart > dataXEnd)
       {
        ISWAP( dataXStart, dataXEnd );
       }

    if (dataYStart > dataYEnd)
       {
        ISWAP( dataYStart, dataYEnd );
       }

     
/* Allocate memory:
 ***/

    *dPtr = mat2PtrZ( matrix, dataXSize, dataXStart, dataYStart, dataYEnd );
    if (!(*dPtr)) return( 1 );

    if (!(*xPtr = fltAlloc( "conrec", 1 + dataXEnd - dataXStart ))) return( 1 );
    if (!(*yPtr = fltAlloc( "conrec", 1 + dataYEnd - dataYStart ))) return( 1 );

     
/* Initialize X Grid.
 * Initialize Y Grid.
 ***/

    (void) genGridNX( *xPtr, graphXStart, graphXEnd, 1 + dataXEnd - dataXStart );
    (void) genGridNX( *yPtr, graphYStart, graphYEnd, 1 + dataYEnd - dataYStart );

    return( 0 );
}

int conrecNXFree( float ***dPtr, float **xPtr, float **yPtr, int iub, int jub )
{

     
/* DeAllocate memory.
 ***/

    (void) deAlloc( "conrec", *dPtr, sizeof(float *)*jub );
    (void) deAlloc( "conrec", *xPtr, sizeof(float)*iub );
    (void) deAlloc( "conrec", *yPtr, sizeof(float)*jub );

    *dPtr = (float **) NULL;
    *xPtr = (float *)  NULL;
    *yPtr = (float *)  NULL;

    return( 0 );
}

     
/* genGridNX: generate equally spaced grid point locations.
 ***/

static int genGridNX( float *grid, float gStart, float gEnd, int gCount )
{
    float incr;

    if (gCount > 1)
       incr = (gEnd - gStart)/(float)(gCount - 1);
    else
       incr = 0.0;

    while( gCount-- )
       {
        *grid++ = gStart;
        gStart += incr;
       }

    return( 0 );
}

     
/* Bottom.
 ***/
