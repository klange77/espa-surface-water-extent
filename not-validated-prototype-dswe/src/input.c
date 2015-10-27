
#include <stdio.h>

#include "dswe.h"
#include "utilities.h"
#include "input.h"


/*****************************************************************************
  NAME:  open_band

  PURPOSE:  Open the specified file and allocate the memory for the filename.

  RETURN VALUE:  None
*****************************************************************************/
void
open_band
(
    char *filename,           /* I: input filename */
    Input_Data_t *input_data, /* IO: updated with information from XML */
    Input_Bands_e band_index  /* I: index to place the band into */
)
{
    char msg[256];

    /* Grab the name from the input */
    input_data->band_name[band_index] = strdup (filename);

    /* Open a file descriptor for the band */
    input_data->band_fd[band_index] =
        fopen (input_data->band_name[band_index], "rb");

    if (input_data->band_fd[band_index] == NULL)
    {
        snprintf (msg, sizeof (msg), "Failed to open (%s)",
                  input_data->band_name[band_index]);
        ERROR_MESSAGE (msg, MODULE_NAME);
    }
}


/*****************************************************************************
  NAME:  GetXMLInput

  PURPOSE:  Find the files needed by this application in the XML file and open
            them.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
*****************************************************************************/
int
GetXMLInput
(
    Espa_internal_meta_t *metadata, /* I: input metadata */
    bool use_toa_flag,              /* I: use TOA or SR data */
    char *dem_filename,             /* I: the name of the DEM file */
    Input_Data_t *input_data        /* O: updated with information from XML */
)
{
    int index;
    char msg[256];

    char product_name[30];
    char blue_band_name[30];
    char green_band_name[30];
    char red_band_name[30];
    char nir_band_name[30];
    char swir1_band_name[30];
    char swir2_band_name[30];

    /* Figure out the band names and product name to use */
    if ((strcmp (metadata->global.satellite, "LANDSAT_4") == 0)
        || (strcmp (metadata->global.satellite, "LANDSAT_5") == 0)
        || (strcmp (metadata->global.satellite, "LANDSAT_7") == 0))
    {
        if (use_toa_flag)
        {
            snprintf (product_name, sizeof (product_name), "toa_refl");

            snprintf (blue_band_name, sizeof (blue_band_name), "toa_band1");
            snprintf (green_band_name, sizeof (green_band_name), "toa_band2");
            snprintf (red_band_name, sizeof (red_band_name), "toa_band3");
            snprintf (nir_band_name, sizeof (nir_band_name), "toa_band4");
            snprintf (swir1_band_name, sizeof (swir1_band_name), "toa_band5");
            snprintf (swir2_band_name, sizeof (swir2_band_name), "toa_band7");
        }
        else
        {
            snprintf (product_name, sizeof (product_name), "sr_refl");

            snprintf (blue_band_name, sizeof (blue_band_name), "sr_band1");
            snprintf (green_band_name, sizeof (green_band_name), "sr_band2");
            snprintf (red_band_name, sizeof (red_band_name), "sr_band3");
            snprintf (nir_band_name, sizeof (nir_band_name), "sr_band4");
            snprintf (swir1_band_name, sizeof (swir1_band_name), "sr_band5");
            snprintf (swir2_band_name, sizeof (swir2_band_name), "sr_band7");
        }
    }
    else if (strcmp (metadata->global.satellite, "LANDSAT_8") == 0)
    {
        if (use_toa_flag)
        {
            snprintf (product_name, sizeof (product_name), "toa_refl");

            snprintf (blue_band_name, sizeof (blue_band_name), "toa_band2");
            snprintf (green_band_name, sizeof (green_band_name), "toa_band3");
            snprintf (red_band_name, sizeof (red_band_name), "toa_band4");
            snprintf (nir_band_name, sizeof (nir_band_name), "toa_band5");
            snprintf (swir1_band_name, sizeof (swir1_band_name), "toa_band6");
            snprintf (swir2_band_name, sizeof (swir2_band_name), "toa_band7");
        }
        else
        {
            snprintf (product_name, sizeof (product_name), "sr_refl");

            snprintf (blue_band_name, sizeof (blue_band_name), "sr_band2");
            snprintf (green_band_name, sizeof (green_band_name), "sr_band3");
            snprintf (red_band_name, sizeof (red_band_name), "sr_band4");
            snprintf (nir_band_name, sizeof (nir_band_name), "sr_band5");
            snprintf (swir1_band_name, sizeof (swir1_band_name), "sr_band6");
            snprintf (swir2_band_name, sizeof (swir2_band_name), "sr_band7");
        }
    }
    else
    {
        RETURN_ERROR ("Error invalid satellite", MODULE_NAME, ERROR);
    }

    /* Scan the metadata searching for the bands to open */
    for (index = 0; index < metadata->nbands; index++)
    {
        /* Only look at the ones with the product name we are looking for */
        if (strcmp (metadata->band[index].product, product_name) == 0)
        {
            if (strcmp (metadata->band[index].name, blue_band_name) == 0)
            {
                open_band (metadata->band[index].file_name, input_data,
                           I_BAND_BLUE);

                if (metadata->band[index].data_type != ESPA_INT16)
                {
                    snprintf (msg, sizeof (msg),
                              "%s incompatable data type expecting INT16",
                              blue_band_name);
                    RETURN_ERROR(msg, MODULE_NAME, ERROR);
                }

                /* Always use this one for the lines and samples since
                   they will be the same for us, along with the pixel
                   size values */
                input_data->lines = metadata->band[index].nlines;
                input_data->samples = metadata->band[index].nsamps;
                input_data->x_pixel_size =
                    metadata->band[index].pixel_size[0];
                input_data->y_pixel_size =
                    metadata->band[index].pixel_size[1];

                /* Grab the scale factor for this band */
                input_data->scale_factor[I_BAND_BLUE] =
                    metadata->band[index].scale_factor;

                /* Grab the fill value for this band */
                input_data->fill_value[I_BAND_BLUE] =
                    metadata->band[index].fill_value;
            }
            else if (strcmp (metadata->band[index].name, green_band_name) == 0)
            {
                open_band (metadata->band[index].file_name, input_data,
                           I_BAND_GREEN);

                if (metadata->band[index].data_type != ESPA_INT16)
                {
                    snprintf (msg, sizeof (msg),
                              "%s incompatable data type expecting INT16",
                              green_band_name);
                    RETURN_ERROR(msg, MODULE_NAME, ERROR);
                }

                /* Grab the scale factor for this band */
                input_data->scale_factor[I_BAND_GREEN] =
                    metadata->band[index].scale_factor;

                /* Grab the fill value for this band */
                input_data->fill_value[I_BAND_GREEN] =
                    metadata->band[index].fill_value;
            }
            else if (strcmp (metadata->band[index].name, red_band_name) == 0)
            {
                open_band (metadata->band[index].file_name, input_data,
                           I_BAND_RED);

                if (metadata->band[index].data_type != ESPA_INT16)
                {
                    snprintf (msg, sizeof (msg),
                              "%s incompatable data type expecting INT16",
                              red_band_name);
                    RETURN_ERROR(msg, MODULE_NAME, ERROR);
                }

                /* Grab the scale factor for this band */
                input_data->scale_factor[I_BAND_RED] =
                    metadata->band[index].scale_factor;

                /* Grab the fill value for this band */
                input_data->fill_value[I_BAND_RED] =
                    metadata->band[index].fill_value;
            }
            else if (strcmp (metadata->band[index].name, nir_band_name) == 0)
            {
                open_band (metadata->band[index].file_name, input_data,
                           I_BAND_NIR);

                if (metadata->band[index].data_type != ESPA_INT16)
                {
                    snprintf (msg, sizeof (msg),
                              "%s incompatable data type expecting INT16",
                              nir_band_name);
                    RETURN_ERROR(msg, MODULE_NAME, ERROR);
                }

                /* Grab the scale factor for this band */
                input_data->scale_factor[I_BAND_NIR] =
                    metadata->band[index].scale_factor;

                /* Grab the fill value for this band */
                input_data->fill_value[I_BAND_NIR] =
                    metadata->band[index].fill_value;
            }
            else if (strcmp (metadata->band[index].name, swir1_band_name) == 0)
            {
                open_band (metadata->band[index].file_name, input_data,
                           I_BAND_SWIR1);

                if (metadata->band[index].data_type != ESPA_INT16)
                {
                    snprintf (msg, sizeof (msg),
                              "%s incompatable data type expecting INT16",
                              swir1_band_name);
                    RETURN_ERROR(msg, MODULE_NAME, ERROR);
                }

                /* Grab the scale factor for this band */
                input_data->scale_factor[I_BAND_SWIR1] =
                    metadata->band[index].scale_factor;

                /* Grab the fill value for this band */
                input_data->fill_value[I_BAND_SWIR1] =
                    metadata->band[index].fill_value;
            }
            else if (strcmp (metadata->band[index].name, swir2_band_name) == 0)
            {
                open_band (metadata->band[index].file_name, input_data,
                           I_BAND_SWIR2);

                if (metadata->band[index].data_type != ESPA_INT16)
                {
                    snprintf (msg, sizeof (msg),
                              "%s incompatable data type expecting INT16",
                              swir2_band_name);
                    RETURN_ERROR(msg, MODULE_NAME, ERROR);
                }

                /* Grab the scale factor for this band */
                input_data->scale_factor[I_BAND_SWIR2] =
                    metadata->band[index].scale_factor;

                /* Grab the fill value for this band */
                input_data->fill_value[I_BAND_SWIR2] =
                    metadata->band[index].fill_value;
            }
        }

        /* Search for the CFMASK band */
        if (!strcmp (metadata->band[index].product, "cfmask"))
        {
            if (!strcmp (metadata->band[index].name, "cfmask"))
            {
                open_band (metadata->band[index].file_name, input_data,
                           I_BAND_CFMASK);

                if (metadata->band[index].data_type != ESPA_UINT8)
                {
                    RETURN_ERROR("cfmask incompatable data type expecting"
                                 " UINT8", MODULE_NAME, ERROR);
                }

                /* Default to a no-op since CFMASK doesn't have a scale
                   factor */
                input_data->scale_factor[I_BAND_CFMASK] = 1.0;

                /* Grab the fill value for this band */
                input_data->fill_value[I_BAND_CFMASK] =
                    metadata->band[index].fill_value;
            }
        }
    }

    /* Add the DEM band to the list */
    open_band (dem_filename, input_data, I_BAND_DEM);
    /* Default to a no-op since DEM doesn't have a scale factor */
    input_data->scale_factor[I_BAND_DEM] = 1.0;
    /* Default so the variable is initialized
       The DEM should not have values this negative */
    input_data->fill_value[I_BAND_DEM] = -9999;

    /* Verify all the bands have something (all are required for DSWE) */
    for (index = 0; index < MAX_INPUT_BANDS; index++)
    {
        if (input_data->band_fd[index] == NULL ||
            input_data->band_name[index] == NULL)
        {
            ERROR_MESSAGE ("Error opening required input data", MODULE_NAME);

            close_input (input_data);
            return ERROR;
        }
    }

    return SUCCESS;
}


