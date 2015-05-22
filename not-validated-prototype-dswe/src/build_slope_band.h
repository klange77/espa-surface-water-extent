
#ifndef BUILD_SLOPE_BAND_H
#define BUILD_SLOPE_BAND_H


#include <stdbool.h>
#include <stdint.h>


void build_slope_band
(
    int16_t *band_dem,    /* I: the elevation data to use in meters */
    int num_lines,        /* I: the number of lines in the data */
    int num_samples,      /* I: the number of samples in the data */
    double ew_resolution, /* I: east/west resolution of the elevation data in
                                meters */
    double ns_resolution, /* I: north/south resolution of the elevation data
                                in meters */
    bool use_zeven_thorne_flag, /* I: wether or not to use this algorithm
                                      for the percent slope calculation */
    float *band_ps        /* O: the percent slope band generated from the
                                DEM */
);


#endif /* BUILD_SLOPE_BAND_H */
