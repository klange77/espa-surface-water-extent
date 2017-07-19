
#ifndef GET_ARGS_H
#define GET_ARGS_H


#include <stdbool.h>


#include "espa_metadata.h"


int
get_args (int argc,                    /* I: number of cmd-line args */
          char *argv[],                /* I: string of cmd-line args */
          char **xml_filename,         /* O: input XML filename */
          Espa_internal_meta_t *xml_metadata, /* O: input metadata */
          bool *use_zeven_thorne_flag, /* O: use zeven thorne */
          bool *use_toa_flag,          /* O: process using TOA */
          bool *include_tests_flag,    /* O: include raw DSWE with output */
          bool *include_ps_flag,       /* O: include ps with output */
          bool *include_hs_flag,       /* O: include hillshade with output */
          float *wigt,                 /* O: tolerance value */
          float *awgt,                 /* O: tolerance value */
          float *pswt_1_mndwi,         /* O: tolerance value */
          int *pswt_1_nir,             /* O: tolerance value */
          int *pswt_1_swir1,           /* O: tolerance value */
          float *pswt_1_ndvi,          /* O: tolerance value */
          float *pswt_2_mndwi,         /* O: tolerance value */
          int *pswt_2_blue,            /* O: tolerance value */
          int *pswt_2_nir,             /* O: tolerance value */
          int *pswt_2_swir1,           /* O: tolerance value */
          int *pswt_2_swir2,           /* O: tolerance value */
          float *percent_slope_high,   /* O: slope tolerance for high confidence
                                             water */
          float *percent_slope_moderate, /* O: slope tolerance for moderate
                                             confidence water */
          float *percent_slope_wetland, /* O: slope tolerance for potential 
                                             wetland */
          float *percent_slope_low,    /* O: slope tolerance for low confidence
                                          water or wetland */
          int *hillshade,              /* O: hillshade tolerance value */ 
          bool * verbose_flag);        /* O: verbose messaging */


#endif /* GET_ARGS_H */
