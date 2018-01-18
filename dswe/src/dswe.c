
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>
#include <error.h>
#include <string.h>
#include <float.h>

#include "error_handler.h"
#include "espa_metadata.h"
#include "parse_metadata.h"
#include "write_metadata.h"
#include "envi_header.h"
#include "espa_geoloc.h"
#include "raw_binary_io.h"

#include "const.h"
#include "dswe.h"
#include "utilities.h"
#include "get_args.h"
#include "input.h"
#include "output.h"
#include "build_slope_band.h"
#include "build_hillshade_band.h"


#define PIXELQA_CLOUD_SHADOW_BIT_MASK (1<<3)
#define PIXELQA_SNOW_BIT_MASK (1<<4)
#define PIXELQA_CLOUD_BIT_MASK (1<<5)


/*****************************************************************************
  NAME:  free_band_memory

  PURPOSE:  Free the memory allocated by allocate_band_memory.

  RETURN VALUE:  None
*****************************************************************************/
void
free_band_memory
(
    int16_t *band_blue,
    int16_t *band_green,
    int16_t *band_red,
    int16_t *band_nir,
    int16_t *band_swir1,
    int16_t *band_swir2,
    int16_t *band_elevation,
    uint16_t *band_pixelqa,
    float *band_ps,
    int16_t *band_ps_int16,
    uint8_t *band_hillshade,
    int16_t *band_dswe_diag,
    uint8_t *band_dswe_interpreted,
    uint8_t *band_dswe_pshsccss,
    uint8_t *band_mask
)
{
    free (band_blue);
    free (band_green);
    free (band_red);
    free (band_nir);
    free (band_swir1);
    free (band_swir2);
    free (band_elevation);
    free (band_pixelqa);
    free (band_ps);
    free (band_ps_int16);
    free (band_hillshade);
    free (band_dswe_diag);
    free (band_dswe_interpreted);
    free (band_dswe_pshsccss);
    free (band_mask);
}


