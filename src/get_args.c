
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>
#include <error.h>
#include <string.h>
#include <limits.h>

#include "const.h"
#include "utilities.h"
#include "get_args.h"

/* Default input parameter values */
static float default_wigt = 0.015;
static float default_awgt = 0.0;

static float default_pswt_1 = -0.05;
static float default_pswt_2 = -0.05;

static float default_percent_slope = 3.0;

static int default_pswnt_1 = 1500;
static int default_pswst_1 = 1000;

static int default_pswnt_2 = 1700;
static int default_pswst_2 = 650;


/*****************************************************************************
  NAME:  usage

  PURPOSE:  Displays the help/usage to the terminal.

  RETURN VALUE:  None
*****************************************************************************/
void
usage ()
{
    printf ("Dynamic Surface Water Extent\n"
            "Determines and builds surface water extent output bands from"
            " surface\nreflectance input data in ESPA raw binary format.\n\n");
    printf ("usage: dswe"
            " --xml <input_xml_filename> [--help]\n\n");
    printf ("where the following parameters are required:\n");
    printf ("    --xml: name of the input XML file which contains the surface"
            " reflectance,\n"
            "           and top of atmos files output from LEDAPS in raw binary"
            "\n           (envi) format\n");
    printf ("    --dem: name of the input DEM file which will is expected to"
            " be the same\n"
            "           size and area as the source data\n");
    printf ("where the following parameters are optional:\n");
    printf ("    --wigt: Modified Normalized Difference Wetness Index"
            " Threshold between 0.00 and 2.00 (default value is %0.3f)\n",
            default_wigt);
    printf ("    --awgt: Automated Water Extent Shadow"
            " Threshold between -2.00 and 2.00"
            " (default value is %0.2f)\n", default_awgt);

    printf ("    --pswt_1: Partial Surface Water 1 Threshold between -2.00 and"
            " 2.00 (default value is %0.2f)\n", default_pswt_1);
    printf ("    --pswt_2: Partial Surface Water 2 Threshold between -2.00 and"
            " 2.00 (default value is %0.2f)\n", default_pswt_2);

    printf ("    --pswnt_1: Partial Surface Water 1 NIR Threshold"
            " between 0 and data maximum (default value is %d)\n",
            default_pswnt_1);
    printf ("    --pswnt_2: Partial Surface Water 2 NIR Threshold"
            " between 0 and data maximum (default value is %d)\n",
            default_pswnt_2);

    printf ("    --pswst_1: Partial Surface Water 1 SWIR1 Threshold"
            " between 0 and data maximum (default value is %d)\n",
            default_pswst_1);
    printf ("    --pswst_2: Partial Surface Water 2 SWIR1 Threshold"
            " between 0 and data maximum (default value is %d)\n",
            default_pswst_2);

    printf ("    --percent-slope: Threshold between 0.00 and 100.00"
            " (default value is %0.1f)\n", default_percent_slope);
    printf ("    --use_zeven_thorne: should Zevenbergen&Thorne's shaded"
            " algorithm be used?\n"
            "                        (default is false, meaning Horn's shaded"
            " algorithm will\n" "                        be used)\n");
    printf ("    --use-toa: should Top of Atmosphere be used instead of"
            " Surface Reflectance\n"
            "               (default is false, meaning Surface Reflectance"
            " will be used)\n");
    printf ("    --verbose: should intermediate messages be printed? (default"
            " is false)\n\n");
    printf ("dswe --help will print this usage statement\n\n");
    printf ("Example: dswe"
            " --xml LE70760172000175AGS00.xml\n");
}

