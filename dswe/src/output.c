
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
#include "dswe.h"
#include "utilities.h"


#define MAX_DATE_LEN 28


/*****************************************************************************
  NAME:  write_u8bit_dswe_product

  PURPOSE:  Create the *.img file and the associated ENVI header.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
*****************************************************************************/
int
write_u8bit_dswe_product
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


/*****************************************************************************
  NAME:  write_16bit_dswe_product

  PURPOSE:  Create the *.img file and the associated ENVI header.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
*****************************************************************************/
int
write_16bit_dswe_product
(
    char *output_filename,
    int element_count,
    int16_t *data
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

    if (write_raw_binary(fd, 1, element_count, sizeof (int16_t), data)
        != SUCCESS)
    {
        snprintf (msg, sizeof (msg), "Failed writing file %s",
                  output_filename);
        RETURN_ERROR (msg, MODULE_NAME, ERROR);
    }

    fclose(fd);

    return SUCCESS;
}


/*****************************************************************************
  NAME:  write_float_dswe_product

  PURPOSE:  Create the *.img file and the associated ENVI header.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
*****************************************************************************/
int
write_float_dswe_product
(
    char *output_filename,
    int element_count,
    float *data
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

    if (write_raw_binary(fd, 1, element_count, sizeof (float), data)
        != SUCCESS)
    {
        snprintf (msg, sizeof (msg), "Failed writing file %s",
                  output_filename);
        RETURN_ERROR (msg, MODULE_NAME, ERROR);
    }

    fclose(fd);

    return SUCCESS;
}


