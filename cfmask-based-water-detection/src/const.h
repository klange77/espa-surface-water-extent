
#ifndef CONST_H
#define CONST_H


#include "espa_common.h"


/* L2QA integer classification values */
#define L2QA_CLEAR_PIXEL        0
#define L2QA_WATER_PIXEL        1
#define L2QA_CLOUD_SHADOW_PIXEL 2
#define L2QA_SNOW_PIXEL         3
#define L2QA_CLOUD_PIXEL        4
#define L2QA_FILL_PIXEL         255


/* These are used in arrays, and they are position dependent */
typedef enum
{
    I_BAND_RED,
    I_BAND_NIR,
    I_BAND_L2QA, /* This band and above are all from the XML */
    MAX_INPUT_BANDS
} Input_Bands_e;


#endif /* CONST_H */
