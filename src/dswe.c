
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
bool
read_bands_into_memory
(
    Input_Data_t *input_data,
    int16_t *band_blue,
    int16_t *band_green,
    int16_t *band_red,
    int16_t *band_nir,
    int16_t *band_swir1,
    char *band_bt,
    char *band_fmask,
    int element_count
)
{
    int count;

    count = fread (band_blue, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_BLUE]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading blue band data", MODULE_NAME);

        return false;
    }

    count = fread (band_green, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_GREEN]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading green band data", MODULE_NAME);

        return false;
    }

    count = fread (band_red, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_RED]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading red band data", MODULE_NAME);

        return false;
    }

    count = fread (band_nir, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_NIR]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading nir band data", MODULE_NAME);

        return false;
    }

    count = fread (band_swir1, sizeof (int16_t), element_count,
                   input_data->band_fd[I_BAND_SWIR1]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading swir1 band data", MODULE_NAME);

        return false;
    }

    count = fread (band_bt, sizeof (char), element_count,
                   input_data->band_fd[I_BAND_SWIR2]);
    if (count != element_count)
    {
        ERROR_MESSAGE ("Failed reading bt band data", MODULE_NAME);

        return false;
    }

    return true;
}


/*****************************************************************************
  NAME:  free_allocated_input_memory

  PURPOSE:  Free the memory allocated by allocate_input_memory.

  RETURN VALUE:  None
*****************************************************************************/
void
free_allocated_input_memory
(
    int16_t *band_blue,
    int16_t *band_green,
    int16_t *band_red,
    int16_t *band_nir,
    int16_t *band_swir1,
    char *band_bt
)
{
    free (band_blue);
    free (band_green);
    free (band_red);
    free (band_nir);
    free (band_swir1);
    free (band_bt);
}


