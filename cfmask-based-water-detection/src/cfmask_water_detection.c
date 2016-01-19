
#include <stdlib.h>
#include <stdbool.h>
#if 0
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
#endif
#include "espa_metadata.h"
#include "parse_metadata.h"


#include "const.h"
#include "cfmask_water_detection.h"
#include "utilities.h"
#include "get_args.h"
#include "input.h"
#if 0
#include "output.h"


#define CFMASK_CLOUD_SHADOW 2
#define CFMASK_SNOW 3
#define CFMASK_CLOUD 4


#define DSWE_NOT_WATER 0
#define DSWE_WATER_HIGH_CONFIDENCE 1
#define DSWE_WATER_MODERATE_CONFIDENCE 2
#define DSWE_PARTIAL_SURFACE_WATER_PIXEL 3
#define DSWE_CLOUD_CLOUD_SHADOW_SNOW 9
#endif


/*****************************************************************************
  NAME:  free_band_memory

  PURPOSE:  Free the memory allocated by allocate_band_memory.

  RETURN VALUE:  None
*****************************************************************************/
void
free_band_memory
(
    int16_t *band_red,
    int16_t *band_nir,
    uint8_t *band_l2qa
)
{
    free (band_red);
    band_red = NULL;
    free (band_nir);
    band_nir = NULL;
    free (band_l2qa);
    band_l2qa = NULL;
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
    int16_t **band_red,
    int16_t **band_nir,
    uint8_t **band_l2qa,
    int pixel_count
)
{
    *band_red = calloc(pixel_count, sizeof(int16_t));
    if (*band_red == NULL)
    {
        ERROR_MESSAGE("Failed allocating memory for RED band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory(*band_red, *band_nir, *band_l2qa);
        return ERROR;
    }

    *band_nir = calloc(pixel_count, sizeof(int16_t));
    if (*band_nir == NULL)
    {
        ERROR_MESSAGE("Failed allocating memory for NIR band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory(*band_red, *band_nir, *band_l2qa);
        return ERROR;
    }

    *band_l2qa = calloc(pixel_count, sizeof(uint8_t));
    if (*band_l2qa == NULL)
    {
        ERROR_MESSAGE("Failed allocating memory for CFMASK band",
                      MODULE_NAME);

        /* Free allocated memory */
        free_band_memory(*band_red, *band_nir, *band_l2qa);
        return ERROR;
    }

    return SUCCESS;
}


bool zhe_water_test
(
    int16_t *band_nir,
    int index,
    float ndvi
)
{

    return false;
}


/*****************************************************************************
  NAME:  main

  PURPOSE:  Implements the core algorithm for cfmask based water detection.

  ALGORITHM DEVELOPERS:

      The algorithm implemented here was developed by the following:

      TODO TODO TODO

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
    Espa_internal_meta_t xml_metadata;  /* XML metadata structure */
    bool verbose_flag = false;

    Input_Data_t *input_data = NULL;
    /* Band data */
    int16_t *band_red = NULL;  /* TM TOA_Band3,  OLI TOA_Band4 */
    int16_t *band_nir = NULL;  /* TM TOA_Band4,  OLI TOA_Band5 */
    uint8_t *band_l2qa = NULL; /* Level2 QA Band */

    float ndvi;

    int16_t red_fill_value;
    int16_t nir_fill_value;
    uint8_t l2qa_fill_value;

    /* Other variables */
    int pixel_index;
    int pixel_count;


    /* Get the command line arguments */
    if (get_args(argc, argv, &xml_filename, &verbose_flag) != SUCCESS)
    {
        /* get_args generates all the error messages we need */
        return EXIT_FAILURE;
    }

    LOG_MESSAGE("Starting CFmask based water detection processing ...",
                MODULE_NAME);

    /* -------------------------------------------------------------------- */
    /* Provide user information if verbose is turned on */
    if (verbose_flag)
    {
        printf("   XML Input File: %s\n", xml_filename);
    }

    /* -------------------------------------------------------------------- */
    /* Validate the input XML metadata file */
    if (validate_xml_file(xml_filename) != SUCCESS)
    {
        /* Error messages already written */
        return EXIT_FAILURE;
    }

    /* Initialize the metadata structure */
    init_metadata_struct(&xml_metadata);

    /* Parse the metadata file into our internal metadata structure; also
       allocates space as needed for various pointers in the global and band
       metadata */
    if (parse_metadata(xml_filename, &xml_metadata) != SUCCESS)
    {
        /* Error messages already written */
        return EXIT_FAILURE;
    }

    /* -------------------------------------------------------------------- */
    /* Open the input files */
    input_data = open_input(&xml_metadata);
    if (input_data == NULL)
    {
        ERROR_MESSAGE("Failed opening input files", MODULE_NAME);

        /* Cleanup memory */
        free_metadata(&xml_metadata);
        return EXIT_FAILURE;
    }

    /* Free the metadata structure */
    /* ******** NO LONGER NEEDED IN THIS MAIN CODE ******** */
    free_metadata(&xml_metadata);

    /* -------------------------------------------------------------------- */
    /* Figure out the number of elements in the data */
    pixel_count = input_data->lines * input_data->samples;
    if (verbose_flag)
    {
        printf ("Pixel Count = %d\n", pixel_count);
    }

    /* Allocate memory buffers for input and temp processing */
    if (allocate_band_memory(&band_red, &band_nir, &band_l2qa,
                             pixel_count) != SUCCESS)
    {
        ERROR_MESSAGE ("Failed reading bands into memory", MODULE_NAME);

        return EXIT_FAILURE;
    }

    /* -------------------------------------------------------------------- */
    /* Read the input files into the buffers */
    if (read_bands_into_memory(input_data, band_red, band_nir,
                               band_l2qa, pixel_count) != SUCCESS)
    {
        ERROR_MESSAGE("Failed reading bands into memory", MODULE_NAME);

        /* Cleanup memory */
        free_band_memory(band_red, band_nir, band_l2qa);
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

    red_fill_value = input_data->fill_value[I_BAND_RED];
    nir_fill_value = input_data->fill_value[I_BAND_NIR];
    l2qa_fill_value = input_data->fill_value[I_BAND_L2QA];

#if 0
    /* Free memory no longer needed */
    free (input_data);
    input_data = NULL;
#endif

    /* -------------------------------------------------------------------- */
    /* Process through each data element and populate the dswe band memory */
    for (pixel_index = 0; pixel_index < pixel_count; pixel_index++)
    {
        /* If any of the input is fill, make the output fill */
        if (band_red[pixel_index] == red_fill_value ||
            band_nir[pixel_index] == nir_fill_value ||
            band_l2qa[pixel_index] == l2qa_fill_value)
        {
            band_l2qa[pixel_index] = l2qa_fill_value;
            continue;
        }
        /* If the data is already marked we do not need to process it */
// TODO TODO TODO - Maybe if the L1 -> L2 converter marks clear, we only need to test for clear pixels here
        if (band_l2qa[pixel_index] == L2QA_CLOUD_PIXEL ||
            band_l2qa[pixel_index] == L2QA_CLOUD_SHADOW_PIXEL ||
            band_l2qa[pixel_index] == L2QA_SNOW_PIXEL)
        {
            continue;
        }

        if ((band_red[pixel_index] + band_nir[pixel_index]) != 0)
        {
            ndvi = (float)(band_nir[pixel_index] - band_red[pixel_index])
                   / (float)(band_nir[pixel_index] + band_red[pixel_index]);
        }
        else
            ndvi = 0.01;

        /* Zhe's water test (works over thin cloud), equation 5 (CFmask) */
        if ((ndvi < 0.01 && band_nir[pixel_index] < 1100)
            || (ndvi < 0.1 && ndvi > 0.0 && band_nir[pixel_index] < 500))
        {
            band_l2qa[pixel_index] |= L2QA_WATER_PIXEL;
        }
        else
            band_l2qa[pixel_index] &= ~L2QA_WATER_PIXEL;

        /* Let the use know where we are in the processing */
        if (pixel_index%99999 == 0)
        {
            printf("\rProcessed data element %d", pixel_index);
        }
    }
    /* Status output cleanup to match the final output size */
    printf("\rProcessed data element %d\n", pixel_index);

// TODO TODO TODO
# if 0
    /* The L2 QA Band already exists in the metadata, so we only need to write
       the band data to disk */
    // TODO TODO TODO - is that really true????????????????????
    if (add_dswe_band_product (xml_filename, use_toa_flag,
                               RAW_PRODUCT_NAME, RAW_BAND_NAME,
                               RAW_SHORT_NAME, RAW_LONG_NAME, DSWE_NOT_WATER,
                               DSWE_PARTIAL_SURFACE_WATER_PIXEL,
                               band_dswe_raw)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Failed adding Raw DSWE band product", MODULE_NAME);

        /* Cleanup memory */
        free (xml_filename);
        free (dem_filename);

        return EXIT_FAILURE;
    }
#endif

    /* CLEANUP & EXIT ----------------------------------------------------- */

    /* Cleanup all the input band memory */
    free_band_memory(band_red, band_nir, band_l2qa);
    band_red = NULL;
    band_nir = NULL;
    band_l2qa = NULL;

    /* Free remaining allocated memory */
    free(xml_filename);

    LOG_MESSAGE("Processing complete.", MODULE_NAME);

    return EXIT_SUCCESS;
}
