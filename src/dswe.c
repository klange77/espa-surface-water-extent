
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

#include "dswe_const.h"
#include "utilities.h"
#include "get_args.h"
#include "dswe_input.h"


/*****************************************************************************
*****************************************************************************/
int main (int argc, char *argv[])
{
    int status;
    char *xml_filename = NULL;          /* filename for the XML input */
    char *dem_filename = NULL;          /* filename for the DEM input */
    Espa_internal_meta_t xml_metadata;  /* XML metadata structure */
    bool use_ledaps_mask_flag;
    bool use_zeven_thorne_flag;
    bool use_toa_flag;
    float wigt;
    float awgt;
    float pswt;
    float percent_slope;
    int pswb4t;
    int pswb5t;
    bool verbose_flag;
    int line_buffer_size;
    Input_Data_t *input_data = NULL;

    /* Get the command line arguments */
    status = get_args (argc, argv,
        &xml_filename,
        &dem_filename,
        &use_ledaps_mask_flag,
        &use_zeven_thorne_flag,
        &use_toa_flag,
        &wigt,
        &awgt,
        &pswt,
        &percent_slope,
        &pswb4t,
        &pswb5t,
        &verbose_flag,
        &line_buffer_size);
    if (status != SUCCESS)
    {
        /* get_args generates all the error messages we need */
        return EXIT_FAILURE;
    }

    LOG_MESSAGE("Starting dynamic surface water extent processing ...",
        MODULE_NAME);

    /* Provide user information if verbose is turned on */
    if (verbose_flag)
    {
        printf ("  XML Input File: %s\n", xml_filename);
        printf ("  DEM Input File: %s\n", dem_filename);
        printf ("            WIGT: %0.3f\n", wigt);
        printf ("            AWGT: %0.3f\n", awgt);
        printf ("            PSWT: %0.3f\n", pswt);
        printf ("          PSWB4T: %d\n", pswb4t);
        printf ("          PSWB5T: %d\n", pswb5t);
        printf ("   Percent Slope: %0.1f\n", percent_slope);
        printf (" Use LEDAPS Mask:");
        if (use_ledaps_mask_flag)
            printf (" true\n");
        else
            printf (" false\n");
        printf ("Use Zeven Thorne:");
        if (use_zeven_thorne_flag)
            printf (" true\n");
        else
            printf (" false\n");
        printf ("Use Top Of Atmos:");
        if (use_toa_flag)
            printf (" true\n");
        else
            printf (" false\n");
        printf ("Line Buffer Size: %d\n", line_buffer_size);
    }

    /* Validate the input XML metadata file */
    if (validate_xml_file (xml_filename, ESPA_SCHEMA) != SUCCESS)
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

    /* Open the input files */
    input_data = open_input (&xml_metadata, use_toa_flag, dem_filename);
    if (input_data == NULL)
    {
        ERROR_MESSAGE("Failed opening input files", MODULE_NAME);

        free_metadata (&xml_metadata);
        return EXIT_FAILURE;
    }

    /* TODO TODO TODO - Do the processing here........ */
    /* TODO TODO TODO - Do the processing here........ */
    // allocate memory buffers for input and temp processing
    // open the output files for writing
    // allocate memory buffers for output
    // perform surface water extent *** This is big ***
    // Update the XML file with the new bands
    /* TODO TODO TODO - Do the processing here........ */
    /* TODO TODO TODO - Do the processing here........ */

    /* Close the input files */
    if (! close_input (input_data))
    {
        WARNING_MESSAGE("Failed closing input files", MODULE_NAME);
    }

    /* Free the metadata structure */
    free_metadata (&xml_metadata);

    LOG_MESSAGE("Processing complete.", MODULE_NAME);

    return EXIT_SUCCESS;
}

