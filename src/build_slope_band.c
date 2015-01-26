
#include <stdbool.h>
#include <stdint.h>
#include <math.h>


#include "const.h"


/*****************************************************************************
  NAME: calculate_slope_horn

  PURPOSE: Performs the Horn's slope algorithm (from GDALDEM) to compute the
           terrain slope for the current 3x3 window.

  RETURN VALUE: Type = float
      Value    Description
      -----------  -----------------------------------------------------------
      0.0 - 100.0  Represents the percent slope for the current pixel area

  NOTES:
    Original Development (based on GDALHillshade algorithm in GDALDEM v1.9.2)

    1. Algorithm is based on Lambert's cosine law using Horn's algorithm for
       calculating the slope of the current point.  The other option is to use
       Zevenbergen and Thorn's algorithm.  The litterature suggests
       Zevenbergen and Thorne to be more suited to smooth landscapes, where-as
       Horn's formula to perform better on rougher terrain.

    2. Input and output arrays are 1D arrays of size 3 lines x 3 samples where
       the 3x3 indices are and the current pixel being processed is 4.
         0 1 2
         3 4 5
         6 7 8
*****************************************************************************/
double calculate_slope_horn
(
    double *elevation_window, /* I: 3x3 array of elevation values in meters */
    double ew_resolution,     /* I: east/west resolution of the elevation
                                    data in meters */
    double ns_resolution      /* I: north/south resolution of the elevation
                                    data in meters */
)
{
    double x_slope;
    double y_slope;
    double ew_res;
    double ns_res;
    double slope;

    /* Since the date goes from west to east, leave the ew_resolution as
       positive.  However since the data goes from north to south, we need to
       negate the ns_resolution. */
    ew_res = ew_resolution;
    ns_res = -ns_resolution;

    /* Compute the slope */
    x_slope = ((elevation_window[0]
                + 2.0 * elevation_window[3]
                + elevation_window[6])
               - (elevation_window[2]
                  + 2.0 * elevation_window[5]
                  + elevation_window[8]))
              / (8.0 * ew_res);

    y_slope = ((elevation_window[6]
                + 2.0 * elevation_window[7]
                + elevation_window[8])
               - (elevation_window[0]
                  + 2.0 * elevation_window[1]
                  + elevation_window[2]))
              / (8.0 * ns_res);

    slope = sqrt(x_slope * x_slope + y_slope * y_slope);

    return slope;
}


/******************************************************************************
MODULE:  calculate_slope_zevenbergen_thorne

PURPOSE:  Performs the Zevenbergen and Thorne's slope algorithm  
          to compute the terrain slope for the current 3x3 window.

RETURN VALUE:
Type = float 
Value      Description
-----      -----------
0.0 - 1.000  Represents the percent slope for the current pixel area

HISTORY:
Date        Programmer       Reason
--------    ---------------  -------------------------------------
06/21/2013  Song Guo         Original Development (based on Zevenbergen and 
                             Thorne, 1987)

NOTES:
  1. Algorithm is based on Zevenbergen and Thorn's algorithm.  The litterature 
     suggests Zevenbergen and Thorne to be more suited to smooth landscapes, 
     whereas Horn's formula to perform better on rougher terrain.
  2. Input and output arrays are 1D arrays of size 3 lines x 3 samples where
     the 3x3 indices are and the current pixel being processed is 4.
       0 1 2
       3 4 5
       6 7 8
******************************************************************************/
double calculate_slope_zevenbergen_thorne
(
    double *elevation_window, /* I: 3x3 array of elevation values in meters */
    double ew_resolution,     /* I: east/west resolution of the elevation
                                    data in meters */
    double ns_resolution      /* I: north/south resolution of the elevation
                                    data in meters */
)
{
    double x_slope;
    double y_slope;
    double ew_res;
    double ns_res;
    double slope; /* value of -sqrt(x * x + y * y) */

    /* Since the data goes from west to east, leave the ew_res as positive.
       However since the data goes from north to south, we need to negate the
       ns_res. */
    ew_res = ew_resolution;
    ns_res = -ns_resolution;

    /* Compute the slope */
    x_slope = ((elevation_window[5] - elevation_window[3])) / (2.0 * ew_res);
    y_slope = ((elevation_window[1] - elevation_window[7])) / (2.0 * ns_res);

    /* The negative sign from the algorithm has been ignored as it only
       shows the direction which down-slope is negative */
    slope = sqrt(x_slope * x_slope +  y_slope * y_slope); 

    return slope;
}


/*****************************************************************************
  NAME: build_slope_band

  PURPOSE: Takes a DEM band as input and create a percent slope band as output
           for further processing.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  Successfully created the percent slope band.
      ERROR    Failed to create the percent slope band.
*****************************************************************************/
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
)
{
    int line;
    int sample;
    int current_pixel;
    int output_pixel;
    double elevation_window[9];
    double slope;

    for (line = 0; line < num_lines; line++)
    {
        for (sample = 0; sample < num_samples; sample++)
        {
            output_pixel = line * num_samples + sample;

            /* Don't process the first and last lines and first and last
               samples of the DEM since we can't determine what the preceding
               and following values are */
            if ((line > 1) && (line < num_lines-1)
                && (sample > 1) && (sample < num_samples-1))
            {
                /* Fill in the 3x3 elevation window surrounding the current
                   pixel
                                    [0, 1, 2,
                                     3, 4, 5,
                                     6, 7, 8]
                 */
                /* TOP row [0, 1, 2] */
                current_pixel = output_pixel - num_samples - 1;  // [0]
                elevation_window[0] = band_dem[current_pixel];   // [0]
                elevation_window[1] = band_dem[current_pixel+1]; // [1]
                elevation_window[2] = band_dem[current_pixel+2]; // [2]

                /* MIDDLE row [3, 4, 5] */
                current_pixel += num_samples;                    // [3]
                elevation_window[3] = band_dem[current_pixel];   // [3]
                elevation_window[4] = band_dem[current_pixel+1]; // [4]
                elevation_window[5] = band_dem[current_pixel+2]; // [5]

                /* BOTTOM row [6, 7, 8] */
                current_pixel += num_samples;                    // [6]
                elevation_window[6] = band_dem[current_pixel];   // [6]
                elevation_window[7] = band_dem[current_pixel+1]; // [7]
                elevation_window[8] = band_dem[current_pixel+2]; // [8]

                if (use_zeven_thorne_flag)
                    slope = calculate_slope_zevenbergen_thorne(
                                elevation_window, ew_resolution, ns_resolution);
                else
                    slope = calculate_slope_horn(elevation_window,
                                                 ew_resolution, ns_resolution);

                band_ps[output_pixel] = 100.0 * slope;
            }
            else
            {
                /* Default the first and last to 0.0 */
                band_ps[output_pixel] = 0.0F;
            }
        }
    }
}
