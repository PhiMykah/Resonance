
#define PAPER_US     1
#define PAPER_METRIC 2

#define PAPER_A1   1
#define PAPER_A2   2
#define PAPER_A3   3
#define PAPER_A4   4
#define PAPER_A5   5
#define PAPER_A6   6
#define PAPER_A7   7

#define PAPER_A   -1
#define PAPER_B   -2
#define PAPER_C   -3
#define PAPER_D   -4
#define PAPER_E   -5
#define PAPER_U   -666

#define CM_PER_INCH  2.54
#define PP_PER_INCH 72.00

struct PaperInfo
   {
    char *name;
    int   code;
    float cmX,     cmY;
    float inchesX, inchesY;
    int   type;
   };

#ifdef USE_PAPERTYPES

static struct PaperInfo paperTypes[] =
   {
    "A1",         PAPER_A1, 59.40,  84.10,  23.385827, 33.110236, PAPER_METRIC,
    "A2",         PAPER_A2, 42.00,  59.40,  16.535433, 23.385827, PAPER_METRIC,
    "A3",         PAPER_A3, 29.70,  42.00,  11.692913, 16.535433, PAPER_METRIC,
    "A4",         PAPER_A4, 21.00,  29.70,   8.267716, 11.692913, PAPER_METRIC,
    "A5",         PAPER_A5, 14.80,  21.00,   5.826772,  8.267716, PAPER_METRIC,
    "A6",         PAPER_A6, 10.50,  14.80,   4.133858,  5.826772, PAPER_METRIC,
    "A7",         PAPER_A7,  7.40,  10.50,   2.913386,  4.133858, PAPER_METRIC,

    "A",          PAPER_A,  21.59,  27.94,   8.500000, 11.000000, PAPER_US,
    "B",          PAPER_B,  27.94,  43.18,  11.000000, 17.000000, PAPER_US,
    "C",          PAPER_C,  43.18,  55.88,  17.000000, 22.000000, PAPER_US,
    "D",          PAPER_D,  55.88,  86.36,  22.000000, 34.000000, PAPER_US,
    "E",          PAPER_E,  86.36, 111.76,  34.000000, 44.000000, PAPER_US,

    "U",          PAPER_U,  25.40,  25.40,  10.000000, 10.000000, PAPER_US,
    (char *)NULL, 0,         0.00,   0.00,   0.000000,  0.000000, 0
   };

#endif

#define PAPER_COUNT 13

struct PaperInfo *getPaperByName( char * name ), *getPaperByCode( int code ), *getPaperByLoc( int loc );