/*****************************************************************************
  NAME:  allocate_band_memory

  PURPOSE:  Allocate memory for all the input bands.

  RETURN VALUE:  Type = bool
      Value    Description
      -------  ---------------------------------------------------------------
      true     Success with allocating all of the memory needed for the input
               bands.
      false    Failed to allocate memory for an input band.
*****************************************************************************/
int
allocate_band_memory
(
    bool include_tests_flag,
    int16_t **band_blue,
    int16_t **band_green,
    int16_t **band_red,
    int16_t **band_nir,
    int16_t **band_swir1,
    int16_t **band_swir2,
    int16_t **band_elevation,
    uint16_t **band_pixelqa,
    float **band_ps,
    int16_t **band_ps_int16,
    uint8_t **band_hillshade,
    int16_t **band_dswe_diag,
    uint8_t **band_dswe_interpreted,
    uint8_t **band_dswe_pshsccss,
    uint8_t **band_mask,
    int pixel_count
)
{
    *band_blue = calloc (pixel_count, sizeof (int16_t));
    if (*band_blue == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for BLUE band", MODULE_NAME);

        /* No free because we have not allocated any memory yet */
        return ERROR;
    }

    *band_green = calloc (pixel_count, sizeof (int16_t));
    if (*band_green == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for GREEN band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_red = calloc (pixel_count, sizeof (int16_t));
    if (*band_red == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for RED band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_nir = calloc (pixel_count, sizeof (int16_t));
    if (*band_nir == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for NIR band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_swir1 = calloc (pixel_count, sizeof (int16_t));
    if (*band_swir1 == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for SWIR1 band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_swir2 = calloc (pixel_count, sizeof (int16_t));
    if (*band_swir2 == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for brightness temp band",
                       MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_elevation = calloc (pixel_count, sizeof (int16_t));
    if (*band_elevation == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for elevation band",
                       MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_pixelqa = calloc (pixel_count, sizeof (uint16_t));
    if (*band_pixelqa == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for Pixel QA band",
                       MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_ps = calloc (pixel_count, sizeof (float));
    if (*band_ps == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for percent slope band",
                       MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_ps_int16 = calloc (pixel_count, sizeof (int16_t));
    if (*band_ps_int16 == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for int16 percent slope band",
                       MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_hillshade = calloc (pixel_count, sizeof (uint8_t));
    if (*band_hillshade == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for hillshade band",
                       MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }


    if (include_tests_flag)
    {
        *band_dswe_diag = calloc (pixel_count, sizeof (int16_t));
        if (band_dswe_diag == NULL)
        {
            ERROR_MESSAGE ("Failed allocating memory for DSWE Diagnostic band",
                           MODULE_NAME);

            /* Cleanup memory */
            free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                              *band_swir1, *band_swir2, *band_elevation,
                              *band_pixelqa, *band_ps, *band_ps_int16, 
                              *band_hillshade, *band_dswe_diag, 
                              *band_dswe_interpreted, *band_dswe_pshsccss, 
                              *band_mask);
            return ERROR;
        }
    }

    *band_dswe_interpreted = calloc (pixel_count, sizeof (uint8_t));
    if (band_dswe_interpreted == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for Interpreted DSWE band",
                       MODULE_NAME);

        /* Cleanup memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_dswe_pshsccss = calloc (pixel_count, sizeof (uint8_t));
    if (band_dswe_pshsccss == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for Interpreted Shadow Cloud "
                       "PS DSWE band", MODULE_NAME);

        /* Cleanup memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    *band_mask = calloc (pixel_count, sizeof (uint8_t));
    if (band_mask == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for output mask band",
                       MODULE_NAME);

        /* Cleanup memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_elevation,
                          *band_pixelqa, *band_ps, *band_ps_int16, 
                          *band_hillshade, *band_dswe_diag, 
                          *band_dswe_interpreted, *band_dswe_pshsccss, 
                          *band_mask);
        return ERROR;
    }

    return SUCCESS;
}


/*****************************************************************************
  NAME:  main

  PURPOSE:  Implements the core algorithm for DSWE.

  ALGORITHM DEVELOPERS:

      The algorithm implemented here was developed by the following:

      John W. Jones
      Research Geographer
      Eastern Geographic Science Center
      U.S. Geological Survey
      email: jwjones@usgs.gov

      Michael J. Starbuck
      Physical Scientist
      Earth Resources Observation and Science Center
      U.S. Geological Survey
      email: mstarbuck@usgs.gov

  RETURN VALUE:  Type = int
      Value           Description
      --------------  --------------------------------------------------------
      EXIT_FAILURE    An unrecoverable error occured during processing.
      EXIT_SUCCESS    No errors encountered processing successful.
*****************************************************************************/
int
main (int argc, char *argv[])
{
    /* Command line parameters */
    char *xml_filename = NULL;  /* filename for the XML input */
    Espa_internal_meta_t xml_metadata;  /* XML metadata structure */
    bool use_zeven_thorne_flag = false;
    bool use_toa_flag = false;
    bool include_tests_flag = false;
    bool include_ps_flag = false; /* Flag for including percent slope output */
    bool include_hs_flag = false; /* Flag for including hillshade output */
    float wigt;                  /* tolerance value */
    float awgt;                  /* tolerance value */
    float pswt_1_mndwi;          /* tolerance value */
    int pswt_1_nir;              /* tolerance value */
    int pswt_1_swir1;            /* tolerance value */
    float pswt_1_ndvi;           /* tolerance value */
    float pswt_2_mndwi;          /* tolerance value */
    int pswt_2_blue;             /* tolerance value */
    int pswt_2_nir;              /* tolerance value */
    int pswt_2_swir1;            /* tolerance value */
    int pswt_2_swir2;            /* tolerance value */
    float percent_slope_high;    /* Slope tolerance for high confidence water */
    float percent_slope_moderate; /* Slope tolerance for moderate confidence
                                   water */
    float percent_slope_wetland; /* Slope tolerance for potential wetland */
    float percent_slope_low;     /* Slope tolerance for low confidence water or
                                     wetland */
    int hillshade;               /* Hillshade tolerance value */ 
    bool verbose_flag = false;

    /* Band data */
    Input_Data_t *input_data = NULL;
    int16_t *band_blue = NULL;  /* TM SR_Band1,  OLI SR_Band2 */
    int16_t *band_green = NULL; /* TM SR_Band2,  OLI SR_Band3 */
    int16_t *band_red = NULL;   /* TM SR_Band3,  OLI SR_Band4 */
    int16_t *band_nir = NULL;   /* TM SR_Band4,  OLI SR_Band5 */
    int16_t *band_swir1 = NULL; /* TM SR_Band5,  OLI SR_Band6 */
    int16_t *band_swir2 = NULL; /* TM SR_Band7,  OLI SR_Band7 */
    int16_t *band_elevation = NULL; /* Contains the elevation band */
    uint16_t *band_pixelqa = NULL;  /* Pixel QA */
    float *band_ps = NULL;       /* Contains the generated percent slope */
    int16_t *band_ps_int16 = NULL; /* Scaled percent slope converted to int16 */
    uint8_t *band_hillshade = NULL; /* Contains the generated hillshade */
    int16_t *band_dswe_diag = NULL;   /* Output DSWE diagnostic band data */
    uint8_t *band_dswe_interpreted = NULL;    /* Output interpreted DSWE band 
                                   data */
    uint8_t *band_dswe_pshsccss = NULL; /* Output interpreted DSWE band data 
                                         with Percent Slope, Hillshade, Cloud, 
                                         and Cloud Shadow filtering applied */
    uint8_t *band_mask = NULL;  /* Output mask band data */

    /* Temp variables */
    float mndwi;                /* (green - swir1) / (green + swir1) */
    float mbsrv;                /* (green + red) */
    float mbsrn;                /* (nir + swir1) */
    float awesh;                /* (blue
                                   + (2.5 * green)
                                   - (1.5 * MBSRN)
                                   - (0.25 * bt)) */
    float ndvi;                /* (nir - red) / (nir + red) */

    float band_blue_float;
    float band_green_float;
    float band_red_float;
    float band_nir_float;
    float band_swir1_float;
    float band_swir2_float;

    float percent_slope;        /* Single percent slope value */
    bool hillshade_flag;

    int16_t blue_fill_value;
    int16_t green_fill_value;
    int16_t red_fill_value;
    int16_t nir_fill_value;
    int16_t swir1_fill_value;
    int16_t swir2_fill_value;
    uint16_t pixelqa_fill_value;

    int16_t raw_dswe_value;
    uint8_t interp_ps_hs_ccss_dswe_value; /* Interpreted DSWE value, but set to 
                                   DSWE_NOT_WATER if percent slope or hillshade
                                   apply, and DSWE_CLOUD_CLOUD_SHADOW_SNOW if
                                   one or more of those is set in the QA band */
    uint8_t mask_value;         /* Tracks whether a pixel is masked due to snow,
                                   shadow, cloud, slope, and/or hillshade */                       

    float pswt_1_nir_float;     /* Float version of tolerance value */
    float pswt_1_swir1_float;   /* Float version of tolerance value */
    float pswt_2_blue_float;    /* Float version of tolerance value */
    float pswt_2_nir_float;     /* Float version of tolerance value */
    float pswt_2_swir1_float;   /* Float version of tolerance value */
    float pswt_2_swir2_float;   /* Float version of tolerance value */


    /* Other variables */
    int status;
    int index;
    int pixel_count;


    /* Get the command line arguments */
    status = get_args (argc, argv,
                       &xml_filename,
                       &xml_metadata,
                       &use_zeven_thorne_flag,
                       &use_toa_flag,
                       &include_tests_flag,
                       &include_ps_flag,
                       &include_hs_flag,
                       &wigt,
                       &awgt,
                       &pswt_1_mndwi,
                       &pswt_1_nir,
                       &pswt_1_swir1,
                       &pswt_1_ndvi,
                       &pswt_2_mndwi,
                       &pswt_2_blue,
                       &pswt_2_nir,
                       &pswt_2_swir1,
                       &pswt_2_swir2,
                       &percent_slope_high,
                       &percent_slope_moderate,
                       &percent_slope_wetland,
                       &percent_slope_low,
                       &hillshade,
                       &verbose_flag);
    if (status != SUCCESS)
    {
        /* get_args generates all the error messages we need */
        return EXIT_FAILURE;
    }

    LOG_MESSAGE ("Starting dynamic surface water extent processing ...",
                 MODULE_NAME);

    /* -------------------------------------------------------------------- */
    /* Provide user information if verbose is turned on */
    if (verbose_flag)
    {
        printf ("            XML Input File: %s\n", xml_filename);
        printf ("                      WIGT: %0.3f\n", wigt);
        printf ("                      AWGT: %0.3f\n", awgt);
        printf ("              PSWT_1_MNDWI: %0.3f\n", pswt_1_mndwi);
        printf ("                PSWT_1_NIR: %d\n", pswt_1_nir);
        printf ("              PSWT_1_SWIR1: %d\n", pswt_1_swir1);
        printf ("               PSWT_1_NDVI: %0.3f\n", pswt_1_ndvi);
        printf ("              PSWT_2_MNDWI: %0.3f\n", pswt_2_mndwi);
        printf ("               PSWT_2_BLUE: %d\n", pswt_2_blue);
        printf ("                PSWT_2_NIR: %d\n", pswt_2_nir);
        printf ("              PSWT_2_SWIR1: %d\n", pswt_2_swir1);
        printf ("              PSWT_2_SWIR2: %d\n", pswt_2_swir2);
        printf ("        Percent Slope High: %0.1f\n", percent_slope_high);
        printf ("    Percent Slope Moderate: %0.1f\n", percent_slope_moderate);
        printf ("     Percent Slope Wetland: %0.1f\n", percent_slope_wetland);
        printf ("         Percent Slope Low: %0.1f\n", percent_slope_low);
        printf ("       Hillshade Threshold: %d\n", hillshade);

        printf ("          Use Zeven Thorne:");
        if (use_zeven_thorne_flag)
            printf (" TRUE\n");
        else
            printf (" FALSE\n");

        printf ("          Use Top Of Atmos:");
        if (use_toa_flag)
            printf (" TRUE\n");
        else
            printf (" FALSE\n");

        printf (" Make Diagnostic Test Band:");
        if (include_tests_flag)
            printf (" TRUE\n");
        else
            printf (" FALSE\n");

        printf ("   Make Percent Slope Band:");
        if (include_ps_flag)
            printf (" TRUE\n");
        else
            printf (" FALSE\n");

        printf ("       Make Hillshade Band:");
        if (include_hs_flag)
            printf (" TRUE\n");
        else
            printf (" FALSE\n");
    }

    /* -------------------------------------------------------------------- */
    /* Open the input files */
    input_data = open_input (&xml_metadata, use_toa_flag);
    if (input_data == NULL)
    {
        ERROR_MESSAGE ("Failed opening input files", MODULE_NAME);

        /* Cleanup memory */
        free_metadata (&xml_metadata);
        return EXIT_FAILURE;
    }

    /* Free the metadata structure */
    /* ******** NO LONGER NEEDED IN THIS MAIN CODE ******** */
    free_metadata (&xml_metadata);

    /* -------------------------------------------------------------------- */
    /* Figure out the number of elements in the data */
    pixel_count = input_data->lines * input_data->samples;

    /* Allocate memory buffers for input and temp processing */
    if (allocate_band_memory (include_tests_flag, &band_blue, &band_green,
                              &band_red, &band_nir, &band_swir1, &band_swir2,
                              &band_elevation, &band_pixelqa, &band_ps,
                              &band_ps_int16, &band_hillshade, &band_dswe_diag, 
                              &band_dswe_interpreted, &band_dswe_pshsccss, 
                              &band_mask, pixel_count)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed reading bands into memory", MODULE_NAME);

        return EXIT_FAILURE;
    }

    /* -------------------------------------------------------------------- */
    /* Read the input files into the buffers */
    if (read_bands_into_memory (input_data, band_blue, band_green, band_red,
                                band_nir, band_swir1, band_swir2,
                                band_elevation, band_pixelqa, pixel_count)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed reading bands into memory", MODULE_NAME);

        /* Cleanup memory */
        free_band_memory (band_blue, band_green, band_red, band_nir,
                          band_swir1, band_swir2, band_elevation,
                          band_pixelqa, band_ps, band_ps_int16, band_hillshade, 
                          band_dswe_diag, band_dswe_interpreted, 
                          band_dswe_pshsccss, band_mask);
        free (xml_filename);
        free (input_data);

        return EXIT_FAILURE;
    }

    /* -------------------------------------------------------------------- */
    /* Close the input files */
    if (close_input (input_data) != SUCCESS)
    {
        WARNING_MESSAGE ("Failed closing input files", MODULE_NAME);
    }

    /* -------------------------------------------------------------------- */
    build_slope_band (band_elevation, input_data->lines, input_data->samples,
                      input_data->x_pixel_size, input_data->y_pixel_size,
                      use_zeven_thorne_flag, band_ps);

    /* -------------------------------------------------------------------- */
    build_hillshade_band (band_elevation, input_data->lines, 
                      input_data->samples, input_data->x_pixel_size, 
                      input_data->y_pixel_size, input_data->solar_elevation, 
                      input_data->solar_azimuth, band_hillshade);

    /* -------------------------------------------------------------------- */
    blue_fill_value = input_data->fill_value[I_BAND_BLUE];
    green_fill_value = input_data->fill_value[I_BAND_GREEN];
    red_fill_value = input_data->fill_value[I_BAND_RED];
    nir_fill_value = input_data->fill_value[I_BAND_NIR];
    swir1_fill_value = input_data->fill_value[I_BAND_SWIR1];
    swir2_fill_value = input_data->fill_value[I_BAND_SWIR2];
    pixelqa_fill_value = input_data->fill_value[I_BAND_PIXELQA];

    /* Free memory no longer needed */
    free (input_data);
    input_data = NULL;

    /* Just convert to float */
    pswt_1_nir_float = pswt_1_nir;
    pswt_1_swir1_float = pswt_1_swir1;
    pswt_2_blue_float = pswt_2_blue;
    pswt_2_nir_float = pswt_2_nir;
    pswt_2_swir1_float = pswt_2_swir1;
    pswt_2_swir2_float = pswt_2_swir2;

    /* -------------------------------------------------------------------- */
    /* Process through each data element and populate the dswe band memory */
    if (verbose_flag)
    {
        printf ("               Pixel Count: %d\n", pixel_count);
    }
    for (index = 0; index < pixel_count; index++)
    {
        /* If any of the input is fill, make the output fill */
        if (band_blue[index] == blue_fill_value ||
            band_green[index] == green_fill_value ||
            band_red[index] == red_fill_value ||
            band_nir[index] == nir_fill_value ||
            band_swir1[index] == swir1_fill_value ||
            band_swir2[index] == swir2_fill_value ||
            band_pixelqa[index] == pixelqa_fill_value)
        {
            if (include_tests_flag)
            {
                band_dswe_diag[index] = TESTS_NO_DATA_VALUE;
            }
            band_dswe_interpreted[index] = DSWE_NO_DATA_VALUE;
            band_dswe_pshsccss[index] = DSWE_NO_DATA_VALUE;
            band_mask[index] = DSWE_NO_DATA_VALUE;
            continue;
        }

        /* Convert to float */
        band_blue_float = band_blue[index];
        band_green_float = band_green[index];
        band_red_float = band_red[index];
        band_nir_float = band_nir[index];
        band_swir1_float = band_swir1[index];
        band_swir2_float = band_swir2[index];

        /* Modified Normalized Difference Wetness Index (MNDWI) */
        mndwi = (band_green_float - band_swir1_float) /
                (band_green_float + band_swir1_float);

        /* Multi-band Spectral Relationship Visible (MBSRV) */
        mbsrv = band_green_float + band_red_float;

        /* Multi-band Spectral Relationship Near-Infrared (MBSRN) */
        mbsrn = band_nir_float + band_swir1_float;

        /* Automated Water Extent Shadow (AWEsh) */
        awesh = (band_blue_float
                 + (2.5 * band_green_float)
                 - (1.5 * mbsrn)
                 - (0.25 * band_swir2_float));

        /* Initialize to 0 or 1 on the first test */
        if (mndwi > wigt)
            raw_dswe_value = 1; /* > wigt */  /* Set the ones digit */
        else
            raw_dswe_value = 0;

        if (mbsrv > mbsrn)
            raw_dswe_value += 10; /* Set the tens digit */

        if (awesh > awgt)
            raw_dswe_value += 100; /* Set the hundreds digit */

        /* Calculate NDVI */
        ndvi = (band_nir_float - band_red_float) /
               (band_nir_float + band_red_float);

        /* Partial Surface Water 1 (PSW1)
           The logic in the if results in a true/false called PSW1 */
        if (mndwi > pswt_1_mndwi &&
            band_swir1_float < pswt_1_swir1_float &&
            band_nir_float < pswt_1_nir_float &&
            ndvi < pswt_1_ndvi)
        {
            raw_dswe_value += 1000; /* Set the thousands digit */
        }

        /* Partial Surface Water 2 (PSW2)
           The logic in the if results in a true/false called PSW2 */
        if (mndwi > pswt_2_mndwi &&
            band_blue_float < pswt_2_blue_float &&
            band_swir1_float < pswt_2_swir1_float &&
            band_swir2_float < pswt_2_swir2_float &&
            band_nir_float < pswt_2_nir_float)
        {
            raw_dswe_value += 10000; /* Set the ten thousands digit */
        }

        /* Assign it to the tests band */
        if (include_tests_flag)
        {
            band_dswe_diag[index] = raw_dswe_value;
        }

        /* Determine if hillshade exceeds threshold */
        if (band_hillshade[index] > hillshade)
        {
            hillshade_flag = true;
        }
        else
        {
            hillshade_flag = false;
        }

        /* Recode the raw value to an interpreted value to fit an 8bit output 
           product */
        switch (raw_dswe_value)
        {
            case 0:
            case 1:
            case 10:
            case 100:
            case 1000:
                raw_dswe_value = DSWE_NOT_WATER;
                break;

            case 1111:
            case 10111:
            case 11011:
            case 11101:
            case 11110:
            case 11111:
                raw_dswe_value = DSWE_WATER_HIGH_CONFIDENCE;
                break;

            case 111:
            case 1011:
            case 1101:
            case 1110:
            case 10011:
            case 10101:
            case 10110:
            case 11001:
            case 11010:
            case 11100:
                raw_dswe_value = DSWE_WATER_MODERATE_CONFIDENCE;
                break;

            case 11000:
                raw_dswe_value = DSWE_POTENTIAL_WETLAND;
                break;

            case 11:
            case 101:
            case 110:
            case 1001:
            case 1010:
            case 1100:
            case 10000:
            case 10001:
            case 10010:
            case 10100:
                raw_dswe_value = DSWE_LOW_CONFIDENCE_WATER_OR_WETLAND;
                break;

            default:
                raw_dswe_value = DSWE_NO_DATA_VALUE;
                break;
        }

        /* The following few chunks of code produce the following paths to the
           output products.

           interpreted -> output
           interpreted -> percent-slope -> hillshade -> cloud -> cloud shadow ->
                  snow -> output
           percent-slope -> hillshade -> cloud -> cloud shadow -> snow -> output
        */

        /* Default the Percent Slope, Hillshade, Cloud, Cloud Shadow, and Snow 
           output to the interpreted DSWE value */
        interp_ps_hs_ccss_dswe_value = raw_dswe_value;

        /* Initialize the mask value based on some bits in the pixel QA. */ 
        mask_value = 0;
        if (band_pixelqa[index] & PIXELQA_CLOUD_SHADOW_BIT_MASK)
        {
            mask_value |= (1 << MASK_SHADOW);
        }
        if (band_pixelqa[index] & PIXELQA_SNOW_BIT_MASK)
        {
            mask_value |= (1 << MASK_SNOW);
        }
        if (band_pixelqa[index] & PIXELQA_CLOUD_BIT_MASK)
        {
            mask_value |= (1 << MASK_CLOUD);
        }

        /* Apply the Percent Slope constraint to the Percent Slope, Cloud,
           Cloud Shadow, and Snow output.  Also update the mask output. */
        if (raw_dswe_value == DSWE_WATER_MODERATE_CONFIDENCE)
        {
            if (band_ps[index] >= percent_slope_moderate)
            {
                interp_ps_hs_ccss_dswe_value = DSWE_NOT_WATER;
                mask_value |= (1 << MASK_PS);
            }
        }
        else if (raw_dswe_value == DSWE_POTENTIAL_WETLAND)
        {
            if (band_ps[index] >= percent_slope_wetland)
            {
                interp_ps_hs_ccss_dswe_value = DSWE_NOT_WATER;
                mask_value |= (1 << MASK_PS);
            }
        }
        else if (raw_dswe_value == DSWE_LOW_CONFIDENCE_WATER_OR_WETLAND)
        {
            if (band_ps[index] >= percent_slope_low)
            {
                interp_ps_hs_ccss_dswe_value = DSWE_NOT_WATER;
                mask_value |= (1 << MASK_PS);
            }
        }
        else if (raw_dswe_value == DSWE_WATER_HIGH_CONFIDENCE)
        {
            if (band_ps[index] >= percent_slope_high)
            {
                interp_ps_hs_ccss_dswe_value = DSWE_NOT_WATER;
                mask_value |= (1 << MASK_PS);
            }
        }

        /* Apply the hillshade constraint to the Percent Slope, Cloud,
           Cloud Shadow, and Snow output.  Also update the mask output. */
        if (!hillshade_flag)
        {
            interp_ps_hs_ccss_dswe_value = DSWE_NOT_WATER;
            mask_value |= (1 << MASK_HS);
        }

        /* Apply the Pixel QA Cloud constraint to the Percent Slope, Hillshade,
           Cloud, Cloud Shadow, and Snow output */
        if ((band_pixelqa[index] & PIXELQA_CLOUD_BIT_MASK)
             || (band_pixelqa[index] & PIXELQA_CLOUD_SHADOW_BIT_MASK)
             || (band_pixelqa[index] & PIXELQA_SNOW_BIT_MASK))
        {
            /* classified as 11999 in prototype code using 9 due to recode */
            interp_ps_hs_ccss_dswe_value = DSWE_CLOUD_CLOUD_SHADOW_SNOW;
        }

        /* Assign the values to the correct output band */
        band_dswe_interpreted[index] = raw_dswe_value;
        band_dswe_pshsccss[index] = interp_ps_hs_ccss_dswe_value;
        band_mask[index] = mask_value;

        /* Let the user know where we are in the processing */
        if (index%99999 == 0)
        {
            printf ("\r");
            printf ("Processed data element %d", index);
        }
    }

    /* Status output cleanup to match the final output size */
    printf ("\r");
    printf ("Processed data element %d", index);
    printf ("\n");

    /* Add the DSWE bands to the metadata file and generate the ENVI images
       and header files */
    if (add_dswe_band_product (xml_filename, use_toa_flag,
                               INTERPRETED_PRODUCT_NAME, INTERPRETED_BAND_NAME,
                               INTERPRETED_SHORT_NAME, INTERPRETED_LONG_NAME, 
                               DSWE_NOT_WATER, 
                               DSWE_LOW_CONFIDENCE_WATER_OR_WETLAND, 1, 0,
                               band_dswe_interpreted)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed adding Interpreted DSWE band product", 
                       MODULE_NAME);

        /* Cleanup memory */
        free (xml_filename);

        return EXIT_FAILURE;
    }

    if (add_dswe_band_product (xml_filename, use_toa_flag,
                               PS_SC_PRODUCT_NAME, PS_SC_BAND_NAME,
                               PS_SC_SHORT_NAME, PS_SC_LONG_NAME,
                               DSWE_NOT_WATER, DSWE_CLOUD_CLOUD_SHADOW_SNOW,
                               1, 0, band_dswe_pshsccss)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed adding DSWE PERCENT-SLOPE SHADOW CLOUD band"
                       " product", MODULE_NAME);

        /* Cleanup memory */
        free (xml_filename);

        return EXIT_FAILURE;
    }

    if (add_dswe_band_product (xml_filename, use_toa_flag,
                               MASK_PRODUCT_NAME, MASK_BAND_NAME,
                               MASK_SHORT_NAME, MASK_LONG_NAME, 0, 31,
                               0, 1, band_mask)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed adding DSWE mask band", MODULE_NAME);

        /* Cleanup memory */
        free (xml_filename);

        return EXIT_FAILURE;
    }

    if (include_tests_flag)
    {
        if (add_test_band_product (xml_filename, use_toa_flag,
                                   DIAG_PRODUCT_NAME, DIAG_BAND_NAME,
                                   DIAG_SHORT_NAME, DIAG_LONG_NAME,
                                   0, 11111, band_dswe_diag)
            != SUCCESS)
        {
            ERROR_MESSAGE ("Failed adding DIAGNOSTIC DSWE band product",
                           MODULE_NAME);

            /* Cleanup memory */
            free (xml_filename);

            return EXIT_FAILURE;
        }
    }

    if (include_ps_flag)
    {
        /* Convert to a scaled 16 bit integer value */
        for (index = 0; index < pixel_count; index++)
        {
            percent_slope = (band_ps[index] * PERCENT_SLOPE_MULT_FACTOR) + 0.5;

            /* If the scaled value is outside the range, pull it back */
            if (percent_slope > GDAL_INT16_MAX)
            {
                percent_slope = GDAL_INT16_MAX;
            }
            band_ps_int16[index] = (int16_t)percent_slope; 
        }

        if (add_ps_band_product (xml_filename, use_toa_flag,
                                 PS_PRODUCT_NAME, PS_BAND_NAME,
                                 PS_SHORT_NAME, PS_LONG_NAME,
                                 0, GDAL_INT16_MAX, band_ps_int16)
            != SUCCESS)
        {
            ERROR_MESSAGE ("Failed adding DSWE PERCENT-SLOPE band product",
                           MODULE_NAME);

            /* Cleanup memory */
            free (xml_filename);

            return EXIT_FAILURE;
        }
    }

    if (include_hs_flag)
    {
        if (add_dswe_band_product (xml_filename, use_toa_flag,
                                   HS_PRODUCT_NAME, HS_BAND_NAME,
                                   HS_SHORT_NAME, HS_LONG_NAME,
                                   0, 255, 0, 0, band_hillshade)
            != SUCCESS)
        {
            ERROR_MESSAGE ("Failed adding DSWE hillshade band product",
                           MODULE_NAME);

            /* Cleanup memory */
            free (xml_filename);

            return EXIT_FAILURE;
        }
    }

    /* CLEANUP & EXIT ----------------------------------------------------- */

    /* Cleanup all the input band memory */
    free_band_memory (band_blue, band_green, band_red, band_nir, band_swir1,
                      band_swir2, band_elevation, band_pixelqa, band_ps,
                      band_ps_int16, band_hillshade, band_dswe_diag, 
                      band_dswe_interpreted, band_dswe_pshsccss, band_mask);
                      
    band_blue = NULL;
    band_green = NULL;
    band_red = NULL;
    band_nir = NULL;
    band_swir1 = NULL;
    band_swir2 = NULL;
    band_elevation = NULL;
    band_pixelqa = NULL;
    band_ps = NULL;
    band_ps_int16 = NULL;
    band_hillshade = NULL;
    band_dswe_diag = NULL;
    band_dswe_interpreted = NULL;
    band_dswe_pshsccss = NULL;
    band_mask = NULL;

    /* Free remaining allocated memory */
    free (xml_filename);

    LOG_MESSAGE ("Processing complete.", MODULE_NAME);

    return EXIT_SUCCESS;
}

