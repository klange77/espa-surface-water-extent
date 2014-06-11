
#ifndef GET_ARGS_H
#define GET_ARGS_H

#include <stdbool.h>

int get_args
(
    int argc,          /* I: number of cmd-line args */
    char *argv[],      /* I: string of cmd-line args */
    char **xml_infile, /* O: address of input XML filename */
    char **dem_infile, /* O: address of input DEM filename */
    bool *use_ledaps_mask_flag,
    bool *use_zeven_thorne_flag,
    bool *use_toa_flag,
    float *wigt,
    float *awgt,
    float *pswt,
    float *percent_slope,
    int *pswb4t,
    int *pswb5t,
    bool *verbose_flag,
    int *line_buffer_size
);

#endif /* GET_ARGS_H */