/*****************************************************************************
  NAME:  add_dswe_band_product

  PURPOSE:  Create a new envi output file including envi header and add the
            associated information to the XML metadata file.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
*****************************************************************************/
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
    int add_class,
    int add_bitmap,
    uint8_t *data
)
{
    int count;
    int band_index = -1;
    int src_index = -1;
    int element_count;
    char scene_name[PATH_MAX];
    char image_filename[PATH_MAX];
    char *my_char = NULL;
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
    int bit_count;            /* number of bits in the bitmap */
    char search_string[PATH_MAX];

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
    snprintf (search_string, sizeof(search_string), "_%s",
              in_meta.band[src_index].name);
    my_char = strstr(scene_name, search_string);
    if (my_char != NULL)
        *my_char = '\0';

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

    if (strftime (production_date, MAX_DATE_LEN, "%Y-%m-%dT%H:%M:%SZ", tm)
        == 0)
    {
        RETURN_ERROR ("formatting the production date/time", MODULE_NAME,
                      ERROR);
    }

    /* Figure out the output filename */
    count = snprintf (image_filename, sizeof (image_filename),
                      "%s_%s.img", scene_name, band_name);
    if (count < 0 || count >= sizeof (image_filename))
    {
        RETURN_ERROR ("Failed creating output filename", MODULE_NAME, ERROR);
    }

    /* Figure out how many elements are in the data */
    element_count = in_meta.band[src_index].nlines *
                    in_meta.band[src_index].nsamps;

    /* First write out the ENVI band and header files */
    if (write_u8bit_dswe_product (image_filename, element_count, data)
        != SUCCESS)
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
              "%s", in_meta.band[src_index].short_name);
    bmeta[0].short_name[4] = '\0';
    strcat (bmeta[0].short_name, short_name);
    snprintf (bmeta[0].product, sizeof (bmeta[0].product),
              "%s", product_name);
    if (use_toa_flag)
    {
        snprintf (bmeta[0].source, sizeof (bmeta[0].source), "toa_refl");
    }
    else
    {
        snprintf (bmeta[0].source, sizeof (bmeta[0].source), "sr_refl");
    }
    if (!strcmp (band_name, HS_BAND_NAME)) 
    {
        snprintf (bmeta[0].category, sizeof (bmeta[0].category), "image");
    }
    else
    {
        snprintf (bmeta[0].category, sizeof (bmeta[0].category), "qa");
    }
    bmeta[0].nlines = in_meta.band[src_index].nlines;
    bmeta[0].nsamps = in_meta.band[src_index].nsamps;
    bmeta[0].pixel_size[0] = in_meta.band[src_index].pixel_size[0];
    bmeta[0].pixel_size[1] = in_meta.band[src_index].pixel_size[1];
    snprintf (bmeta[0].pixel_units, sizeof (bmeta[0].pixel_units), "meters");
    snprintf (bmeta[0].app_version, sizeof (bmeta[0].app_version),
              "dswe_%s", DSWE_VERSION);
    snprintf (bmeta[0].production_date, sizeof (bmeta[0].production_date),
              "%s", production_date);
    bmeta[0].data_type = ESPA_UINT8;
    bmeta[0].fill_value = DSWE_NO_DATA_VALUE;
    bmeta[0].valid_range[0] = min_range;
    bmeta[0].valid_range[1] = max_range;
    snprintf (bmeta[0].name, sizeof (bmeta[0].name),
              "%s", band_name);
    snprintf (bmeta[0].long_name, sizeof (bmeta[0].long_name),
              "%s", long_name);
    snprintf (bmeta[0].data_units, sizeof (bmeta[0].data_units),
              "quality/feature classification");
    snprintf (bmeta[0].file_name, sizeof (bmeta[0].file_name),
              "%s", image_filename);

    if (add_class)
    {
        /* Figure out how many classes we have */
        if (max_range == DSWE_CLOUD_CLOUD_SHADOW_SNOW)
        {
            class_count = 7;
        }
        else
        {
            class_count = 6;
        }

        /* Set up class values information */
        if (allocate_class_metadata (&bmeta[0], class_count) != SUCCESS)
            RETURN_ERROR ("allocating dswe classes", MODULE_NAME, ERROR);

        bmeta[0].class_values[0].class = DSWE_NOT_WATER;
        snprintf (bmeta[0].class_values[0].description,
                  sizeof (bmeta[0].class_values[0].description),
                  "not water");

        bmeta[0].class_values[1].class = DSWE_WATER_HIGH_CONFIDENCE;
        snprintf (bmeta[0].class_values[1].description,
                  sizeof (bmeta[0].class_values[1].description),
                  "water - high confidence");

        bmeta[0].class_values[2].class = DSWE_WATER_MODERATE_CONFIDENCE;
        snprintf (bmeta[0].class_values[2].description,
                  sizeof (bmeta[0].class_values[2].description),
                  "water - moderate confidence");

        bmeta[0].class_values[3].class = DSWE_POTENTIAL_WETLAND;
        snprintf (bmeta[0].class_values[3].description,
                  sizeof (bmeta[0].class_values[3].description),
                  "potential wetland");

        bmeta[0].class_values[4].class = DSWE_LOW_CONFIDENCE_WATER_OR_WETLAND;
        snprintf (bmeta[0].class_values[4].description,
                  sizeof (bmeta[0].class_values[4].description),
                  "water or wetland - low confidence");

        /* The pshsccss band has the classes the interpreted band has, plus 
           this */
        if (class_count == 7)
        {
            bmeta[0].class_values[5].class = DSWE_CLOUD_CLOUD_SHADOW_SNOW;
            snprintf (bmeta[0].class_values[5].description,
                      sizeof (bmeta[0].class_values[5].description),
                      "cloud, cloud shadow, and snow");
        }

        bmeta[0].class_values[class_count-1].class = DSWE_NO_DATA_VALUE;
        snprintf (bmeta[0].class_values[class_count-1].description,
                  sizeof (bmeta[0].class_values[class_count-1].description),
                  "fill");
    }

    /* This is for the mask band */
    if (add_bitmap)
    {
        bit_count = 5;

        /* Set up bit values information */
        if (allocate_bitmap_metadata (&bmeta[0], bit_count) != SUCCESS)
            RETURN_ERROR ("allocating dswe mask bitmap", MODULE_NAME, ERROR);

        snprintf (bmeta[0].bitmap_description[0], STR_SIZE, "shadow");
        snprintf (bmeta[0].bitmap_description[1], STR_SIZE, "snow");
        snprintf (bmeta[0].bitmap_description[2], STR_SIZE, "cloud");
        snprintf (bmeta[0].bitmap_description[3], STR_SIZE, "percent slope");
        snprintf (bmeta[0].bitmap_description[4], STR_SIZE, "hillshade");
    }

    /* Create the ENVI header file this band */
    if (create_envi_struct (&bmeta[0], &in_meta.global, &envi_hdr) != SUCCESS)
    {
        RETURN_ERROR ("Failed to create ENVI header structure.", MODULE_NAME,
                      ERROR);
    }

    /* Write the ENVI header */
    snprintf (envi_file, sizeof(envi_file), "%s", bmeta[0].file_name);
    my_char = strchr (envi_file, '.');
    if (my_char == NULL)
    {
        RETURN_ERROR ("Failed creating ENVI header filename", MODULE_NAME,
                      ERROR);
    }

    sprintf (my_char, ".hdr");
    if (write_envi_hdr (envi_file, &envi_hdr) != SUCCESS)
    {
        RETURN_ERROR ("Failed writing ENVI header file", MODULE_NAME, ERROR);
    }

    /* Append the DSWE band to the XML file */
    if (append_metadata (1, bmeta, xml_filename)
        != SUCCESS)
    {
        RETURN_ERROR ("Appending DSWE band to XML file", MODULE_NAME, ERROR);
    }

    free_metadata (&in_meta);
    free_metadata (&tmp_meta);

    return SUCCESS;
}


