/******************************************************************************/
/*                                                                            */
/*                   ---- NIH NMR Software System ----                        */
/*                        Copyright 1992 and 1993                             */
/*                             Frank Delaglio                                 */
/*                   NIH Laboratory of Chemical Physics                       */
/*                                                                            */
/*               This software is not for distribution without                */
/*                  the written permission of the author.                     */
/*                                                                            */
/******************************************************************************/

     
/* drawAxis: a generic axis and grid line drawing tool; uses argument-supplied
 *           functions to draw vector graphics (tic marks) and text.  The axis
 *           graphics will be orthogonal to the X or Y axis of the graphics
 *           space.
 *
 * Graphics Coordinates:
 *
 *   The axis graphics space coordinates are specified in
 *   terms of the rectangular region (gx1,gy1) (gx3,gy3).
 *
 *   Likewise, the grid line graphics space coordinates are specified
 *   in terms of the rectangular region (px1,py1) (px4,py4).
 *
 *   The X coords are assumed to increase from left to right; the
 *   Y coords however can increase top to bottom or bottom to top.
 *
 * Graphics Function int drLine( gx1, gy1, gx3, gy3, vecType )
 *
 *   Draws a vector between the given coords; vecType will be
 *   zero for axis tic vectors, and one for grid vectors.
 *
 * Graphics Function int drText( gx1, gy1, text, bigFlag )
 *
 *   Draws the text whose lower right bound begins at the given coords.
 *   The value bigFlag will be one for requests to draw large size text.
 *   Large text is used exclusively for the axis title.
 *
 * Graphics Function int drSize( text, *gWidth, *gHeight, bigFlag )
 *
 *   Returns the sizes of the given text in the current graphic units.
 *   The value bigFlag will be one for requests to get size of large text. 
 ***/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef _WIN
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif // _WIN

#include "drawaxis.h"
#include "specunit.h"
#include "token.h"

#include "atof.h"
#include "prec.h"

#define IN_X(GX) \
   (((GX) >= gx1) && ((GX) <= gx3))

#define IN_Y(GY) \
   (gy1 > gy3 ? ((GY) <= gy1) && ((GY) >= gy3) : ((GY) >= gy1) && ((GY) <= gy3))

#if defined (SOLARIS) || !defined(SUN)
#define exp10( Q ) (pow( 10.0, (double)(Q) ))
#endif

static float tiltXOff( float x0, float y0, float gx1, float gy1, float gx2, float gy2 );
static float tiltYOff( float x0, float y0, float gx1, float gy1, float gx2, float gy2 );

int drawAxis( float gx1, float gy1,     /* Lower left graphics coord for axis.          */
              float gx3, float gy3,     /* Upper right graphics coord for axis.         */
              float px1, float py1,     /* Lower left graphics coord for grid.          */
              float px3, float py3,     /* Upper right graphics coord for grid.         */
              float lwAdj,              /* Adjustment for drawing line width.           */
              float xAdj,               /* Adjustment for horizontal char space, ~1.1.  */
              float yAdj,               /* Adjustment for vertical char space, ~1.1.    */
              float origin,             /* Origin of axis to draw.                      */
              float endpoint,           /* Endpoint of axis to draw.                    */
              char *orient,             /* Orientation of axis, "top", "bottom", etc.   */
              char *title,              /* Title label for the axis.                    */
              int (*drLine)(  float gx1, float gy1, float gx3, float gy3, int vecType ),  /* Procedure to draw a line.                    */
              int (*drTextR)( float gx1, float gy3, char *text, int bigFlag ),            /* Procedure to draw regular text.              */
              int (*drTextB)( float gx1, float gy3, char *text, int bigFlag ),            /* Procedure to draw bold text.                 */
              int (*drSize)( char *text, float *gWidth, float *gHeight, int bigFlag ),    /* Procedure which returns text sizes.          */
              int useAxis,                                                                /* Indicates whether to draw axis graphics.     */
              int useGrid,                                                                /* Indicates whether to draw grid lines.        */
              struct DrawAxisInfo *daInfo )                                               /* Overrides auto settings if non-null.         */
{
    float range, gWidth, gHeight, sign, step, mant, val, ticVal, maxVal, 
          ticX, ticY, maxX, maxY, txtX, txtY;

    int   i, n, maxCount, maxLength, ticCount, mult, firstMult, lastMult, multStep, length,
          ipow, iter, useStep, maxIter, boldFlag, horizFlag;

    char  ctemp[MAXLAB+1], fmt[MAXLAB+1], label[MAXLAB+1], *sPtr;

    int   (*drText)( float gx1, float gy3, char *text, int bigFlag );

     
/* Estimate how many position labels will probably fit.
 * Estimate a maximum number of positions (iterations) for pathological axis.
 ***/

    useStep   = 0;
    horizFlag = 0;
    boldFlag  = 0;

    (void) strcpy( label, title ); 

    if (daInfo)
       {
        horizFlag = daInfo->horizFlag;
        boldFlag  = daInfo->boldFlag;

        if (daInfo->useLabel)
           {
            (void) strcpy( label, daInfo->label ); 
           }

        if (daInfo->useLimits)
           {
            origin   = daInfo->origin;
            endpoint = daInfo->endpoint;
           }

        if (daInfo->useStep && daInfo->step != 0.0)
           {
            useStep = 1;

            if ((origin - endpoint)*daInfo->step > 0.0) daInfo->step = -daInfo->step;

            if (daInfo->step == 0.0) useStep = 0;
           }
       }

    drText = boldFlag ? drTextB : drTextR;
    maxVal = fabs((double)endpoint) > fabs((double)origin) ? endpoint : origin;
    range  = endpoint == origin ? 1.0 : endpoint - origin;

    (void) sprintf( ctemp, "__%+g_", maxVal );
    (void) drSize( ctemp, &gWidth, &gHeight, 0 );

    length = strlen( ctemp );

    switch( *orient )
       {
        case 'l':
           maxCount = fabs( (double)(gy3 - gy1)/(3.0*gHeight) );
           maxIter  = 2 + 2*fabs( (double)(gy3 - gy1)/gHeight );
           maxX     = gx3;
           maxY     = gy1;
           break;
        case 'r':
           maxCount = fabs( (double)(gy3 - gy1)/(3.0*gHeight) );
           maxIter  = 2 + 2*fabs( (double)(gy3 - gy1)/gHeight );
           maxX     = gx1;
           maxY     = gy1;
           break;
        case 't':
           maxCount = fabs( (double)(gx3 - gx1)/gWidth );
           maxIter  = 2 + 2*fabs( (double)(gx3 - gx1)/(gWidth/(1.0 + length)));
           maxX     = gx1;
           maxY     = gy1;
           break;
        case 'b':
           maxCount = fabs( (double)(gx3 - gx1)/gWidth );
           maxIter  = 2 + 2*fabs( (double)(gx3 - gx1)/(gWidth/(1.0 + length)));
           maxX     = gx1;
           maxY     = gy3;
           break;
        default:
           maxCount = 1;
           maxIter  = 1;
           maxX     = gx1;
           maxY     = gy1;
           break;
       }

     
/* Based on the estimated number of labels, find the smallest possible step.
 * Extract its sign, exponent and mantissa.
 * Round it up to a reasonable value.
 * Compute a format for the axis labels.
 ***/

