
#include <stdlib.h>
#include <stdbool.h>
#include "espa_metadata.h"
#include "parse_metadata.h"
#include "write_metadata.h"
#include "raw_binary_io.h"


#include "const.h"
#include "cfmask_water_detection.h"
#include "utilities.h"
#include "get_args.h"
#include "input.h"


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
    uint8_t *band_class_qa
)
{
    free (band_red);
    band_red = NULL;
    free (band_nir);
    band_nir = NULL;
    free (band_class_qa);
    band_class_qa = NULL;
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
    uint8_t **band_class_qa,
    int pixel_count
)
{
    *band_red = calloc(pixel_count, sizeof(int16_t));
    if (*band_red == NULL)
    {
        ERROR_MESSAGE("Failed allocating memory for RED band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory(*band_red, *band_nir, *band_class_qa);
        return ERROR;
    }

    *band_nir = calloc(pixel_count, sizeof(int16_t));
    if (*band_nir == NULL)
    {
        ERROR_MESSAGE("Failed allocating memory for NIR band", MODULE_NAME);

        /* Free allocated memory */
        free_band_memory(*band_red, *band_nir, *band_class_qa);
        return ERROR;
    }

    *band_class_qa = calloc(pixel_count, sizeof(uint8_t));
    if (*band_class_qa == NULL)
    {
        ERROR_MESSAGE("Failed allocating memory for CFMASK band",
                      MODULE_NAME);

        /* Free allocated memory */
        free_band_memory(*band_red, *band_nir, *band_class_qa);
        return ERROR;
    }

    return SUCCESS;
}


/******************************************************************************
  NAME:  write_u8bit_data

  PURPOSE:  Create the *.img file and the associated ENVI header.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
******************************************************************************/
int
write_u8bit_data
(
    char *output_filename,
    int element_count,
    uint8_t *data
)
{
    FILE *fd = NULL;
    char msg[512];

    fd = fopen(output_filename, "w");
    if (fd == NULL)
    {
        snprintf(msg, sizeof(msg), "Failed creating file %s",
                 output_filename);
        RETURN_ERROR(msg, MODULE_NAME, ERROR);
    }

    if (write_raw_binary(fd, 1, element_count, sizeof(uint8_t),
                         data) != SUCCESS)
    {
        snprintf(msg, sizeof(msg), "Failed writing file %s", output_filename);
        RETURN_ERROR(msg, MODULE_NAME, ERROR);
    }

    fclose(fd);

    return SUCCESS;
}


/*****************************************************************************
  NAME:  main

  PURPOSE:  Implements the core algorithm for cfmask based water detection.

  ALGORITHM DEVELOPERS:

      The algorithm implemented here was developed by the following:

      This software is based on the Matlab code developed by Zhe Zhu,
      and Curtis E. Woodcock

      Zhu, Z. and Woodcock, C. E., Object-based cloud and cloud shadow
      detection in Landsat imagery, Remote Sensing of Environment (2012),
      doi:10.1016/j.rse.2011.10.028

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
    uint8_t *band_class_qa = NULL; /* Class QA Band */

    float ndvi;

    int16_t red_fill_value;
    int16_t nir_fill_value;
    uint8_t class_qa_fill_value;

    /* Other variables */
    int pixel_index;
    int pixel_count;
    char temp_filename[PATH_MAX];


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
        /* Cleanup memory */
        free(xml_filename);

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
        free(xml_filename);

        return EXIT_FAILURE;
    }

    /* -------------------------------------------------------------------- */
    /* Figure out the number of elements in the data */
    pixel_count = input_data->lines * input_data->samples;
    if (verbose_flag)
    {
        printf ("Pixel Count = %d\n", pixel_count);
    }

    /* Allocate memory buffers for input and temp processing */
    if (allocate_band_memory(&band_red, &band_nir, &band_class_qa,
                             pixel_count) != SUCCESS)
    {
        ERROR_MESSAGE ("Failed reading bands into memory", MODULE_NAME);

        /* Cleanup memory */
        free_metadata(&xml_metadata);
        free(input_data);
        free(xml_filename);

        return EXIT_FAILURE;
    }

    /* -------------------------------------------------------------------- */
    /* Read the input files into the buffers */
    if (read_bands_into_memory(input_data, band_red, band_nir,
                               band_class_qa, pixel_count) != SUCCESS)
    {
        ERROR_MESSAGE("Failed reading bands into memory", MODULE_NAME);

        /* Cleanup memory */
        free_metadata(&xml_metadata);
        free(input_data);
        free_band_memory(band_red, band_nir, band_class_qa);
        free(xml_filename);

        return EXIT_FAILURE;
    }

    red_fill_value = input_data->fill_value[I_BAND_RED];
    nir_fill_value = input_data->fill_value[I_BAND_NIR];
    class_qa_fill_value = input_data->fill_value[I_BAND_CLASS_QA];

    int total_image_pixels = 0;
    int total_clear_pixels = 0;
    int total_water_pixels = 0;
    /* -------------------------------------------------------------------- */
    /* Process through each data element and populate the dswe band memory */
    for (pixel_index = 0; pixel_index < pixel_count; pixel_index++)
    {
        /* If any of the input is fill, make the output fill */
        if (band_red[pixel_index] == red_fill_value ||
            band_nir[pixel_index] == nir_fill_value ||
            band_class_qa[pixel_index] == class_qa_fill_value)
        {
            band_class_qa[pixel_index] = class_qa_fill_value;
            continue;
        }

        /* Get the total image data pixels */
        total_image_pixels++;

        /* Only need to process clear pixels */
        if (band_class_qa[pixel_index] != CLASS_QA_CLEAR_PIXEL)
        {
            continue;
        }

        /* Get the total clear image data pixels */
        total_clear_pixels++;

        if ((band_red[pixel_index] + band_nir[pixel_index]) != 0)
        {
            ndvi = (float)(band_nir[pixel_index] - band_red[pixel_index])
                   / (float)(band_nir[pixel_index] + band_red[pixel_index]);
        }
        else
            ndvi = 0.01;

        /* Zhe's water test (works over thin cloud),
           equation 5 from (CFmask) */
        if ((ndvi < 0.01 && band_nir[pixel_index] < 1100)
            || (ndvi < 0.1 && ndvi > 0.0 && band_nir[pixel_index] < 500))
        {
            band_class_qa[pixel_index] = CLASS_QA_WATER_PIXEL;

            /* Update the counts */
            total_clear_pixels--;
            total_water_pixels++;
        }

        /* Let the use know where we are in the processing */
        if (pixel_index%99999 == 0)
        {
            printf("\rProcessed data element %d", pixel_index);
        }
    }
    /* Status output cleanup to match the final output size */
    printf("\rProcessed data element %d\n", pixel_index);

    float percent_clear = 100.0 * (float)total_clear_pixels
                                  / (float)total_image_pixels;
    float percent_water = 100.0 * (float)total_water_pixels
                                  / (float)total_image_pixels;
    if (verbose_flag)
    {
        printf ("Total Image Pixels = %d\n", total_image_pixels);
        printf ("Total Clear Pixels = %d\n", total_clear_pixels);
        printf ("Total Water Pixels = %d\n", total_water_pixels);
        printf ("Percent Clear Pixels = %f\n", percent_clear);
        printf ("Percent Water Pixels = %f\n", percent_water);
    }

    /* Update percentages in the XML File for clear and water */
    int cover_index = 0;
    for (cover_index = 0;
         cover_index <
             xml_metadata.band[input_data->meta_index[I_BAND_CLASS_QA]].ncover;
         cover_index++)
    {
        if (strcmp(xml_metadata.band[input_data->meta_index[I_BAND_CLASS_QA]]
                   .percent_cover[cover_index].description, "clear") == 0)
        {
            xml_metadata.band[input_data->meta_index[I_BAND_CLASS_QA]]
                .percent_cover[cover_index].percent = percent_clear;
            continue;
        }
        if (strcmp(xml_metadata.band[input_data->meta_index[I_BAND_CLASS_QA]]
                   .percent_cover[cover_index].description, "water") == 0)
        {
            xml_metadata.band[input_data->meta_index[I_BAND_CLASS_QA]]
                .percent_cover[cover_index].percent = percent_water;
            continue;
        }
    }

    /* Write the updated metadata to the XML file */
    if (write_metadata(&xml_metadata, xml_filename) != SUCCESS)
    {
        ERROR_MESSAGE("Writing XML file", MODULE_NAME);

        /* Cleanup memory */
        free_metadata(&xml_metadata);
        free(input_data);
        free_band_memory(band_red, band_nir, band_class_qa);
        free(xml_filename);

        return EXIT_FAILURE;
    }

    /* Write the L2 QA Band data to temp file on disk */
    snprintf(temp_filename, sizeof(temp_filename), "temp_%s",
             input_data->band_name[I_BAND_CLASS_QA]);

    if (write_u8bit_data(temp_filename, pixel_count, band_class_qa) != SUCCESS)
    {
        ERROR_MESSAGE("Failed writing L2 QA band data", MODULE_NAME);

        /* Cleanup memory */
        free_metadata(&xml_metadata);
        free(input_data);
        free_band_memory(band_red, band_nir, band_class_qa);
        free(xml_filename);

        return EXIT_FAILURE;
    }

    /* Rename the file over-writing the L2 QA Band filename */
    if (rename(temp_filename, input_data->band_name[I_BAND_CLASS_QA]) == -1)
    {
        ERROR_MESSAGE("Failed over-writing L2 QA band data with new results",
                      MODULE_NAME);

        /* Cleanup memory */
        free_metadata(&xml_metadata);
        free(input_data);
        free_band_memory(band_red, band_nir, band_class_qa);
        free(xml_filename);

        return EXIT_FAILURE;
    }

    /* CLEANUP & EXIT ----------------------------------------------------- */

    /* Free the metadata structure */
    free_metadata(&xml_metadata);

    /* Close the input files */
    if (close_input(input_data) != SUCCESS)
    {
        WARNING_MESSAGE("Failed closing input files", MODULE_NAME);
    }

    /* Cleanup the input data structure */
    free(input_data);
    input_data = NULL;

    /* Cleanup all the input band memory */
    free_band_memory(band_red, band_nir, band_class_qa);

    /* Free remaining allocated memory */
    free(xml_filename);

    LOG_MESSAGE("Processing complete.", MODULE_NAME);

    return EXIT_SUCCESS;
}