/*****************************************************************************
  NAME:  add_test_band_product

  PURPOSE:  Create a new envi output file including envi header and add the
            associated information to the XML metadata file.

  NOTE: Only for the "test" DSWE band output.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
*****************************************************************************/
int
add_test_band_product
(
    char *xml_filename,
    bool use_toa_flag,
    char *product_name,
    char *band_name,
    char *short_name,
    char *long_name,
    int min_range,
    int max_range,
    int16_t *data
)
{
    int count;
    int band_index = -1;
    int src_index = -1;
    int element_count;
    char scene_name[PATH_MAX];
    char image_filename[PATH_MAX];
    char *my_char = NULL;
    Espa_internal_meta_t in_meta;
    Espa_internal_meta_t tmp_meta;
    Espa_band_meta_t *bmeta = NULL; /* pointer to the band metadata array
                                       within the output structure */
    time_t tp;                   /* time structure */
    struct tm *tm = NULL;        /* time structure for UTC time */
    char production_date[MAX_DATE_LEN+1]; /* current date/time for production */
    Envi_header_t envi_hdr;   /* output ENVI header information */
    char envi_file[PATH_MAX];
    char search_string[PATH_MAX];

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
    snprintf (scene_name, sizeof(scene_name), "%s",
              in_meta.band[src_index].file_name);
    snprintf (search_string, sizeof(search_string), "_%s",
              in_meta.band[src_index].name);
    my_char = strstr(scene_name, search_string);
    if (my_char != NULL)
        *my_char = '\0';

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

    if (strftime (production_date, MAX_DATE_LEN, "%Y-%m-%dT%H:%M:%SZ", tm)
        == 0)
    {
        RETURN_ERROR ("formatting the production date/time", MODULE_NAME,
                      ERROR);
    }

    /* Figure out the output filename */
    count = snprintf (image_filename, sizeof (image_filename),
                      "%s_%s.img", scene_name, band_name);
    if (count < 0 || count >= sizeof (image_filename))
    {
        RETURN_ERROR ("Failed creating output filename", MODULE_NAME, ERROR);
    }

    /* Figure out how many elements are in the data */
    element_count = in_meta.band[src_index].nlines *
                    in_meta.band[src_index].nsamps;

    /* First write out the ENVI band and header files */
    if (write_16bit_dswe_product (image_filename, element_count, data)
        != SUCCESS)
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
              "%s", in_meta.band[src_index].short_name);
    bmeta[0].short_name[4] = '\0';
    strcat (bmeta[0].short_name, short_name);
    snprintf (bmeta[0].product, sizeof (bmeta[0].product),
              "%s", product_name);
    if (use_toa_flag)
    {
        snprintf (bmeta[0].source, sizeof (bmeta[0].source), "toa_refl");
    }
    else
    {
        snprintf (bmeta[0].source, sizeof (bmeta[0].source), "sr_refl");
    }
    snprintf (bmeta[0].category, sizeof (bmeta[0].category), "qa");
    bmeta[0].nlines = in_meta.band[src_index].nlines;
    bmeta[0].nsamps = in_meta.band[src_index].nsamps;
    bmeta[0].pixel_size[0] = in_meta.band[src_index].pixel_size[0];
    bmeta[0].pixel_size[1] = in_meta.band[src_index].pixel_size[1];
    snprintf (bmeta[0].pixel_units, sizeof (bmeta[0].pixel_units), "meters");
    snprintf (bmeta[0].app_version, sizeof (bmeta[0].app_version),
              "dswe_%s", DSWE_VERSION);
    snprintf (bmeta[0].production_date, sizeof (bmeta[0].production_date),
              "%s", production_date);
    bmeta[0].data_type = ESPA_INT16;
    bmeta[0].fill_value = TESTS_NO_DATA_VALUE;
    bmeta[0].valid_range[0] = min_range;
    bmeta[0].valid_range[1] = max_range;
    snprintf (bmeta[0].name, sizeof (bmeta[0].name),
              "%s", band_name);
    snprintf (bmeta[0].long_name, sizeof (bmeta[0].long_name),
              "%s", long_name);
    snprintf (bmeta[0].data_units, sizeof (bmeta[0].data_units),
              "quality/feature classification");
    snprintf (bmeta[0].file_name, sizeof (bmeta[0].file_name),
              "%s", image_filename);

    /* Create the ENVI header file this band */
    if (create_envi_struct (&bmeta[0], &in_meta.global, &envi_hdr) != SUCCESS)
    {
        RETURN_ERROR ("Failed to create ENVI header structure.", MODULE_NAME,
                      ERROR);
    }

    /* Write the ENVI header */
    snprintf (envi_file, sizeof(envi_file), "%s", bmeta[0].file_name);
    my_char = strchr (envi_file, '.');
    if (my_char == NULL)
    {
        RETURN_ERROR ("Failed creating ENVI header filename", MODULE_NAME,
                      ERROR);
    }

    sprintf (my_char, ".hdr");
    if (write_envi_hdr (envi_file, &envi_hdr) != SUCCESS)
    {
        RETURN_ERROR ("Failed writing ENVI header file", MODULE_NAME, ERROR);
    }

    /* Append the DSWE test band to the XML file */
    if (append_metadata (1, bmeta, xml_filename)
        != SUCCESS)
    {
        RETURN_ERROR ("Appending DSWE test band to XML file", MODULE_NAME, 
                      ERROR);
    }

    free_metadata (&in_meta);
    free_metadata (&tmp_meta);

    return SUCCESS;
}


