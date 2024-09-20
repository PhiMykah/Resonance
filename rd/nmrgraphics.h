
int mat2mesh( float **vertexListPtr,  /* On return, allocated array of x,y,z vertex coords [3*vertexCount]. Use deAlloc() to deallocate. */
              int   *vertexCountPtr,  /* On return, number of vertices in vertexList.                                                    */
              int   **indexListPtr,   /* On return, allocated array of indices for mesh triangles (tris). Use deAlloc() to deallocate.   */
              int   *indexCountPtr,   /* Number of indices in indexList.                                                                 */
              float *mat,             /* Input 2D matrix.                                                                                */
              int   xSize,            /* X-axis size of 2D matrix.                                                                       */
              int   ySize,            /* Y-Axis size of 2D matrix.                                                                       */
              float minVal,           /* Lower value for clipping intensities in mat.                                                    */
              float maxVal,           /* Upper value for clipping intensities in mat.                                                    */
              float width1D );

int findGridNormals( float **normXYZPtr,  /* On return, allocated matrix of normals x1 y1 z1 .... [xSize*ySize*3] */
                     int   *normCountPtr, /* On return, size of normal matrix (3*xSize*ySize)                    */       
                     float *mat,          /* Input 2D matrix [xSize*ySize]                                        */
                     int   xSize,         /* X-Axis size of mat                                                   */
                     int   ySize,         /* Y-Axis size of mat                                                   */
                     float minVal,        /* Lower clipping value for intensities in mat                          */
                     float maxVal );      /* Upper clipping value for intensities in mat                          */
    
int vGenCoords( float *xList, int n, float val1, float valN );

#define vecMin( V, N ) vecMin64( V, (NMR_INT)((NMR_INT)(N)) );
#define vecMax( V, N ) vecMax64( V, (NMR_INT)((NMR_INT)(N)) );

float vecMin64( float *v, NMR_INT n );
float vecMax64( float *v, NMR_INT n );
