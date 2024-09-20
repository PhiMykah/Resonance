     
/* Read single-file NMRPipe-format data. 
 ***/

#include <stdio.h>
#include <string.h>

#include "fdatap.h"
#include "dataio.h"
#include "inquire.h"
#include "memory.h"
#include "readnmr.h"
#include "prec.h"

/* Allocate and read entire matrix from single-file data:
 *  Space is allocated and data returned in matPtr.
 *  Effective dimension count is returned in dimCountPtr.
 *  Sizes of each dimension are returned in sizeList.
 *  Quad Sizes (1 Real, 2 Complex) of each dimension are returned in qSizeList.
 *  Total number of points is returned in totalPts.
 *  Quad size is returned in qSizePtr (1 Real, 2 Complex). 
 *  Note: previous version doubled X-axis size for complex data, this version does not.
 ***/

int readNMR( char *inName, float fdata[FDATASIZE], float **matPtr, int *sizeList, int *qSizeList, NMR_INT *totalPts, int *qSizePtr, int *dimCountPtr )
{
    int i, inUnit, error;

    inUnit       = UNIT_NULL;
    error        = 0;

    *matPtr      = (float *)NULL;
    *totalPts    = 0;
    *qSizePtr    = 0;
    *dimCountPtr = 0;

    for( i = 0; i < MAXDIM; i++ )
       {
        sizeList[i]  = 0;
        qSizeList[i] = 0;
       }

    if (!fileExists( inName ))
       {
        return( 1 );
       }

    if ((error = dataOpen( inName, &inUnit, FB_READ )))
       {
        return( 2 );
       }

    error = readNMRU( inUnit, fdata, matPtr, sizeList, qSizeList, totalPts, qSizePtr, dimCountPtr );

    return( error );
}

int readNMRU( int inUnit, float fdata[FDATASIZE], float **matPtr, int *sizeList, int *qSizeList, NMR_INT *totalPts, int *qSizePtr, int *dimCountPtr )
{
    int     i, error;
    float   *rPtr;
    NMR_INT n;

    error        = 0;
    *matPtr      = (float *)NULL;
    *totalPts    = 0;
    *qSizePtr    = 0;
    *dimCountPtr = 0;

    for( i = 0; i < MAXDIM; i++ )
       {
        sizeList[i]  = 0;
        qSizeList[i] = 0;
       }

    if ((error = rdFDATAU( inUnit, fdata ))) return( 3 );

    (void) getNMRParms( fdata, sizeList, qSizeList, totalPts, qSizePtr, dimCountPtr );

    if (!(rPtr = fltAlloc( "nmr", *totalPts ))) return( 4 );

    *matPtr  = rPtr;

    if ((error = dataRead( inUnit, rPtr, sizeof(float)*(*totalPts) ))) return( 5 );

    return( 0 );
}

int getNMRParms( float fdata[FDATASIZE], int *sizeList, int *qSizeList, NMR_INT *totalPts, int *qSizePtr, int *dimCountPtr )
{
    int     i, dimCount, pipeFlag, cubeFlag, quadSize, xQuadSize, yQuadSize, zQuadSize, aQuadSize, error;

    NMR_INT n;

    error        = 0;
    *totalPts    = 0;
    *qSizePtr    = 0;
    *dimCountPtr = 0;

    for( i = 0; i < MAXDIM; i++ )
       {
        qSizeList[i] = 1;
        sizeList[i]  = 1;
       }

    dimCount = getParm( fdata, FDDIMCOUNT, NULL_DIM );
    pipeFlag = getParm( fdata, FDPIPEFLAG, NULL_DIM );
    cubeFlag = getParm( fdata, FDCUBEFLAG, NULL_DIM );

    quadSize  = getQuad( fdata, FDQUADFLAG, NULL_DIM );
    xQuadSize = getQuad( fdata, NDQUADFLAG, CUR_XDIM );
    yQuadSize = getQuad( fdata, NDQUADFLAG, CUR_YDIM );
    zQuadSize = getQuad( fdata, NDQUADFLAG, CUR_ZDIM );
    aQuadSize = getQuad( fdata, NDQUADFLAG, CUR_ADIM );

    if (dimCount == 1)
       {
        sizeList[XLOC]  = getParm( fdata, NDSIZE, CUR_XDIM );
        qSizeList[XLOC] = xQuadSize;
       }
    else if (dimCount == 2)
       {
        sizeList[XLOC]  = getParm( fdata, NDSIZE, CUR_XDIM );
        sizeList[YLOC]  = getParm( fdata, NDSIZE, CUR_YDIM );

        qSizeList[XLOC] = xQuadSize;
        qSizeList[YLOC] = yQuadSize;
       }
    else if (dimCount == 3)
       {
        sizeList[XLOC] = getParm( fdata, NDSIZE, CUR_XDIM );
        sizeList[YLOC] = getParm( fdata, NDSIZE, CUR_YDIM );

        if (pipeFlag)
           {
            sizeList[ZLOC]  = getParm( fdata, NDSIZE, CUR_ZDIM );

            qSizeList[XLOC] = xQuadSize;
            qSizeList[YLOC] = yQuadSize;
            qSizeList[ZLOC] = zQuadSize;
           }
        else
           {
            qSizeList[XLOC] = xQuadSize;
            qSizeList[YLOC] = yQuadSize;

            dimCount = 2;
           }
       } 
    else if (dimCount == 4)
       {
        sizeList[XLOC] = getParm( fdata, NDSIZE, CUR_XDIM );
        sizeList[YLOC] = getParm( fdata, NDSIZE, CUR_YDIM );

        if (pipeFlag)
           {
            sizeList[ZLOC] = getParm( fdata, NDSIZE, CUR_ZDIM );
            sizeList[ALOC] = getParm( fdata, NDSIZE, CUR_ADIM );

            qSizeList[XLOC] = xQuadSize;
            qSizeList[YLOC] = yQuadSize;
            qSizeList[ZLOC] = zQuadSize;
            qSizeList[ALOC] = aQuadSize;
           }
        else if (cubeFlag)
           {
            sizeList[ZLOC]  = getParm( fdata, NDSIZE, CUR_ZDIM );

            qSizeList[XLOC] = xQuadSize;
            qSizeList[YLOC] = yQuadSize;
            qSizeList[ZLOC] = zQuadSize;

            dimCount = 3;
           }
        else
           {
            dimCount = 2;
           }
       }

    /* if (quadSize == 2) sizeList[XLOC] *= 2; previous version doubled size for quad data */

    if (dimCount == 2 && sizeList[YLOC] == 1) dimCount = 1;

    n = 1;

    for( i = 0; i < dimCount; i++ ) n *= sizeList[i];

    *dimCountPtr = dimCount;
    *totalPts    = n;
    *qSizePtr    = quadSize;

    return( 0 );
}

int writeNMR( char *outName, float fdata[FDATASIZE], float *mat, NMR_INT totalPts )
{
   int outUnit, error;

   error   = 0;
   outUnit = UNIT_NULL;
 
   if ((error = dataOpen( outName, &outUnit, FB_CREATE ))) return( error );
   if ((error = wrFDATAU( outUnit, fdata )))               return( error );

   if ((error = dataWrite( outUnit, mat, sizeof(float)*totalPts ))) return( error );

   (void) dataClose( outUnit );

   return( 0 );
}

