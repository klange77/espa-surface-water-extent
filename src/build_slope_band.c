
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdarg.h>
//#include <stdbool.h>
#include <stdint.h>
//#include <getopt.h>
//#include <error.h>
//#include <string.h>


//#include "error_handler.h"
//#include "espa_metadata.h"
//#include "parse_metadata.h"
//#include "write_metadata.h"
//#include "envi_header.h"
//#include "espa_geoloc.h"
//#include "raw_binary_io.h"


#include "const.h"
//#include "utilities.h"
//#include "get_args.h"
//#include "input.h"
//#include "output.h"


/*****************************************************************************
  NAME: build_slope_band

  PURPOSE: Takes a DEM band as input and create a percent slope band as output
           for further processing.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      SUCCESS  Successfully created the percent slope band.
      ERROR    Failed to create the percent slope band.
*****************************************************************************/
int build_slope_band
(
    int16_t *band_dem,
    int num_lines,
    int num_samples,
    float *band_ps
)
{
    int line;
    int sample;

    for (line = 0; line < num_lines; line++)
    {
        for (sample = 0; sample < num_samples; sample++)
        {
        }
    }

    return SUCCESS;
}
