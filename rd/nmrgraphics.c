
/*
 * Utilities for generating graphics from NMR data.
 * frank.delaglio@nist.gov 8/14/2024
 */

#include <math.h>
#include <stdio.h>

#include "memory.h"
#include "nmrgraphics.h"

/*
 * Create a triangle mesh from a 2D NMR matrix, with coords from -1.0 to 1.0 in all dimensions.
 * The XY plane of the 2D NMR matrix corresponds with the XZ plane of the graphics space.
 * Returns non-zero on error.
 */

int mat2mesh( float **vertexListPtr,  /* On return, allocated array of x,y,z vertex coords [3*vertexCount]. Use deAlloc() to deallocate. */
              int   *vertexCountPtr,  /* Number of vertices in vertexList.                                                               */
              int   **indexListPtr,   /* On return, allocated array of indices for mesh triangles (tris). Use deAlloc() to deallocate.   */
              int   *indexCountPtr,   /* Number of indices in indexList.                                                                 */
              float *mat,             /* Input 2D matrix.                                                                                */
              int   xSize,            /* X-axis size of 2D matrix.                                                                       */
              int   ySize,            /* Y-Axis size of 2D matrix.                                                                       */
              float minVal,           /* Lower value for clipping intensities in mat.                                                    */
              float maxVal,           /* Upper value for clipping intensities in mat.                                                    */
              float width1D )         /* Tri width for 1D mat data, 0.0 to 1.0.                                                          */
{
   int   *indexList, *iPtr, indexCount, vertexCount, ix, iy, locSW, locNW, locNE, locSE, error;
   float *vertexList, *xList, *yList, *mPtr, *vPtr, thisVal, scale, offset;

   error            = 0;
 
   *vertexListPtr   = (float *)NULL;
   *indexListPtr    = (int *)NULL;

   *vertexCountPtr  = 0;
   *indexCountPtr   = 0;

   vertexList       = (float *)NULL;
   indexList        = (int *)NULL;

   xList            = (float *)NULL;  /* Used to hold mat X-axis index converted to graphic X coord from -1.0 to  1.0 */
   yList            = (float *)NULL;  /* Used to hold mat Y-axis index converted to graphic Z coord from  1.0 to -1.0 */
 
   if (xSize < 1 || ySize < 1) return( 1 );

   if (!(xList = fltAlloc( "nmrgraphics", xSize )))
      {
       error = 2;
       goto shutdown;
      }

   if (!(yList = fltAlloc( "nmrgraphics", ySize )))
      {
       error = 3;
       goto shutdown;
      }

   if (xSize == 1 && ySize == 1)
      {
       vertexCount = 4;
       indexCount  = 6;
      }
   else if (xSize == 1)
      {
       vertexCount = 2*ySize;
       indexCount   = (ySize - 1)*3*2;
      }
   else if (ySize == 1)
      {
       vertexCount = 2*xSize;
       indexCount   = (xSize - 1)*2*3;
      }
   else
      {
       vertexCount = xSize*ySize;
       indexCount  = (xSize - 1)*(ySize - 1)*2*3; /* Each rectangle has two tris, each tri has 3 x,y,z coords. */
      }

   if (!(vertexList = fltAlloc( "nmrgraphics", 3*vertexCount )))
      {
       error = 4;
       goto shutdown;
      }

   *vertexListPtr  = vertexList;
   *vertexCountPtr = vertexCount;

   if (!(indexList = intAlloc( "nmrgraphics", indexCount )))
      {
       error = 4;
       goto shutdown;
      }

   *indexListPtr  = indexList;
   *indexCountPtr = indexCount;

/* For converting minVal ... maxVal to -1.0 ... 1.0 */

   scale  = minVal == maxVal ? 1.0 :  2.0/(maxVal - minVal);
   offset = minVal == maxVal ? 0.0 : -1.0;

   if (xSize == 1 && ySize == 1)
      {
      }
   else if (xSize == 1)
      {
      }
   else if (ySize == 1)
      {
      }
   else
      {
       (void) vGenCoords( xList, xSize, (float)-1.0, (float)1.0 );
       (void) vGenCoords( yList, ySize, (float)1.0,  (float)-1.0 );

       mPtr = mat;
       vPtr = vertexList;

       /* Get 3D coords for xSize*ySize points (vertices). */

       for( iy = 0; iy < ySize; iy++ )
          {
           for( ix = 0; ix < xSize; ix++ )
              {
               thisVal = *mPtr++;

               if (thisVal < minVal) thisVal = minVal;   /* Clip intensity value to requested range. */ 
               if (thisVal > maxVal) thisVal = maxVal;

               *vPtr++ = xList[ix];                          /* Graphics X coresponds to mat X-Axis.                       */
               *vPtr++ = scale*(thisVal - minVal) + offset;  /* Graphics Y correspond to mat value (intensity).            */
               *vPtr++ = yList[iy];                          /* Graphics Z correspond to mat Y-Axis, in reverse direction. */
              }
          }

       /* Get indices of 2 tris for each rectangle in the grid. Each index points to first value of an x,y,z triplet. */

       iPtr = indexList;

       for( iy = 0; iy < ySize - 1; iy++ )
          {
           for( ix = 0; ix < xSize - 1; ix++ )
              {
               locSW = ix*3 + iy*xSize*3; /* Corners of rectangle, south west, north west, etc. */
               locNW = locSW + xSize*3;
               locNE = locNW + 3;
               locSE = locSW + 3; 

               *iPtr++ = locSW;         /* Upper triangle, clockwise. */
               *iPtr++ = locNW;
               *iPtr++ = locNE;

               *iPtr++ = locSW;         /* Lower triangle, clockwise. */
               *iPtr++ = locNE;
               *iPtr++ = locSE;
              }
          }
      }

shutdown:

   if (xList) (void) deAlloc( "nmrgraphics", xList, sizeof(float)*xSize );
   if (yList) (void) deAlloc( "nmrgraphics", yList, sizeof(float)*ySize );
   
   return( error );
}