    maxCount = maxCount == 0 ? 1 : maxCount;
    step     = useStep ? daInfo->step : range/(float)maxCount;

    sign     = step < 0.0 ? -1.0 : 1.0;
    ipow     = log10( (double)sign*step );
    mant     = sign*step/exp10( (double)ipow );
    ticCount = 0;

    if (mant < 1.0)
       {
        mant *= 10.0;
        ipow -= 1;
       }

    switch( (int)mant )
       {
        case 0: case 1:
           if (!useStep) mant = 1.0;
           ticCount = 9;
           break;
        case 2:
           if (!useStep) mant = 2.0;
           ticCount = 1;
           break;
        case 3: case 4:
           if (!useStep) mant = 5.0;
           ticCount = 4;
           break;
        case 5: case 6: case 7: case 8: case 9:
           if (!useStep)
              {
               mant     = 1.0;
               ipow    += 1;
              }
           ticCount = 9;
           break;
        default:
           break;
       }

    if (ipow < 0)
       {
        if (ipow < -4)
           (void) strcpy( fmt, "%5.3e" );
        else
           (void) sprintf( fmt, "%%.%df", -ipow );
       }
    else
       {
        if (ipow > 4)
           (void) strcpy( fmt, "%5.3e" );
        else
           (void) strcpy( fmt, "%.0f" );
       }

   if (daInfo)
      {
       if (daInfo->useFmt) (void) strcpy( fmt, daInfo->fmt );
      }

   if (!strcasecmp( fmt, "None" )) useAxis = 0;

     
/* Find the new step.
 * Find the first/last coordinates of the labels as multiples of the step.
 * Reduce the number of ticmarks if needed.
 ***/

    if (!useStep)
       {
        step      = mant*exp10( (double)ipow );
        step      = step == 0.0 ? range/2.0  : step;
        step      = step == 0.0 ? origin/2.0 : step;
       }

    firstMult = range < 0.0 ? (int)(origin/step)+1   : (int)(origin/step)-1;
    lastMult  = range < 0.0 ? (int)(endpoint/step)-1 : (int)(endpoint/step)+1;
    multStep  = firstMult > lastMult ? -1 : 1;

    if (useStep)
       {
        if (firstMult < lastMult)
           {
            firstMult -= 5;
            lastMult  += 5;
           }
        else
           {
            firstMult += 5;
            lastMult  -= 5;
           }
       }

    mult = firstMult;

    (void) sprintf( ctemp, fmt, maxVal );
    (void) drSize( ctemp, &gWidth, &gHeight, 0 );

    if (ticCount == 9)
       {
        switch( *orient )
           {
            case 'l': case 'r':
               if (2.5*gHeight > fabs((double)(gy3 - gy1)*step/range))
                  {
                   ticCount = 1;
                  }
               break;
            case 't': case 'b':
               if (1.5*gWidth > fabs((double)(gx3 - gx1)*step/range))
                  {
                   ticCount = 1;
                  }
               break;
            default:
               break;
           }
      }

   if (daInfo)
      {
       if (daInfo->useTicCount && daInfo->ticCount >= 0)
          {
           ticCount = daInfo->ticCount;
          }
      }

     
/* Loop through range of multiples of step to generate labels:
 *   Calculate placement of tic mark for this position label.
 *   Calculate placement of the position label text.
 *   Tally extreme label locations, used later to place axis title.
 *  
 *   If axis drawing is enabled:
 *      Draw major tic mark and text.
 *      Draw minor tic marks.
 *
 *   If grid drawing is enabled:
 *      Draw grid lines.  
 ***/

