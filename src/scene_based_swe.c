#include "swe.h"

#define NO_VALUE -9999
#define MINSIGMA 1e-5
/* Define the output SDS names to be written to the HDF-EOS file */
/* #define NUM_OUT_SDS 6 -- defined in output.h */
char *out_sds_names[NUM_OUT_SDS] = {"raw_swe", "slope_revised_swe",
    "cloud_corrected_swe", "slope_cloud_swe", "ned_elevation", "slope"};

/******************************************************************************
MODULE:  scene_based_swe

PURPOSE:  Calculate the snow water extent, for the current scene, using the TOA
reflectance, or surface reflectance, and elevation.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           An error occurred during processing of the snow cover
SUCCESS         Processing was successful

HISTORY:
Date          Programmer       Reason
----------    ---------------  -------------------------------------
04/22/2013    Song Guo         
                           
NOTES:
  1. The scene-based surface water extent is based on an algorithm developed by
     John Jones, Eastern Geographic Science Center, US Geological Survey
******************************************************************************/
int main (int argc, char *argv[])
{
    bool verbose;            /* verbose flag for printing messages */
    bool write_binary;       /* should we write raw binary output? */
    bool use_fmask;          /* should we use cfmask results? */
    bool dem_top;            /* are we at the top of the dem for shaded
                                relief processing */
    bool dem_bottom;         /* are we at the bottom of the dem for shaded
                                relief processing */
    float mgt;
    float mlt1;
    float mlt2;
    int16 b4t1;
    int16 b4t2;
    int16 b5t1;
    int16 b5t2;
    float per_slope;
    char lndcal_name[STR_SIZE];
    char lndsr_name[STR_SIZE];
    char raw_swe_bin[STR_SIZE];
    char slope_revised_swe_bin[STR_SIZE];
    char cloud_corrected_swe_bin[STR_SIZE];
    char slope_cloud_swe_bin[STR_SIZE];
    char scaled_percent_slope_bin[STR_SIZE];
    char raw_swe_hdr[STR_SIZE];
    char slope_revised_swe_hdr[STR_SIZE];
    char cloud_corrected_swe_hdr[STR_SIZE];
    char slope_cloud_swe_hdr[STR_SIZE];
    char scaled_percent_slope_hdr[STR_SIZE];
    char swe_hdf_name[STR_SIZE];
    char scene_name[STR_SIZE];
    char directory[STR_SIZE];
    char extension[STR_SIZE];
    char FUNC_NAME[] = "main"; /* function name */
    char errmsg[STR_SIZE];     /* error message */
    char *hdf_grid_name = "Grid";  /* name of the grid for HDF-EOS */
    char *reflectance_infile=NULL; /* input TOA or Surface Reflectance 
                                      filename */
    char *dem_infile=NULL;   /* input DEM filename */
    int retval;              /* return status */
    int k;                   /* variable to keep track of the % complete */
    int band;                /* current band to be processed */
    int line;                /* current starting line to be processed */
    int pix;                 /* current pixel to be processed */
    int nlines_proc;         /* number of lines to process at one time */
    int start_line;          /* line of DEM to start reading */
    int extra_lines_start;   /* number of extra lines at the start of the DEM
                                to be read as part of the 3x3 window */
    int extra_lines_end;     /* number of extra lines at the end of the DEM
                                to be read as part of the 3x3 window */
    int nvals_to_read;       /* actual number of values to be read from the DEM
                                to include padding for the 3x3 */
    int offset;              /* offset in the raw binary DEM file to seek to
                                to begin reading the window in the DEM */
    int out_sds_types[NUM_OUT_SDS];  /* array of image SDS types */
    int16 *raw_swe=NULL;       /* Raw surface water extent */
    int16 *slope_revised_swe=NULL;    /* Slope revised SWE */
    int16 *cloud_corrected_swe=NULL;  /* Cloud corrected SWE */
    int16 *slope_cloud_swe=NULL;      /* Slope revised & cloud corrected SWE */
    float *percent_slope=NULL; /* percent slope values between 0.0 and 100.0 */
    int16 *scaled_slope=NULL;  /* percent slope values between 0 and 10000 */
    int16 *dem=NULL;           /* input DEM data (meters) */
    Input_t *input=NULL;       /* input structure for both the TOA reflectance
                                  and brightness temperature products */
    Space_def_t space_def;     /* spatial definition information */
    Output_t *output = NULL;   /* output structure and metadata */

    FILE *dem_fptr=NULL;             /* input scene-based DEM file pointer */
    FILE *raw_swe_fptr=NULL;         /* raw SWE file pointer */
    FILE *slope_swe_fptr=NULL;       /* slope revised SWE file pointer */
    FILE *cloud_swe_fptr=NULL;       /* cloud corrected SWE file pointer */
    FILE *slope_cloud_swe_fptr=NULL; /* slope revised & cloud corrected SWE 
                                        file pointer */
    FILE *scaled_slope_fptr=NULL;    /* slope revised & cloud corrected SWE 
                                        file pointer */
    char dem_envi_hdr[STR_SIZE];
    bool use_toa;

    printf ("Starting scene-based snow cover processing ...\n");

    /* Read the command-line arguments, including the name of the input
       Landsat TOA reflectance product and the DEM */
    retval = get_args (argc, argv, &reflectance_infile, &dem_infile,
                       &mgt, &mlt1, &mlt2, &b4t1, &b4t2, &b5t1, &b5t2, 
                       &per_slope, &write_binary, &use_fmask, &verbose);
    if (retval != SUCCESS)
    {   
        sprintf (errmsg, "Error calling get_args");
        error_handler (true, FUNC_NAME, errmsg);
        exit (ERROR);
    }

    printf("mgt, mlt1, mlt2, b4t1, b4t2, b5t1, b5t2, per_slope, write_binary,"
           "use_fmask,verbose=%f,%f,%f,%d,%d,%d,%d,%f,%d,%d,%d\n",mgt, mlt1, mlt2, b4t1, 
           b4t2, b5t1, b5t2, per_slope, write_binary,use_fmask,verbose);


    /* Provide user information if verbose is turned on */
    if (verbose)
    {
        printf ("  TOA reflectance input file: %s\n", reflectance_infile);
        printf ("  DEM input file: %s\n", dem_infile);
        printf ("  MGT: %f\n", mgt);
        printf ("  MLT1: %f\n", mlt1);
        printf ("  MLT2: %f\n", mlt2);
        printf ("  B4T1: %d\n", b4t1);
        printf ("  B4T2: %d\n", b4t2);
        printf ("  B5T1: %d\n", b5t1);
        printf ("  B5T2: %d\n", b5t2);
        printf ("  Percent_slope: %f\n", per_slope);
        if (write_binary)
            printf ("    -- Also writing raw binary output.\n");
    }

    split_filename(reflectance_infile, directory, scene_name, extension);

    if (verbose)
        printf("directory, scene_name, extension=%s,%s,%s\n", 
            directory, scene_name, extension);
    sprintf(lndsr_name, "%slndsr.%s.hdf", directory, scene_name);
    sprintf(lndcal_name, "%slndcal.%s.hdf", directory, scene_name);
    if (strstr(reflectance_infile, "lndcal") != NULL)
        use_toa = true;
    else
        use_toa = false;
    sprintf(swe_hdf_name, "%sswe.%s.hdf", directory, scene_name);
    if (write_binary)
    {
        sprintf(raw_swe_bin, "%sraw_swe.bin", directory);
        sprintf(raw_swe_hdr, "%sraw_swe.bin.hdr", directory);
        sprintf(slope_revised_swe_bin, "%sslope_revised_swe.bin", directory);
        sprintf(slope_revised_swe_hdr, "%sslope_revised_swe.bin.hdr", 
            directory);
        sprintf(cloud_corrected_swe_bin, "%scloud_corrected_swe.bin", 
            directory);
        sprintf(cloud_corrected_swe_hdr, "%scloud_corrected_swe.bin.hdr", 
            directory);
        sprintf(slope_cloud_swe_bin, "%sslope_cloud_swe.bin", directory);
        sprintf(slope_cloud_swe_hdr, "%sslope_cloud_swe.bin.hdr", directory);
        sprintf(scaled_percent_slope_bin, "%sscaled_percent_slope.bin", 
            directory);
        sprintf(scaled_percent_slope_hdr, "%sscaled_percent_slope.bin.hdr", 
            directory);
    }
    if (verbose)
    {
        printf("lndcal_name, lndsr_name = %s, %s\n", lndcal_name, lndsr_name); 
        printf("Output swe_hdf_name = %s\n", swe_hdf_name); 
        if (write_binary)
        {
            printf("raw_swe_binary_name = %s\n", raw_swe_bin); 
            printf("slope_revised_swe_binary_name = %s\n", 
                slope_revised_swe_bin); 
            printf("cloud_corrected_swe_binary_name = %s\n", 
                cloud_corrected_swe_bin); 
            printf("slope_revised_cloud_corrected_swe_binary_name = %s\n", 
                slope_cloud_swe_bin); 
            printf("scaled_percent_slope_binary_name = %s\n", 
                scaled_percent_slope_bin); 
        }
    }

    /* Open the output files for raw binary output */
    if (write_binary)
    {
        raw_swe_fptr = fopen (raw_swe_bin, "wb");
        slope_swe_fptr = fopen (slope_revised_swe_bin, "wb");
        cloud_swe_fptr = fopen (cloud_corrected_swe_bin, "wb");
        slope_cloud_swe_fptr = fopen (slope_cloud_swe_bin, "wb");
        scaled_slope_fptr = fopen (scaled_percent_slope_bin, "wb");
    }

    /* Open the TOA reflectance or surface reflectance products, set up
       the input data structure, allocate memory for the data buffers, and
       read the associated metadata and attributes. */
    input = open_input (lndcal_name, lndsr_name, use_toa, use_fmask);
    if (input == (Input_t *)NULL)
    {
        sprintf (errmsg, "Error opening/reading the reflectance file: %s "
            "and %s", lndcal_name, lndsr_name);
        error_handler (true, FUNC_NAME, errmsg);
        exit (ERROR);
    }
    input->use_toa = use_toa;

    /* Output some information from the input files if verbose */
    if (verbose)
    {
        printf ("  WRS path/row: %03d/%02d\n", input->meta.path,
            input->meta.row);
        printf ("  Number of lines/samples: %d/%d\n", input->nlines,
            input->nsamps);
        printf ("  Number of reflective bands: %d\n", input->nrefl_band);
        printf ("  Pixel size: %f\n", input->meta.pixsize);
        printf ("  Solar elevation angle: %f radians (%f degrees)\n",
            input->meta.solar_elev, input->meta.solar_elev * DEG);
        printf ("  Solar azimuth angle: %f radians (%f degrees)\n",
            input->meta.solar_az, input->meta.solar_az * DEG);
        printf ("  Fill value (refl): %d\n", input->refl_fill);
        printf ("  Scale factor (refl): %f\n",
            input->refl_scale_fact);
        printf ("  Saturation value (refl, btemp): %d\n",
            input->refl_saturate_val);
    }

    /* Allocate memory for the raw Surface Water Extent (SWE) */
    raw_swe = (int16 *) calloc (PROC_NLINES * input->nsamps,
        sizeof (int16));
    if (raw_swe == NULL)
    {
        sprintf (errmsg, "Error allocating memory for the raw SWE");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Allocate memory for the slope revised Surface Water Extent (SWE) */
    slope_revised_swe = (int16 *) calloc (PROC_NLINES * input->nsamps,
        sizeof (int16));
    if (slope_revised_swe == NULL)
    {
        sprintf (errmsg, "Error allocating memory for the slope revised SWE");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Allocate memory for the cloud corrected Surface Water Extent (SWE) */
    cloud_corrected_swe = (int16 *) calloc (PROC_NLINES * input->nsamps,
        sizeof (int16));
    if (cloud_corrected_swe == NULL)
    {
        sprintf (errmsg, "Error allocating memory for the cloud corrected SWE");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Allocate memory for the slope revised & cloud corrected SWE */
    slope_cloud_swe = (int16 *) calloc (PROC_NLINES * input->nsamps,
        sizeof (int16));
    if (slope_cloud_swe == NULL)
    {
        sprintf (errmsg, "Error allocating memory for the slope revised & "
            "cloud corrected SWE");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Get the projection and spatial information from the input TOA
       reflectance product */
    retval = get_space_def_hdf (&space_def, lndsr_name, hdf_grid_name);
    if (retval != SUCCESS)
    {
        sprintf (errmsg, "Error reading spatial metadata from the HDF file: "
            "%s", lndsr_name);
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Create and open the output HDF-EOS file */
    if (create_output (swe_hdf_name) != SUCCESS)
    {  
        sprintf (errmsg, "Error calling create_output");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    output = open_output (swe_hdf_name, NUM_OUT_SDS, out_sds_names,
        input->nlines, input->nsamps);
    if (output == NULL)
    {   
        sprintf (errmsg, "Error calling open_output");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Open the DEM for reading raw binary */
    dem_fptr = fopen (dem_infile, "rb");
    if (dem_fptr == NULL)
    {
        sprintf (errmsg, "Error opening the DEM file: %s", dem_infile);
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Allocate memory for the DEM, which will hold PROC_NLINES of data.  The
       DEM should be the same size as the input scene, since the scene was
       used to resample the DEM.  To process the shaded relieve we need to
       read an extra two lines to process a 3x3 window. */
    dem = (int16 *) calloc ((PROC_NLINES+2) * input->nsamps, sizeof(int16));
    if (dem == NULL)
    {
        sprintf (errmsg, "Error allocating memory for the DEM data");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Allocate memory for the percent_slope */
    percent_slope = (float *) calloc (PROC_NLINES * input->nsamps,
        sizeof (float));
    if (percent_slope == NULL)
    {
        sprintf (errmsg, "Error allocating memory for the percent slope");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Allocate memory for the scaled_slope */
    scaled_slope = (int16 *) calloc (PROC_NLINES * input->nsamps,
        sizeof (int16));
    if (scaled_slope == NULL)
    {
        sprintf (errmsg, "Error allocating memory for the scaled");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Print the processing status if verbose */
    if (verbose)
    {
        printf ("  Processing %d lines at a time\n", PROC_NLINES);
        printf ("  Cloud and snow cover -- %% complete: 0%%\r");
    }

    /* Loop through the lines and samples in the reflectance and
       QA products, computing the cloud and snow cover */
    nlines_proc = PROC_NLINES;
    k = 0;
    for (line = 0; line < input->nlines; line += PROC_NLINES)
    {
        /* Do we have nlines_proc left to process? */
        if (line + nlines_proc >= input->nlines)
            nlines_proc = input->nlines - line;

        /* Update processing status? */
        if (verbose && (100 * line / input->nlines > k))
        {
            k = 100 * line / input->nlines;
            if (k % 10 == 0)
            {
                printf ("  Surface Water Extent -- %% complete: %d%%\r", k);
                fflush (stdout);
            }
        }

        /* Read the current lines from the reflectance file for each of the 
           reflectance bands */
        for (band = 0; band < input->nrefl_band; band++)
        {
            if (get_input_refl_lines (input, band, line, nlines_proc) !=
                SUCCESS)
            {
                sprintf (errmsg, "Error reading %d lines from band %d of the "
                    "TOA reflectance file starting at line %d", nlines_proc,
                    band, line);
                error_handler (true, FUNC_NAME, errmsg);
                close_input (input);
                free_input (input);
                exit (ERROR);
            }

            /* Read the current lines for the qa cloud band */
            if (get_input_qa_line (input, band, line, nlines_proc) != SUCCESS)
            {
                sprintf (errmsg, "Error reading %d lines from the brightness "
                    "temperature file starting at line %d", nlines_proc, line);
                error_handler (true, FUNC_NAME, errmsg);
                close_input (input);
                free_input (input);
                exit (ERROR);
            }
        }

        /* Read the current lines for the cfmask band */
        if (use_fmask)
        {
            /* Read the current lines for the fmask band */
            if (get_input_fmask_line (input, line, nlines_proc) != SUCCESS)
            {
                sprintf (errmsg, "Error reading %d lines from the fmask "
                    "file starting at line %d", nlines_proc, line);
                error_handler (true, FUNC_NAME, errmsg);
                close_input (input);
                free_input (input);
                exit (ERROR);
            }
        } 

        /* Set up mask for the TOA reflectance values */
        surface_water_extent(input->refl_buf[1] /*b2*/, 
            input->refl_buf[2] /*b3*/, input->refl_buf[3] /*b4*/, 
            input->refl_buf[4] /*b5*/, nlines_proc, input->nsamps, 
            input->refl_scale_fact, mgt, mlt1, mlt2, b4t1, b4t2, b5t1, b5t2, 
            raw_swe);

        /* Prepare to read the current lines from the DEM.  We need an extra
           line at the start and end for the slope calculation.  If we are just
           starting at line 0, then read an extra line from the end of the
           image window.  If we aren't at line 0, then read an extra line at
           the start and end of the image window.  If we are at the end of the
           image, then read an extra line from the start of the image window. */
        start_line = line - 1;
        extra_lines_start = 1;
        dem_top = false;
        dem_bottom = false;
        if (start_line < 0)
        {   /* adjust for the number of lines at the start which we
               don't have then read at line 0 */
            extra_lines_start = 0;
            start_line = 0;
            dem_top = true;
        }
        if (line + nlines_proc < input->nlines)
            extra_lines_end = 1;
        else
        {
            extra_lines_end = 0;
            dem_bottom = true;
        }

        /* Start reading from the start_line and read
           nlines_proc + extra_lines_start + extra_lines_end lines */
        nvals_to_read = (nlines_proc+extra_lines_start+extra_lines_end) *
            input->nsamps;
        offset = sizeof (int16) * start_line * input->nsamps;
        fseek (dem_fptr, offset, SEEK_SET);
        if (fread (dem, sizeof (int16), nvals_to_read, dem_fptr)
            != nvals_to_read)
        {
            sprintf (errmsg, "Error reading %d values from the DEM file "
                "starting at line %d.", nvals_to_read, start_line);
            error_handler (true, FUNC_NAME, errmsg);
            close_input (input);
            free_input (input);
            fclose (dem_fptr);
            exit (ERROR);
        }

        /* Reset the percent slope to 0s for the current window.  The first
           and last pixel will not get processed.  */
        memset ((void *) percent_slope, 0, PROC_NLINES * input->nsamps
            * sizeof (float));

        /* Compute the percent slope */
        calc_slope (dem, dem_top, dem_bottom, nlines_proc, input->nsamps,
            input->meta.pixsize, input->meta.pixsize,
            percent_slope);
    
        for (pix = 0; pix < nlines_proc * input->nsamps; pix++)
        {
            if (use_fmask)
            {
                /* Cloud screening to get cloud corrected SWE */
                if (input->fmask_buf[pix] == 2 || input->fmask_buf[pix] == 4)
                    cloud_corrected_swe[pix] = NO_VALUE;
                else
                    cloud_corrected_swe[pix] = raw_swe[pix];

                /* Use percent slope to get slope revised SWE */
                if ((percent_slope[pix] - per_slope) <= MINSIGMA)
                    slope_revised_swe[pix] = raw_swe[pix];
                else
                    slope_revised_swe[pix] = 0;

                /* Cloud screening to get slope revised cloud corrected SWE */
                if (input->fmask_buf[pix] == 2 || input->fmask_buf[pix] == 4)
                    slope_cloud_swe[pix] = NO_VALUE;
                else
                    slope_cloud_swe[pix] = slope_revised_swe[pix];
            }
            else
            {
                /* Cloud screening to get cloud corrected SWE */
                if (input->qa_buf[2][pix] == 255 || input->qa_buf[3][pix] == 
                    255)
                    cloud_corrected_swe[pix] = NO_VALUE;
                else
                    cloud_corrected_swe[pix] = raw_swe[pix];

                /* Use percent slope to get slope revised SWE */
                if ((percent_slope[pix] - per_slope) <= MINSIGMA)
                    slope_revised_swe[pix] = raw_swe[pix];
                else
                    slope_revised_swe[pix] = 0;

                /* Cloud screening to get slope revised cloud corrected SWE */
                if (input->qa_buf[2][pix] == 255 || input->qa_buf[3][pix] == 
                    255)
                    slope_cloud_swe[pix] = NO_VALUE;
                else
                    slope_cloud_swe[pix] = slope_revised_swe[pix];
            }

            /* Set SWE mask values to NO_VALUE if either band data is -9999 */
            if (input->refl_buf[1][pix] == -9999 ||
                input->refl_buf[2][pix] == -9999 ||
                input->refl_buf[3][pix] == -9999 ||
                input->refl_buf[4][pix] == -9999)
            {
                raw_swe[pix] = NO_VALUE;
                cloud_corrected_swe[pix] = NO_VALUE;
                slope_revised_swe[pix] = NO_VALUE;
                slope_cloud_swe[pix] = NO_VALUE;
            }

            /* Convert percent slope to int16 with values between 0 & 10000 */
            scaled_slope[pix] =  (int)rint(100.0 * percent_slope[pix]);
        } /* end for pix */

        /* write the non snow-related masks to raw binary output */
        if (write_binary)
        {
            fwrite (raw_swe, sizeof(int16), nlines_proc * input->nsamps,
                raw_swe_fptr);
            fwrite (slope_revised_swe, sizeof(int16), nlines_proc * 
                    input->nsamps, slope_swe_fptr);
            fwrite (cloud_corrected_swe, sizeof(int16), nlines_proc * 
                    input->nsamps, cloud_swe_fptr);
            fwrite (slope_cloud_swe, sizeof(int16), nlines_proc * 
                    input->nsamps, slope_cloud_swe_fptr);
            fwrite (scaled_slope, sizeof(int16), nlines_proc * input->nsamps, 
                    scaled_slope_fptr);
        }

        /* Write the data to the HDF file */
        output->buf[0] = raw_swe;
        output->buf[1] = slope_revised_swe;
        output->buf[2] = cloud_corrected_swe;
        output->buf[3] = slope_cloud_swe;
        output->buf[4] = &dem[input->nsamps];
        output->buf[5] = scaled_slope;

        for (band = 0; band < NUM_OUT_SDS; band++)
        {
            if (put_output_line (output, band, line, nlines_proc) != SUCCESS)
            {
                sprintf (errmsg, "Writing output data to HDF for band %d", 
                         band);
                error_handler (true, FUNC_NAME, errmsg);
                close_input (input);
                free_input (input);
                exit (ERROR);
            }
        }
    }  /* end for line */


    /* Print the processing status if verbose */
    if (verbose)
        printf ("  Surface Water Extent -- %% complete: 100%%\n");

    /* Temporary -- close the mask output files */
    if (write_binary)
    {
        fclose (raw_swe_fptr);
        fclose (slope_swe_fptr);
        fclose (cloud_swe_fptr);
        fclose (slope_cloud_swe_fptr);
        fclose (scaled_slope_fptr);
    }

    /* Write the output metadata */
    if (put_metadata (output, NUM_OUT_SDS, out_sds_names, &input->meta) != 
        SUCCESS)
    {
        sprintf (errmsg, "Error writing metadata to the output HDF file");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* Close the TOA reflectance and brightness temperature products and the
       output snow cover product */
    close_input (input);
    close_output (output);
    free_output (output);

    /* Write the spatial information, after the file has been closed */
    for (band = 0; band < NUM_OUT_SDS; band++)
        out_sds_types[band] = DFNT_INT16;
    if (put_space_def_hdf (&space_def, swe_hdf_name, NUM_OUT_SDS, out_sds_names,
        out_sds_types, hdf_grid_name) != SUCCESS)
    {
        sprintf (errmsg, "Error writing spatial metadata to the output HDF "
            "file");
        error_handler (true, FUNC_NAME, errmsg);
        close_input (input);
        free_input (input);
        exit (ERROR);
    }

    /* write the ENVI headers */
    if (write_binary)
    {
        if (verbose)
            printf ("  Creating ENVI headers for each mask.\n");
        if (write_envi_hdr (raw_swe_hdr, input, &space_def)
            == ERROR)
            exit (ERROR);
        if (write_envi_hdr (slope_revised_swe_hdr, input,
            &space_def) == ERROR)
            exit (ERROR);
        if (write_envi_hdr (cloud_corrected_swe_hdr, input, &space_def)
            == ERROR)
            exit (ERROR);
        if (write_envi_hdr (slope_cloud_swe_hdr, input, &space_def)
            == ERROR)
            exit (ERROR);
        if (write_envi_hdr (scaled_percent_slope_hdr, input, 
            &space_def) == ERROR)
            exit (ERROR);
        sprintf(dem_envi_hdr, "%s%s", dem_infile, ".hdr");
        if (write_envi_hdr (dem_envi_hdr, input, &space_def)
            == ERROR)
            exit (ERROR);
    }

    /* Free the TOA reflectance and brightness temperature pointers */
    free_input (input);

    /* Free the mask pointers */
    if (raw_swe != NULL)
    {
        free (raw_swe);
        raw_swe = NULL;
    }
    if (slope_revised_swe != NULL)
    {
        free (slope_revised_swe);
        slope_revised_swe = NULL;
    }
    if (cloud_corrected_swe != NULL)
    {
        free (cloud_corrected_swe);
        cloud_corrected_swe = NULL;
    }
    if (slope_cloud_swe != NULL)
    {
        free (slope_cloud_swe);
        slope_cloud_swe = NULL;
    }
    if (percent_slope != NULL)
    {
        free (percent_slope);
        percent_slope = NULL;
    }
    if (scaled_slope != NULL)
    {
        free (scaled_slope);
        scaled_slope = NULL;
    }

    /* close and free the dem pointers */
    fclose (dem_fptr);
    if (dem != NULL)
    {
        free (dem);
        dem = NULL;
    }

    /* Free the filename pointers */
    if (dem_infile != NULL)
        free (dem_infile);

    /* Indicate successful completion of processing */
    printf ("Scene-based snow cover processing complete!\n");
    exit (SUCCESS);
}


/******************************************************************************
MODULE:  usage

PURPOSE:  Prints the usage information for this application.

RETURN VALUE:
Type = None

HISTORY:
Date        Programmer       Reason
--------    ---------------  -------------------------------------
1/2/2013    Song Guo     

NOTES:
******************************************************************************/
void usage()
{
    printf ("scene_based_swe handles the surface water extent classification "
            "for the input Landsat scene using either TOA or surface "
            "reflectance, including a cloud cover correction and a terrain "
            "slope revision of the surface water extent\n\n");
    printf ("usage: scene_based_swe "
            "--toa=input_TOA(or surface)_reflectance_filename_with_full_path "
            "--dem=input_DEM_filename_with_full_path "
            "--mgt=mgt_threshold (value between 0.00 and 2.00) "
            "--mlt1=mlt1_threshold (value between -2.00 and 2.00) "
            "--mlt2=mlt2_threshold (value between -2.00 and 2.00) "
            "--b4t1=b4t1_threshold "
            "--b4t2=b4t2_threshold "
            "--b5t1=b5t1_threshold "
            "--b5t2=b5t2_threshold "
            "--per_slope=percent_slope_threshold (value between 0.00 & 100.00 "
            "[--write_binary] [--use_fmask] [--verbose]\n");

    printf ("\nwhere the following parameters are required:\n");
    printf ("    -reflectance: name of the input Landsat TOA or surface "
            "reflectance file to be classified (HDF)\n");
    printf ("    -dem: name of the DEM associated with the Landsat TOA file "
            "(raw binary 16-bit integers)\n");
    printf ("\nwhere the following parameters are optional:\n");
    printf ("    -mgt: MNDWI_threshold\n");
    printf ("    -mlt1: mlt1_threshold\n");
    printf ("    -mlt2: mlt2_threshold\n");
    printf ("    -b4t1: b4t1_threshold\n");
    printf ("    -b4t2: b4t2_threshold\n");
    printf ("    -b5t1: b5t1_threshold\n");
    printf ("    -b5t2: b5t2_threshold\n");
    printf ("    -per_slope: percent_slope_threshold\n");
    printf ("    -write_binary: should raw binary outputs and ENVI header "
            "files be written in addition to the HDF file? (default is false)"
            "\n");
    printf ("    -use_fmask: should fmask cloud and shadow mask "
            "be used? (default is false)\n");
    printf ("    -verbose: should intermediate messages be printed? (default "
            "is false)\n");
    printf ("\n./scene_based_swe --help will print the usage statement\n");
    printf ("\nExample: ./scene_based_swe "
            "--reflectance=/data1/sguo/lndsr.LT50450302001272LGS01.hdf "
            "--dem=/data1/sguo/lsrd_scene_based_dem.bin "
            "--mgt=0.123 --mlt1=-0.5 --mlt2=-0.4 --b4t1=1500 "
            "--b4t2=1500 --b5t1=1000 --b5t2=1700 --per_slope=3.0 "
            "--write_binary --use_fmask --verbose\n");
}