/* In progress. */

int findGridNormals( float **normXYZPtr,  /* On return, allocated matrix of normals x1 y1 z1 .... [xSize*ySize*3]. Use deAlloc() to deallocate. */
                     int   *normCountPtr, /* On return, size of allocated norm matrix (3*xSize*ySize)                                           */
                     float *mat,          /* Input 2D matrix [xSize*ySize]                                                                      */
                     int   xSize,         /* X-Axis size of mat                                                                                 */
                     int   ySize,         /* Y-Axis size of mat                                                                                 */
                     float minVal,        /* Lower clipping value for intensities in mat                                                        */
                     float maxVal )       /* Upper clipping value for intensities in mat                                                        */
{
   float *mPtr, *nPtr, *norm, sNormX, sNormY, sNormZ, sMag, dx, dy, dz, q;
   int   ix, iy, ixM, ixP, iyM, iyP;

/*
 * For NMR data, we'll approximate the normal using +/- 1 point in X, and then +/- 1 point in Y.
 * Special cases if data is a single point, or 1D.
 */

   *normXYZPtr   = (float *)NULL;
   *normCountPtr = 0;

   if (xSize < 1 || ySize < 1) return( 1 );

   if (!(norm = fltAlloc( "nmrgraphics", 3*xSize*ySize ))) return( 2 );

   *normCountPtr = 3*xSize*ySize;
   *normXYZPtr   = norm;

   if (xSize == 1 && ySize == 1)
      {
      }
   else if (xSize == 1)
      {
      }
   else if (ySize == 1)
      {
      }
   else
      {
       mPtr = mat; /* mPtr is the point at ix,iy. */
       nPtr = norm;

       for( iy = 0; iy < ySize; iy++ )
          {
           /* Point at ix,iy-1 is mPtr[iyM], point at ix,iy+1 is mPtr[iYP]. */

           iyP = iy == 0         ?  xSize*(ySize - 1) : -xSize;  /* Offset for Y coord + 1 relative to mPtr, wraps to other side at edge. */
           iyM = iy == ySize - 1 ? -xSize*(ySize - 1) :  xSize;  /* Offset for Y coord - 1 relative to mPtr, wraps to other side at edge. */

           for( ix = 0; ix < xSize; ix++ )
              {
               if (mPtr[0] < minVal || mPtr[0] > maxVal) /* Special case, clipped points have normals straight up along graphic Y. */
                  {
                   *nPtr++ = 0.0;
                   *nPtr++ = 1.0;
                   *nPtr++ = 0.0;

                   mPtr++;

                   continue;
                  }

               /* Point at ix-1,iy is mPtr[ixM], point at ix+1,iy is mPtr[ixP]. */

               ixM = ix == 0         ? xSize - 1 : -1; /* Offset for X coord - 1 relative to mPtr, wraps to other side at edge. */
               ixP = ix == xSize - 1 ? 0         :  1; /* Offset for X coord + 1 relative to mPtr, wraps to other side at edge. */

               /* Slope at point ix,iy along X-Axis is roughly same as line from point at ix-1,iy to point at ix+1,iy */
               /* This slope is s = (mPtr[ixP] - mPtr[ixM])/(1 - -1) and slope of normal is -1/s                      */

               dx     = mPtr[ixP] - mPtr[ixM];
               sNormX = dx == 0.0 ? 0.0 : (-2.0/dx)/sqrt( 4.0 + dx*dx ); /* X component in the XY grahpic plane. */

               dy     = mPtr[iyP] - mPtr[iyM];
               sNormY = dy == 0.0 ? 0.0 : (-2.0/dy)/sqrt( 4.0 + dy*dy ); /* Z component in the ZY graphic plane. */

               q = 1.0 - sNormX*sNormX + sNormY*sNormY;

               sNormZ = q <= 0.0 ? 0.0 : sqrt( q );
               sMag   = sqrt( sNormX*sNormX + sNormY*sNormY + sNormZ*sNormZ );

               if (sMag == 0.0) sMag = 1.0;

               *nPtr++ = sNormX/sMag;
               *nPtr++ = sNormZ/sMag;
               *nPtr++ = sNormY/sMag;

               mPtr++;
              }
          }
      }

   return( 0 );
}

/*
 * Generate a list of N values from val1 to valN.
 */

int vGenCoords( float *xList, int n, float val1, float valN )
{
   float range;
   int   i;

   if (n < 1) return( 0 );

/* Special case, return center for a single point. */

   if (n == 1)
      {
       xList[0] = 0.5*(val1 + valN);
       return( 0 );
      }

   range = val1 == valN ? 1.0 : valN - val1;

   for( i = 0; i < n; i++ ) xList[i] = val1 + range*i/(n - 1);

   return( 0 );
}

float vecMin64( src, size )

   float   *src;
   NMR_INT size;
{
    float   v;
    NMR_INT i;

    if (!src || size < 1) return( (float)0.0 );

    v = *src;

    for( i = 0; i < size; i++ )
       {
        if (*src < v) v = *src;
        src++;
       }

    return( v );
}

float vecMax64( src, size )

   float   *src;
   NMR_INT size;
{
    float   v;
    NMR_INT i;

    if (!src || size < 1) return( (float)0.0 );

    v = *src;

    for( i = 0; i < size; i++ )
       {
        if (*src > v) v = *src;
        src++;
       }

    return( v );
}

