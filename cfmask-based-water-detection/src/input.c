
#include <stdio.h>

#include "cfmask_water_detection.h"
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
    input_data->band_name[band_index] = strdup(filename);

    /* Open a file descriptor for the band */
    input_data->band_fd[band_index] =
        fopen(input_data->band_name[band_index], "rb");

    if (input_data->band_fd[band_index] == NULL)
    {
        snprintf(msg, sizeof(msg), "Failed to open (%s)",
                 input_data->band_name[band_index]);
        ERROR_MESSAGE(msg, MODULE_NAME);
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
    Input_Data_t *input_data        /* O: updated with information from XML */
)
{
    int index;
    char msg[256];

    char toa_product_name[30];
    char red_band_name[30];
    char nir_band_name[30];
    char qa_product_name[30];
    char class_qa_band_name[30];

    /* Figure out the band names and product name to use */
    if ((strcmp(metadata->global.satellite, "LANDSAT_4") == 0)
        || (strcmp(metadata->global.satellite, "LANDSAT_5") == 0)
        || (strcmp(metadata->global.satellite, "LANDSAT_7") == 0))
    {
        snprintf(red_band_name, sizeof(red_band_name), "toa_band3");
        snprintf(nir_band_name, sizeof(nir_band_name), "toa_band4");
    }
    else if (strcmp(metadata->global.satellite, "LANDSAT_8") == 0)
    {
        snprintf(red_band_name, sizeof(red_band_name), "toa_band4");
        snprintf(nir_band_name, sizeof(nir_band_name), "toa_band5");
    }
    else
    {
        RETURN_ERROR("Error invalid satellite", MODULE_NAME, ERROR);
    }

    snprintf(toa_product_name, sizeof(toa_product_name), "toa_refl");

    /* Class QA Band information */
    snprintf(qa_product_name, sizeof(qa_product_name), "class_based_qa");
    snprintf(class_qa_band_name, sizeof(class_qa_band_name), "class_based_qa");

    /* Scan the metadata searching for the bands to open */
    for (index = 0; index < metadata->nbands; index++)
    {
        /* Only look at the ones with the product name we are looking for */
        if (strcmp(metadata->band[index].product, toa_product_name) == 0)
        {
            if (strcmp(metadata->band[index].name, red_band_name) == 0)
            {
                open_band(metadata->band[index].file_name, input_data,
                          I_BAND_RED);

                if (metadata->band[index].data_type != ESPA_INT16)
                {
                    snprintf(msg, sizeof(msg),
                             "%s incompatable data type expecting INT16",
                             red_band_name);
                    RETURN_ERROR(msg, MODULE_NAME, ERROR);
                }

                /* Always use this one for the lines and samples since
                   they will be the same for us, along with the pixel
                   size values */
                input_data->lines = metadata->band[index].nlines;
                input_data->samples = metadata->band[index].nsamps;

                /* Grab the fill value for this band */
                input_data->fill_value[I_BAND_RED] =
                    metadata->band[index].fill_value;

                /* Grab the metadata index value for this band */
                input_data->meta_index[I_BAND_RED] = index;
            }
            else if (strcmp(metadata->band[index].name, nir_band_name) == 0)
            {
                open_band(metadata->band[index].file_name, input_data,
                          I_BAND_NIR);

                if (metadata->band[index].data_type != ESPA_INT16)
                {
                    snprintf(msg, sizeof(msg),
                             "%s incompatable data type expecting INT16",
                             nir_band_name);
                    RETURN_ERROR(msg, MODULE_NAME, ERROR);
                }

                /* Grab the fill value for this band */
                input_data->fill_value[I_BAND_NIR] =
                    metadata->band[index].fill_value;

                /* Grab the metadata index value for this band */
                input_data->meta_index[I_BAND_NIR] = index;
            }
        }

        if (strcmp(metadata->band[index].product, qa_product_name) == 0)
        {
            if (strcmp(metadata->band[index].name, class_qa_band_name) == 0)
            {
                open_band(metadata->band[index].file_name, input_data,
                          I_BAND_CLASS_QA);
                if (metadata->band[index].data_type != ESPA_UINT8)
                {
                    snprintf(msg, sizeof(msg),
                             "%s incompatable data type expecting UINT8",
                             class_qa_band_name);
                    RETURN_ERROR(msg, MODULE_NAME, ERROR);
                }

                /* Grab the fill value for this band */
                input_data->fill_value[I_BAND_CLASS_QA] =
                    metadata->band[index].fill_value;

                /* Grab the metadata index value for this band */
                input_data->meta_index[I_BAND_CLASS_QA] = index;
            }
        }
    }

    /* Verify all the bands have something (all are required) */
    for (index = 0; index < MAX_INPUT_BANDS; index++)
    {
        if (input_data->band_fd[index] == NULL ||
            input_data->band_name[index] == NULL)
        {
            ERROR_MESSAGE("Error opening required input data", MODULE_NAME);

            close_input(input_data);
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
    Espa_internal_meta_t *metadata /* I: input metadata */
)
{
    int index;
    Input_Data_t *input_data = NULL;

    input_data = (Input_Data_t *)malloc(sizeof(Input_Data_t));
    if (input_data == NULL)
    {
        ERROR_MESSAGE("Error allocating memory for input data structure",
                      MODULE_NAME);
        return NULL;
    }

    /* Initialize the band fields */
    for (index = 0; index < MAX_INPUT_BANDS; index++)
    {
        input_data->band_name[index] = NULL;
        input_data->band_fd[index] = NULL;
        input_data->fill_value[index] = -1;
        input_data->meta_index[index] = -1;
    }

    input_data->lines = 0;
    input_data->samples = 0;

    /* Open the input images from the XML file */
    if (GetXMLInput(metadata, input_data) != SUCCESS)
    {
        /* error messages provided by GetXMLInput */
        close_input(input_data);
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
        if (input_data->band_fd[index] != NULL)
        {
            status = fclose(input_data->band_fd[index]);
            if (status != 0)
            {
                snprintf(msg, sizeof (msg),
                         "Failed to close (%s)",
                         input_data->band_name[index]);
                WARNING_MESSAGE(msg, MODULE_NAME);

                had_issue = true;
            }
            input_data->band_fd[index] = NULL;

            free(input_data->band_name[index]);
            input_data->band_name[index] = NULL;
        }
        else
        {
            free(input_data->band_name[index]);
            input_data->band_name[index] = NULL;
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
    int16_t *band_red,
    int16_t *band_nir,
    uint8_t *band_class_qa,
    int pixel_count
)
{
    int count;

    count = fread(band_red, sizeof(int16_t), pixel_count,
                  input_data->band_fd[I_BAND_RED]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE("Failed reading red band data", MODULE_NAME);

        return ERROR;
    }

    count = fread(band_nir, sizeof(int16_t), pixel_count,
                  input_data->band_fd[I_BAND_NIR]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE("Failed reading nir band data", MODULE_NAME);

        return ERROR;
    }

    count = fread(band_class_qa, sizeof(uint8_t), pixel_count,
                  input_data->band_fd[I_BAND_CLASS_QA]);
    if (count != pixel_count)
    {
        ERROR_MESSAGE("Failed reading Class QA band data", MODULE_NAME);

        return ERROR;
    }

    return SUCCESS;
}