    for( iter = 0; iter < maxIter && mult != lastMult; iter++ )
       {
        val = step*mult;

        (void) sprintf( ctemp, fmt, val );
        (void) drSize( ctemp, &gWidth, &gHeight, 0 );
       
        length = strlen( ctemp );
    
        switch( *orient )
           {
            case 'l':
               ticX = gx3 - gWidth/(1.0 + length);
               ticY = gy1 + (val - origin)*(gy3 - gy1)/range;
               txtX = gx3 - gWidth - 2.0*gWidth/(1.0 + length);
               txtY = gy1 > gy3 ? ticY + gHeight/2.0 : ticY - gHeight/2.0;

               maxX = maxX > txtX ? txtX : maxX; 

               if (IN_Y( ticY ))
                  {              
                   if (useAxis)
                      { 
                       (void)drLine( (float)(gx3-lwAdj), ticY, (float)(ticX-lwAdj), ticY, 0 );
                       (void)drText( (float)(txtX-lwAdj), txtY, ctemp, 0 );
                      }

                   if (useGrid)
                      {
                       (void)drLine( (float)(px1+lwAdj), ticY, (float)(px3-lwAdj), ticY, 1 );
                      }
                  }

               for( i = 1; i <= ticCount; i++ )
                  {
                   ticVal = val + sign*step*i/(float)(ticCount + 1);
                   ticY   = gy1 + (ticVal - origin)*(gy3 - gy1)/range;
                   ticX   = i == 5 ? gx3 - 0.75*gWidth/(1.0 + length) :
                                       gx3 - 0.50*gWidth/(1.0 + length);

                   if (IN_Y( ticY ))
                      {
                       if (useAxis)
                          {
                           (void)drLine( (float)(gx3-lwAdj), ticY, (float)(ticX-lwAdj), ticY, 0 );
                          }
                      }
                  }

               break;

            case 'r':
               ticX = gx1 + gWidth/(1 + strlen(ctemp));
               ticY = gy1 + (val - origin)*(gy3 - gy1)/range;
               txtX = gx1 + 2.0*gWidth/(1.0 + length);
               txtY = gy1 > gy3 ? ticY + gHeight/2.0 : ticY - gHeight/2.0;

               maxX = maxX < gx1 + gWidth ? gx1 + gWidth : maxX;

               if (IN_Y( ticY ))
                  {
                   if (useAxis)
                      {
                       (void)drLine( (float)(gx1+lwAdj), ticY, (float)(ticX+lwAdj), ticY, 0 );
                       (void)drText( (float)(txtX+lwAdj), txtY, ctemp, 0 );
                      }

                   if (useGrid)
                      {
                       (void) drLine( (float)(px1+lwAdj), ticY, (float)(px3-lwAdj), ticY, 1 );
                      }
                  }

               for( i = 1; i <= ticCount; i++ )
                  {
                   ticVal = val + sign*step*i/(float)(ticCount + 1);
                   ticY   = gy1 + (ticVal - origin)*(gy3 - gy1)/range;
                   ticX   = i == 5 ? gx1 + 0.75*gWidth/(1.0 + length) : gx1 + 0.50*gWidth/(1.0 + length);

                   if (IN_Y( ticY ))
                      {
                       if (useAxis)
                          {
                           (void)drLine( (float)(gx1+lwAdj), ticY, (float)(ticX+lwAdj), ticY, 0 );
                          }
                      }
                  }

               break;

            case 't':
               ticX = gx1 + (val - origin)*(gx3 - gx1)/range;
               ticY = gy1 > gy3 ? gy1 - gHeight/2.0 : gy1 - gHeight/2.0;

               txtX = ticX - gWidth/2;
               txtY = gy1 > gy3 ? gy1 - gHeight : gy1 - gHeight;

               maxY = gy1 > gy3 ? txtY - gHeight : txtY + gHeight;

               if (IN_X( ticX ))
                  {       
                   if (useAxis)
                      {
                       (void)drLine( ticX, (float)(gy1-lwAdj), ticX, (float)(ticY-lwAdj), 0 );
                       (void)drText( txtX, (float)(txtY-lwAdj), ctemp, 0 );
                      }

                   if (useGrid)
                      {
                       (void)drLine( ticX, (float)(py3+lwAdj), ticX, (float)(py1-lwAdj), 1 );
                      }
                  }

               for( i = 1; i <= ticCount; i++ )
                  {
                   ticVal = val + sign*step*i/(float)(ticCount + 1);
                   ticX   = gx1 + (ticVal - origin)*(gx3 - gx1)/range;
                   ticY   = i == 5 ? (gy1 > gy3 ? gy1-gHeight/2.5 : gy1-gHeight/2.5) : (gy1 > gy3 ? gy1-gHeight/4 : gy1-gHeight/4);

                   if (IN_X( ticX ))
                      {
                       if (useAxis)
                          {
                           (void)drLine( ticX, (float)(gy1-lwAdj), ticX, (float)(ticY-lwAdj), 0 );
                          }
                      }
                  }

               break;

            case 'b':
               ticX = gx1 + (val - origin)*(gx3 - gx1)/range;
               ticY = gy1 > gy3 ? gy3 + gHeight/2.0 : gy3 - gHeight/2.0;

               txtX = ticX - gWidth/2;
               txtY = gy1 > gy3 ? gy3 + 2.0*gHeight : gy3 - 2.0*gHeight;

               maxY = txtY;

               if (IN_X( ticX ))
                  {
                   if (useAxis)
                      {
                       (void)drLine( ticX, (float)(gy3+lwAdj), ticX, (float)(ticY+lwAdj), 0 );
                       (void)drText( txtX, txtY, ctemp, 0 );
                      } 
 
                   if (useGrid)
                      {
                       (void)drLine( ticX, (float)(py3+lwAdj), ticX, (float)(py1-lwAdj), 1 );
                      }
                  }

               for( i = 1; i <= ticCount; i++ )
                  {
                   ticVal = val + sign*step*i/(float)(ticCount + 1);
                   ticX   = gx1 + (ticVal - origin)*(gx3 - gx1)/range;
                   ticY   = i == 5 ? (gy1>gy3?gy3+gHeight/2.5:gy3-gHeight/2.5) : (gy1>gy3?gy3+gHeight/4:gy3-gHeight/4);

                   if (IN_X( ticX ))
                      {
                       if (useAxis)
                          {
                           (void)drLine( ticX, (float)(gy3+lwAdj), ticX, (float)(ticY+lwAdj), 0 );
                          }
                      }
                  }

               break;

            default:
               break;
           }

        mult += multStep;
       }

     
/* Draw the axis title:
 ***/

    (void) drSize( label, &gWidth, &gHeight, 1 );
     
    length    = strlen( label );
    maxLength = length;
    n         = 1;

    gWidth   *= xAdj;
    gHeight  *= yAdj;

    switch( *orient )
       {
        case 'l':
           if (useAxis)
              {
               if (horizFlag)
                  {
                   maxLength = getTokenMaxLength( label );
                   n         = cntToken( label );

                   txtX = maxX - gWidth*(maxLength + 1)/(length + 1);
                   txtY = gy1 > gy3 ? (gy1 + gy3 - 0.5*gHeight*n)/2.0 : (gy1 + gy3 + 0.5*gHeight*n)/2.0;

                   for( i = 1; i <= n; i++ )
                      {
                       (void) strcpy( ctemp, getToken( i, label ));
                       (void) drText( txtX, txtY, ctemp, 1 );
                       txtY += gHeight;
                      }
                  }
               else
                  {
                   txtX = maxX - 3.0*gWidth/(1.0 + length);
                   txtY = gy1 > gy3 ? (gy1+gy3-gHeight*length)/2.0 : (gy1+gy3+gHeight*length)/2.0;
                   sPtr = label;

                   while( *sPtr )
                      {
                       (void) sprintf( ctemp, "%c", *sPtr );
                       (void) drText( (float)(txtX-lwAdj), txtY, ctemp, 1 );

                       txtY = gy1 > gy3 ? txtY + gHeight : txtY - gHeight;
                       sPtr++;
                      }
                  }
              }

           break;

        case 'r':
           if (useAxis)
              {
               if (horizFlag)
                  {
                   maxLength = getTokenMaxLength( label );
                   n         = cntToken( label );

                   txtX = maxX + 3.0*gWidth/(length + 1);
                   txtY = gy1 > gy3 ? (gy1 + gy3 - 0.5*gHeight*n)/2.0 : (gy1 + gy3 + 0.5*gHeight*n)/2.0;
 
                   for( i = 1; i <= n; i++ ) 
                      {
                       (void) strcpy( ctemp, getToken( i, label ));
                       (void) drText( txtX, txtY, ctemp, 1 );
                       txtY += gHeight;
                      }
                  }
               else
                  {
                   txtX = maxX + 3.0*gWidth/(1.0 + length);
                   txtY = gy1 > gy3 ? (gy1+gy3-gHeight*length)/2.0 : (gy1+gy3+gHeight*length)/2.0;
                   sPtr = label;

                   while( *sPtr )
                      {
                       (void) sprintf( ctemp, "%c", *sPtr );
                       (void) drText( (float)(txtX+lwAdj), txtY, ctemp, 1 );

                       txtY = gy1 > gy3 ? txtY + gHeight : txtY - gHeight;
                       sPtr++;
                      }
                  }
              }

           break;

        case 't':
           txtX = (gx3 + gx1 - gWidth)/2.0;
           txtY = gy1 > gy3 ? maxY - 0.5*gHeight : maxY - 2.5*gHeight;

           if (useAxis)
              {
               (void) drText( txtX, (float)(txtY-lwAdj), label, 1 );
              }

           break;

        case 'b':
           txtX = (gx3 + gx1 - gWidth)/2.0;
           txtY = gy1 > gy3 ? maxY + 1.25*gHeight : maxY - 1.25*gHeight;

           if (useAxis)
              {
               (void) drText( txtX, (float)(txtY+lwAdj), label, 1 );
              }

           break;

        default:
           break;
       }

