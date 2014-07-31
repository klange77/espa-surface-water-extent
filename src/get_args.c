
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>
#include <error.h>
#include <string.h>
#include <limits.h>

#include "dswe_const.h"
#include "utilities.h"
#include "get_args.h"

/* Default input parameter values */
static float default_wigt = 0.015;
static float default_awgt = 0.0;
static float default_pswt = -0.05;
static float default_percent_slope = 3.0;
static int default_pswnt = 1500;
static int default_pswst = 1000;


/*****************************************************************************
 TODO TODO TODO
*****************************************************************************/
void
usage ()
{
    printf ("Dynamic Surface Water Extent\n"
            "Determines and builds surface water extent output bands from"
            " surface\nreflectance input data in ESPA raw binary format.\n\n");
    printf ("usage: dswe"
            " --xml <input_xml_filename>"
            " --dem <input_dem_filename>" " [--help]\n\n");
    printf ("where the following parameters are required:\n");
    printf ("    --xml: name of the input XML file which contains the surface"
            " reflectance,\n"
            "           and top of atmos files output from LEDAPS in raw binary"
            "\n           (envi) format\n");
    printf ("    --dem: name of the input DEM file which contains the"
            " elevation for the image\n"
            "           extent of the data to be processed in envi"
            " format\n\n");
    printf ("where the following parameters are optional:\n");
    printf ("    --wigt: Modified Normalized Difference Wetness Index"
            " Threshold between 0.00 and 2.00 (default value is %0.3f)\n",
            default_wigt);
    printf ("    --awgt: Threshold between -2.00 and 2.00"
            " (default value is %0.2f)\n", default_awgt);
    printf ("    --pswt: Partial Surface Water Threshold between -2.00 and"
            " 2.00 (default value is %0.2f)\n", default_pswt);
    printf ("    --pswnt: Partial Surface Water NIR Threshold between 0 and"
            " data maximum (default value is %d)\n", default_pswnt);
    printf ("    --pswst: Partial Surface Water SWIR1 Threshold between 0 and"
            " data maximum (default value is %d)\n", default_pswst);
    printf ("    --percent-slope: Threshold between 0.00 and 100.00"
            " (default value is %0.1f)\n", default_percent_slope);
    printf ("    --use_ledaps_mask: should ledaps cloud/shadow mask be used?"
            " (default is\n"
            "                       false, meaning fmask cloud/shadow will be"
            " used)\n");
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
            " --xml LE70760172000175AGS00.xml"
            " --dem dem.LE70760172000175AGS00.img\n");
}

/*****************************************************************************
 TODO TODO TODO
*****************************************************************************/
int
get_args (int argc,             /* I: number of cmd-line args */
          char *argv[],         /* I: string of cmd-line args */
          char **xml_infile,    /* O: address of input XML filename */
          char **dem_infile,    /* O: address of input DEM filename */
          bool *use_ledaps_mask_flag,
          bool *use_zeven_thorne_flag,
          bool *use_toa_flag,
          float *wigt,
          float *awgt,
          float *pswt,
          float *percent_slope,
          int *pswnt,
          int *pswst,
          bool * verbose_flag)
{
    int c;
    int option_index;
    char msg[256];

    /* Make these method parameters????? */
    static int tmp_ledaps_mask_flag;
    static int tmp_zeven_thorne_flag;
    static int tmp_toa_flag;
    static int tmp_verbose_flag;

    static struct option long_options[] = {
        /* These options set a flag */
        {"use-ledaps-mask", no_argument, &tmp_ledaps_mask_flag, true},
        {"use-zeven-thorne", no_argument, &tmp_zeven_thorne_flag, true},
        {"use-toa", no_argument, &tmp_toa_flag, true},

        /* These options provide values */
        {"xml", required_argument, 0, 'x'},
        {"dem", required_argument, 0, 'd'},
        {"wigt", required_argument, 0, 'w'},
        {"awgt", required_argument, 0, 'a'},
        {"pswt", required_argument, 0, 'p'},
        {"pswnt", required_argument, 0, 'n'},
        {"pswst", required_argument, 0, 'r'},
        {"percent-slope", required_argument, 0, 's'},

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
    *pswt = default_pswt;
    *percent_slope = default_percent_slope;
    *pswnt = default_pswnt;
    *pswst = default_pswst;

    /* loop through all the cmd-line options */
    opterr = 0;                 /* turn off getopt_long error msgs as we'll print our own */
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
            *pswt = atof (optarg);
            break;
        case 'n':
            *pswnt = atoi (optarg);
            break;
        case 'r':
            *pswst = atoi (optarg);
            break;
        case 's':
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
    if (tmp_ledaps_mask_flag)
        *use_ledaps_mask_flag = true;
    else
        *use_ledaps_mask_flag = false;

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

    if ((*pswt + 2) < MINSIGMA || (*pswt - 2.0) > MINSIGMA)
    {
        ERROR_MESSAGE ("PSWT is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Only checking the low side here check the high side outside of this
       routine TODO TODO TODO */
    if (*pswnt < 0)
    {
        ERROR_MESSAGE ("PSWB4T is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Only checking the low side here check the high side outside of this
       routine TODO TODO TODO */
    if (*pswst < 0)
    {
        ERROR_MESSAGE ("PSWB4T is out of range\n\n", MODULE_NAME);

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
