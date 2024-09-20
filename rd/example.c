
/*
 * Example: using the "readNMR" function to read a single NMRPipe-format file.
 * frank.delaglio@nist.gov 8/11/2024
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "fdatap.h"
#include "memory.h"
#include "cmndargs.h"
#include "specunit.h"
#include "readnmr.h"
#include "inquire.h"
#include "vutil.h"
#include "prec.h"
#include "rand.h"
#include "atof.h"

#include "nmrgraphics.h"

/*
 * inName:       File name of input data.
 * qSize:        (quadrature size) = 1 vectors are real, 2 = vectors are complex.
 * qSizeList[]:  qSize values for each dimension.
 * sizeList[]:   sizes of each dimension.
 * mat:          matrix of intensities.
 * fdata[]:      NMRPipe-format header (see fdatap.h).
 *
 * We use type NMR_INT for "long" integers, for example as needed to express the total
 * number of points in a large multidimensional NMR data set. Individual dimension sizes
 * are small enough to be expressed as 32-bit integers.
 *
 * Note that the NMRPipe format stores integer dimension sizes as 32-bit floats, so that
 * the max dimension size allowed is the max integer value that can be stored exactly as
 * a float (2^24 = 16,777,216).
 */

#define FPR (void)fprintf
#define PR  (void)printf

int exampleMain( int argc, char **argv )
{
   char    inName[NAMELEN+1];
   int     qSize, sizeList[MAXDIM], qSizeList[MAXDIM], dimCount; 
   float   *mat, fdata[FDATASIZE];

   NMR_INT totalSize;

   int     *indexList, vertexCount, indexCount;
   float   *vertexList;

   float minVal, maxVal;
   int   i, meshFlag, error;

/* Initialization. */

   error       = 0;
   meshFlag    = 0;

   mat         = (float *)NULL;
   vertexList  = (float *)NULL;
   indexList   = (int *)NULL;

   totalSize   = (NMR_INT)0;
   vertexCount = 0;
   indexCount  = 0;
   qSize       = 0;

   (void) strcpy( inName, "None" );

   (void) iSet( sizeList,  MAXDIM, 0 );
   (void) iSet( qSizeList, MAXDIM, 0 );

   (void) setHdrNull( fdata );

/* Help text. */

   if (flagLoc( argc, argv, "-help" ))
      {
       FPR( stderr, "Example: Use the readNMR function to read a single NMRPipe-format file;\n" );
       FPR( stderr, "Use the -mesh option to report mesh coords for 2D data:\n" );
       FPR( stderr, "\n" );
       FPR( stderr, " %s -in inName [-mesh]\n", argv[0] );
       return( 0 );
      }

/* Extract command-line parameters. */

   (void) strArgD( argc, argv, "-in",  inName );

   if (flagLoc( argc, argv, "-mesh" ))   meshFlag = 1;
   if (flagLoc( argc, argv, "-nomesh" )) meshFlag = 0;

/* 
 * Check if input file exists.
 * Read the input data, which allocates memory in mat.
 * Report max and min values (demo).
 */

   if (!fileExists( inName )) 
      {
       FPR( stderr, "Error finding input data %s\n", inName );
       error = 1;
       goto shutdown;
      }

   error = readNMR( inName, fdata, &mat, sizeList, qSizeList, &totalSize, &qSize, &dimCount );

   if (error)
      {
       FPR( stderr, "Error %d reading input data %s\n", error, inName );
       goto shutdown;
      }

   minVal = vecMin( mat, totalSize );
   maxVal = vecMax( mat, totalSize );

   FPR( stderr,
        "inName %s minVal %e maxVal %e dimCount %d xSize %d ySize %d zSize %d aSize %d qSize %d N %ld\n",
        inName, minVal, maxVal, dimCount, sizeList[XLOC], sizeList[YLOC], sizeList[ZLOC], sizeList[ALOC], qSize, (long)totalSize );

   if (meshFlag)
      {
       if (dimCount != 2 || sizeList[XLOC] < 2 || sizeList[YLOC] < 2)
          {
           FPR( stderr, "The -mesh option currently requires 2D data.\n" );
           goto shutdown;
          }

       error = mat2mesh( &vertexList, &vertexCount, &indexList, &indexCount,
                         mat, qSize*sizeList[XLOC], sizeList[YLOC],
                         minVal, maxVal, (float)0.01 );

       if (error)
          {
           FPR( stderr, "Error %d creating mesh.\n", error );
           goto shutdown;
          }

       i = 0;

       while( i < indexCount ) /* Print the three sets of x,y,z coords for a given tri. */
          {
           PR( " % 8.5f % 8.5f %8.5f\n",   vertexList[indexList[i]], vertexList[indexList[i]+1], vertexList[indexList[i]+2] );
           i++;

           PR( " % 8.5f % 8.5f %8.5f\n",   vertexList[indexList[i]], vertexList[indexList[i]+1], vertexList[indexList[i]+2] );
           i++;

           PR( " % 8.5f % 8.5f %8.5f\n\n", vertexList[indexList[i]], vertexList[indexList[i]+1], vertexList[indexList[i]+2] );
           i++;
          }
      }

/*
 * Deallocate data if needed before exit.
 */

shutdown:

   if (totalSize   > 0) (void) deAlloc( "nmr", mat,        sizeof(float)*totalSize );
   if (vertexCount > 0) (void) deAlloc( "nmr", vertexList, sizeof(float)*3*vertexCount );
   if (indexCount  > 0) (void) deAlloc( "nmr", indexList,  sizeof(int)*indexCount );

   return( error );
}

