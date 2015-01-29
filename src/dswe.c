
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>
#include <error.h>
#include <string.h>

#include "error_handler.h"
#include "espa_metadata.h"
#include "parse_metadata.h"
#include "write_metadata.h"
#include "envi_header.h"
#include "espa_geoloc.h"
#include "raw_binary_io.h"

#include "const.h"
#include "utilities.h"
#include "get_args.h"
#include "input.h"
#include "output.h"
#include "build_slope_band.h"


#define CFMASK_SHADOW 2
#define CFMASK_CLOUD 4


#define DSWE_NOT_WATER 0
#define DSWE_WATER_HIGH_CONFIDENCE 1
#define DSWE_WATER_MODERATE_CONFIDENCE 2
#define DSWE_PARTIAL_SURFACE_WATER_PIXEL 3
#define DSWE_CLOUD_SHADOW_CLOUD 9


/*****************************************************************************
  NAME: read_bands_into_memory

  PURPOSE: To read the specified input band data into memory for later
           processing.

  RETURN VALUE:  Type = bool
      Value    Description
      -------  ---------------------------------------------------------------
      true     Success with reading all of the bands into memory.
      false    Failed to read a band into memory.
*****************************************************************************/
int
read_bands_into_memory
(
    Input_Data_t *input_data,
    int16_t *band_blue,
    int16_t *band_green,
    int16_t *band_red,
    int16_t *band_nir,
    int16_t *band_swir1,
    int16_t *band_swir2,
    int16_t *band_dem,
    uint8_t *band_cfmask,
    int element_count
)
{
    int count;

    count = fread (band_blue, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_BLUE]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading blue band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_green, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_GREEN]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading green band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_red, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_RED]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading red band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_nir, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_NIR]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading nir band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_swir1, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_SWIR1]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading swir1 band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_swir2, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_SWIR2]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading swir2 band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_dem, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_DEM]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading DEM band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_cfmask, sizeof (uint8_t), element_count,
                   input_data->band_fd[I_BAND_CFMASK]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading CFMASK band data", MODULE_NAME);

        return ERROR;
    }

    return SUCCESS;
}


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
    int16_t *band_dem,
    uint8_t *band_cfmask,
    float *band_ps,
    uint8_t *band_raw_dswe,
    uint8_t *band_raw_csc_dswe,
    uint8_t *band_raw_ps_csc_dswe
)
{
    free (band_blue);
    free (band_green);
    free (band_red);
    free (band_nir);
    free (band_swir1);
    free (band_swir2);
    free (band_dem);
    free (band_cfmask);
    free (band_ps);
    free (band_raw_dswe);
    free (band_raw_csc_dswe);
    free (band_raw_ps_csc_dswe);
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
    int16_t **band_blue,
    int16_t **band_green,
    int16_t **band_red,
    int16_t **band_nir,
    int16_t **band_swir1,
    int16_t **band_swir2,
    int16_t **band_dem,
    uint8_t **band_cfmask,
    float **band_ps,
    uint8_t **band_raw_dswe,
    uint8_t **band_raw_csc_dswe,
    uint8_t **band_raw_ps_csc_dswe,
    int element_count
)
{
    *band_blue = calloc (element_count, sizeof (int16_t));
    if (*band_blue == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for BLUE band", MODULE_NAME);

        /* No free because we have not allocated any memory yet */
        return ERROR;
    }

    *band_green = calloc (element_count, sizeof (int16_t));
    if (*band_green == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for GREEN band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    *band_red = calloc (element_count, sizeof (int16_t));
    if (*band_red == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for RED band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    *band_nir = calloc (element_count, sizeof (int16_t));
    if (*band_nir == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for NIR band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    *band_swir1 = calloc (element_count, sizeof (int16_t));
    if (*band_swir1 == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for SWIR1 band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    *band_swir2 = calloc (element_count, sizeof (int16_t));
    if (*band_swir2 == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for brightness temp band",
                       MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    *band_dem = calloc (element_count, sizeof (int16_t));
    if (*band_dem == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for DEM band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    *band_cfmask = calloc (element_count, sizeof (uint8_t));
    if (*band_cfmask == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for CFMASK band",
                       MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    *band_ps = calloc (element_count, sizeof (float));
    if (*band_ps == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for percent slope band",
                       MODULE_NAME);

        /* Free allocated memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    *band_raw_dswe = calloc (element_count, sizeof (uint8_t));
    if (band_raw_dswe == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for Raw DSWE band",
                       MODULE_NAME);

        /* Cleanup memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    *band_raw_csc_dswe = calloc (element_count, sizeof (uint8_t));
    if (band_raw_csc_dswe == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for Raw Shadow Cloud DSWE"
                       " band", MODULE_NAME);

        /* Cleanup memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    *band_raw_ps_csc_dswe = calloc (element_count, sizeof (uint8_t));
    if (band_raw_ps_csc_dswe == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for Raw Shadow Cloud PS DSWE"
                       " band", MODULE_NAME);

        /* Cleanup memory */
        free_band_memory (*band_blue, *band_green, *band_red, *band_nir,
                          *band_swir1, *band_swir2, *band_dem, *band_cfmask,
                          *band_ps, *band_raw_dswe, *band_raw_csc_dswe,
                          *band_raw_ps_csc_dswe);
        return ERROR;
    }

    return SUCCESS;
}


/*****************************************************************************
  NAME:  main

  PURPOSE:  Implements the core algorithm for DSWE.

  RETURN VALUE:  Type = int
      Value           Description
      --------------  --------------------------------------------------------
      EXIT_FAILURE    An unrecoverable error occured during processing.
      EXIT_SUCCESS    No errors encountered processing succesfull.
*****************************************************************************/
int
main (int argc, char *argv[])
{
    /* Command line parameters */
    char *xml_filename = NULL;  /* filename for the XML input */
    char *dem_filename = NULL;  /* filename for the DEM input */
    Espa_internal_meta_t xml_metadata;  /* XML metadata structure */
    bool use_zeven_thorne_flag = false;
    bool use_toa_flag = false;
    float wigt;
    float awgt;
    float pswt_1;
    float pswt_2;
    float percent_slope;
    int pswnt_1;
    int pswnt_2;
    int pswst_1;
    int pswst_2;
    bool verbose_flag = false;

    /* Band data */
    Input_Data_t *input_data = NULL;
    int16_t *band_blue = NULL;  /* TM SR_Band1,  OLI SR_Band2 */
    int16_t *band_green = NULL; /* TM SR_Band2,  OLI SR_Band3 */
    int16_t *band_red = NULL;   /* TM SR_Band3,  OLI SR_Band4 */
    int16_t *band_nir = NULL;   /* TM SR_Band4,  OLI SR_Band5 */
    int16_t *band_swir1 = NULL; /* TM SR_Band5,  OLI SR_Band6 */
    int16_t *band_swir2 = NULL; /* TM SR_Band7,  OLI SR_Band7 */
    int16_t *band_dem = NULL;   /* Contains the DEM band */
    uint8_t *band_cfmask = NULL; /* CFMASK */
    float *band_ps = NULL;       /* Contains the generated percent slope */
    uint8_t *band_raw_dswe = NULL; /* Output Raw DSWE band data */
    uint8_t *band_raw_csc_dswe = NULL; /* Output Raw Shadow Cloud DSWE band
                                          data */
    uint8_t *band_raw_ps_csc_dswe = NULL; /* Output Raw shadow Cloud PS DSWE
                                             band data */

    /* Temp variables */
    float mndwi;                /* (green - swir1) / (green + swir1) */
    float mbsrv;                /* (green + red) */
    float mbsrn;                /* (nir + swir1) */
    float awesh;                /* (blue
                                   + (2.5 * green)
                                   - (1.5 * MBSRN)
                                   - (0.25 * bt)) */

    float band_blue_float;
    float band_green_float;
    float band_red_float;
    float band_nir_float;
    float band_swir1_float;
    float band_swir2_float;

    float band_green_scaled;
    float band_swir1_scaled;

    float green_scale_factor;
    float swir1_scale_factor;

    int16_t blue_fill_value;
    int16_t green_fill_value;
    int16_t red_fill_value;
    int16_t nir_fill_value;
    int16_t swir1_fill_value;
    int16_t swir2_fill_value;
    uint8_t cfmask_fill_value;

    int16_t raw_dswe_value;
    uint8_t raw_csc_dswe_value;
    uint8_t raw_ps_csc_dswe_value;

    float pswnt_1_float;
    float pswnt_2_float;
    float pswst_1_float;
    float pswst_2_float;

    /* Other variables */
    int status;
    int index;
    int element_count;


    /* Get the command line arguments */
    status = get_args (argc, argv,
                       &xml_filename,
                       &dem_filename,
                       &use_zeven_thorne_flag,
                       &use_toa_flag,
                       &wigt,
                       &awgt,
                       &pswt_1,
                       &pswt_2,
                       &percent_slope,
                       &pswnt_1,
                       &pswnt_2,
                       &pswst_1,
                       &pswst_2,
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
        printf ("   XML Input File: %s\n", xml_filename);
        printf ("   DEM Input File: %s\n", dem_filename);
        printf ("             WIGT: %0.3f\n", wigt);
        printf ("             AWGT: %0.3f\n", awgt);
        printf ("           PSWT_1: %0.3f\n", pswt_1);
        printf ("           PSWT_2: %0.3f\n", pswt_2);
        printf ("          PSWNT_1: %d\n", pswnt_1);
        printf ("          PSWNT_2: %d\n", pswnt_2);
        printf ("          PSWST_1: %d\n", pswst_1);
        printf ("          PSWST_2: %d\n", pswst_2);
        printf ("    Percent Slope: %0.1f\n", percent_slope);

        printf (" Use Zeven Thorne:");
        if (use_zeven_thorne_flag)
            printf (" TRUE\n");
        else
            printf (" FALSE\n");

        printf (" Use Top Of Atmos:");
        if (use_toa_flag)
            printf (" TRUE\n");
        else
            printf (" FALSE\n");
    }

    /* -------------------------------------------------------------------- */
    /* Validate the input XML metadata file */
    if (validate_xml_file (xml_filename) != SUCCESS)
    {
        /* Error messages already written */
        return EXIT_FAILURE;
    }

    /* Initialize the metadata structure */
    init_metadata_struct (&xml_metadata);

    /* Parse the metadata file into our internal metadata structure; also
       allocates space as needed for various pointers in the global and band
       metadata */
    if (parse_metadata (xml_filename, &xml_metadata) != SUCCESS)
    {
        /* Error messages already written */
        return EXIT_FAILURE;
    }

    /* -------------------------------------------------------------------- */
    /* Open the input files */
    input_data = open_input (&xml_metadata, use_toa_flag, dem_filename);
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
    element_count = input_data->lines * input_data->samples;

    /* Allocate memory buffers for input and temp processing */
    if (allocate_band_memory (&band_blue, &band_green, &band_red, &band_nir,
                              &band_swir1, &band_swir2, &band_dem,
                              &band_cfmask, &band_ps, &band_raw_dswe,
                              &band_raw_csc_dswe, &band_raw_ps_csc_dswe,
                              element_count)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed reading bands into memory", MODULE_NAME);

        return EXIT_FAILURE;
    }

    /* -------------------------------------------------------------------- */
    /* Read the input files into the buffers */
    if (read_bands_into_memory (input_data, band_blue, band_green, band_red,
                                band_nir, band_swir1, band_swir2, band_dem,
                                band_cfmask, element_count)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed reading bands into memory", MODULE_NAME);

        /* Cleanup memory */
        free_band_memory (band_blue, band_green, band_red, band_nir,
                          band_swir1, band_swir2, band_dem, band_cfmask,
                          band_ps, band_raw_dswe, band_raw_csc_dswe,
                          band_raw_ps_csc_dswe);
        free (xml_filename);
        free (dem_filename);
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
    build_slope_band (band_dem, input_data->lines, input_data->samples,
                      input_data->x_pixel_size, input_data->y_pixel_size,
                      use_zeven_thorne_flag, band_ps);

    /* -------------------------------------------------------------------- */
    /* Place the scale factor values into local variables mostly for code
       clarity */
    green_scale_factor = input_data->scale_factor[I_BAND_GREEN];
    swir1_scale_factor = input_data->scale_factor[I_BAND_SWIR1];

    blue_fill_value = input_data->fill_value[I_BAND_BLUE];
    green_fill_value = input_data->fill_value[I_BAND_GREEN];
    red_fill_value = input_data->fill_value[I_BAND_RED];
    nir_fill_value = input_data->fill_value[I_BAND_NIR];
    swir1_fill_value = input_data->fill_value[I_BAND_SWIR1];
    swir2_fill_value = input_data->fill_value[I_BAND_SWIR2];
    cfmask_fill_value = input_data->fill_value[I_BAND_CFMASK];

    /* Free memory no longer needed */
    free (input_data);
    input_data = NULL;

    /* Just convert to float */
    pswnt_1_float = pswnt_1;
    pswnt_2_float = pswnt_2;
    pswst_1_float = pswst_1;
    pswst_2_float = pswst_2;

    /* -------------------------------------------------------------------- */
    /* Process through each data element and populate the dswe band memory */
    printf ("Element Count = %d\n", element_count);
    for (index = 0; index < element_count; index++)
    {
        /* If any of the input is fill, make the output fill */
        if (band_blue[index] == blue_fill_value ||
            band_green[index] == green_fill_value ||
            band_red[index] == red_fill_value ||
            band_nir[index] == nir_fill_value ||
            band_swir1[index] == swir1_fill_value ||
            band_swir2[index] == swir2_fill_value ||
            band_cfmask[index] == cfmask_fill_value)
        {
            band_raw_dswe[index] = DSWE_NO_DATA_VALUE;
            band_raw_csc_dswe[index] = DSWE_NO_DATA_VALUE;
            band_raw_ps_csc_dswe[index] = DSWE_NO_DATA_VALUE;
            continue;
        }

        /* Apply the scaling to these bands accordingly */
        band_green_scaled = band_green[index] * green_scale_factor;
        band_swir1_scaled = band_swir1[index] * swir1_scale_factor;

        /* Just convert to float for now */
        band_blue_float = band_blue[index];
        band_green_float = band_green[index];
        band_red_float = band_red[index];
        band_nir_float = band_nir[index];
        band_swir1_float = band_swir1[index];
        band_swir2_float = band_swir2[index];

        /* Modified Normalized Difference Wetness Index (MNDWI) */
        mndwi = (band_green_scaled - band_swir1_scaled) /
                (band_green_scaled + band_swir1_scaled);

        /* Multi-band Spectral Relationship Visible (MBSRV) */
        mbsrv = band_green_float + band_red_float;

        /* Multi-band Spectral Relationship Near-Infrared (MBSRN) */
        mbsrn = band_nir_float + band_swir1_float;

        /* Automated Water Extent shadow (AWEsh) */
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

        /* Partial Surface Water 1 (PSW1)
           The logic in the if results in a true/false called PSW1 */
        if (mndwi > pswt_1 &&
            band_swir1_float < pswst_1_float &&
            band_nir_float < pswnt_1_float)
        {
            raw_dswe_value += 1000; /* Set the thousands digit */
        }

        /* Partial Surface Water 2 (PSW2)
           The logic in the if results in a true/false called PSW2 */
        if (mndwi > pswt_2 &&
            band_swir2_float < pswst_2_float &&
            band_nir_float < pswnt_2_float)
        {
            raw_dswe_value += 10000; /* Set the ten thousands digit */
        }

        /* Recode the value to fit an 8bit output product */
        switch (raw_dswe_value)
        {
            /* From ESPA_recode.rmp prototype
               11999 11999 : 9    ** Not included here it is only for
                                  ** cfmask tests performed after this
             */

            /* 11001 11111 : 1 */
            case 11111:
            case 11110:
            case 11101:
            case 11100:
            case 11011:
            case 11010:
            case 11001:
            /* 10111 10999 : 1 */
            case 10111:
            /* 1111 1111 : 1 */
            case 1111:
                raw_dswe_value = DSWE_WATER_HIGH_CONFIDENCE;
                break;

            /* 11000 11000 : 3 */
            case 11000:
            /* 10000 10000 : 3 */
            case 10000:
            /* 1000 1000 : 3 */
            case 1000:
                raw_dswe_value = DSWE_PARTIAL_SURFACE_WATER_PIXEL;
                break;

            /* 10012 10110 : 2 */
            case 10110:
            case 10101:
            case 10100:
            /* 10011 10011 : 2 */
            case 10011:
            /* 10001 10010 : 2 */
            case 10010:
            case 10001:
            /* 1001 1110 : 2 */
            case 1110:
            case 1101:
            case 1100:
            case 1011:
            case 1010:
            case 1001:
            /* 10 111 : 2 */
            case 111:
            case 110:
            case 101:
            case 100:
            case 11:
            case 10:
                raw_dswe_value = DSWE_WATER_MODERATE_CONFIDENCE;
                break;

            /* 0 9 : 0 */
            case 1:
            case 0:
            default:
                raw_dswe_value = DSWE_NOT_WATER;
                break;
        }

        /* The following few chunks of code produce the following paths to the
           output products.

           raw -> output
           raw -> cloud shadow -> cloud -> output
           raw -> percent-slope -> cloud shadow -> cloud -> output
           */

        /* Default the
           Cloud Shadow, and Cloud output
           and the
           Percent Slope, Cloud Shadow, and Cloud output
           to the Raw DSWE value */
        raw_csc_dswe_value = raw_dswe_value;
        raw_ps_csc_dswe_value = raw_dswe_value;

        /* Apply the Percent Slope constraint to Percent Slope, Cloud Shadow,
           and Cloud output */
        if (band_ps[index] >= percent_slope)
        {
            raw_ps_csc_dswe_value = DSWE_NOT_WATER;
        }

        /* Apply the CFMASK Cloud Shadow constraint to both the
           Cloud Shadow, and Cloud output
           and the
           Percent Slope, Cloud Shadow, and Cloud output */
        if (band_cfmask[index] == CFMASK_SHADOW)
        {
            /* classified as 11999 in prototype code using 9 due to recode */
            raw_csc_dswe_value = DSWE_CLOUD_SHADOW_CLOUD;
            raw_ps_csc_dswe_value = DSWE_CLOUD_SHADOW_CLOUD;
        }

        /* Apply the CFMASK Cloud constraint to both the
           Cloud Shadow, and Cloud output
           and the
           Percent Slope, Cloud Shadow, and Cloud output */
        if (band_cfmask[index] == CFMASK_CLOUD)
        {
            /* classified as 11999 in prototype code using 9 due to recode */
            raw_csc_dswe_value = DSWE_CLOUD_SHADOW_CLOUD;
            raw_ps_csc_dswe_value = DSWE_CLOUD_SHADOW_CLOUD;
        }

        /* Assign the values to the correct output band */
        band_raw_dswe[index] = raw_dswe_value;
        band_raw_csc_dswe[index] = raw_csc_dswe_value;
        band_raw_ps_csc_dswe[index] = raw_ps_csc_dswe_value;

        /* Let the use know where we are in the processing */
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
                               RAW_PRODUCT_NAME, RAW_BAND_NAME,
                               RAW_SHORT_NAME, RAW_LONG_NAME, DSWE_NOT_WATER,
                               DSWE_PARTIAL_SURFACE_WATER_PIXEL,
                               band_raw_dswe)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed adding Raw DSWE band product", MODULE_NAME);

        /* Cleanup memory */
        free (xml_filename);
        free (dem_filename);

        return EXIT_FAILURE;
    }

    if (add_dswe_band_product (xml_filename, use_toa_flag,
                               SC_PRODUCT_NAME, SC_BAND_NAME,
                               SC_SHORT_NAME, SC_LONG_NAME,
                               DSWE_NOT_WATER, DSWE_CLOUD_SHADOW_CLOUD,
                               band_raw_csc_dswe)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed adding DSWE SHADOW CLOUD band product",
                       MODULE_NAME);

        /* Cleanup memory */
        free (xml_filename);
        free (dem_filename);

        return EXIT_FAILURE;
    }

    if (add_dswe_band_product (xml_filename, use_toa_flag,
                               PS_SC_PRODUCT_NAME, PS_SC_BAND_NAME,
                               PS_SC_SHORT_NAME, PS_SC_LONG_NAME,
                               DSWE_NOT_WATER, DSWE_CLOUD_SHADOW_CLOUD,
                               band_raw_ps_csc_dswe)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed adding DSWE SHADOW CLOUD PERCENT-SLOPE band"
                       " product", MODULE_NAME);

        /* Cleanup memory */
        free (xml_filename);
        free (dem_filename);

        return EXIT_FAILURE;
    }

    /* CLEANUP & EXIT ----------------------------------------------------- */

    /* Cleanup all the input band memory */
    free_band_memory (band_blue, band_green, band_red, band_nir, band_swir1,
                      band_swir2, band_dem, band_cfmask, band_ps,
                      band_raw_dswe, band_raw_csc_dswe, band_raw_ps_csc_dswe);
    band_blue = NULL;
    band_green = NULL;
    band_red = NULL;
    band_nir = NULL;
    band_swir1 = NULL;
    band_swir2 = NULL;
    band_dem = NULL;
    band_cfmask = NULL;
    band_ps = NULL;
    band_raw_dswe = NULL;
    band_raw_csc_dswe = NULL;
    band_raw_ps_csc_dswe = NULL;

    /* Free remaining allocated memory */
    free (xml_filename);
    free (dem_filename);

    LOG_MESSAGE ("Processing complete.", MODULE_NAME);

    return EXIT_SUCCESS;
}
