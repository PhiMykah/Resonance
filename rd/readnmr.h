
int readNMR( char *inName, float fdata[FDATASIZE], float **matPtr, int *sizeList, int *qSizeList, NMR_INT *totalPts, int *qSizePtr, int *dimCountPtr );
int readNMRU( int inUnit,  float fdata[FDATASIZE], float **matPtr, int *sizeList, int *qSizeList, NMR_INT *totalPts, int *qSizePtr, int *dimCountPtr );
int getNMRParms( float fdata[FDATASIZE], int *sizeList, int *qSizeList, NMR_INT *totalPts, int *qSizePtr, int *dimCountPtr );