/*****************************************************************************
  NAME:  add_ps_band_product

  PURPOSE:  Create a new envi output file including envi header and add the
            associated information to the XML metadata file.

  NOTE: Only for the Percent-Slope DSWE band output.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
*****************************************************************************/
int
add_ps_band_product
(
    char *xml_filename,
    bool use_toa_flag,
    char *product_name,
    char *band_name,
    char *short_name,
    char *long_name,
    int min_range,
    float max_range,
    float *data
)
{
    int count;
    int band_index = -1;
    int src_index = -1;
    int element_count;
    char scene_name[PATH_MAX];
    char image_filename[PATH_MAX];
    char *my_char = NULL;
    Espa_internal_meta_t in_meta;
    Espa_internal_meta_t tmp_meta;
    Espa_band_meta_t *bmeta = NULL; /* pointer to the band metadata array
                                       within the output structure */
    time_t tp;                   /* time structure */
    struct tm *tm = NULL;        /* time structure for UTC time */
    char production_date[MAX_DATE_LEN+1]; /* current date/time for production */
    Envi_header_t envi_hdr;   /* output ENVI header information */
    char envi_file[PATH_MAX];
    char search_string[PATH_MAX];

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
    snprintf (search_string, sizeof(search_string), "_%s",
              in_meta.band[src_index].name);
    my_char = strstr(scene_name, search_string);
    if (my_char != NULL)
        *my_char = '\0';

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

    if (strftime (production_date, MAX_DATE_LEN, "%Y-%m-%dT%H:%M:%SZ", tm)
        == 0)
    {
        RETURN_ERROR ("formatting the production date/time", MODULE_NAME,
                      ERROR);
    }

    /* Figure out the output filename */
    count = snprintf (image_filename, sizeof (image_filename),
                      "%s_%s.img", scene_name, band_name);
    if (count < 0 || count >= sizeof (image_filename))
    {
        RETURN_ERROR ("Failed creating output filename", MODULE_NAME, ERROR);
    }

    /* Figure out how many elements are in the data */
    element_count = in_meta.band[src_index].nlines *
                    in_meta.band[src_index].nsamps;

    /* First write out the ENVI band and header files */
    if (write_float_dswe_product (image_filename, element_count, data)
        != SUCCESS)
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
              "%s", in_meta.band[src_index].short_name);
    bmeta[0].short_name[4] = '\0';
    strcat (bmeta[0].short_name, short_name);
    snprintf (bmeta[0].product, sizeof (bmeta[0].product),
              "%s", product_name);
    if (use_toa_flag)
    {
        snprintf (bmeta[0].source, sizeof (bmeta[0].source), "toa_refl");
    }
    else
    {
        snprintf (bmeta[0].source, sizeof (bmeta[0].source), "sr_refl");
    }
    snprintf (bmeta[0].category, sizeof (bmeta[0].category), "image");
    bmeta[0].nlines = in_meta.band[src_index].nlines;
    bmeta[0].nsamps = in_meta.band[src_index].nsamps;
    bmeta[0].pixel_size[0] = in_meta.band[src_index].pixel_size[0];
    bmeta[0].pixel_size[1] = in_meta.band[src_index].pixel_size[1];
    snprintf (bmeta[0].pixel_units, sizeof (bmeta[0].pixel_units), "meters");
    snprintf (bmeta[0].app_version, sizeof (bmeta[0].app_version),
              "dswe_%s", DSWE_VERSION);
    snprintf (bmeta[0].production_date, sizeof (bmeta[0].production_date),
              "%s", production_date);
    bmeta[0].data_type = ESPA_FLOAT32;
    bmeta[0].fill_value = TESTS_NO_DATA_VALUE;
    bmeta[0].valid_range[0] = min_range;
    bmeta[0].valid_range[1] = max_range;
    snprintf (bmeta[0].name, sizeof (bmeta[0].name),
              "%s", band_name);
    snprintf (bmeta[0].long_name, sizeof (bmeta[0].long_name),
              "%s", long_name);
    snprintf (bmeta[0].data_units, sizeof (bmeta[0].data_units),
              "quality/feature classification");
    snprintf (bmeta[0].file_name, sizeof (bmeta[0].file_name),
              "%s", image_filename);

    /* Create the ENVI header file this band */
    if (create_envi_struct (&bmeta[0], &in_meta.global, &envi_hdr) != SUCCESS)
    {
        RETURN_ERROR ("Failed to create ENVI header structure.", MODULE_NAME,
                      ERROR);
    }

    /* Write the ENVI header */
    snprintf (envi_file, sizeof(envi_file), "%s", bmeta[0].file_name);
    my_char = strchr (envi_file, '.');
    if (my_char == NULL)
    {
        RETURN_ERROR ("Failed creating ENVI header filename", MODULE_NAME,
                      ERROR);
    }

    sprintf (my_char, ".hdr");
    if (write_envi_hdr (envi_file, &envi_hdr) != SUCCESS)
    {
        RETURN_ERROR ("Failed writing ENVI header file", MODULE_NAME, ERROR);
    }

    /* Append the percent slope DSWE band to the XML file */
    if (append_metadata (1, bmeta, xml_filename)
        != SUCCESS)
    {
        RETURN_ERROR ("Appending percent slope DSWE band to XML file",
                       MODULE_NAME, ERROR);
    }

    free_metadata (&in_meta);
    free_metadata (&tmp_meta);

    return SUCCESS;
}
