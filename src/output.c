
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>

#include "espa_metadata.h"
#include "parse_metadata.h"
#include "write_metadata.h"
#include "envi_header.h"
#include "raw_binary_io.h"

#include "const.h"
#include "utilities.h"


/******************************************************************************
  NAME:  write_dswe_product

  PURPOSE:  Create the *.img file and the associated ENVI header.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
******************************************************************************/
int
write_dswe_product
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
        snprintf (msg, sizeof (msg), "Failed creating file %s",
                  output_filename);
        RETURN_ERROR (msg, MODULE_NAME, ERROR);
    }

    if (write_raw_binary(fd, 1, element_count, sizeof (uint8_t), data)
        != SUCCESS)
    {
        snprintf (msg, sizeof (msg), "Failed writing file %s",
                  output_filename);
        RETURN_ERROR (msg, MODULE_NAME, ERROR);
    }

    fclose(fd);

    return SUCCESS;
}


/******************************************************************************
  NAME:  add_dswe_band_product

  PURPOSE:  Create a new envi output file including envi header and add the
            associated information to the XML metadata file.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
******************************************************************************/
int
add_dswe_band_product
(
    char *xml_filename,
    bool use_toa_flag,
    char *product_name,
    char *band_name,
    char *short_name,
    char *long_name,
    int min_range,
    int max_range,
    uint8_t *data
)
{
    int count;
    int band_index = -1;
    int src_index = -1;
    int element_count;
    char scene_name[PATH_MAX];
    char image_filename[PATH_MAX];
    char *tmp_char = NULL;
    Espa_internal_meta_t in_meta;
    Espa_internal_meta_t tmp_meta;
    Espa_band_meta_t *bmeta = NULL; /* pointer to the band metadata array
                                       within the output structure */
    time_t tp;                   /* time structure */
    struct tm *tm = NULL;        /* time structure for UTC time */
    char production_date[MAX_DATE_LEN+1]; /* current date/time for production */
    Envi_header_t envi_hdr;   /* output ENVI header information */
    char envi_file[PATH_MAX];
    int class_count;

    /* Initialize the input metadata structure */
    init_metadata_struct (&in_meta);

    /* Parse the metadata file into our internal metadata structure; also
       allocates space as needed for various pointers in the global and band
       metadata */
    if (parse_metadata (xml_filename, &in_meta) != SUCCESS)
    {
        /* Error messages already written */
        return ERROR;
    }

    /* Find the representative band for metadata information */
    for (band_index = 0; band_index < in_meta.nbands; band_index++)
    {
        if (use_toa_flag)
        {
            if (!strcmp (in_meta.band[band_index].name, "toa_band1") &&
                !strcmp (in_meta.band[band_index].product, "toa_refl"))
            {
                /* this is the index we'll use for reflectance band info */
                src_index = band_index;
                break;
            }
        }
        else
        {
            if (!strcmp (in_meta.band[band_index].name, "sr_band1") &&
                !strcmp (in_meta.band[band_index].product, "sr_refl"))
            {
                /* this is the index we'll use for reflectance band info */
                src_index = band_index;
                break;
            }
        }
    }

    /* Figure out the scene name */
    strcpy (scene_name, in_meta.band[src_index].file_name);
    tmp_char = strchr (scene_name, '_');
    if (tmp_char != NULL)
        *tmp_char = '\0';

    /* Get the current date/time (UTC) for the production date of each band */
    if (time (&tp) == -1)
    {
        RETURN_ERROR ("unable to obtain current time", MODULE_NAME, ERROR);
    }

    tm = gmtime (&tp);
    if (tm == NULL)
    {
        RETURN_ERROR ("converting time to UTC", MODULE_NAME, ERROR);
    }

    if (strftime (production_date, MAX_DATE_LEN, "%Y-%m-%dT%H:%M:%SZ", tm) == 0)
    {
        RETURN_ERROR ("formatting the production date/time", MODULE_NAME,
                      ERROR);
    }

    /* Figure out the output filename */
    count = snprintf (image_filename, sizeof (image_filename),
                      "%s_%s.img", scene_name, product_name);
    if (count < 0 || count >= sizeof (image_filename))
    {
        RETURN_ERROR ("Failed creating output filename", MODULE_NAME, ERROR);
    }

    /* Figure out how many elements are in the data */
    element_count = in_meta.band[src_index].nlines *
                    in_meta.band[src_index].nsamps;

    /* First write out the ENVI band and header files */
    if (write_dswe_product (image_filename, element_count, data) != SUCCESS)
    {
        RETURN_ERROR ("Failed creating output ENVI files", MODULE_NAME,
                      ERROR);
    }

    /* Gather all the band information from the representative band */

    /* Initialize the internal metadata for the output product. The global
       metadata won't be updated, however the band metadata will be updated
       and used later for appending to the original XML file. */
    init_metadata_struct (&tmp_meta);

    /* Allocate memory for the output band */
    if (allocate_band_metadata (&tmp_meta, 1) != SUCCESS)
        RETURN_ERROR("allocating band metadata", MODULE_NAME, ERROR);
    bmeta = tmp_meta.band;

    snprintf (bmeta[0].short_name, sizeof (bmeta[0].short_name),
              in_meta.band[src_index].short_name);
    bmeta[0].short_name[3] = '\0';
    strcat (bmeta[0].short_name, short_name);
    snprintf (bmeta[0].product, sizeof (bmeta[0].product), product_name);
    snprintf (bmeta[0].source, sizeof (bmeta[0].source), "sr_refl");
    snprintf (bmeta[0].category, sizeof (bmeta[0].category), "qa");
    bmeta[0].nlines = in_meta.band[src_index].nlines;
    bmeta[0].nsamps = in_meta.band[src_index].nsamps;
    bmeta[0].pixel_size[0] = in_meta.band[src_index].pixel_size[0];
    bmeta[0].pixel_size[1] = in_meta.band[src_index].pixel_size[1];
    snprintf (bmeta[0].pixel_units, sizeof (bmeta[0].pixel_units), "meters");
    snprintf (bmeta[0].app_version, sizeof (bmeta[0].app_version),
              "dswe_%s", DSWE_VERSION);
    snprintf (bmeta[0].production_date, sizeof (bmeta[0].production_date),
              production_date);
    bmeta[0].data_type = ESPA_UINT8;
    bmeta[0].fill_value = DSWE_NO_DATA_VALUE;
    bmeta[0].valid_range[0] = min_range;
    bmeta[0].valid_range[1] = max_range;
    snprintf (bmeta[0].name, sizeof (bmeta[0].name), band_name);
    snprintf (bmeta[0].long_name, sizeof (bmeta[0].long_name), long_name);
    snprintf (bmeta[0].data_units, sizeof (bmeta[0].data_units),
              "quality/feature classification");
    snprintf (bmeta[0].file_name, sizeof (bmeta[0].file_name), image_filename);

    /* Figure out how many classes we have */
    if (max_range == 9)
    {
        class_count = 6;
    }
    else
    {
        class_count = 5;
    }

    /* Set up class values information */
    if (allocate_class_metadata (&bmeta[0], class_count) != SUCCESS)
        RETURN_ERROR ("allocating dswe classes", MODULE_NAME, ERROR);

    bmeta[0].class_values[0].class = 0;
    snprintf (bmeta[0].class_values[0].description,
              sizeof (bmeta[0].class_values[0].description),
              "not water");

    bmeta[0].class_values[1].class = 1;
    snprintf (bmeta[0].class_values[1].description,
              sizeof (bmeta[0].class_values[1].description),
              "water - high confidence");

    bmeta[0].class_values[2].class = 2;
    snprintf (bmeta[0].class_values[2].description,
              sizeof (bmeta[0].class_values[2].description),
              "water - moderate confidence");

    bmeta[0].class_values[3].class = 3;
    snprintf (bmeta[0].class_values[3].description,
              sizeof (bmeta[0].class_values[3].description),
              "partial surface water pixel");

    if (class_count == 6)
    {
        bmeta[0].class_values[4].class = 9;
        snprintf (bmeta[0].class_values[4].description,
                  sizeof (bmeta[0].class_values[4].description),
                  "cloud or cloud shadow");
    }

    bmeta[0].class_values[class_count-1].class = DSWE_NO_DATA_VALUE;
    snprintf (bmeta[0].class_values[class_count-1].description,
              sizeof (bmeta[0].class_values[class_count-1].description),
              "fill");

    /* Create the ENVI header file this band */
    if (create_envi_struct (&bmeta[0], &in_meta.global, &envi_hdr) != SUCCESS)
    {
        RETURN_ERROR ("Failed to create ENVI header structure.", MODULE_NAME,
                      ERROR);
    }

    /* Write the ENVI header */
    snprintf (envi_file, sizeof(envi_file), bmeta[0].file_name);
    tmp_char = strchr (envi_file, '.');
    if (tmp_char == NULL)
    {
        RETURN_ERROR ("Failed creating ENVI header filename", MODULE_NAME,
                      ERROR);
    }

    sprintf (tmp_char, ".hdr");
    if (write_envi_hdr (envi_file, &envi_hdr) != SUCCESS)
    {
        RETURN_ERROR ("Failed writing ENVI header file", MODULE_NAME, ERROR);
    }

    /* Append the DSWE band to the XML file */
    if (append_metadata (1, bmeta, xml_filename)
        != SUCCESS)
    {
        RETURN_ERROR ("Appending spectral index bands to XML file",
                       MODULE_NAME, ERROR);
    }

    free_metadata (&in_meta);
    free_metadata (&tmp_meta);

    return SUCCESS;
}
