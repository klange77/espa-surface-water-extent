#include "swe.h"

#define MINSIGMA 1e-5
/******************************************************************************

MODULE:  surface_water_extent

PURPOSE:  Performs surface water extent mask lebeling based on rules
          provided by John Jones, Eastern Geographic Science Center, 
          US Geological Survey

RETURN VALUE:
Type = None

HISTORY:
Date        Programmer       Reason
--------    ---------------  -------------------------------------
4/26/2013   Song Guo         Original development

NOTES:
  1. Input and output arrays are 1D arrays of size nlines * nsamps.
******************************************************************************/
void surface_water_extent
(
    int16 *b2,     /* I: array of unscaled band 2 reflectance values */
    int16 *b3,     /* I: array of unscaled band 3 reflectance values */
    int16 *b4,     /* I: array of unscaled band 4 reflectance values */
    int16 *b5,     /* I: array of unscaled band 5 reflectance values */
    int nlines,    /* I: number of lines in the data arrays */
    int nsamps,    /* I: number of samples in the data arrays */
    float refl_scale_fact,  /* I: scale factor for the reflectance values */  
    float mgt,              /* I: mgt threshold */
    float mlt1,             /* I: mlt1 threshold */
    float mlt2,             /* I: mlt2 threshold */
    int16 b4lt1,            /* I: b4lt1 threshold */
    int16 b4lt2,            /* I: b4lt2 threshold */
    int16 b5lt1,            /* I: b5lt1 threshold */
    int16 b5lt2,            /* I: b5lt2 threshold */
    int16 *raw_swe     /* O: array of surface water extent values (non-zero
                                values represent water) */
)
{
    int16 mask;
    int pix;           /* current pixel being processed */
    float b2_pix;      /* scaled band 2 value for current pixel */
    float b3_pix;      /* scaled band 3 value for current pixel */
    float b4_pix;      /* scaled band 4 value for current pixel */
    float b5_pix;      /* scaled band 5 value for current pixel */
    float scale_b4lt1; /* scaled b4lt1 threshold */
    float scale_b4lt2; /* scaled b4lt2 threshold */
    float scale_b5lt1; /* scaled b5lt1 threshold */
    float scale_b5lt2; /* scaled b5lt2 threshold */
    float mndwi;       /* Modified Normalized Difference Wetness Index */
    float vbb;         /* Visible brightness band */
    float sbb;         /* SWIR brightness band */

    /* Loop through the pixels in the array to determine the cloud cover
       classification */
    for (pix = 0; pix < nlines*nsamps; pix++)
    {
        /* Scale the current pixel for each band */
        b2_pix = b2[pix] * refl_scale_fact;
        b3_pix = b3[pix] * refl_scale_fact;
        b4_pix = b4[pix] * refl_scale_fact;
        b5_pix = b5[pix] * refl_scale_fact;
        scale_b4lt1 = b4lt1 * refl_scale_fact;
        scale_b4lt2 = b4lt2 * refl_scale_fact;
        scale_b5lt1 = b5lt1 * refl_scale_fact;
        scale_b5lt2 = b5lt2 * refl_scale_fact;

        /* Rule 1 */
        mndwi = (b2_pix - b5_pix) / (b2_pix + b5_pix);
        if ((mndwi - mgt) >= MINSIGMA)
            mask = 1;
        else
            mask = 0;

        /* Rule 2 */
        if (((mndwi - mlt1) > MINSIGMA) && ((b4_pix - scale_b4lt1) < MINSIGMA) &&
         ((b5_pix - scale_b5lt1) < MINSIGMA))
            mask += 10;

        /* Rule 3 */
        if (((mndwi - mlt2) > MINSIGMA) && ((b4_pix - scale_b4lt2) < MINSIGMA) &&
         ((b5_pix - scale_b5lt2) < MINSIGMA))
            mask += 100;

        /* Rule 4 */
        vbb = b2_pix + b3_pix;
        sbb = b4_pix + b5_pix;
        if ((vbb - sbb) >= MINSIGMA)
            mask += 1000;

        raw_swe[pix] = mask;
    }  /* end for pix */
}
