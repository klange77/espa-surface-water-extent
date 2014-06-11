
#include <stdio.h>

#include "dswe_input.h"
#include "utilities.h"


/* Open the specified file and allocate the memory for the filename */
void open_band
(
    Espa_internal_meta_t *metadata, /* I: input metadata */
    Input_Data_t *input_data,       /* IO: updated with information from XML */
    int meta_band_index,            /* I: index to get the band from */
    Input_Bands_e band_index        /* I: index to place the band into */
)
{
    char msg[256];

    /* Grab the band name from the metadata */
    input_data->band_name[band_index] =
        strdup (metadata->band[meta_band_index].file_name);

    printf ("Using band file %s\n", input_data->band_name[band_index]);

    /* Open a file descriptor for the band */
    input_data->band_fd[band_index] =
        fopen (input_data->band_name[band_index], "r");

    if (input_data->band_fd[band_index] == NULL)
    {
        snprintf (msg, sizeof(msg), "Failed to open (%s)",
            input_data->band_name[band_index]);
        WARNING_MESSAGE (msg, MODULE_NAME);
    }
}


/* Open the specified file and allocate the memory for the filename */
void open_dem_band
(
    char *dem_filename,             /* I: input DEM filename */
    Input_Data_t *input_data,       /* IO: updated with information from XML */
    Input_Bands_e band_index        /* I: index to place the band into */
)
{
    char msg[256];

    /* Grab the DEM name from the input */
    input_data->band_name[band_index] = strdup (dem_filename);

    printf ("Using DEM file %s\n", input_data->band_name[band_index]);

    /* Open a file descriptor for the DEM */
    input_data->band_fd[band_index] =
        fopen (input_data->band_name[band_index], "r");

    if (input_data->band_fd[band_index] == NULL)
    {
        snprintf (msg, sizeof(msg), "Failed to open (%s)",
            input_data->band_name[band_index]);
        WARNING_MESSAGE (msg, MODULE_NAME);
    }
}


/* Find the files needed by this application in the XML file and open them */
bool GetXMLInput
(
    Espa_internal_meta_t *metadata, /* I: input metadata */
    bool use_toa_flag,              /* I: use TOA or SR data */
    Input_Data_t *input_data        /* O: updated with information from XML */
)
{
    int index;

    /* Initialize the band fields */
    for (index = 0; index < MAX_INPUT_BANDS; index++)
    {
        input_data->band_name[index] = NULL;
        input_data->band_fd[index] = NULL;
    }

    input_data->lines = 0;
    input_data->samples = 0;

    for (index = 0; index < metadata->nbands; index++)
    {
        if (use_toa_flag)
        {
            if (!strcmp (metadata->band[index].product, "toa_refl"))
            {
                if (!strcmp (metadata->band[index].name, "toa_band1"))
                {
                    open_band (metadata, input_data, index, I_BAND_1);

                    /* Always use this one for the lines and samples since
                       they will be the same for us */
                    input_data->lines = metadata->band[index].nlines;
                    input_data->samples = metadata->band[index].nsamps;
                }
                else if (!strcmp (metadata->band[index].name, "toa_band2"))
                {
                    open_band (metadata, input_data, index, I_BAND_2);
                }
                else if (!strcmp (metadata->band[index].name, "toa_band3"))
                {
                    open_band (metadata, input_data, index, I_BAND_3);
                }
                else if (!strcmp (metadata->band[index].name, "toa_band4"))
                {
                    open_band (metadata, input_data, index, I_BAND_4);
                }
                else if (!strcmp (metadata->band[index].name, "toa_band5"))
                {
                    open_band (metadata, input_data, index, I_BAND_5);
                }
            }
        }
        else
        {
            if (!strcmp (metadata->band[index].product, "sr_refl"))
            {
                if (!strcmp (metadata->band[index].name, "sr_band1"))
                {
                    open_band (metadata, input_data, index, I_BAND_1);

                    /* Always use this one for the lines and samples since
                       they will be the same for us */
                    input_data->lines = metadata->band[index].nlines;
                    input_data->samples = metadata->band[index].nsamps;
                }
                else if (!strcmp (metadata->band[index].name, "sr_band2"))
                {
                    open_band (metadata, input_data, index, I_BAND_2);
                }
                else if (!strcmp (metadata->band[index].name, "sr_band3"))
                {
                    open_band (metadata, input_data, index, I_BAND_3);
                }
                else if (!strcmp (metadata->band[index].name, "sr_band4"))
                {
                    open_band (metadata, input_data, index, I_BAND_4);
                }
                else if (!strcmp (metadata->band[index].name, "sr_band5"))
                {
                    open_band (metadata, input_data, index, I_BAND_5);
                }
            }
        }

        /* Search for the brightness temperature band */
        if (! strcmp (metadata->band[index].product, "toa_bt"))
        {
            if (! strcmp (metadata->band[index].name, "toa_band6"))
            {
                open_band (metadata, input_data, index, I_BAND_6);
            }
        }

        /* Search for the fmask band */
        if (! strcmp (metadata->band[index].product, "cfmask"))
        {
            if (! strcmp (metadata->band[index].name, "fmask"))
            {
                open_band (metadata, input_data, index, FMASK_BAND);
            }
        }
    }

    /* Verify all the bands have something (all are required for dswe) */
    /* Minus 1 is so that the DEM is skipped, it is checked later */
    for (index = 0; index < MAX_INPUT_BANDS-1; index++)
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


/* Open all the input files and allocate associated memory for the filenames
   that reside in the data structure */
Input_Data_t *open_input
(
    Espa_internal_meta_t *metadata, /* I: input metadata */
    bool use_toa_flag,              /* I: use TOA or SR data */
    char *dem_filename              /* I: input DEM filename */
)
{
    Input_Data_t *input_data = NULL;

    input_data = (Input_Data_t*) malloc (sizeof(Input_Data_t));
    if (input_data == NULL)
    {
        ERROR_MESSAGE ("Error allocating memory for input data structure",
            MODULE_NAME);
        return NULL;
    }

    if (! GetXMLInput (metadata, use_toa_flag, input_data))
    {
        /* error messages provided by GetXMLInput */
        close_input (input_data);
        return NULL;
    }

    /* DEM band is done separate because it is not part of the XML */
    open_dem_band (dem_filename, input_data, DEM_BAND);
    if (input_data->band_fd[DEM_BAND] == NULL ||
    input_data->band_name[DEM_BAND] == NULL)
    {
        ERROR_MESSAGE ("Error opening required input data", MODULE_NAME);

        close_input (input_data);
        return NULL;
    }

    return input_data;
}


/* Close all the input files and free associated memory that resides in the
   data structure */
bool close_input
(
    Input_Data_t *input_data /* I: the opened filenames and file descriptors */
)
{
    int index;
    int status;
    bool had_issue;
    char msg[256];

    had_issue = false;
    for (index = 0; index < MAX_INPUT_BANDS-1; index++)
    {
        if (input_data->band_fd[index] == NULL &&
            input_data->band_name[index] == NULL)
        {
            status = fclose (input_data->band_fd[index]);
            if (status != 0)
            {
                snprintf (msg, sizeof(msg), "Failed to close (%s)",
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

