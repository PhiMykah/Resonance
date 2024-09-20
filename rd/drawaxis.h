     
/* drawaxis.h: definitions for manual axis drawing.
 ***/

#define MAXLAB 32

struct DrawAxisInfo
   {
    float origin;
    float endpoint;
    float step;
    int   ticCount;
    int   unitsCode;
    char  fmt[MAXLAB+1];
    char  label[MAXLAB+1];
    int   useLimits;
    int   useStep;
    int   useTicCount;
    int   useUnits;
    int   useFmt;
    int   useLabel;
    int   horizFlag;
    int   boldFlag;
   };

#define DA_NULL (struct DrawAxisInfo *)NULL

int drawAxis( float gx1, float gy1,
              float gx3, float gy3,
              float px1, float py1,
              float px3, float py3,
              float lwAdj, 
              float xAdj, 
              float yAdj,
              float origin,
              float endpoint,
              char  *orient, 
              char  *title, 
              int   (*drLine)(  float gx1, float gy1, float gx3, float gy3, int vecType ),
              int   (*drTextR)( float gx1, float gy3, char *text, int bigFlag ),
              int   (*drTextB)( float gx1, float gy3, char *text, int bigFlag ),
              int   (*drSize)( char *text, float *gWidth, float *gHeight, int bigFlag ),
              int   useAxis,
              int   useGrid,
              struct DrawAxisInfo *daInfo );

int drawTic( float gx1, float gy1,
             float gx3, float gy3,
             float px1, float py1,
             float px3, float py3,
             float xAdj,   
             float yAdj,   
             float origin, 
             float endpoint,
             float ticVal,
             char  *orient,
             char  *title, 
             int   (*drLine)(  float gx1, float gy1, float gx3, float gy3, int vecType ),
             int   (*drTextR)( float gx1, float gy3, char *text, int bigFlag ),
             int   (*drTextB)( float gx1, float gy3, char *text, int bigFlag ),
             int   (*drSize)( char *text, float *gWidth, float *gHeight, int bigFlag ),
             int   useAxis,
             int   useGrid,
             struct DrawAxisInfo *daInfo );

int drawAxisTilt( float gx1, float gy1,
                  float gx3, float gy3,
                  float px1, float py1,
                  float px3, float py3,
                  float yAdj, float origin, float endpoint,
                  char *orient, char *title,
                  int (*drLine)( float gx1, float gy1, float gx3, float gy3, int vecType ),
                  int (*drText)( float gx1, float gy3, char *text, int bigFlag ),
                  int (*drSize)( char *text, float *gWidth, float *gHeight, int bigFlag ),
                  int useAxis, int useGrid,
                  struct DrawAxisInfo * daInfo );

int drawTicTilt( float gx1, float gy1,
                 float gx3, float gy3,
                 float px1, float py1,
                 float px3, float py3,
                 float yAdj, float origin, float endpoint,
                 float ticVal, char *orient, char *title,
                 int (*drLine)( float gx1, float gy1, float gx3, float gy3, int vecType ),
                 int (*drText)( float gx1, float gy3, char *text, int bigFlag ),
                 int (*drSize)( char *text, float *gWidth, float *gHeight, int bigFlag ),
                 int useAxis, int useGrid, 
                 struct DrawAxisInfo * daInfo );

int drawAxisNull( struct DrawAxisInfo *daInfo );
int drawAxisParse( char *str, struct DrawAxisInfo *daInfo );