/*****************************************************************************
  NAME:  allocate_input_memory

  PURPOSE:  Allocate memory for all the input bands.

  RETURN VALUE:  Type = bool
      Value    Description
      -------  ---------------------------------------------------------------
      true     Success with allocating all of the memory needed for the input
               bands.
      false    Failed to allocate memory for an input band.
*****************************************************************************/
bool
allocate_input_memory
(
    int16_t **band_blue,
    int16_t **band_green,
    int16_t **band_red,
    int16_t **band_nir,
    int16_t **band_swir1,
    char **band_bt,
    int element_count
)
{
    *band_blue = (int16_t *) malloc (element_count * sizeof (int16_t));
    if (*band_blue == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for BLUE band", MODULE_NAME);

        /* No free because we have not allocated any memory yet */
        return false;
    }

    *band_green = (int16_t *) malloc (element_count * sizeof (int16_t));
    if (*band_green == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for GREEN band", MODULE_NAME);

        /* Free allocated memory */
        free_allocated_input_memory (*band_blue, *band_green, *band_red,
                                     *band_nir, *band_swir1, *band_bt);
        return false;
    }

    *band_red = (int16_t *) malloc (element_count * sizeof (int16_t));
    if (*band_red == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for RED band", MODULE_NAME);

        /* Free allocated memory */
        free_allocated_input_memory (*band_blue, *band_green, *band_red,
                                     *band_nir, *band_swir1, *band_bt);
        return false;
    }

    *band_nir = (int16_t *) malloc (element_count * sizeof (int16_t));
    if (*band_nir == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for NIR band", MODULE_NAME);

        /* Free allocated memory */
        free_allocated_input_memory (*band_blue, *band_green, *band_red,
                                     *band_nir, *band_swir1, *band_bt);
        return false;
    }

    *band_swir1 = (int16_t *) malloc (element_count * sizeof (int16_t));
    if (*band_swir1 == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for SWIR1 band", MODULE_NAME);

        /* Free allocated memory */
        free_allocated_input_memory (*band_blue, *band_green, *band_red,
                                     *band_nir, *band_swir1, *band_bt);
        return false;
    }

    *band_bt = (char *) malloc (element_count * sizeof (char));
    if (*band_bt == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for brightness temp band",
                       MODULE_NAME);

        /* Free allocated memory */
        free_allocated_input_memory (*band_blue, *band_green, *band_red,
                                     *band_nir, *band_swir1, *band_bt);
        return false;
    }

    return true;
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
    Espa_internal_meta_t xml_metadata;  /* XML metadata structure */
    bool use_zeven_thorne_flag;
    bool use_toa_flag;
    float wigt;
    float awgt;
    float pswt_1;
    float pswt_2;
    float percent_slope;
    int pswnt_1;
    int pswnt_2;
    int pswst_1;
    int pswst_2;
    bool verbose_flag;

    /* Band data */
    Input_Data_t *input_data = NULL;
    int16_t *band_blue = NULL;  /* TM SR_Band1,  OLI SR_Band2 */
    int16_t *band_green = NULL; /* TM SR_Band2,  OLI SR_Band3 */
    int16_t *band_red = NULL;   /* TM SR_Band3,  OLI SR_Band4 */
    int16_t *band_nir = NULL;   /* TM SR_Band4,  OLI SR_Band5 */
    int16_t *band_swir1 = NULL; /* TM SR_Band5,  OLI SR_Band6 */
    char *band_bt = NULL;       /* TM TOA_Band6, OLI XXXXXXXX */
    char *band_fmask = NULL;    /* FMASK */

    /* Temp variables */
    float mndwi;                /* (blue - swir1) / (blue + swir1) */
    float mbsrv;                /* (blue + red) */
    float mbsrn;                /* (nir + swir1) */
    float awesh;                /* (blue
                                   + (2.5 * green)
                                   - (1.5 * MBSRN)
                                   - (0.25 * bt)) */
    int16_t *band_dswe = NULL;  /* Output DSWE band data */

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

    float blue_fill_value;
    float green_fill_value;
    float red_fill_value;
    float nir_fill_value;
    float swir1_fill_value;
    float swir2_fill_value;

    int16_t band_dswe_value;

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
            printf (" true\n");
        else
            printf (" false\n");
        printf (" Use Top Of Atmos:");
        if (use_toa_flag)
            printf (" true\n");
        else
            printf (" false\n");
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
    element_count = input_data->lines * input_data->samples;

    /* Allocate memory buffers for input and temp processing */
    if (! allocate_input_memory (&band_blue, &band_green, &band_red,
                                 &band_nir, &band_swir1, &band_bt,
                                 element_count))
    {
        ERROR_MESSAGE ("Failed reading bands into memory", MODULE_NAME);

        return EXIT_FAILURE;
    }

    /* Use calloc on dswe to set the data to zero */
    band_dswe = (int16_t *) calloc (element_count, sizeof (int16_t));
    if (band_dswe == NULL)
    {
        ERROR_MESSAGE ("Failed allocating memory for DSWE band", MODULE_NAME);

        /* Cleanup memory */
        free_allocated_input_memory (band_blue, band_green, band_red,
                                     band_nir, band_swir1, band_bt);
        return false;
    }


    /* -------------------------------------------------------------------- */
    /* Read the input files into the buffers */
    if (! read_bands_into_memory (input_data, band_blue, band_green,
                                  band_red, band_nir, band_swir1, band_bt,
                                  band_fmask, element_count))
    {
        ERROR_MESSAGE ("Failed reading bands into memory", MODULE_NAME);

        /* Cleanup memory */
        free_allocated_input_memory (band_blue, band_green, band_red,
                                     band_nir, band_swir1, band_bt);
        free (band_dswe);
        free (xml_filename);
        free (input_data);

        return EXIT_FAILURE;
    }

    /* -------------------------------------------------------------------- */
    /* Close the input files */
    if (!close_input (input_data))
    {
        WARNING_MESSAGE ("Failed closing input files", MODULE_NAME);
    }

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
            band_bt[index] == swir2_fill_value)
        {
            band_dswe[index] = DSWE_NO_DATA_VALUE;
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
        band_swir2_float = band_bt[index];

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
        if (mndwi < wigt)
            band_dswe_value = 0;
        else
            band_dswe_value = 1; /* >= wigt */  /* Set the ones digit */

        if (mbsrv > mbsrn)
            band_dswe_value += 10; /* Set the tens digit */

        if (awesh > awgt)
            band_dswe_value += 100; /* Set the hundreds digit */

        /* Partial Surface Water 1 (PSW1)
           The logic in the if results in a true/false called PSW1 */
        if (mndwi > pswt_1 &&
            band_swir1_float < pswst_1_float &&
            band_nir_float < pswnt_1_float)
        {
            band_dswe_value += 1000; /* Set the thousands digit */
        }

        /* Partial Surface Water 2 (PSW2)
           The logic in the if results in a true/false called PSW2 */
        if (mndwi > pswt_2 &&
            band_swir1_float < pswst_2_float &&
            band_nir_float < pswnt_2_float)
        {
            band_dswe_value += 10000; /* Set the ten thousands digit */
        }

        band_dswe[index] = band_dswe_value;

        if (index%99999 == 0)
        {
            printf ("\r");
            printf ("Processed data element %d", index);
        }
    }
    printf ("\r");
    printf ("Processed data element %d", index);
    printf ("\n");

    /* Cleanup all the input band memory */
    free_allocated_input_memory (band_blue, band_green, band_red, band_nir,
                                 band_swir1, band_bt);
    band_blue = NULL;
    band_green = NULL;
    band_red = NULL;
    band_nir = NULL;
    band_swir1 = NULL;
    band_bt = NULL;

    /* Add the DSWE band to the metadata file and generate the ENVI image and
       header files */
    if (! add_dswe_band_product(xml_filename, band_dswe))
    {
        ERROR_MESSAGE ("Failed adding DSWE band product", MODULE_NAME);

        /* Cleanup memory */
        free (band_dswe);
        free (xml_filename);
        free (input_data);

        return EXIT_FAILURE;
    }

    /* CLEANUP & EXIT ----------------------------------------------------- */

    /* Free the DSWE band memory */
    free (band_dswe);

    /* Free remaining allocated memory */
    free (xml_filename);
    free (input_data);

    LOG_MESSAGE ("Processing complete.", MODULE_NAME);

    return EXIT_SUCCESS;
}
