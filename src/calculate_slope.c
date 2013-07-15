#include "swe.h"

/******************************************************************************
MODULE:  horn_slope

PURPOSE:  Performs the Horn's slope algorithm (from GDALDEM) to compute the
terrain slope for the current 3x3 window.

RETURN VALUE:
Type = float 
Value      Description
-----      -----------
0.0 - 100.0  Represents the percent slope for the current pixel area

HISTORY:
Date        Programmer       Reason
--------    ---------------  -------------------------------------
12/31/2012  Gail Schmidt     Original Development (based on GDALHillshade
                             algorithm in GDALDEM v1.9.2)
04/28/2013  Song Guo         Modified to only calculate slope 

NOTES:
  1. Algorithm is based on Lambert's cosine law using Horn's algorithm for
     calculating the slope of the current point.  The other option is to use
     Zevenbergen and Thorn's algorithm.  The litterature suggests Zevenbergen
     and Thorne to be more suited to smooth landscapes, whereas Horn's formula
     to perform better on rougher terrain.
  2. Input and output arrays are 1D arrays of size 3 lines x 3 samples where
     the 3x3 indices are and the current pixel being processed is 4.
       0 1 2
       3 4 5
       6 7 8
******************************************************************************/
float horn_slope
(
    int16 *elev_window,   /* I: 3x3 array of elevation values in meters */
    float ew_res,         /* I: east/west resolution of the elevation data in
                                meters */
    float ns_res          /* I: north/south resolution of the elevation data in
                                meters */
)
{
    float x_slope;        /* slope at this point in east/west direction */
    float y_slope;        /* slope at this point in north/south direction */
    float slope;          /* value of sqrt(x * x + y * y) */

    /* Since the data goes from west to east, leave the ew_res as positive.
       However since the data goes from north to south, we need to negate the
       ns_res. */
    ns_res = -ns_res;

    /* Compute the slope */
    x_slope = ((float)((elev_window[0] + 2.0 * elev_window[3] + elev_window[6])
               -(elev_window[2] + 2.0 * elev_window[5] + elev_window[8]))) /
              (8.0 * ew_res);
    y_slope = ((float)((elev_window[6] + 2.0 * elev_window[7] + elev_window[8])
               - (elev_window[0] + 2.0 * elev_window[1] + elev_window[2]))) /
              (8.0 * ns_res);
    slope = sqrt(x_slope * x_slope + y_slope * y_slope);

    return slope;
}


/******************************************************************************
MODULE:  zevenbergen_thorne_slope

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
float zevenbergen_thorne_slope
(
    int16 *elev_window,   /* I: 3x3 array of elevation values in meters */
    float ew_res,         /* I: east/west resolution of the elevation data in
                                meters */
    float ns_res          /* I: north/south resolution of the elevation data in
                                meters */
)
{
    float g;        /* G parameter in the algorithm */
    float h;        /* H parameter in the algorithm */
    float slope;    /* value of -sqrt(x * x + y * y) */

    /* Since the data goes from west to east, leave the ew_res as positive.
       However since the data goes from north to south, we need to negate the
       ns_res. */
    ns_res = -ns_res;

    /* Compute the slope */
    g = ((float)(elev_window[5] - elev_window[3])) / (2.0 * ew_res);
    h = ((float)(elev_window[1] - elev_window[7])) / (2.0 * ns_res);
    /* The negative sign from the algorithm has been ignored as it only
       shows the direction which down-slope is negative */
    slope = sqrt(g * g +  h * h); 

    return slope;
}


