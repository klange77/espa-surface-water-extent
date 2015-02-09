
#ifndef CONST_H
#define CONST_H


#define MODULE_NAME "DSWE"
#define DSWE_VERSION "1.0.0"


#define SUCCESS 0
#define ERROR 1


#define RAW_TESTS_PRODUCT_NAME "raw_tests_dswe"
#define RAW_TESTS_BAND_NAME "raw_tests_dswe"
#define RAW_TESTS_SHORT_NAME "RAW_TESTS_DSWE"
#define RAW_TESTS_LONG_NAME "dynamic surface water extent - raw tests"

#define PS_PRODUCT_NAME "percent_slope"
#define PS_BAND_NAME "percent_slope"
#define PS_SHORT_NAME "PERCENT_SLOPE"
#define PS_LONG_NAME "dynamic surface water extent - percent slope"

#define RAW_PRODUCT_NAME "raw_dswe"
#define RAW_BAND_NAME "raw_dswe"
#define RAW_SHORT_NAME "RAW_DSWE"
#define RAW_LONG_NAME "dynamic surface water extent - raw"

#define SC_PRODUCT_NAME "sc_dswe"
#define SC_BAND_NAME "sc_dswe"
#define SC_SHORT_NAME "SC_DSWE"
#define SC_LONG_NAME "dynamic surface water extent - cloud shadow and cloud filtered"

#define PS_SC_PRODUCT_NAME "ps_sc_dswe"
#define PS_SC_BAND_NAME "ps_sc_dswe"
#define PS_SC_SHORT_NAME "PS_SC_DSWE"
#define PS_SC_LONG_NAME "dynamic surface water extent - percent-slope, cloud shadow, and cloud filtered"


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