/*****************************************************************************
  NAME: open_input

  PURPOSE:  Open all the input files and allocate associated memory for the
            filenames that reside in the data structure.

  RETURN VALUE:  Type = Input_Data_t *
      Value    Description
      -------  ---------------------------------------------------------------
      NULL     An error was encountered.
      *        A pointer to the populated Input_Data_t structure.
*****************************************************************************/
Input_Data_t *
open_input
(
    Espa_internal_meta_t *metadata, /* I: input metadata */
    bool use_toa_flag,              /* I: use TOA or SR data */
    char *dem_filename              /* I: the name of the DEM file */
)
{
    int index;
    Input_Data_t *input_data = NULL;

    input_data = (Input_Data_t *) malloc (sizeof (Input_Data_t));
    if (input_data == NULL)
    {
        ERROR_MESSAGE
            ("Error allocating memory for input data structure", MODULE_NAME);
        return NULL;
    }

    /* Initialize the band fields */
    for (index = 0; index < MAX_INPUT_BANDS; index++)
    {
        input_data->band_name[index] = NULL;
        input_data->band_fd[index] = NULL;
    }

    input_data->lines = 0;
    input_data->samples = 0;

    /* Open the input images from the XML file */
    if (GetXMLInput (metadata, use_toa_flag, dem_filename, input_data)
        != SUCCESS)
    {
        /* error messages provided by GetXMLInput */
        close_input (input_data);
        return NULL;
    }

    return input_data;
}


