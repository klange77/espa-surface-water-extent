
#ifndef DSWE_CONST_H
#define DSWE_CONST_H

#ifndef SUCCESS
#define SUCCESS  0
#endif

#ifndef ERROR
#define ERROR   -1
#endif

#define MODULE_NAME "DSWE"

/* These are use in arrays, so they are position dependent */
typedef enum
{
    I_BAND_1,
    I_BAND_2,
    I_BAND_3,
    I_BAND_4,
    I_BAND_5,
    I_BAND_6,
    FMASK_BAND,
    DEM_BAND, /* This one always needs to be last in the list */
    MAX_INPUT_BANDS
} Input_Bands_e;

#define MINSIGMA 1e-5

#endif /* DSWE_CONST_H */