    return( 0 );
}

     
/* drawAxisTilt: like drawAxis, but the axis is drawn along the
 *               line from gx1,gy1 to gx3,gy3.
 ***/

int drawAxisTilt( float gx1, float gy1, float gx3, float gy3,  
                  float px1, float py1, float px3, float py3,
                  float yAdj, float origin, float endpoint, char *orient, char *title,  
                  int (*drLine)( float gx1, float gy1, float gx3, float gy3, int vecType ),
                  int (*drText)( float gx1, float gy3, char *text, int bigFlag ),
                  int (*drSize)( char *text, float *gWidth, float *gHeight, int bigFlag ),
                  int useAxis, int useGrid,
                  struct DrawAxisInfo * daInfo ) 
{
    float range, gWidth, gHeight, sign, step, mant, val, ticVal, maxVal, 
          ticX, ticY, maxX, maxY, txtX, txtY, xOff, yOff;

    int   i, maxCount, ticCount, mult, firstMult, lastMult, multStep, length,
          ipow, iter, useStep, maxIter;

    char  ctemp[MAXLAB+1], fmt[MAXLAB+1], label[MAXLAB+1], *sPtr;

     
/* Estimate how many position labels will probably fit.
 * Estimate a maximum number of positions (iterations) for pathological axis.
 ***/

    useStep = 0;

    (void) strcpy( label, title ); 

    if (daInfo)
       {
        if (daInfo->useLabel)
           {
            (void) strcpy( label, daInfo->label ); 
           }

        if (daInfo->useLimits)
           {
            origin   = daInfo->origin;
            endpoint = daInfo->endpoint;
           }

        if (daInfo->useStep && daInfo->step != 0.0)
           {
            useStep = 1;

            if ((origin - endpoint)*daInfo->step > 0.0)
               {
                daInfo->step = -daInfo->step;
               }
           }
       }

    maxVal = fabs((double)endpoint) > fabs((double)origin) ? endpoint : origin;
    range  = endpoint == origin ? 1.0 : endpoint - origin;

    (void) sprintf( ctemp, "__%+g_", maxVal );
    (void) drSize( ctemp, &gWidth, &gHeight, 0 );

    length = strlen( ctemp );

    switch( *orient )
       {
        case 'l':
           maxCount = fabs( (double)(gy3 - gy1)/(3.0*gHeight) );
           maxIter  = 2 + 2*fabs( (double)(gy3 - gy1)/gHeight );
           maxX     = gx3;
           maxY     = gy1;
           break;
        case 'r':
           maxCount = fabs( (double)(gy3 - gy1)/(3.0*gHeight) );
           maxIter  = 2 + 2*fabs( (double)(gy3 - gy1)/gHeight );
           maxX     = gx1;
           maxY     = gy1;
           break;
        case 't':
           maxCount = fabs( (double)(gx3 - gx1)/gWidth );
           maxIter  = 2 + 2*fabs( (double)(gx3 - gx1)/(gWidth/(1.0 + length)));
           maxX     = gx1;
           maxY     = gy1;
           break;
        case 'b':
           maxCount = fabs( (double)(gx3 - gx1)/gWidth );
           maxIter  = 2 + 2*fabs( (double)(gx3 - gx1)/(gWidth/(1.0 + length)));
           maxX     = gx1;
           maxY     = gy3;
           break;
        default:
           maxCount = 1;
           maxIter  = 1;
           maxX     = gx1;
           maxY     = gy1;
           break;
       }

     
/* Based on the estimated number of labels, find the smallest possible step.
 * Extract its sign, exponent and mantissa.
 * Round it up to a reasonable value.
 * Compute a format for the axis labels.
 ***/

    maxCount = maxCount == 0 ? 1 : maxCount;
    step     = range/(float)maxCount;

    if (daInfo)
       {
        if (daInfo->useStep && daInfo->step != 0.0) step = daInfo->step;
       }

    sign     = step < 0.0 ? -1.0 : 1.0;
    ipow     = log10( (double)sign*step );
    mant     = sign*step/exp10( (double)ipow );
    ticCount = 0;

    if (mant < 1.0)
       {
        mant *= 10.0;
        ipow -= 1;
       }

    switch( (int)mant )
       {
        case 0: case 1:
           if (!useStep) mant = 1.0;
           ticCount = 9;
           break;
        case 2:
           if (!useStep) mant = 2.0;
           ticCount = 1;
           break;
        case 3: case 4:
           if (!useStep) mant = 5.0;
           ticCount = 4;
           break;
        case 5: case 6: case 7: case 8: case 9:
           if (!useStep)
              {
               mant     = 1.0;
               ipow    += 1;
              }
           ticCount = 9;
           break;
        default:
           break;
       }

    if (ipow < 0)
       {
        if (ipow < -4)
           (void) strcpy( fmt, "%5.3e" );
        else
           (void) sprintf( fmt, "%%.%df", -ipow );
       }
    else
       {
        if (ipow > 4)
           (void) strcpy( fmt, "%5.3e" );
        else
           (void) strcpy( fmt, "%.0f" );
       }

   if (daInfo)
      {
       if (daInfo->useFmt) (void) strcpy( fmt, daInfo->fmt );
      }

   if (!strcasecmp( fmt, "None" )) useAxis = 0;

     
/* Find the new step.
 * Find the first/last coordinates of the labels as multiples of the step.
 * Reduce the number of ticmarks if needed.
 ***/

    step      = mant*exp10( (double)ipow );
    step      = step == 0.0 ? range/2.0  : step;
    step      = step == 0.0 ? origin/2.0 : step;

    if (daInfo)
       {
        if (daInfo->useStep && daInfo->step != 0.0) step = daInfo->step;
       }

    firstMult = range < 0.0 ? (int)(origin/step)+1   : (int)(origin/step)-1;
    lastMult  = range < 0.0 ? (int)(endpoint/step)-1 : (int)(endpoint/step)+1;
    multStep  = firstMult > lastMult ? -1 : 1;
    mult      = firstMult;

    if (daInfo)
       {
        if (daInfo->useStep)
           {
            if (firstMult < lastMult)
               {
                firstMult--;
                lastMult++;
               }
            else
               {
                firstMult++;
                lastMult--;
               }
           }
       }

    (void) sprintf( ctemp, fmt, maxVal );
    (void) drSize( ctemp, &gWidth, &gHeight, 0 );

    if (ticCount == 9)
       {
        switch( *orient )
           {
            case 'l': case 'r':
               if (2.5*gHeight > fabs((double)(gy3 - gy1)*step/range))
                  {
                   ticCount = 1;
                  }
               break;
            case 't': case 'b':
               if (1.5*gWidth > fabs((double)(gx3 - gx1)*step/range))
                  {
                   ticCount = 1;
                  }
               break;
            default:
               break;
           }
      }