/*****************************************************************************
  NAME:  close_input

  PURPOSE:  Close all the input files and free associated memory that resides
            in the data structure.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  No errors were encountered.
      ERROR    An error was encountered.
*****************************************************************************/
int
close_input
(
    Input_Data_t *input_data /* I: the opened filenames and file descriptors */
)
{
    int index;
    int status;
    bool had_issue;
    char msg[256];

    had_issue = false;
    for (index = 0; index < MAX_INPUT_BANDS; index++)
    {
        if (input_data->band_fd[index] == NULL &&
            input_data->band_name[index] == NULL)
        {
            status = fclose (input_data->band_fd[index]);
            if (status != 0)
            {
                snprintf (msg, sizeof (msg),
                          "Failed to close (%s)",
                          input_data->band_name[index]);
                WARNING_MESSAGE (msg, MODULE_NAME);

                had_issue = true;
            }

            free (input_data->band_name[index]);
        }
        else
        {
            fclose (input_data->band_fd[index]);
            free (input_data->band_name[index]);
        }
    }

    if (had_issue)
        return ERROR;

    return SUCCESS;
}


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
    int pixel_count
)
{
    int count;

    count = fread (band_blue, sizeof (int16_t), pixel_count,
                   input_data->band_fd[I_BAND_BLUE]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE ("Failed reading blue band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_green, sizeof (int16_t), pixel_count,
                   input_data->band_fd[I_BAND_GREEN]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE ("Failed reading green band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_red, sizeof (int16_t), pixel_count,
                   input_data->band_fd[I_BAND_RED]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE ("Failed reading red band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_nir, sizeof (int16_t), pixel_count,
                   input_data->band_fd[I_BAND_NIR]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE ("Failed reading nir band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_swir1, sizeof (int16_t), pixel_count,
                   input_data->band_fd[I_BAND_SWIR1]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE ("Failed reading swir1 band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_swir2, sizeof (int16_t), pixel_count,
                   input_data->band_fd[I_BAND_SWIR2]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE ("Failed reading swir2 band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_dem, sizeof (int16_t), pixel_count,
                   input_data->band_fd[I_BAND_DEM]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE ("Failed reading DEM band data", MODULE_NAME);

        return ERROR;
    }

    count = fread (band_cfmask, sizeof (uint8_t), pixel_count,
                   input_data->band_fd[I_BAND_CFMASK]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE ("Failed reading CFMASK band data", MODULE_NAME);

        return ERROR;
    }

    return SUCCESS;
}

