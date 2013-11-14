#ifndef _SHADED_RELIEF_H
#define _SHADED_RELIEF_H

#include <hdf.h>
#include <math.h>

#include "bool.h"

/* Set up the deep shadow mask values */
#define DEEP_SHADOW 255
#define NO_DEEP_SHADOW 0

/* Define the terrain-derived deep shadow threshold */
#define TERRAIN_DEEP_SHADOW_THRESH 0.03

void deep_shadow
(
    int16 *dem,          /* I: array of DEM values in meters (nlines+[1or2] x
                               nsamps values - see NOTES);  if processing
                               at the top of the image, then an extra line
                               before will not be available;  if processing
                               at the bottom of the image, then an extra line
                               at the end will not be available */
    bool dem_top,        /* I: are we at the top of the dem and therefore no
                               extra lines at the start of the dem? */
    bool dem_bottom,     /* I: are we at the bottom of the dem and therefore no
                               extra lines at the end of the dem? */
    int nlines,          /* I: number of lines of data to be processed in the
                               mask array; dem array will have one or two lines
                               more depending on top, middle, bottom */
    int nsamps,          /* I: number of samples of data to be processed in the
                               mask array; dem array will have the same number
                               of samples therefore the first and last sample
                               will not be processed as part of the mask since
                               a 3x3 window won't be available */
    float ew_res,        /* I: east/west resolution of the elevation data in
                               meters */
    float ns_res,        /* I: north/south resolution of the elevation data in
                               meters */
    float sun_elev,      /* I: sun elevation angle in radians */
    float solar_azimuth, /* I: solar azimuth angle in radians */
    float *shaded_relief,    /* O: array of shaded relief values (multiplied
                                   by 100 to indicate percent intensity)
                                   of size nlines * nsamps */
    uint8 *deep_shadow_mask  /* O: array of deep shadow masked values (non-zero
                                   values represent terrain-derived deep
                                   shadow areas) of size nlines * nsamps */
);

#endif /* _SHADED_RELIEF_H */

