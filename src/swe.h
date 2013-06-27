#ifndef _SWE_H_
#define _SWE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bool.h"
#include "mystring.h"
#include "error_handler.h"
#include "input.h"
#include "output.h"
#include "space.h"

/* Prototypes */
void usage();

short get_args
(
    int argc,             /* I: number of cmd-line args */
    char *argv[],         /* I: string of cmd-line args */
    char **reflectance_infile, /* O:address of the input TOA or Surface 
                                    Reflectance filename */
    char **dem_infile,     /* O: address of input DEM filename */
    float *mgt,            /* O: MNDWI threshold */
    float *mlt1,           /* O: mlt1 threshold */
    float *mlt2,           /* O: mlt2 threshold */
    int16 *b4lt1,          /* O: b4lt1 threshold */
    int16 *b4lt2,          /* O: b4lt2 threshold */
    int16 *b5lt1,          /* O: b5lt1 threshold */
    int16 *b5lt2,          /* O: b5lt2 threshold */
    float *per_slope,      /* O: percent slope threshold */
    bool *write_binary,    /* O: write raw binary flag */
    bool *use_ledaps_mask, /* O: use LEDAPS cloud/shadow mask result flag */
    bool *use_zeven_thorne,/* O: use Zevenbergen&Thorne's slope algorithm flag */
    bool *verbose          /* O: verbose flag */
);

float horn_slope
(
    int16 *elev_window,   /* I: 3x3 array of elevation values in meters */
    float ew_res,         /* I: east/west resolution of the elevation data in
                                meters */
    float ns_res          /* I: north/south resolution of the elevation data in
                                meters */
);

int calc_slope
(
    int16 *dem,           /* I: array of DEM values in meters (nlines+[1or2] x
                                nsamps values - see NOTES);  if processing
                                at the top of the image, then an extra line
                                before will not be available;  if processing
                                at the bottom of the image, then an extra line
                                at the end will not be available */
    bool dem_top,         /* I: are we at the top of the dem and therefore no
                                extra lines at the start of the dem? */
    bool dem_bottom,      /* I: are we at the bottom of the dem and therefore no
                                extra lines at the end of the dem? */
    bool use_zeven_thorne,/* I: should we use Zevenbergen&Thorne's slope 
                                algorithm? */
    int nlines,           /* I: number of lines of data to be processed in the
                                mask array; dem array will have one or two lines
                                more depending on top, middle, bottom */
    int nsamps,           /* I: number of samples of data to be processed in the
                                mask array; dem array will have the same number
                                of samples therefore the first and last sample
                                will not be processed as part of the mask since
                                a 3x3 window won't be available */
    float ew_res,         /* I: east/west resolution of the elevation data in
                                meters */
    float ns_res,         /* I: north/south resolution of the elevation data in
                                meters */
    float *percent_slope  /* O: array of percent slope values (multiplied
                                by 100 to indicate percent intensity)
                                of size nlines * nsamps */
);

int write_envi_hdr
(
    char *hdr_file,     /* I: name of header file to be generated */
    Input_t *toa_input, /* I: input structure for both the TOA reflectance
                              and brightness temperature products */
    Space_def_t *space_def /* I: spatial definition information */
);

void split_filename 
(
    const char *filename,       /* I: Name of file to split */
    char *directory,            /* O: Directory portion of file name */
    char *root,                 /* O: Root portion of the file name */
    char *extension             /* O: Extension portion of the file name */
);

void surface_water_extent
(
    int16 *b2,     /* I: array of unscaled band 2 reflectance values */
    int16 *b3,     /* I: array of unscaled band 3 reflectance values */
    int16 *b4,     /* I: array of unscaled band 4 reflectance values */
    int16 *b5,     /* I: array of unscaled band 5 reflectance values */
    int nlines,    /* I: number of lines in the data arrays */
    int nsamps,    /* I: number of samples in the data arrays */
    float refl_scale_fact,  /* I: scale factor for the reflectance values */  
    float mgt,              /* I: mgt threshold */
    float mlt1,             /* I: mlt1 threshold */
    float mlt2,             /* I: mlt2 threshold */
    int16 b4lt1,            /* I: b4lt1 threshold */
    int16 b4lt2,            /* I: b4lt2 threshold */
    int16 b5lt1,            /* I: b5lt1 threshold */
    int16 b5lt2,            /* I: b5lt2 threshold */
    int16 *raw_swe     /* O: array of surface water extent values (non-zero
                                values represent water) */
);

#endif
