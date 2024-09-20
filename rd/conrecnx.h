
int conrecNX( float **d,
              float *z,
              float *x,
              float *y,
              int   iub,
              int   jub,
              int   nc,
              int   (*drawVec)( float rx1, float ry1, float rxn, float ryn, int ii ),
              int   (*stopFn)() );

int conrecNXInit( float *matrix,
                  float graphXStart, float graphXEnd,
                  float graphYStart, float graphYEnd,
                  int   dataXStart,  int   dataXEnd,
                  int   dataYStart,  int   dataYEnd,
                  float ***dPtr, float **xPtr, float **yPtr,
                  int   dataXSize,   int dataYSize );

int conrecNXFree( float ***dPtr, float **xPtr, float **yPtr, int iub, int jub );
