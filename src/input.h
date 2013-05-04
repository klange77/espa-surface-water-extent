#ifndef _INPUT_H_
#define _INPUT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bool.h"
#include "mystring.h"
#include "myhdf.h"
#include "error_handler.h"
#include "date.h"

/* Conversion factor for degrees to radians */
#ifndef PI
#ifndef M_PI
#define PI (3.141592653589793238)
#else
#define PI (M_PI)
#endif
#endif

#define TWO_PI (2.0 * PI)
#define HALF_PI (PI / 2.0)

#define DEG (180.0 / PI)
#define RAD (PI / 180.0)

/* There are currently a maximum of 6 reflective bands in the output surface
   reflectance product */
#define NBAND_REFL_MAX 6
#define NUM_QA_BAND 7

/* How many lines of TOA reflectance and brightness temperature data should be
   processed at one time */
#define PROC_NLINES 100

/* How many lines of DEM data should be processed at one time, multiple of 3
   since the shade relief input needs to be a factor of 3. */
#define DEM_PROC_NLINES 300

/* Structure for bounding geographic coords */
typedef struct {
  double min_lon;  /* Geodetic longitude coordinate (degrees) */ 
  double min_lat;  /* Geodetic latitude coordinate (degrees) */ 
  double max_lon;  /* Geodetic longitude coordinate (degrees) */ 
  double max_lat;  /* Geodetic latitude coordinate (degrees) */ 
  bool is_fill;    /* Flag to indicate whether the point is a fill value; */
} Geo_bounds_t;

/* Structure for lat/long coordinates */
typedef struct {
  double lon;           /* Geodetic longitude coordinate (degrees) */ 
  double lat;           /* Geodetic latitude coordinate (degrees) */ 
  bool is_fill;         /* Flag to indicate whether the point is a fill value;
                           'true' = fill; 'false' = not fill */
} Geo_coord_t;

/* Structure for the global metadata */
typedef struct {
    char provider[STR_SIZE]; /* data provider type */
    char sat[STR_SIZE];      /* satellite */
    char inst[STR_SIZE];     /* instrument */
    Date_t acq_date;         /* acqsition date/time (scene center) */
    Date_t prod_date;        /* production date */
    float solar_elev;        /* solar elevation angle (radians; scene center) */
    float solar_az;          /* solar azimuth angle (radians; scene center) */
    char wrs_sys[STR_SIZE];  /* WRS system */
    int path;                /* WRS path number */
    int row;                 /* WRS row number */
    float pixsize;           /* pixel size */
    Geo_coord_t ul_corner;   /* UL lat/long corner coord */
    Geo_coord_t lr_corner;   /* LR lat/long corner coord */
    Geo_bounds_t bounds;     /* Geographic bounding coordinates */
    int refl_band[NBAND_REFL_MAX]; /* band numbers for TOA reflectance data */
} Input_meta_t;

/* Structure for the 'input' data type, particularly to handle the file/SDS
   IDs and the band-specific information */
typedef struct {
    Input_meta_t meta;       /* input metadata */
    char *refl_file_name;    /* input TOA reflectance file name */
    char *sr_file_name;      /* input surface reflectance file name */
    bool use_toa;            /* use TOA reflectance flag; use = true */
    bool refl_open;          /* open TOS reflectance file flag; open = true */
    bool sr_open;            /* open surface reflectance file flag; 
                                open = true */
    int nrefl_band;          /* number of input reflectance bands */
    int nqa_band;            /* number of qa bands */
    int nlines;              /* number of input lines */
    int nsamps;              /* number of input samples */
    int32 refl_sds_file_id;  /* SDS file id for TOA reflectance */
    int32 sr_sds_file_id;    /* SDS file id for Surface reflectance */
    Myhdf_sds_t refl_sds[NBAND_REFL_MAX]; /* SDS data structures for 
                                reflectance data */
    Myhdf_sds_t qa_sds[NUM_QA_BAND]; /* SDS data structures for QA bands */
    int16 *refl_buf[NBAND_REFL_MAX]; /* input data buffer for unscaled 
                                reflectance data (PROC_NLINES lines of data) */
    uint8 *qa_buf;           /* Buffer for qa cloud band 10 */
    int refl_fill;           /* fill value for TOA reflectance bands */
    float refl_scale_fact;   /* scale factor for TOA reflectance bands */
    int refl_saturate_val;   /* saturation value for TOA reflectance bands */
} Input_t;

/* Prototypes */
Input_t *open_input
(
    char *lndcal_name,     /* I: input TOA reflectance filename */
    char *lndsr_name,      /* I: input TOA reflectance filename */
    bool use_toa           /* I: flag to indicate if TOA reflectance is used */
);

void close_input
(
    Input_t *this    /* I: pointer to input data structure */
);

void free_input
(
    Input_t *this    /* I: pointer to input data structure */
);

int get_input_refl_lines
(
    Input_t *this,   /* I: pointer to input data structure */
    int iband,       /* I: current TOA band to read (0-based) */
    int iline,       /* I: current line to read (0-based) */
    int nlines       /* I: number of lines to read */
);

int get_input_qa_line
(
    Input_t *this,   /* I: pointer to input data structure */
    int iline,       /* I: current line to read (0-based) */
    int nlines       /* I: number of lines to read */
);

int get_input_meta
(
    Input_t *this    /* I: pointer to input data structure */
);

#endif
