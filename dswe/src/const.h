
#ifndef CONST_H
#define CONST_H

#include "espa_common.h"

#define DSWE_NOT_WATER 0
#define DSWE_WATER_HIGH_CONFIDENCE 1
#define DSWE_WATER_MODERATE_CONFIDENCE 2
#define DSWE_POTENTIAL_WETLAND 3
#define DSWE_LOW_CONFIDENCE_WATER_OR_WETLAND 4
#define DSWE_CLOUD_CLOUD_SHADOW_SNOW 9

#define DIAG_PRODUCT_NAME "dswe"
#define DIAG_BAND_NAME "dswe_diag"
#define DIAG_SHORT_NAME "DSWE_DIAG"
#define DIAG_LONG_NAME "dynamic surface water extent: diagnostic tests"

#define PS_PRODUCT_NAME "dswe"
#define PS_BAND_NAME "percent_slope"
#define PS_SHORT_NAME "PERCENT_SLOPE"
#define PS_LONG_NAME "dynamic surface water extent: percent slope"

#define HS_PRODUCT_NAME "dswe"
#define HS_BAND_NAME "hillshade"
#define HS_SHORT_NAME "HILLSHADE"
#define HS_LONG_NAME "dynamic surface water extent: hillshade"

#define INTERPRETED_PRODUCT_NAME "dswe"
#define INTERPRETED_BAND_NAME "dswe_intrpd"
#define INTERPRETED_SHORT_NAME "DSWE_INTRPD"
#define INTERPRETED_LONG_NAME "dynamic surface water extent: interpreted"

#define PS_SC_PRODUCT_NAME "dswe"
#define PS_SC_BAND_NAME "dswe_pshsccss"
#define PS_SC_SHORT_NAME "DSWE_PSHSCCSS"
#define PS_SC_LONG_NAME "dynamic surface water extent: filtered by: percent slope - hillshade - cloud - cloud shadow - snow"

#define MASK_PRODUCT_NAME "dswe"
#define MASK_BAND_NAME "dswe_mask"
#define MASK_SHORT_NAME "DSWE_MASK"
#define MASK_LONG_NAME "dynamic surface water extent: mask including percent slope - hillshade - cloud - cloud shadow - snow"

/* These are used in arrays, and they are position dependent */
typedef enum
{
    I_BAND_BLUE,
    I_BAND_GREEN,
    I_BAND_RED,
    I_BAND_NIR,
    I_BAND_SWIR1,
    I_BAND_SWIR2,
    I_BAND_PIXELQA,
    I_BAND_ELEVATION, /* This band and above are all from the XML */
    MAX_INPUT_BANDS
} Input_Bands_e;


#define DSWE_NO_DATA_VALUE 255
#define TESTS_NO_DATA_VALUE -9999

#ifndef RAD
#define RAD (M_PI / 180.0)
#endif

/* Define bit values contributed to the output mask from different sources */
#define MASK_SHADOW 0
#define MASK_SNOW   1
#define MASK_CLOUD  2
#define MASK_PS     3
#define MASK_HS     4

#endif /* CONST_H */
