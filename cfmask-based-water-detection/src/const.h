
#ifndef CONST_H
#define CONST_H


#include "espa_common.h"


/* Defines for the bits in the pixel QA band */
#define L2QA_FILL 0
#define L2QA_CLEAR 1
#define L2QA_WATER 2
#define L2QA_CLD_SHADOW 3
#define L2QA_SNOW  4
#define L2QA_CLOUD 5
#define L2QA_CLOUD_CONF1 6
#define L2QA_CLOUD_CONF2 7

#define L2QA_SINGLE_BIT 0x01             /* 00000001 */
#define L2QA_DOUBLE_BIT 0x03             /* 00000011 */

/* These are used in arrays, and they are position dependent */
typedef enum
{
    I_BAND_RED,
    I_BAND_NIR,
    I_BAND_QA, /* This band and above are all from the XML */
    MAX_INPUT_BANDS
} Input_Bands_e;


#endif /* CONST_H */