   if (daInfo)
      {
       if (daInfo->useTicCount && daInfo->ticCount >= 0)
          {
           ticCount = daInfo->ticCount;
          }
      }

     
/* Loop through range of multiples of step to generate labels:
 *   Calculate placement of tic mark for this position label.
 *   Calculate placement of the position label text.
 *   Tally extreme label locations, used later to place axis title.
 *  
 *   If axis drawing is enabled:
 *      Draw major tic mark and text.
 *      Draw minor tic marks.
 *
 *   If grid drawing is enabled:
 *      Draw grid lines.  
 ***/

    for( iter = 0; iter < maxIter && mult != lastMult; iter++ )
       {
        val = step*mult;

        (void) sprintf( ctemp, fmt, val );
        (void) drSize( ctemp, &gWidth, &gHeight, 0 );
       
        length = strlen( ctemp );
    
        switch( *orient )
           {
            case 'l':
            case 't':
               ticX = gx1 - gWidth/(1.0 + length);
               ticY = gy1 + (val - origin)*(gy3 - gy1)/range;
               txtX = gx1 - gWidth - 2.0*gWidth/(1.0 + length);
               txtY = gy1 > gy3 ? ticY + gHeight/2.0 : ticY - gHeight/2.0;

               xOff = tiltXOff( ticX, ticY, gx1, gy1, gx3, gy3 );
               maxX = maxX > txtX ? txtX : maxX; 

               if (IN_Y( ticY ))
                  {              
                   if (useAxis)
                      { 
                       (void) drLine( (float)(gx1+xOff), ticY, (float)(ticX+xOff), ticY, 0 );
                       (void) drText( (float)(txtX+xOff), txtY, ctemp, 0 );
                      }

                   if (useGrid)
                      {
                       (void) drLine( (float)(px1+xOff), ticY, (float)(px3+xOff), ticY, 1 );
                      }
                  }

               for( i = 1; i <= ticCount; i++ )
                  {
                   ticVal = val + sign*step*i/(float)(ticCount + 1);
                   ticY   = gy1 + (ticVal - origin)*(gy3 - gy1)/range;
                   ticX   = i == 5 ? gx1 - 0.75*gWidth/(1.0 + length) :
                                       gx1 - 0.50*gWidth/(1.0 + length);

                   xOff = tiltXOff( ticX, ticY, gx1, gy1, gx3, gy3 );

                   if (IN_Y( ticY ))
                      {
                       if (useAxis)
                          {
                           (void) drLine( (float)(gx1+xOff), ticY, (float)(ticX+xOff), ticY, 0 );
                          }
                      }
                  }

               break;

            case 'r':
            case 'b':
               ticX = gx1 + gWidth/(1 + strlen(ctemp));
               ticY = gy1 + (val - origin)*(gy3 - gy1)/range;
               txtX = gx1 + 2.0*gWidth/(1.0 + length);
               txtY = gy1 > gy3 ? ticY + gHeight/2.0 : ticY - gHeight/2.0;

               xOff = tiltXOff( ticX, ticY, gx1, gy1, gx3, gy3 );
               maxX = maxX < gx1 + gWidth ? gx1 + gWidth : maxX;

               if (IN_Y( ticY ))
                  {
                   if (useAxis)
                      {
                       (void) drLine( (float)(gx1+xOff), ticY, (float)(ticX+xOff), ticY, 0 );
                       (void) drText( (float)(txtX+xOff), txtY, ctemp, 0 );
                      }

                   if (useGrid)
                      {
                       (void) drLine( (float)(px1+xOff), ticY, (float)(px3+xOff), ticY, 1 );
                      }
                  }

               for( i = 1; i <= ticCount; i++ )
                  {
                   ticVal = val + sign*step*i/(float)(ticCount + 1);
                   ticY   = gy1 + (ticVal - origin)*(gy3 - gy1)/range;
                   ticX   = i == 5 ? gx1 + 0.75*gWidth/(1.0 + length) : gx1 + 0.50*gWidth/(1.0 + length);
               
                   xOff = tiltXOff( ticX, ticY, gx1, gy1, gx3, gy3 );

                   if (IN_Y( ticY ))
                      {
                       if (useAxis)
                          {
                           (void) drLine( (float)(gx1+xOff), ticY, (float)(ticX+xOff), ticY, 0 );
                          }
                      }
                  }

               break;

            default:
               break;
           }

        mult += multStep;
       }

     
/* Draw the axis title:
 ***/

    (void) drSize( label, &gWidth, &gHeight, 1 );

    length   = strlen( label );
    gHeight *= yAdj;

    switch( *orient )
       {
        case 'l':
        case 't':
           txtX = maxX - 3.0*gWidth/(1.0 + length);
           txtY = gy1 > gy3 ? (gy1+gy3-gHeight*length)/2.0 : (gy1+gy3+gHeight*length)/2.0;

           if (useAxis)
              {
               for ( i = length - 1; i >= 0; i-- )
                  {
                   xOff = tiltXOff( txtX, txtY, gx1, gy1, gx3, gy3 );

                   (void) sprintf( ctemp, "%c", label[i] );

                   (void) drText( (float)(txtX + 0.00*gHeight + xOff), (float)(txtY - 0.50*gHeight), ctemp, 1 );

                   txtY = gy1 > gy3 ? txtY + gHeight : txtY - gHeight;
                  }
              }

           break;

        case 'r':
        case 'b':
           txtX = maxX + 3.0*gWidth/(1.0 + length);
           txtY = gy1 > gy3 ? (gy1+gy3-gHeight*length)/2.0 : (gy1+gy3+gHeight*length)/2.0;

           if (useAxis)
              {
               for ( i = length - 1; i >= 0; i-- )
                  {
                   xOff = tiltXOff( txtX, txtY, gx1, gy1, gx3, gy3 );

                   (void) sprintf( ctemp, "%c", label[i] );

                   (void) drText( (float)(txtX + 0.00*gHeight + xOff), (float)(txtY + 0.50*gHeight), ctemp, 1 );

                   txtY = gy1 > gy3 ? txtY + gHeight : txtY - gHeight;
                  }
              }

           break;

        default:
           break;
       }

    return( 0 );
}

     
/* drawTic: draws a single axis-tic with label and grid-line.
 ***/

