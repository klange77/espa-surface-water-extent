
#ifndef INPUT_H
#define INPUT_H


#include <stdint.h>

#include "espa_metadata.h"

#include "const.h"


/* Structure for the 'input' data */
typedef struct
{
    int lines;
    int samples;
    char *band_name[MAX_INPUT_BANDS]; /* Name of the input image files */
    FILE *band_fd[MAX_INPUT_BANDS];   /* Open fd's for the image */
    int fill_value[MAX_INPUT_BANDS];  /* Fill value from the metadata */
    int meta_index[MAX_INPUT_BANDS];  /* Index in the band metadata */
} Input_Data_t;


Input_Data_t *
open_input
(
    Espa_internal_meta_t *metadata /* I: input metadata */
);


int
close_input
(
    Input_Data_t *input_data /* I: updated with information from XML */
);


int
read_bands_into_memory
(
    Input_Data_t *input_data, /* I: input data record */
    int16_t *band_red,        /* I: pointer to allocated memory */
    int16_t *band_nir,        /* I: pointer to allocated memory */
    uint16_t *band_pixel_qa,  /* I: pointer to allocated memory */
    int pixel_count           /* I: how many pixel are to be read in */
);


#endif /* INPUT_H */
