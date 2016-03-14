
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
    double x_pixel_size;
    double y_pixel_size;
    char *band_name[MAX_INPUT_BANDS];    /* Name of the input image files */
    FILE *band_fd[MAX_INPUT_BANDS];      /* Open fd's for the image */
    float scale_factor[MAX_INPUT_BANDS]; /* Scale factors from the metadata */
    int fill_value[MAX_INPUT_BANDS];     /* Fill value from the metadata */
} Input_Data_t;


Input_Data_t *
open_input
(
    Espa_internal_meta_t *metadata, /* I: input metadata */
    bool use_toa_flag               /* I: use TOA or SR data */
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
    int16_t *band_blue,       /* I: pointer to allocated memory */
    int16_t *band_green,      /* I: pointer to allocated memory */
    int16_t *band_red,        /* I: pointer to allocated memory */
    int16_t *band_nir,        /* I: pointer to allocated memory */
    int16_t *band_swir1,      /* I: pointer to allocated memory */
    int16_t *band_swir2,      /* I: pointer to allocated memory */
    int16_t *band_elevation,  /* I: pointer to allocated memory */
    uint8_t *band_cfmask,     /* I: pointer to allocated memory */
    int pixel_count           /* I: how many pixel are to be read in */
);


#endif /* INPUT_H */
