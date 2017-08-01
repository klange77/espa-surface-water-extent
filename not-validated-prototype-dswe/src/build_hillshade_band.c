#include <math.h>
#include <stdint.h>

/******************************************************************************
 * MODULE:  hillshade
 *
 * PURPOSE:  Performs the hillshade algorithm (from GDALDEM) to compute the
 * shaded relief for the current 3x3 window.
 *
 * RETURN VALUE:
 * Type = float 
 * Value      Description
 * -----      -----------
 *  0.0 - 1.0  Represents the shaded relief for the current pixel area
 *
 *  PROJECT:  Land Satellites Data System Science Research and Development 
 *  (LSRD) at the USGS EROS
 *
 *  HISTORY:
 *  Date        Programmer       Reason
 *  --------    ---------------  -------------------------------------
 *  12/31/2012  Gail Schmidt     Original Development (based on GDALHillshade
 *                               algorithm in GDALDEM v1.9.2)
 *                               2/1/2013    Gail Schmidt     
 *                               The north/south resolution needs to indicate
 *                               that we decrease in meters as we go from the
 *                               top to the bottom of the image.  Thus the 
 *                               ns_resolution needs to be negative or the 
 *                               slope is incorrect.
 *                               2/1/2013    Gail Schmidt     Added z scaling 
 *                               for Horn's algorithm.
 *
 *                               NOTES:
 *                               1. Algorithm is based on Lambert's cosine law 
 *                               using Horn's algorithm for calculating the
 *                               slope of the current point.  The other option 
 *                               is to use Zevenbergen and Thorn's algorithm.
 *                               The literature suggests Zevenbergen and Thorne
 *                               to be more suited to smooth landscapes, 
 *                               whereas Horn's formulai to perform better on
 *                               rougher terrain.
 *                               2. Input and output arrays are 1D arrays of 
 *                               size 3 lines x 3 samples where the 3x3 indices
 *                               are and the current pixel being processed is 4.
 *                               0 1 2
 *                               3 4 5
 *                               6 7 8
 ******************************************************************************/
float hillshade
(
    double *elevation_window, /* I: 3x3 array of elevation values in meters */
    float ew_resolution,  /* I: east/west resolution of the elevation data in
                                meters */
    float ns_resolution,  /* I: north/south resolution of the elevation data in
                                meters */
    float sun_elevation,  /* I: sun elevation angle in radians; 0 deg at the
                                horizon and 90 deg if directly above the DEM */
    float solar_azimuth   /* I: solar azimuth angle in radians; 0 deg=North,
                                90 deg=East, 180 deg=South, 270 deg=West */
)
{
    float x_slope;        /* slope at this point in east/west direction */
    float y_slope;        /* slope at this point in north/south direction */
    float xx_plus_yy;     /* value of x * x + y * y */
    float aspect;         /* aspect at this point in radians */
    float relief;         /* shaded relief value at this point */
    float z_scale = 0.125; /* constant from GDAL for Horn algorithm (1/8) */

    /* Since the data goes from west to east, leave the ew_resolution as 
       positive. However since the data goes from north to south, we need to 
       negate the ns_resolution. */
    ns_resolution = -ns_resolution;

    /* Compute the slope */
    x_slope = ((elevation_window[0] 
                + 2.0 * elevation_window[3] + elevation_window[6]) -
               (elevation_window[2] + 2.0 * elevation_window[5] 
                + elevation_window[8])) / ew_resolution;
    y_slope = ((elevation_window[6] + 2.0 * elevation_window[7] 
                + elevation_window[8]) -
               (elevation_window[0] + 2.0 * elevation_window[1] 
                + elevation_window[2])) / ns_resolution;
    xx_plus_yy = x_slope * x_slope + y_slope * y_slope;

    /* Compute the aspect */
    aspect = atan2 (y_slope, x_slope);

    /* Compute the shade value */
    relief = (sin (sun_elevation) - cos (sun_elevation) * z_scale 
        * sqrt (xx_plus_yy) * sin (aspect - solar_azimuth)) 
        / sqrt (1.0 + z_scale * z_scale * xx_plus_yy);

    return relief;
}


