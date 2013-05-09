#include <getopt.h>
#include "swe.h"

#define MINSIGMA 1e-5
/******************************************************************************
MODULE:  get_args

PURPOSE:  Gets the command-line arguments and validates that the required
arguments were specified.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error getting the command-line arguments or a command-line
                argument and associated value were not specified
SUCCESS         No errors encountered

HISTORY:
Date        Programmer       Reason
--------    ---------------  -------------------------------------
1/2/2013    Gail Schmidt     Original Development
4/22/2013   Song Guo         Modified for SWE (Surface Water Extent) use
NOTES:
  1. Memory is allocated for the input and output files.  All of these should
     be character pointers set to NULL on input.  The caller is responsible
     for freeing the allocated memory upon successful return.
******************************************************************************/
short get_args
(
    int argc,             /* I: number of cmd-line args */
    char *argv[],         /* I: string of cmd-line args */
    char **reflectance_infile, /* O:address of the input TOA or Surface 
                                    Reflectance filename */
    char **dem_infile,    /* O: address of input DEM filename */
    float *mgt,           /* O: */
    float *mlt1,          /* O: */
    float *mlt2,          /* O: */
    int16 *b4t1,          /* O: */
    int16 *b4t2,          /* O: */
    int16 *b5t1,          /* O: */
    int16 *b5t2,          /* O: */
    float *per_slope, /* O: percent slope */
    bool *write_binary,   /* O: write raw binary flag */
    bool *verbose         /* O: verbose flag */
)
{
    int c;                           /* current argument index */
    int option_index;                /* index for the command-line option */
    static float default_mgt = 0.123;/* Default MGT value */
    static float default_mlt1 = -0.5;/* Default MLT1 value */
    static float default_mlt2 = -0.4;/* Default MLT2 value */
    static int16 default_b4t1 = 1500;/* Default B4T1 value */
    static int16 default_b4t2 = 1500;/* Default B4T2 value */
    static int16 default_b5t1 = 1000;/* Default B5T1 value */
    static int16 default_b5t2 = 1700;/* Default B5T2 value */
    static float default_per_s = 3.0;/* Default percent slope value */
    static int verbose_flag=0;       /* verbose flag */
    static int binary_flag=0;        /* write binary flag */
    char errmsg[STR_SIZE];           /* error message */
    char FUNC_NAME[] = "get_args";   /* function name */
    static struct option long_options[] =
    {
        {"verbose", no_argument, &verbose_flag, 1},
        {"write_binary", no_argument, &binary_flag, 1},
        {"reflectance", required_argument, 0, 'r'},
        {"dem", required_argument, 0, 'd'},
        {"mgt", required_argument, 0, 'g'},
        {"mlt1", required_argument, 0, '1'},
        {"mlt2", required_argument, 0, '2'},
        {"b4t1", required_argument, 0, '3'},
        {"b4t2", required_argument, 0, '4'},
        {"b5t1", required_argument, 0, '5'},
        {"b5t2", required_argument, 0, '6'},
        {"per_slope", required_argument, 0, 'p'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    /* Assign the default values */
    *mgt = default_mgt;
    *mlt1 = default_mlt1;
    *mlt2 = default_mlt2;
    *b4t1 = default_b4t1;
    *b4t2 = default_b4t2;
    *b5t1 = default_b5t1;
    *b5t2 = default_b5t2;
    *per_slope = default_per_s;

    /* Loop through all the cmd-line options */
    opterr = 0;   /* turn off getopt_long error msgs as we'll print our own */
    while (1)
    {
        /* optstring in call to getopt_long is empty since we will only
           support the long options */
        c = getopt_long (argc, argv, "", long_options, &option_index);
        if (c == -1)
        {   /* Out of cmd-line options */
            break;
        }

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
     
            case 'h':  /* help */
                usage();
                return (ERROR);
                break;

            case 'r':  /* toa or sr infile */
                *reflectance_infile = strdup (optarg);
                break;
          
            case 'd':  /* dem infile */
                *dem_infile = strdup (optarg);
                break;
     
            case 'g':  /* GMT */
                *mgt = atof(optarg);
                break;
     
            case '1':  /*  */
                *mlt1 = atof(optarg);
                break;
     
            case '2':  /*  */
                *mlt2 = atof(optarg);
                break;
     
            case '3':  /*  */
                *b4t1 = atoi(optarg);
                break;
     
            case '4':  /*  */
                *b4t2 = atoi(optarg);
                break;
     
            case '5':  /*  */
                *b5t1 = atoi(optarg);
                break;
     
            case '6':  /*  */
                *b5t2 = atoi(optarg);
                break;
     
            case 'p':  /* Percent Slope */
                *per_slope = atof(optarg);
                break;
     
            case '?':
            default:
                sprintf (errmsg, "Unknown option %s", argv[optind-1]);
                error_handler (true, FUNC_NAME, errmsg);
                usage();
                return (ERROR);
                break;
        }
    }

    /* Make sure the infiles and outfiles were specified */
    if (*reflectance_infile == NULL)
    {
        sprintf (errmsg, "TOA input file is a required argument");
        error_handler (true, FUNC_NAME, errmsg);
        usage();
        return (ERROR);
    }

    if (*dem_infile == NULL)
    {
        sprintf (errmsg, "DEM input file is a required argument");
        error_handler (true, FUNC_NAME, errmsg);
        usage();
        return (ERROR);
    }

    if (*mgt < MINSIGMA || (*mgt - 2.0) > MINSIGMA)
    {
        sprintf (errmsg, "MGT is out of range");
        error_handler (true, FUNC_NAME, errmsg);
        usage();
        return (ERROR);
    }

    if ((*mlt1 + 2.0) < MINSIGMA || (*mlt1 - 2.0) > MINSIGMA)
    {
        sprintf (errmsg, "MLT1 is out of range");
        error_handler (true, FUNC_NAME, errmsg);
        usage();
        return (ERROR);
    }

    if ((*mlt2 + 2.0) < MINSIGMA || (*mlt2 - 2.0) > MINSIGMA)
    {
        sprintf (errmsg, "MLT2 is out of range");
        error_handler (true, FUNC_NAME, errmsg);
        usage();
        return (ERROR);
    }

    if (*per_slope < MINSIGMA || (*per_slope - 100.0) > MINSIGMA)
    {
        sprintf (errmsg, "Percent slope is out of range");
        error_handler (true, FUNC_NAME, errmsg);
        usage();
        return (ERROR);
    }

    /* Check the write binary flag */
    if (binary_flag)
        *write_binary = true;

    /* Check the verbose flag */
    if (verbose_flag)
        *verbose = true;

    return (SUCCESS);
}
