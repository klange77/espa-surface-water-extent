
#ifndef INPUT_H
#define INPUT_H

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
    bool use_toa_flag,              /* I: use TOA or SR data */
    char *dem_filename              /* I: the name of the DEM file */
);


int
close_input
(
    Input_Data_t *input_data /* I: updated with information from XML */
);

#endif /* INPUT_H */
