
#ifndef GET_ARGS_H
#define GET_ARGS_H

#include <stdbool.h>

int
get_args (int argc,           /* I: number of cmd-line args */
          char *argv[],       /* I: string of cmd-line args */
          char **xml_infile,  /* O: address of input XML filename */
          bool *use_ledaps_mask_flag,  /* O: use ledaps mask */
          bool *use_zeven_thorne_flag, /* O: use zeven thorne */
          bool *use_toa_flag,          /* O: process using TOA */
          float *wigt,                 /* O: tolerance value */
          float *awgt,                 /* O: tolerance value */
          float *pswt,                 /* O: tolerance value */
          float *percent_slope,        /* O: slope tolerance */
          int *pswnt,                  /* O: tolerance value */
          int *pswst,                  /* O: tolerance value */
          bool * verbose_flag);        /* O: verbose messaging */

#endif /* GET_ARGS_H */
