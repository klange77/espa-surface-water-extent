
#ifndef CONST_H
#define CONST_H


#include "espa_common.h"


#define RAW_DIAG_PRODUCT_NAME "dswe_diag"
#define RAW_DIAG_BAND_NAME "dswe_diag"
#define RAW_DIAG_SHORT_NAME "DSWE_DIAG"
#define RAW_DIAG_LONG_NAME "dynamic surface water extent - raw diagnostic tests"

#define PS_PRODUCT_NAME "percent_slope"
#define PS_BAND_NAME "percent_slope"
#define PS_SHORT_NAME "PERCENT_SLOPE"
#define PS_LONG_NAME "dynamic surface water extent - percent slope"

#define RAW_PRODUCT_NAME "dswe_raw"
#define RAW_BAND_NAME "dswe_raw"
#define RAW_SHORT_NAME "DSWE_RAW"
#define RAW_LONG_NAME "dynamic surface water extent - raw"

#define SC_PRODUCT_NAME "dswe_ccss"
#define SC_BAND_NAME "dswe_ccss"
#define SC_SHORT_NAME "DSWE_CCSS"
#define SC_LONG_NAME "dynamic surface water extent - cloud, cloud shadow, and snow filtered"

#define PS_SC_PRODUCT_NAME "dswe_psccss"
#define PS_SC_BAND_NAME "dswe_psccss"
#define PS_SC_SHORT_NAME "DSWE_PSCCSS"
#define PS_SC_LONG_NAME "dynamic surface water extent - percent-slope, cloud, cloud shadow, and snow filtered"


/* These are used in arrays, and they are position dependent */
typedef enum
{
    I_BAND_BLUE,
    I_BAND_GREEN,
    I_BAND_RED,
    I_BAND_NIR,
    I_BAND_SWIR1,
    I_BAND_SWIR2,
    I_BAND_CFMASK, /* This band and above are all from the XML */
    I_BAND_DEM,
    MAX_INPUT_BANDS
} Input_Bands_e;


#define DSWE_NO_DATA_VALUE 255
#define TESTS_NO_DATA_VALUE -9999


#endif /* CONST_H */
