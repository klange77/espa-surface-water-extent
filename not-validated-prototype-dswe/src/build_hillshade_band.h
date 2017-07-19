
#ifndef BUILD_HILLSHADE_H
#define BUILD_HILLSHADE_H


#include <stdbool.h>
#include <stdint.h>


void build_hillshade_band
(
    int16_t *band_dem,    /* I: the elevation data to use in meters */
    int num_lines,        /* I: the number of lines in the data */
    int num_samples,      /* I: the number of samples in the data */
    double ew_resolution, /* I: east/west resolution of the elevation data in
                                meters */
    double ns_resolution, /* I: north/south resolution of the elevation data
                                in meters */
    float sun_elevation,  /* I: sun elevation angle in radians */
    float solar_azimuth,  /* I: solar azimuth angle in radians */
    uint8_t *band_hillshade /* O: the hillshade band generated from the DEM */
);


#endif /* BUILD_HILLSHADE_H */
