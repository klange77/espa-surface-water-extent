
#ifndef CONST_H
#define CONST_H


#include "espa_common.h"


/* Class QA integer classification values */
#define CLASS_QA_CLEAR_PIXEL        0
#define CLASS_QA_WATER_PIXEL        1
#define CLASS_QA_CLOUD_SHADOW_PIXEL 2
#define CLASS_QA_SNOW_PIXEL         3
#define CLASS_QA_CLOUD_PIXEL        4
#define CLASS_QA_FILL_PIXEL         255


/* These are used in arrays, and they are position dependent */
typedef enum
{
    I_BAND_RED,
    I_BAND_NIR,
    I_BAND_CLASS_QA, /* This band and above are all from the XML */
    MAX_INPUT_BANDS
} Input_Bands_e;


#endif /* CONST_H */