/******************************************************************************
 * MODULE:  build_hillshade_band
 *
 * PURPOSE:  Computes the shaded relief based on the DEM
 *
 * RETURN VALUE:
 * Type = None
 *
 * PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
 * at the USGS EROS
 *
 * HISTORY:
 * Date        Programmer       Reason
 * --------    ---------------  -------------------------------------
 *  12/31/2012  Gail Schmidt     Original Development
 *
 *  NOTES:
 *   1. Algorithm is based on the terrain-derived deep shadow algorithm provided
 *      by Dave Selkowitz, Research Geographer, USGS Alaska Science Center.
 *   2. Input DEM arrays are 1D arrays of size num_lines+[1or2] * num_samples. 
 *      An extra line should be provided before and after (if possible) the 
 *      actual subset of data to be processed.  Since we are processing an 
 *      entire line of data at a time, it will not be expected that there will 
 *      be an extra sample on either end of the line.  The first and last 
 *      sample simply will not be processed for the shaded relief, thus it
 *      should already be initialized to an appropriate value before calling
 *      this function.  The hillshade module requires a 3x3 window to be 
 *      provided surrounding each pixel, thus the extra line(s) of data.
 *    3. Output mask arrays are 1D arrays of size num_lines * num_samples.
 ******************************************************************************/
void build_hillshade_band
(
    int16_t *dem,        /* I: array of DEM values in meters (num_lines+[1or2] x
                               num_samples values - see NOTES);  if processing
                               at the top of the image, then an extra line
                               before will not be available;  if processing
                               at the bottom of the image, then an extra line
                               at the end will not be available */
    int num_lines,       /* I: number of lines of data  */
    int num_samples,     /* I: number of samples of data */
    double ew_resolution, /* I: east/west resolution of the elevation data in
                               meters */
    double ns_resolution, /* I: north/south resolution of the elevation data in
                               meters */
    float sun_elevation, /* I: sun elevation angle in radians */
    float solar_azimuth, /* I: solar azimuth angle in radians */
    uint8_t *shaded_relief /* O: array of shaded relief values (multiplied
                               by 100 to indicate percent intensity)
                               of size num_lines * num_samples */
)
{
    int line;              /* line being processed */
    int sample;            /* sample being processed */
    int current_pixel;     /* current input pixel being processed */
    int output_pixel;      /* current output pixel being processed */
    float shade;           /* shaded relief value for the current pixel */
    double elevation_window[9]; /* 3x3 window of elevation values surrounding 
                              the current pixel */

    /* Don't process the first and last lines and first and last
       samples of the DEM since we can't determine what the preceding
       and following values are */
    for (line = 1; line < num_lines - 1; line++)
    {
        for (sample = 1; sample < num_samples - 1; sample++)
        {
            output_pixel = line * num_samples + sample;

            /* Fill in the 3x3 elevation window surrounding the current 
               pixel */
            current_pixel = output_pixel - num_samples - 1;
            elevation_window[0] = dem[current_pixel];
            elevation_window[1] = dem[current_pixel + 1];
            elevation_window[2] = dem[current_pixel + 2];
            current_pixel += num_samples; 
            elevation_window[3] = dem[current_pixel];
            elevation_window[4] = dem[current_pixel + 1];
            elevation_window[5] = dem[current_pixel + 2];
            current_pixel += num_samples; 
            elevation_window[6] = dem[current_pixel];
            elevation_window[7] = dem[current_pixel + 1];
            elevation_window[8] = dem[current_pixel + 2];

            /* Compute the shaded relief for the current pixel */
            shade = hillshade (elevation_window, ew_resolution, 
                ns_resolution, sun_elevation, solar_azimuth);

            /* Scale the shaded relief values from 0.0 to 1.0 to 0 to 255 */
            if (shade <= 0.0)
            {
                shaded_relief[output_pixel] = 0;
            }
            else
            {
                shaded_relief[output_pixel] = (uint8_t) 
                    (round(254.0 * shade) + 1.0);
            }
        }
    }
}

