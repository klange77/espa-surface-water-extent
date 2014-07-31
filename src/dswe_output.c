
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#include "espa_metadata.h"
#include "parse_metadata.h"
#include "write_metadata.h"
#include "envi_header.h"
#include "raw_binary_io.h"

#include "dswe_const.h"
#include "utilities.h"


/******************************************************************************
MODULE:  write_espa_product

PURPOSE:  Create the *.img file and the associated ENVI header.

******************************************************************************/
bool
write_dswe_product (char *output_filename, int element_count, int16_t *data)
{
    FILE *fd = NULL;
    char msg[512];

    fd = fopen(output_filename, "w");
    if (fd == NULL)
    {
        snprintf (msg, sizeof (msg), "Failed creating file %s",
                  output_filename);
        ERROR_MESSAGE (msg, MODULE_NAME);

        return false;
    }

    if (write_raw_binary(fd, 1, element_count, sizeof (int16_t), data)
        != SUCCESS)
    {
        snprintf (msg, sizeof (msg), "Failed writing file %s",
                  output_filename);
        ERROR_MESSAGE (msg, MODULE_NAME);

        return false;
    }

    fclose(fd);

    return true;
}


/******************************************************************************
MODULE:  add_espa_image_product

PURPOSE:  Create a new envi output file including envi header and add the
          associated information to the XML metadata file.

******************************************************************************/
bool
add_dswe_band_product (char *xml_filename,
                       int16_t *data)
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

    /* Initialize the input metadata structure */
    init_metadata_struct (&in_meta);

    /* Parse the metadata file into our internal metadata structure; also
       allocates space as needed for various pointers in the global and band
       metadata */
    if (parse_metadata (xml_filename, &in_meta) != SUCCESS)
    {
        /* Error messages already written */
        return false;
    }

    /* Find the representative band for metadata information */
    for (band_index = 0; band_index < in_meta.nbands; band_index++)
    {
        if (!strcmp (in_meta.band[band_index].name, "sr_band1") &&
            !strcmp (in_meta.band[band_index].product, "sr_refl"))
        {
            /* this is the index we'll use for reflectance band info */
            src_index = band_index;
            break;
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
        ERROR_MESSAGE ("unable to obtain current time", MODULE_NAME);

        return false;
    }

    tm = gmtime (&tp);
    if (tm == NULL)
    {
        ERROR_MESSAGE ("converting time to UTC", MODULE_NAME);

        return false;
    }

    if (strftime (production_date, MAX_DATE_LEN, "%Y-%m-%dT%H:%M:%SZ", tm) == 0)
    {
        ERROR_MESSAGE ("formatting the production date/time", MODULE_NAME);

        return false;
    }

    /* Figure out the output filename */
    count = snprintf (image_filename, sizeof (image_filename),
                      "%s_%s.img", scene_name, "sr_dswe");
    if (count < 0 || count >= sizeof (image_filename))
    {
        ERROR_MESSAGE ("Failed creating output filename", MODULE_NAME);

        return false;
    }

    /* Figure out how many elements are in the data */
    element_count = in_meta.band[src_index].nlines *
                    in_meta.band[src_index].nsamps;

    /* First write out the ENVI band and header files */
    if (! write_dswe_product (image_filename, element_count, data))
    {
        ERROR_MESSAGE ("Failed creating output ENVI files", MODULE_NAME);

        return false;
    }

    /* Gather all the band information from the representative band */

    /* Initialize the internal metadata for the output product. The global
       metadata won't be updated, however the band metadata will be updated
       and used later for appending to the original XML file. */
    init_metadata_struct (&tmp_meta);

    /* Allocate memory for the output band */
    if (allocate_band_metadata (&tmp_meta, 1) != SUCCESS)
        RETURN_ERROR("allocating band metadata", "OpenOutput", NULL);
    bmeta = tmp_meta.band;

    snprintf (bmeta[0].short_name, sizeof (bmeta[0].short_name),
              in_meta.band[src_index].short_name);
    bmeta[0].short_name[3] = '\0';
    strcat (bmeta[0].short_name, SHORT_NAME);
    snprintf (bmeta[0].product, sizeof (bmeta[0].product), PRODUCT_NAME);
    snprintf (bmeta[0].source, sizeof (bmeta[0].source), "sr_refl");
    snprintf (bmeta[0].category, sizeof (bmeta[0].category), "image");
        /* TODO TODO TODO - maybe qa not image ???? */
    bmeta[0].nlines = in_meta.band[src_index].nlines;
    bmeta[0].nsamps = in_meta.band[src_index].nsamps;
    bmeta[0].pixel_size[0] = in_meta.band[src_index].pixel_size[0];
    bmeta[0].pixel_size[1] = in_meta.band[src_index].pixel_size[1];
    snprintf (bmeta[0].pixel_units, sizeof (bmeta[0].pixel_units), "meters");
    snprintf (bmeta[0].app_version, sizeof (bmeta[0].app_version),
              "dswe_%s", DSWE_VERSION);
    snprintf (bmeta[0].production_date, sizeof (bmeta[0].production_date),
              production_date);
    bmeta[0].data_type = ESPA_INT16;
    bmeta[0].fill_value = NO_DATA_VALUE;
    bmeta[0].valid_range[0] = 0;
    bmeta[0].valid_range[1] = 1111; /* TODO TODO TODO - Change to final */
    snprintf (bmeta[0].name, sizeof (bmeta[0].name), BAND_NAME);
    snprintf (bmeta[0].long_name, sizeof (bmeta[0].long_name), LONG_NAME);
    snprintf (bmeta[0].data_units, sizeof (bmeta[0].data_units),
              "quality/feature classification");
    snprintf (bmeta[0].file_name, sizeof (bmeta[0].file_name), image_filename);

    /* TODO TODO TODO -
       See cfmask if I need to allocate class value information */

    /* Create the ENVI header file this band */
    if (create_envi_struct (&bmeta[0], &in_meta.global, &envi_hdr) != SUCCESS)
    {
        ERROR_MESSAGE ("Failed to create ENVI header structure.", MODULE_NAME);

        return false;
    }

    /* Write the ENVI header */
    snprintf (envi_file, sizeof(envi_file), bmeta[0].file_name);
    tmp_char = strchr (envi_file, '.');
    if (tmp_char == NULL)
    {
        ERROR_MESSAGE ("Failed creating ENVI header filename", MODULE_NAME);

        return false;
    }

    sprintf (tmp_char, ".hdr");
    if (write_envi_hdr (envi_file, &envi_hdr) != SUCCESS)
    {
        ERROR_MESSAGE ("Failed writing ENVI header file", MODULE_NAME);

        return false;
    }

    /* Append the DSWE band to the XML file */
    if (append_metadata (1, bmeta, xml_filename)
        != SUCCESS)
    {
        ERROR_MESSAGE ("Appending spectral index bands to XML file",
                       MODULE_NAME);
    }

    free_metadata (&in_meta);
    free_metadata (&tmp_meta);

    return true;
}