/******************************************************************************
MODULE:  calc_slope

PURPOSE:  Computes the terrain slope based on the DEM

RETURN VALUE:
Type = None

HISTORY:
Date        Programmer       Reason
--------    ---------------  ---------------------------------------------
12/31/2012  Gail Schmidt     Original Development
04/28/2013  Song Guo         Modified to calculate terrain slope two
                             above slope algorithms

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error calculating terrain slope
SUCCESS         No errors encountered

NOTES:
  1. Input DEM arrays are 1D arrays of size nlines+[1or2] * nsamps.  An extra
     line should be provided before and after (if possible) the actual subset
     of data to be processed.  Since we are processing an entire line of data
     at a time, it will not be expected that there will be an extra sample on
     either end of the line.  The first and last sample simply will not be
     processed for the shaded relief and the deep shadow mask, thus they should
     already be initialized to an appropriate value before calling this
     function.  The hillshade module requires a 3x3 window to be provided
     surrounding each pixel, thus the extra line(s) of data.
  2. Output mask arrays are 1D arrays of size nlines * nsamps.
******************************************************************************/
int calc_slope
(
    int16 *dem,           /* I: array of DEM values in meters (nlines+[1or2] x
                                nsamps values - see NOTES);  if processing
                                at the top of the image, then an extra line
                                before will not be available;  if processing
                                at the bottom of the image, then an extra line
                                at the end will not be available */
    bool dem_top,         /* I: are we at the top of the dem and therefore no
                                extra lines at the start of the dem? */
    bool dem_bottom,      /* I: are we at the bottom of the dem and therefore no
                                extra lines at the end of the dem? */
    bool use_zeven_thorne,/* I: should we use Zevenbergen&Thorne's slope 
                                algorithm? */
    int nlines,           /* I: number of lines of data to be processed in the
                                mask array; dem array will have one or two lines
                                more depending on top, middle, bottom */
    int nsamps,           /* I: number of samples of data to be processed in the
                                mask array; dem array will have the same number
                                of samples therefore the first and last sample
                                will not be processed as part of the mask since
                                a 3x3 window won't be available */
    float ew_res,         /* I: east/west resolution of the elevation data in
                                meters */
    float ns_res,         /* I: north/south resolution of the elevation data in
                                meters */
    float *percent_slope  /* O: array of percent slope values (multiplied
                                by 100 to indicate percent intensity)
                                of size nlines * nsamps */
)
{
    int line, samp;        /* line and sample being processed */
    int curr_pix;          /* current input pixel being processed */
    int out_pix;           /* current output pixel being processed */
    int start_line;        /* which line to start processing of the output
                              shaded relief and mask */
    int dem_line;          /* line in the DEM associated with the start_line
                              in the mask; should always be a value of 1 */
    int proc_nlines;       /* number of lines to process in the output mask */
    int16 elev_window[9];  /* 3x3 window of elevation values surrounding the
                              current pixel */
    float slope;           /* percent slope value */
    char FUNC_NAME[] = "calc_slope"; /* function name */
    char errmsg[STR_SIZE];     /* error message */

    /* Loop through the lines samples in the array to calculate the percent
       slope.  The first line and column in the input array of DEM data is 
       padding for the 3x3 window, so they won't get processed.  However if we 
       are at the top of the DEM then we don't have padding to do a 3x3 window,
       so just start at line 1.  At the end of the DEM, the same applies, so 
       don't process the last line. */
    start_line = 0;
    proc_nlines = nlines;
    if (dem_top)
        start_line = 1;
    if (dem_bottom)
        proc_nlines = nlines-1;
    for (line = start_line, dem_line = 1; line < proc_nlines;
         line++, dem_line++)
    {
        for (samp = 1; samp < nsamps-1; samp++)
        {
            /* Determine the output pixel location for the current line/samp */
            out_pix = line * nsamps + samp;

            /* Fill in the 3x3 elevation window surrounding the current pixel */
            curr_pix = (dem_line-1) * nsamps + (samp-1);
            elev_window[0] = dem[curr_pix];
            elev_window[1] = dem[curr_pix+1];
            elev_window[2] = dem[curr_pix+2];
            curr_pix = dem_line * nsamps + (samp-1);
            elev_window[3] = dem[curr_pix];
            elev_window[4] = dem[curr_pix+1];
            elev_window[5] = dem[curr_pix+2];
            curr_pix = (dem_line+1) * nsamps + (samp-1);
            elev_window[6] = dem[curr_pix];
            elev_window[7] = dem[curr_pix+1];
            elev_window[8] = dem[curr_pix+2];

            /* Compute the percent slope for the current pixel */
            if (use_zeven_thorne)
            {
                if (ew_res ==  ns_res)  
                    slope = zevenbergen_thorne_slope(elev_window, ew_res,
                            ns_res);
                else
                {
                    sprintf (errmsg, "Error east/west and south/north "
                             "resolution of the elevation data are not equal, "
                             "can't use Zevenburgen&Thorne slope algorithm");
                    error_handler (true, FUNC_NAME, errmsg);
                    return ERROR;
                }
            }
            else
                slope = horn_slope(elev_window, ew_res, ns_res);

            /* Scale the percent slope values from 0.0 to 1.0 to 0 to 100.0 */
            percent_slope[out_pix] = 100.0 * slope;
        }
    }
   
    return SUCCESS;
}

