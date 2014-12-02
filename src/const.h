
#ifndef CONST_H
#define CONST_H

#ifndef SUCCESS
#define SUCCESS  0
#endif

#ifndef ERROR
#define ERROR   -1
#endif

#define MODULE_NAME "DSWE"
#define DSWE_VERSION "1.0.0"

#define PRODUCT_NAME "dswe"
#define BAND_NAME "dswe"
#define SHORT_NAME "DSWE"
#define LONG_NAME "dynamic surface water extent"

/* These are use in arrays, so they are position dependent */
typedef enum
{
    I_BAND_BLUE,
    I_BAND_GREEN,
    I_BAND_RED,
    I_BAND_NIR,
    I_BAND_SWIR1,
    I_BAND_BT,
    I_BAND_FMASK,
    MAX_INPUT_BANDS
} Input_Bands_e;

#define MINSIGMA 1e-5

#define DSWE_NO_DATA_VALUE -9999

#define MAX_DATE_LEN 28

#endif /* CONST_H */