int drawTic( float gx1, float gy1,     /* Lower left graphics coord for axis.          */
             float gx3, float gy3,     /* Upper right graphics coord for axis.         */
             float px1, float py1,     /* Lower left graphics coord for grid.          */
             float px3, float py3,     /* Upper right graphics coord for grid.         */
             float xAdj,               /* Adjustment for horizontal char space, ~1.1.  */
             float yAdj,               /* Adjustment for vertical char space, ~1.1.    */
             float origin,             /* Origin of axis to draw.                      */
             float endpoint,           /* Endpoint of axis to draw.                    */
             float ticVal,             /* Location of tic in range of origin/endpoint. */
             char *orient,             /* Orientation of axis, "top", "bottom", etc.   */
             char *title,              /* Title label for the axis.                    */
             int (*drLine)(  float gx1, float gy1, float gx3, float gy3, int vecType ),  /* Procedure to draw a line.                    */
             int (*drTextR)( float gx1, float gy3, char *text, int bigFlag ),            /* Procedure to draw regular text.              */
             int (*drTextB)( float gx1, float gy3, char *text, int bigFlag ),            /* Procedure to draw bold text.                 */
             int (*drSize)( char *text, float *gWidth, float *gHeight, int bigFlag ),    /* Procedure which returns text sizes.          */
             int useAxis,                    /* Indicates whether to draw axis graphics.     */
             int useGrid,                    /* Indicates whether to draw grid lines.        */
             struct DrawAxisInfo *daInfo )   /* Overrides auto settings if non-null.         */
{
    float gWidth, gHeight, range, ticX, ticY, maxX, maxY, txtX, txtY;
    char  ctemp[MAXLAB], fmt[MAXLAB], label[MAXLAB], *sPtr;
    int   i, n, length, horizFlag, boldFlag;

    int (*drText)( float gx1, float gy3, char *text, int bigFlag );

     
/* Set default or manual parameters:
 ***/

    (void) strcpy( label, title );

    horizFlag = 0;
    boldFlag  = 0;

    if (daInfo)
       {
        horizFlag = daInfo->horizFlag;
        boldFlag  = daInfo->boldFlag;

        if (daInfo->useLabel)
           {
            (void) strcpy( label, daInfo->label );
           }

        if (daInfo->useLimits)
           {
            origin   = daInfo->origin;
            endpoint = daInfo->endpoint;
           }
       }

    drText = boldFlag ? drTextB : drTextR;

     
/* Create a representative string for this tic position value:
 * Compute a format for the tic label.
 ***/

    range  = endpoint == origin ? 1.0 : endpoint - origin;

    (void) sprintf( ctemp, " %+g ", ticVal );
    (void) drSize( ctemp, &gWidth, &gHeight, 0 );
    
    length = strlen( ctemp );

    switch( *orient )
       {
        case 'l':
           maxX     = gx3;
           maxY     = gy1;
           break;
        case 'r':
           maxX     = gx1;
           maxY     = gy1;
           break;
        case 't':
           maxX     = gx1;
           maxY     = gy1;
           break;
        case 'b':
           maxX     = gx1;
           maxY     = gy3;
           break;
        default:
           maxX     = gx1;
           maxY     = gy1;
           break;
       }

    if (fabs((double)ticVal) > 99999.0)
       (void) strcpy( fmt, "%e" );
    else if (fabs((double)ticVal) > 999.0)
       (void) strcpy( fmt, "%.2f" );
    else
       (void) strcpy( fmt, "%.3f" );

    if (daInfo)
       {
        if (daInfo->useFmt) (void) strcpy( fmt, daInfo->fmt );
       }

    if (!strcasecmp( fmt, "None" )) useAxis = 0;

     
/* Calculate placement of tic mark for this position label.
 * Calculate placement of the position label text.
 * Tally extreme label locations, used later to place axis title.
 *  
 *   If axis drawing is enabled:
 *      Draw major tic mark and text.
 *
 *   If grid drawing is enabled:
 *      Draw a grid line.  
 ***/

    (void) sprintf( ctemp, fmt, ticVal );
    (void) drSize( ctemp, &gWidth, &gHeight, 0 );
       
    length = strlen( ctemp );
     
    switch( *orient )
       {
        case 'l':
           ticX = gx3 - gWidth/(1.0 + length);
           ticY = gy1 + (ticVal - origin)*(gy3 - gy1)/range;
           txtX = gx3 - gWidth - 2.0*gWidth/(1.0 + length);
           txtY = gy1 > gy3 ? ticY + gHeight/2.0 : ticY - gHeight/2.0;

           maxX = maxX > txtX ? txtX : maxX; 

           if (IN_Y( ticY ))
              {              
               if (useAxis)
                  { 
                   (void) drLine( gx3, ticY, ticX, ticY, 0 );
                   (void) drText( txtX, txtY, ctemp, 0 );
                  }

               if (useGrid)
                  {
                   (void) drLine( px1, ticY, px3, ticY, 1 );
                  }
              }

           break;

        case 'r':
           ticX = gx1 + gWidth/(1 + strlen(ctemp));
           ticY = gy1 + (ticVal - origin)*(gy3 - gy1)/range;
           txtX = gx1 + 2.0*gWidth/(1.0 + length);
           txtY = gy1 > gy3 ? ticY + gHeight/2.0 : ticY - gHeight/2.0;

           maxX = maxX < gx1 + gWidth ? gx1 + gWidth : maxX;

           if (IN_Y( ticY ))
              {
               if (useAxis)
                  {
                   (void) drLine( gx1, ticY, ticX, ticY, 0 );
                   (void) drText( txtX, txtY, ctemp, 0 );
                  }

               if (useGrid)
                  {
                   (void) drLine( px1, ticY, px3, ticY, 1 );
                  }
              }

           break;

        case 't':
           ticX = gx1 + (ticVal - origin)*(gx3 - gx1)/range;
           ticY = gy1 > gy3 ? gy1 - gHeight/2.0 : gy1 - gHeight/2.0;

           txtX = ticX - gWidth/2;
           txtY = gy1 > gy3 ? gy1 - gHeight : gy1 - gHeight;

           maxY = gy1 > gy3 ? txtY - gHeight : txtY + gHeight;

           if (IN_X( ticX ))
              {       
               if (useAxis)
                  {
                   (void) drLine( ticX, gy1, ticX, ticY, 0 );
                   (void) drText( txtX, txtY, ctemp, 0 );
                  }

               if (useGrid)
                  {
                   (void) drLine( ticX, py3, ticX, py1, 1 );
                  }
              }

           break;

        case 'b':
           ticX = gx1 + (ticVal - origin)*(gx3 - gx1)/range;
           ticY = gy1 > gy3 ? gy3 + gHeight/2.0 : gy3 - gHeight/2.0;
           txtX = ticX - gWidth/2;
           txtY = gy1 > gy3 ? gy3 + 2.0*gHeight : gy3 - 2.0*gHeight;

           maxY = txtY;

           if (IN_X( ticX ))
              {
               if (useAxis)
                  {
                   (void) drLine( ticX, gy3, ticX, ticY, 0 );
                   (void) drText( txtX, txtY, ctemp, 0 );
                  } 
 
               if (useGrid)
                  {
                   (void) drLine( ticX, py3, ticX, py1, 1 );
                  }
              }

           break;

        default:
           break;
       }

     
/* Draw the axis title:
 ***/

    (void) drSize( label, &gWidth, &gHeight, 0 );

    length   = strlen( label );
    gWidth  *= xAdj;
    gHeight *= yAdj;

