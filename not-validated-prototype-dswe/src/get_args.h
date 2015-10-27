
#ifndef GET_ARGS_H
#define GET_ARGS_H


#include <stdbool.h>


int
get_args (int argc,                    /* I: number of cmd-line args */
          char *argv[],                /* I: string of cmd-line args */
          char **xml_infile,           /* O: input XML filename */
          char **dem_infile,           /* O: input DEM filename */
          bool *use_zeven_thorne_flag, /* O: use zeven thorne */
          bool *use_toa_flag,          /* O: process using TOA */
          bool *include_tests_flag,    /* O: include raw DSWE tests as output */
          bool *include_ps_flag,       /* O: include ps as output */
          float *wigt,                 /* O: tolerance value */
          float *awgt,                 /* O: tolerance value */
          float *pswt_1,               /* O: tolerance value */
          float *pswt_2,               /* O: tolerance value */
          float *percent_slope,        /* O: slope tolerance */
          int *pswnt_1,                /* O: tolerance value */
          int *pswnt_2,                /* O: tolerance value */
          int *pswst_1,                /* O: tolerance value */
          int *pswst_2,                /* O: tolerance value */
          bool * verbose_flag);        /* O: verbose messaging */


#endif /* GET_ARGS_H */
