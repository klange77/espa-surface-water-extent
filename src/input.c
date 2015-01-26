
#include <stdio.h>

#include "input.h"
#include "utilities.h"


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

  RETURN VALUE:  Type = bool
      Value    Description
      -------  ---------------------------------------------------------------
      false    Missing or incompatible bands.
      true     No errors encountered.
*****************************************************************************/
bool
GetXMLInput
(
    Espa_internal_meta_t *metadata, /* I: input metadata */
    bool use_toa_flag,              /* I: use TOA or SR data */
    char *dem_filename,             /* I: the name of the DEM file */
    Input_Data_t *input_data        /* O: updated with information from XML */
)
{
    int index;

    for (index = 0; index < metadata->nbands; index++)
    {
        if (use_toa_flag)
        {
            if (!strcmp (metadata->band[index].product, "toa_refl"))
            {
                if (!strcmp (metadata->band[index].name, "toa_band1"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_BLUE);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("toa_band1 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Always use this one for the lines and samples since
                       they will be the same for us */
                    input_data->lines = metadata->band[index].nlines;
                    input_data->samples = metadata->band[index].nsamps;

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_BLUE] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_BLUE] =
                        metadata->band[index].fill_value;
                }
                else if (!strcmp (metadata->band[index].name, "toa_band2"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_GREEN);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("toa_band2 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_GREEN] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_GREEN] =
                        metadata->band[index].fill_value;
                }
                else if (!strcmp (metadata->band[index].name, "toa_band3"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_RED);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("toa_band3 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_RED] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_RED] =
                        metadata->band[index].fill_value;
                }
                else if (!strcmp (metadata->band[index].name, "toa_band4"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_NIR);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("toa_band4 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_NIR] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_NIR] =
                        metadata->band[index].fill_value;
                }
                else if (!strcmp (metadata->band[index].name, "toa_band5"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_SWIR1);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("toa_band5 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_SWIR1] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_SWIR1] =
                        metadata->band[index].fill_value;
                }
                else if (!strcmp (metadata->band[index].name, "toa_band7"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_SWIR2);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("toa_band7 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_SWIR2] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_SWIR2] =
                        metadata->band[index].fill_value;
                }
            }
        }
        else
        {
            if (!strcmp (metadata->band[index].product, "sr_refl"))
            {
                if (!strcmp (metadata->band[index].name, "sr_band1"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_BLUE);

                    /* Always use this one for the lines and samples since
                       they will be the same for us */
                    input_data->lines = metadata->band[index].nlines;
                    input_data->samples = metadata->band[index].nsamps;

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("sr_band1 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_BLUE] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_BLUE] =
                        metadata->band[index].fill_value;
                }
                else if (!strcmp (metadata->band[index].name, "sr_band2"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_GREEN);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("sr_band2 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_GREEN] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_GREEN] =
                        metadata->band[index].fill_value;
                }
                else if (!strcmp (metadata->band[index].name, "sr_band3"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_RED);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("sr_band3 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_RED] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_RED] =
                        metadata->band[index].fill_value;
                }
                else if (!strcmp (metadata->band[index].name, "sr_band4"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_NIR);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("sr_band4 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_NIR] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_NIR] =
                        metadata->band[index].fill_value;
                }
                else if (!strcmp (metadata->band[index].name, "sr_band5"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_SWIR1);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("sr_band5 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_SWIR1] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_SWIR1] =
                        metadata->band[index].fill_value;
                }
                else if (!strcmp (metadata->band[index].name, "sr_band7"))
                {
                    open_band (metadata->band[index].file_name, input_data,
                               I_BAND_SWIR2);

                    if (metadata->band[index].data_type != 2)
                    {
                        WARNING_MESSAGE("sr_band7 incompatable data type"
                                        " expecting INT16", MODULE_NAME);
                    }

                    /* Grab the scale factor for this band */
                    input_data->scale_factor[I_BAND_SWIR2] =
                        metadata->band[index].scale_factor;

                    /* Grab the fill value for this band */
                    input_data->fill_value[I_BAND_SWIR2] =
                        metadata->band[index].fill_value;
                }
            }
        }

        /* Search for the CFMASK band */
        if (!strcmp (metadata->band[index].product, "cfmask"))
        {
            if (!strcmp (metadata->band[index].name, "cfmask"))
            {
                open_band (metadata->band[index].file_name, input_data,
                           I_BAND_CFMASK);

                if (metadata->band[index].data_type != 1)
                {
                    WARNING_MESSAGE("cfmask incompatable data type"
                                    " expecting UINT8", MODULE_NAME);
                }
            }
        }
    }

    /* Add the DEM band to the list */
    open_band (dem_filename, input_data, I_BAND_DEM);

    /* Verify all the bands have something (all are required for dswe) */
    for (index = 0; index < MAX_INPUT_BANDS; index++)
    {
        if (input_data->band_fd[index] == NULL ||
            input_data->band_name[index] == NULL)
        {
            ERROR_MESSAGE ("Error opening required input data", MODULE_NAME);

            close_input (input_data);
            return false;
        }
    }

    return true;
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
    if (!GetXMLInput (metadata, use_toa_flag, dem_filename, input_data))
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

  RETURN VALUE:  Type = bool
      Value    Description
      -------  ---------------------------------------------------------------
      false    An error was encountered.
      true     No errors encountered.
*****************************************************************************/
bool
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
        return false;

    return true;
}