    switch( *orient )
       {
        case 'l':
           sPtr = label;
           txtX = maxX - 3.0*gWidth/(1.0 + length);
           txtY = gy1 > gy3 ? (gy1+gy3-gHeight*length)/2.0 : (gy1+gy3+gHeight*length)/2.0;

           if (useAxis)
              {
               while( *sPtr )
                  {
                   (void) sprintf( ctemp, "%c", *sPtr );
                   (void) drText( txtX, txtY, ctemp, 0 );

                   txtY = gy1 > gy3 ? txtY + gHeight : txtY - gHeight;
                   sPtr++;
                  }
              }

           break;

        case 'r':
           if (useAxis)
              {
               if (horizFlag)
                  {
                   n    = cntToken( label );
                   txtX = maxX + 3.0*gWidth/(length + 1);
                   txtY = gy1 > gy3 ? (gy1 + gy3 - 0.5*gHeight*n)/2.0 : (gy1 + gy3 + 0.5*gHeight*n)/2.0;

                   for( i = 1; i <= n; i++ )
                      {
                       (void) strcpy( ctemp, getToken( i, label ));
                       (void) drText( txtX, txtY, ctemp, 1 );
                       txtY += gHeight;
                      }
                  }
               else
                  {
                   txtX = maxX + 3.0*gWidth/(1.0 + length);
                   txtY = gy1 > gy3 ? (gy1+gy3-gHeight*length)/2.0 : (gy1+gy3+gHeight*length)/2.0;
                   sPtr = label;

                   while( *sPtr )
                       {
                        (void) sprintf( ctemp, "%c", *sPtr );
                        (void) drText( txtX, txtY, ctemp, 0 );
     
                        txtY = gy1 > gy3 ? txtY + gHeight : txtY - gHeight;
                        sPtr++;
                       }
                  }
              }

           break;

        case 't':
           txtX = (gx3 + gx1 - gWidth)/2.0;
           txtY = gy1 > gy3 ? maxY - 0.5*gHeight : maxY - 2.5*gHeight;

           if (useAxis)
              {
               (void) drText( txtX, txtY, label, 0 );
              }

           break;

        case 'b':
           txtX = (gx3 + gx1 - gWidth)/2.0;
           txtY = gy1 > gy3 ? maxY + 1.25*gHeight : maxY - 1.25*gHeight;

           if (useAxis)
              {
               (void) drText( txtX, txtY, label, 0 );
              }

           break;

        default:
           break;
       }

    return( 0 );
}

     
/* drawTicTilt: draws a single axis-tic with label and grid-line. tilted version.
 ***/

int drawTicTilt( float gx1, float gy1,
                 float gx3, float gy3,    
                 float px1, float py1,
                 float px3, float py3,  
                 float yAdj, float origin, float endpoint,
                 float ticVal, char *orient, char *title,  
                 int (*drLine)( float gx1, float gy1,  float gx3, float gy3, int vecType ), 
                 int (*drText)( float gx1, float gy3, char *text, int bigFlag ),
                 int (*drSize)( char *text, float *gWidth, float *gHeight, int bigFlag ),  
                 int useAxis, int useGrid,
                 struct DrawAxisInfo * daInfo ) 
{
    float gWidth, gHeight, range, ticX, ticY, maxX, maxY, txtX, txtY, xOff, yOff;
    char  ctemp[MAXLAB], fmt[MAXLAB], label[MAXLAB], *sPtr;
    int   i, length;

     
/* Set default or manual parameters:
 ***/

    (void) strcpy( label, title );

    if (daInfo)
       {
        if (daInfo->useLabel)
           {
            (void) strcpy( label, daInfo->label );
           }

        if (daInfo->useLimits)
           {
            origin   = daInfo->origin;
            endpoint = daInfo->endpoint;
           }
       }

     
/* Create a representative string for this tic position value:
 * Compute a format for the tic label.
 ***/

    range  = endpoint == origin ? 1.0 : endpoint - origin;

    (void) sprintf( ctemp, " %+g ", ticVal );
    (void) drSize( ctemp, &gWidth, &gHeight, 0 );
    
    length = strlen( ctemp );

    switch( *orient )
       {
        case 'l':
           maxX     = gx3;
           maxY     = gy1;
           break;
        case 'r':
           maxX     = gx1;
           maxY     = gy1;
           break;
        case 't':
           maxX     = gx1;
           maxY     = gy1;
           break;
        case 'b':
           maxX     = gx1;
           maxY     = gy3;
           break;
        default:
           maxX     = gx1;
           maxY     = gy1;
           break;
       }

    if (fabs((double)ticVal) > 99999.0)
       (void) strcpy( fmt, "%e" );
    else if (fabs((double)ticVal) > 999.0)
       (void) strcpy( fmt, "%.2f" );
    else
       (void) strcpy( fmt, "%.3f" );

    if (daInfo)
       {
        if (daInfo->useFmt) (void) strcpy( fmt, daInfo->fmt );
       }

    if (!strcasecmp( fmt, "None" )) useAxis = 0;

     
/* Calculate placement of tic mark for this position label.
 * Calculate placement of the position label text.
 * Tally extreme label locations, used later to place axis title.
 *  
 *   If axis drawing is enabled:
 *      Draw major tic mark and text.
 *
 *   If grid drawing is enabled:
 *      Draw a grid line.  
 ***/

    (void) sprintf( ctemp, fmt, ticVal );
    (void) drSize( ctemp, &gWidth, &gHeight, 0 );
       
    length = strlen( ctemp );
     
    switch( *orient )
       {
        case 'l':
        case 't':
           ticX = gx1 - gWidth/(1.0 + length);
           ticY = gy1 + (ticVal - origin)*(gy3 - gy1)/range;
           txtX = gx1 - gWidth - 2.0*gWidth/(1.0 + length);
           txtY = gy1 > gy3 ? ticY + gHeight/2.0 : ticY - gHeight/2.0;

           xOff = tiltXOff( ticX, ticY, gx1, gy1, gx3, gy3 );
           maxX = maxX > txtX ? txtX : maxX; 

           if (IN_Y( ticY ))
              {              
               if (useAxis)
                  { 
                   (void) drLine( (float)(gx1 + xOff), ticY, (float)(ticX + xOff), ticY, 0 );
                   (void) drText( (float)(txtX + xOff), txtY, ctemp, 0 );
                  }

               if (useGrid)
                  {
                   (void) drLine( (float)(px1 + xOff), ticY, (float)(px3 + xOff), ticY, 1 );
                  }
              }

           break;

        case 'r':
        case 'b':
           ticX = gx1 + gWidth/(1 + strlen(ctemp));
           ticY = gy1 + (ticVal - origin)*(gy3 - gy1)/range;
           txtX = gx1 + 2.0*gWidth/(1.0 + length);
           txtY = gy1 > gy3 ? ticY + gHeight/2.0 : ticY - gHeight/2.0;

           xOff = tiltXOff( ticX, ticY, gx1, gy1, gx3, gy3 );
           maxX = maxX < gx1 + gWidth ? gx1 + gWidth : maxX;

           if (IN_Y( ticY ))
              {
               if (useAxis)
                  {
                   (void) drLine( (float)(gx1 + xOff), ticY, (float)(ticX + xOff), ticY, 0 );
                   (void) drText( (float)(txtX + xOff), txtY, ctemp, 0 );
                  }

               if (useGrid)
                  {
                   (void) drLine( px1, ticY, px3, ticY, 1 );
                  }
              }

           break;

        default:
           break;
       }

     
/* Draw the axis title:
 ***/