/*****************************************************************************
  NAME:  get_args

  PURPOSE:  Gets the command line arguments and validates that the required
            arguments were specified.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      ERROR    Error getting the command line arguments or a command line
               argument and associated value were not specified.
      SUCCESS  No errors encountered.
*****************************************************************************/
int
get_args
(
    int argc,                    /* I: number of cmd-line args */
    char *argv[],                /* I: string of cmd-line args */
    char **xml_infile,           /* O: input XML filename */
    char **dem_infile,           /* O: input DEM filename */
    bool *use_zeven_thorne_flag, /* O: use zeven thorne */
    bool *use_toa_flag,          /* O: process using TOA */
    float *wigt,                 /* O: tolerance value */
    float *awgt,                 /* O: tolerance value */
    float *pswt_1,               /* O: tolerance value */
    float *pswt_2,               /* O: tolerance value */
    float *percent_slope,        /* O: slope tolerance */
    int *pswnt_1,                /* O: tolerance value */
    int *pswnt_2,                /* O: tolerance value */
    int *pswst_1,                /* O: tolerance value */
    int *pswst_2,                /* O: tolerance value */
    bool * verbose_flag          /* O: verbose messaging */
)
{
    int c;
    int option_index;
    char msg[256];
    int tmp_zeven_thorne_flag;
    int tmp_toa_flag;
    int tmp_verbose_flag;

    struct option long_options[] = {
        /* These options set a flag */
        {"use-zeven-thorne", no_argument, &tmp_zeven_thorne_flag, true},
        {"use-toa", no_argument, &tmp_toa_flag, true},

        /* These options provide values */
        {"xml", required_argument, 0, 'x'},
        {"dem", required_argument, 0, 'd'},

        {"wigt", required_argument, 0, 'w'},
        {"awgt", required_argument, 0, 'a'},

        {"pswt_1", required_argument, 0, 'p'},
        {"pswt_2", required_argument, 0, 'q'},

        {"pswnt_1", required_argument, 0, 'n'},
        {"pswnt_2", required_argument, 0, 'o'},

        {"pswst_1", required_argument, 0, 'r'},
        {"pswst_2", required_argument, 0, 's'},

        {"percent-slope", required_argument, 0, 't'},

        /* Special options */
        {"verbose", no_argument, &tmp_verbose_flag, true},

        /* The help option */
        {"help", no_argument, 0, 'h'},

        /* The option termination set */
        {0, 0, 0, 0}
    };

    if (argc == 1)
    {
        ERROR_MESSAGE ("Missing required command line arguments\n\n",
                       MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Assign the default values */
    *wigt = default_wigt;
    *awgt = default_awgt;
    *pswt_1 = default_pswt_1;
    *pswt_2 = default_pswt_2;
    *percent_slope = default_percent_slope;
    *pswnt_1 = default_pswnt_1;
    *pswnt_2 = default_pswnt_2;
    *pswst_1 = default_pswst_1;
    *pswst_2 = default_pswst_2;

    /* loop through all the cmd-line options */
    opterr = 0; /* turn off getopt_long error msgs as we'll print our own */
    while (1)
    {
        c = getopt_long (argc, argv, "", long_options, &option_index);
        if (c == -1)
        {
            /* out of cmd-line options */
            break;
        }

        switch (c)
        {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
        case 'h':
            usage ();
            return ERROR;
            break;

        case 'x':
            *xml_infile = strdup (optarg);
            break;

        case 'd':
            *dem_infile = strdup (optarg);
            break;

        case 'w':
            *wigt = atof (optarg);
            break;
        case 'a':
            *awgt = atof (optarg);
            break;

        case 'p':
            *pswt_1 = atof (optarg);
            break;
        case 'q':
            *pswt_2 = atof (optarg);
            break;

        case 'n':
            *pswnt_1 = atoi (optarg);
            break;
        case 'o':
            *pswnt_2 = atoi (optarg);
            break;

        case 'r':
            *pswst_1 = atoi (optarg);
            break;
        case 's':
            *pswst_2 = atoi (optarg);
            break;

        case 't':
            *percent_slope = atof (optarg);
            break;
        case '?':
        default:
            snprintf (msg, sizeof (msg),
                      "Unknown option %s\n\n", argv[optind - 1]);
            ERROR_MESSAGE (msg, MODULE_NAME);
            usage ();
            return ERROR;
            break;
        }
    }

    /* Grab the boolean command line options */
    if (tmp_zeven_thorne_flag)
        *use_zeven_thorne_flag = true;
    else
        *use_zeven_thorne_flag = false;

    if (tmp_toa_flag)
        *use_toa_flag = true;
    else
        *use_toa_flag = false;

    if (tmp_verbose_flag)
        *verbose_flag = true;
    else
        *verbose_flag = false;

    /* ---------- Validate the parameters ---------- */
    /* Make sure the XML was specified */
    if (*xml_infile == NULL)
    {
        ERROR_MESSAGE ("XML input file is a required command line"
                       " argument\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Make sure the DEM was specified */
    if (*dem_infile == NULL)
    {
        ERROR_MESSAGE ("DEM input file is a required command line"
                       " argument\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if (*wigt < 0.0 || (*wigt - 2.0) > MINSIGMA)
    {
        ERROR_MESSAGE ("WIGT is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*awgt + 2) < MINSIGMA || (*awgt - 2.0) > MINSIGMA)
    {
        ERROR_MESSAGE ("AWGT is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*pswt_1 + 2) < MINSIGMA || (*pswt_1 - 2.0) > MINSIGMA)
    {
        ERROR_MESSAGE ("PSWT_1 is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*pswt_2 + 2) < MINSIGMA || (*pswt_2 - 2.0) > MINSIGMA)
    {
        ERROR_MESSAGE ("PSWT_2 is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Only checking the low side here */
    if (*pswnt_1 < 0)
    {
        ERROR_MESSAGE ("PSWNT_1 is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Only checking the low side here */
    if (*pswnt_2 < 0)
    {
        ERROR_MESSAGE ("PSWNT_2 is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Only checking the low side here */
    if (*pswst_1 < 0)
    {
        ERROR_MESSAGE ("PSWST_1 is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Only checking the low side here */
    if (*pswst_2 < 0)
    {
        ERROR_MESSAGE ("PSWST_2 is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if (*percent_slope < 0.0 || (*percent_slope - 100.0) > MINSIGMA)
    {
        ERROR_MESSAGE ("Percent Slope is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    return SUCCESS;
}