    (void) drSize( label, &gWidth, &gHeight, 0 );

    length   = strlen( label );
    gHeight *= yAdj;

    switch( *orient )
       {
        case 'l':
        case 't':
           txtX = maxX - 3.0*gWidth/(1.0 + length);
           txtY = gy1 > gy3 ? (gy1+gy3-gHeight*length)/2.0 : (gy1+gy3+gHeight*length)/2.0;

           if (useAxis)
              {
               for ( i = length - 1; i >= 0; i-- )
                  {
                   xOff = tiltXOff( txtX, txtY, gx1, gy1, gx3, gy3 );

                   (void) sprintf( ctemp, "%c", label[i] );

                   (void) drText( (float)(txtX + 0.00*gHeight + xOff), (float)(txtY - 0.50*gHeight), ctemp, 1 );

                   txtY = gy1 > gy3 ? txtY + gHeight : txtY - gHeight;
                  }
              }

           break;

        case 'r':
        case 'b':
          txtX = maxX + 3.0*gWidth/(1.0 + length);
          txtY = gy1 > gy3 ? (gy1+gy3-gHeight*length)/2.0 :
                                (gy1+gy3+gHeight*length)/2.0;

           if (useAxis)
              {
               for ( i = length - 1; i >= 0; i-- )
                  {
                   xOff = tiltXOff( txtX, txtY, gx1, gy1, gx3, gy3 );

                   (void) sprintf( ctemp, "%c", label[i] );
                   (void) drText( (float)(txtX + 0.00*gHeight + xOff), (float)(txtY + 0.50*gHeight), ctemp, 1 );

                   txtY = gy1 > gy3 ? txtY + gHeight : txtY - gHeight;
                  }
              }

           break;

        default:
           break;
       }

    return( 0 );
}

     
/* drawAxisNull: null-initialize manual axis drawing parameters.
 ***/

int drawAxisNull( struct DrawAxisInfo *daInfo )
{
    daInfo->origin      = 0.0;
    daInfo->endpoint    = 0.0;
    daInfo->step        = 0.0;
    daInfo->unitsCode   = LAB_NONE_ID; 
    daInfo->ticCount    = 0;
    daInfo->fmt[0]      = '\0';
    daInfo->label[0]    = '\0';

    daInfo->useStep     = 0;
    daInfo->useTicCount = 0;
    daInfo->useUnits    = 0;
    daInfo->useFmt      = 0;
    daInfo->useLabel    = 0;
    daInfo->useLimits   = 0;
    daInfo->horizFlag   = 0;
    daInfo->boldFlag    = 0;

    return( 0 );
}

     
/* drawAxisParse: extract manual axis drawing parameters.
 ***/

int drawAxisParse( char *str, struct DrawAxisInfo *daInfo )
{
    char ctemp[NAMELEN+1], *src, *dest, *sPtr;

     
/* Parameter/value pairs:
 *   orig=  end=
 *   step=  tic=
 *   lab=   fmt=
 *   units=
 ***/

    if ((src = strstr( str, "labhoriz=" )))
       {
        src += 9;
        daInfo->horizFlag = (*src == '1' || *src == 'y' || *src == 'Y' || *src == 't' || *src == 'T') ? 1 : 0;
       }

    if ((src = strstr( str, "bold=" )))
       {
        src += 5;
        daInfo->boldFlag = (*src == '1' || *src == 'y' || *src == 'Y' || *src == 't' || *src == 'T') ? 1 : 0;
       }

    if ((src = strstr( str, "orig=" )))
       {
        dest = ctemp;
        src  += 5;

        while( *src && *src != ',')
           {
            *dest++ = *src++;
           }

        *dest++ = '\0';

        daInfo->origin    = ATOF( ctemp );
        daInfo->useLimits = 1;
       }

    if ((src = strstr( str, "end=" )))
       {
        dest = ctemp;
        src  += 4;

        while( *src && *src != ',')
           {
            *dest++ = *src++;
           }

        *dest++ = '\0';

        daInfo->endpoint  = ATOF( ctemp );
        daInfo->useLimits = 1;
       }

    if ((src = strstr( str, "step=" )))
       {
        dest  = ctemp;
        src  += 5;

        while( *src && *src != ',')
           {
            *dest++ = *src++;
           }

        *dest++ = '\0';

        daInfo->step    = ATOF( ctemp );
        daInfo->useStep = 1;
       }

    if ((src = strstr( str, "units=" )))
       {
        dest  = ctemp;
        src  += 6;

        while( *src && *src != ',')
           {
            *dest++ = *src++;
           }

        *dest++ = '\0';

        daInfo->unitsCode = getSpecUnits( ctemp );
        daInfo->useUnits  = 1;
       }

    if ((src = strstr( str, "tic=" )))
       {
        dest = ctemp;
        src  += 4;

        while( *src && *src != ',')
           {
            *dest++ = *src++;
           }

        *dest++ = '\0';

        daInfo->ticCount    = atoi( ctemp );
        daInfo->useTicCount = 1;
       }

    if ((src = strstr( str, "lab=" )))
       {
        dest = daInfo->label;
        src  += 4;

        while( *src && *src != ',')
           {
            *dest++ = *src++;
           }

        *dest++ = '\0';

        if (!strcasecmp( daInfo->label, "None" ))
           {
            daInfo->label[0] = '\0';
           }

        daInfo->useLabel = 1;
       }

    if ((src = strstr( str, "fmt=" )))
       {
        dest = daInfo->fmt;
        src  += 4;

        while( *src && *src != ',')
           {
            *dest++ = *src++;
           }

        *dest++ = '\0';

        daInfo->useFmt = 1;
       }

    return( 0 );
}

static float tiltXOff( float x0, float y0, float gx1, float gy1, float gx2, float gy2 )
{
    float dx, dy, xOff;

    dx = gx2 - gx1;
    dy = gy2 - gy1;

    xOff = dy == 0.0 ? 0.0 : dx*(y0 - gy1)/dy;

    return( xOff );
}

static float tiltYOff( float x0, float y0, float gx1, float gy1, float gx2, float gy2 )
{
    float dx, dy, yOff;

    dx = gx2 - gx1;
    dy = gy2 - gy1;
    
    yOff = dx == 0.0 ? 0.0 : dy*(x0 - gx1)/dx;
    
    return( yOff );
}

     
/* Bottom.
 ***/
