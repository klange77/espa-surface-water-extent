
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>
#include <error.h>
#include <string.h>
#include <limits.h>


#include "espa_metadata.h"
#include "parse_metadata.h"


#include "const.h"
#include "dswe.h"
#include "utilities.h"
#include "get_args.h"


/* Specify default parameter values */
/* L4-7 defaults */
static float wigt_l47_default = 0.124;
static float awgt_l47_default = 0.0;
static float pswt_1_mndwi_l47_default = -0.44;
static int pswt_1_nir_l47_default = 1500;
static int pswt_1_swir1_l47_default = 900;
static float pswt_1_ndvi_l47_default = 0.7;
static float pswt_2_mndwi_l47_default = -0.5;
static int pswt_2_blue_l47_default = 1000;
static int pswt_2_nir_l47_default = 2500;
static int pswt_2_swir1_l47_default = 3000;
static int pswt_2_swir2_l47_default = 1000;
/* L8 defaults */
static float wigt_l8_default = 0.124;
static float awgt_l8_default = 0.0;
static float pswt_1_mndwi_l8_default = -0.44;
static int pswt_1_nir_l8_default = 1500;
static int pswt_1_swir1_l8_default = 900;
static float pswt_1_ndvi_l8_default = 0.7;
static float pswt_2_mndwi_l8_default = -0.5;
static int pswt_2_blue_l8_default = 1000;
static int pswt_2_nir_l8_default = 2500;
static int pswt_2_swir1_l8_default = 3000;
static int pswt_2_swir2_l8_default = 1000;

static float percent_slope_high_default = 30;
static float percent_slope_moderate_default = 30;
static float percent_slope_wetland_default = 20;
static float percent_slope_low_default = 10;
static int hillshade_default = 110;

/* Parameter values should never be this, so use it to determine if a
   parameter was specified or not on the command line before applying the
   default value */
#define NOT_SET -9999.0


/*****************************************************************************
  NAME:  version

  PURPOSE:  Prints the version information for this application.

  RETURN VALUE:  Type = None
*****************************************************************************/
void
version ()
{
    printf ("%s version %s\n", DSWE_APP_NAME, DSWE_VERSION);
}


/*****************************************************************************
  NAME:  usage

  PURPOSE:  Displays the help/usage to the terminal.

  RETURN VALUE:  Type = None
*****************************************************************************/
void
usage ()
{
    version();

    printf ("Dynamic Surface Water Extent\n"
            "Determines and builds Dynamic Surface Water Extent output bands"
            " from Surface\n"
            "Reflectance input data in ESPA raw binary format.\n\n");
    printf ("usage: dswe"
            " --xml <input_xml_filename> [--help]\n\n");
    printf ("where the following parameters are required:\n");
    printf ("    --xml: Name of the input XML file which contains the surface"
            " reflectance\n"
            "           and top of atmosphere files output from LEDAPS in raw"
            " binary\n"
            "           (envi) format\n");

    printf ("where the following parameters are optional:\n");
    printf ("    --wigt: Modified Normalized Difference Wetness Index (MNDWI)"
            " Threshold\n"
            "            between 0.00 and 2.00\n"
            "            (Defaults - L4-7 %0.3f, L8 %0.3f)\n",
            wigt_l47_default, wigt_l8_default);
    printf ("    --awgt: Automated Water Extent Shadow Threshold\n"
            "            between -2.00 and 2.00\n"
            "            (Defaults - L4-7 %0.3f, L8 %0.3f)\n",
            awgt_l47_default, awgt_l8_default);

    printf ("    --pswt_1_mndwi: Partial Surface Water Test-1 MNDWI Threshold\n"
            "                    between -2.00 and 2.00\n"
            "                    (Defaults - L4-7 %0.3f, L8 %0.3f)\n",
            pswt_1_mndwi_l47_default, pswt_1_mndwi_l8_default);
    printf ("    --pswt_1_nir: Partial Surface Water Test-1 NIR Threshold\n"
            "                  between 0 and data maximum\n"
            "                  (Defaults - L4-7 %d, L8 %d)\n",
            pswt_1_nir_l47_default, pswt_1_nir_l8_default);
    printf ("    --pswt_1_swir1: Partial Surface Water Test-1 SWIR1 Threshold\n"
            "                    between 0 and data maximum\n"
            "                    (Defaults - L4-7 %d, L8 %d)\n",
            pswt_1_swir1_l47_default, pswt_1_swir1_l8_default);
    printf ("    --pswt_1_ndvi: Partial Surface Water Test-1 NDVI Threshold\n"
            "                   between 0.00 and 2.00\n"
            "                   (Defaults - L4-7 %0.3f, L8 %0.3f)\n",
            pswt_1_ndvi_l47_default, pswt_1_ndvi_l8_default);

    printf ("    --pswt_2_mndwi: Partial Surface Water Test-2 MNDWI Threshold\n"
            "                    between -2.00 and 2.00\n"
            "                    (Defaults - L4-7 %0.3f, L8 %0.3f)\n",
            pswt_2_mndwi_l47_default, pswt_2_mndwi_l8_default);
    printf ("    --pswt_2_blue: Partial Surface Water Test-2 Blue Threshold\n"
            "                   between 0 and data maximum\n"
            "                   (Defaults - L4-7 %d, L8 %d)\n",
            pswt_2_blue_l47_default, pswt_2_blue_l8_default);
    printf ("    --pswt_2_nir: Partial Surface Water Test-2 NIR Threshold\n"
            "                  between 0 and data maximum\n"
            "                  (Defaults - L4-7 %d, L8 %d)\n",
            pswt_2_nir_l47_default, pswt_2_nir_l8_default);
    printf ("    --pswt_2_swir1: Partial Surface Water Test-1 SWIR1 Threshold\n"
            "                    between 0 and data maximum\n"
            "                    (Defaults - L4-7 %d, L8 %d)\n",
            pswt_1_swir1_l47_default, pswt_1_swir1_l8_default);
    printf ("    --pswt_2_swir2: Partial Surface Water Test-2 SWIR2 Threshold\n"
            "                    between 0 and data maximum\n"
            "                    (Defaults - L4-7 %d, L8 %d)\n",
            pswt_2_swir2_l47_default, pswt_2_swir2_l8_default);

    printf ("    --percent_slope_high: Threshold between 0.00 and 100.00\n"
            "                          (default - %0.1f)\n", 
                                       percent_slope_high_default);
    printf ("    --percent_slope_moderate: Threshold between 0.00 and 100.00\n"
            "                              (default - %0.1f)\n", 
                                           percent_slope_moderate_default);
    printf ("    --percent_slope_wetland: Threshold between 0.00 and 100.00\n"
            "                             (default - %0.1f)\n", 
                                          percent_slope_wetland_default);
    printf ("    --percent_slope_low: Threshold between 0.00 and 100.00\n"
            "                         (default - %0.1f)\n", 
                                      percent_slope_low_default);
    printf ("    --hillshade: Threshold between 0 and 255\n"
            "                 (default - %d)\n", hillshade_default);

    printf ("    --include_tests: Should the diagnostic band be included in"
            " output?\n"
            "                  (default is false)\n");
    printf ("    --include_ps: Should percent slope be included in output?\n"
            "                  (default is false)\n");
    printf ("    --include_hs: Should hillshade be included in output?\n"
            "                  (default is false)\n");
    printf ("    --use_zeven_thorne: Should Zevenbergen&Thorne's slope"
            " algorithm be used?\n"
            "                        (default is false, meaning Horn's slope"
            " algorithm will\n"
            "                        be used)\n"
            "                        Output using zeven_thorne has *NOT* been"
            " validated.\n");

    printf ("    --use_toa: Should Top of Atmosphere be used instead of"
            " Surface Reflectance\n"
            "               (default is false, meaning Surface Reflectance"
            " will be used)\n"
            "               Also default parameters are tailored to Surface"
            " Reflectance.\n");

    printf ("    --verbose: Should intermediate messages be printed? (default"
            " is false)\n");
    printf ("    --version: Display application version number\n\n");

    printf ("    --help: prints this usage statement\n\n");
    printf ("Example: dswe"
            " --xml LT04_L1TP_035027_19890712_20161001_01_T1.xml\n");
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
    char **xml_filename,         /* O: input XML filename */
    Espa_internal_meta_t *xml_metadata, /* O: input metadata */
    bool *use_zeven_thorne_flag, /* O: use zeven thorne */
    bool *use_toa_flag,          /* O: process using TOA */
    bool *include_tests_flag,    /* O: include raw DSWE with output */
    bool *include_ps_flag,       /* O: include percent slope with output */
    bool *include_hs_flag,       /* O: include hillshade with output */
    float *wigt,                 /* O: tolerance value */
    float *awgt,                 /* O: tolerance value */
    float *pswt_1_mndwi,         /* O: tolerance value */
    int *pswt_1_nir,             /* O: tolerance value */
    int *pswt_1_swir1,           /* O: tolerance value */
    float *pswt_1_ndvi,          /* O: tolerance value */
    float *pswt_2_mndwi,         /* O: tolerance value */
    int *pswt_2_blue,            /* O: tolerance value */
    int *pswt_2_nir,             /* O: tolerance value */
    int *pswt_2_swir1,           /* O: tolerance value */
    int *pswt_2_swir2,           /* O: tolerance value */
    float *percent_slope_high,   /* O: slope tolerance for high confidence 
                                       water */
    float *percent_slope_moderate, /* O: slope tolerance for moderate 
                                       confidence water */
    float *percent_slope_wetland, /* O: slope tolerance for potential wetland */
    float *percent_slope_low,    /* O: slope tolerance for low confidence 
                                       water or wetland */
    int *hillshade,              /* O: hillshade tolerance value */ 
    bool *verbose_flag           /* O: verbose messaging */
)
{
    int c;
    int option_index;
    char msg[256];
    int tmp_zeven_thorne_flag = false;
    int tmp_toa_flag = false;
    int tmp_verbose_flag = false;
    int tmp_include_tests_flag = false;
    int tmp_include_ps_flag = false;
    int tmp_include_hs_flag = false;

    struct option long_options[] = {
        /* These options set a flag */
        {"use_zeven_thorne", no_argument, &tmp_zeven_thorne_flag, true},
        {"use_toa", no_argument, &tmp_toa_flag, true},

        {"include_tests", no_argument, &tmp_include_tests_flag, true},
        {"include_ps", no_argument, &tmp_include_ps_flag, true},
        {"include_hs", no_argument, &tmp_include_hs_flag, true},

        /* These options provide values */
        {"xml", required_argument, 0, 'x'},

        {"wigt", required_argument, 0, 'w'},
        {"awgt", required_argument, 0, 'a'},

        {"pswt_1_mndwi", required_argument, 0, 'z'},
        {"pswt_1_nir", required_argument, 0, 'n'},
        {"pswt_1_swir1", required_argument, 0, 'r'},
        {"pswt_1_ndvi", required_argument, 0, 'y'},

        {"pswt_2_mndwi", required_argument, 0, 'q'},
        {"pswt_2_blue", required_argument, 0, 'b'},
        {"pswt_2_nir", required_argument, 0, 'i'},
        {"pswt_2_swir1", required_argument, 0, '1'},
        {"pswt_2_swir2", required_argument, 0, '2'},

        {"percent_slope_high", required_argument, 0, 'g'},
        {"percent_slope_moderate", required_argument, 0, 'm'},
        {"percent_slope_wetland", required_argument, 0, 'd'},
        {"percent_slope_low", required_argument, 0, 'l'},
        {"hillshade", required_argument, 0, 's'},

        /* Special options */
        {"verbose", no_argument, &tmp_verbose_flag, true},
        {"version", no_argument, 0, 'v'},

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

    /* Initialize to the not set values */
    *wigt = NOT_SET;
    *awgt = NOT_SET;
    *pswt_1_mndwi = NOT_SET;
    *pswt_1_nir = NOT_SET;
    *pswt_1_swir1 = NOT_SET;
    *pswt_1_ndvi = NOT_SET;
    *pswt_2_mndwi = NOT_SET;
    *pswt_2_blue = NOT_SET;
    *pswt_2_nir = NOT_SET;
    *pswt_2_swir1 = NOT_SET;
    *pswt_2_swir2 = NOT_SET;
    *percent_slope_high = NOT_SET;
    *percent_slope_moderate = NOT_SET;
    *percent_slope_wetland = NOT_SET;
    *percent_slope_low = NOT_SET;
    *hillshade = NOT_SET;

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
            exit (SUCCESS);
            break;

        case 'v':
            version ();
            exit (SUCCESS);
            break;

        case 'x':
            *xml_filename = strdup (optarg);
            break;

        case 'w':
            *wigt = atof (optarg);
            break;
        case 'a':
            *awgt = atof (optarg);
            break;

        case 'z':
            *pswt_1_mndwi = atof (optarg);
            break;
        case 'n':
            *pswt_1_nir = atof (optarg);
            break;
        case 'r':
            *pswt_1_swir1 = atoi (optarg);
            break;
        case 'y':
            *pswt_1_ndvi = atof (optarg);
            break;

        case 'q':
            *pswt_2_mndwi = atof (optarg);
            break;
        case 'b':
            *pswt_2_blue = atoi (optarg);
            break;
        case 'i':
            *pswt_2_nir = atoi (optarg);
            break;
        case '1':
            *pswt_2_swir1 = atoi (optarg);
            break;
        case '2':
            *pswt_2_swir2 = atoi (optarg);
            break;

        case 'g':
            *percent_slope_high = atof (optarg);
            break;
        case 'm':
            *percent_slope_moderate = atof (optarg);
            break;
        case 'd':
            *percent_slope_wetland = atof (optarg);
            break;
        case 'l':
            *percent_slope_low = atof (optarg);
            break;

        case 's':
            *hillshade = atoi (optarg);
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

    if (tmp_include_tests_flag)
        *include_tests_flag = true;
    else
        *include_tests_flag = false;

    if (tmp_include_ps_flag)
        *include_ps_flag = true;
    else
        *include_ps_flag = false;

    if (tmp_include_hs_flag)
        *include_hs_flag = true;
    else
        *include_hs_flag = false;

    if (tmp_verbose_flag)
        *verbose_flag = true;
    else
        *verbose_flag = false;

    /* Make sure the XML was specified */
    if (*xml_filename == NULL)
    {
        ERROR_MESSAGE ("XML input file is a required command line"
                       " argument\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Validate the input XML metadata file */
    if (validate_xml_file (*xml_filename) != SUCCESS)
    {
        /* Error messages already written */
        return ERROR;
    }

    /* Initialize the metadata structure */
    init_metadata_struct (xml_metadata);

    /* Parse the metadata file into our internal metadata structure; also
       allocates space as needed for various pointers in the global and band
       metadata */
    if (parse_metadata (*xml_filename, xml_metadata) != SUCCESS)
    {
        /* Error messages already written */
        return ERROR;
    }

    /* Assign the default values if not provided on the command line */
    if (strcmp(xml_metadata->global.satellite, "LANDSAT_8") == 0)
    {
        if (*wigt == NOT_SET)
            *wigt = wigt_l8_default;

        if (*awgt == NOT_SET)
            *awgt = awgt_l8_default;

        if (*pswt_1_mndwi == NOT_SET)
            *pswt_1_mndwi = pswt_1_mndwi_l8_default;

        if (*pswt_1_nir == NOT_SET)
            *pswt_1_nir = pswt_1_nir_l8_default;

        if (*pswt_1_swir1 == NOT_SET)
            *pswt_1_swir1 = pswt_1_swir1_l8_default;

        if (*pswt_1_ndvi == NOT_SET)
            *pswt_1_ndvi = pswt_1_ndvi_l8_default;

        if (*pswt_2_mndwi == NOT_SET)
            *pswt_2_mndwi = pswt_2_mndwi_l8_default;

        if (*pswt_2_blue == NOT_SET)
            *pswt_2_blue = pswt_2_blue_l8_default;

        if (*pswt_2_nir == NOT_SET)
            *pswt_2_nir = pswt_2_nir_l8_default;

        if (*pswt_2_swir1 == NOT_SET)
            *pswt_2_swir1 = pswt_2_swir1_l8_default;

        if (*pswt_2_swir2 == NOT_SET)
            *pswt_2_swir2 = pswt_2_swir2_l8_default;
    }
    else
    {
        if (*wigt == NOT_SET)
            *wigt = wigt_l47_default;

        if (*awgt == NOT_SET)
            *awgt = awgt_l47_default;

        if (*pswt_1_mndwi == NOT_SET)
            *pswt_1_mndwi = pswt_1_mndwi_l47_default;

        if (*pswt_1_nir == NOT_SET)
            *pswt_1_nir = pswt_1_nir_l47_default;

        if (*pswt_1_swir1 == NOT_SET)
            *pswt_1_swir1 = pswt_1_swir1_l47_default;

        if (*pswt_1_ndvi == NOT_SET)
            *pswt_1_ndvi = pswt_1_ndvi_l47_default;

        if (*pswt_2_mndwi == NOT_SET)
            *pswt_2_mndwi = pswt_2_mndwi_l47_default;

        if (*pswt_2_blue == NOT_SET)
            *pswt_2_blue = pswt_2_blue_l47_default;

        if (*pswt_2_nir == NOT_SET)
            *pswt_2_nir = pswt_2_nir_l47_default;

        if (*pswt_2_swir1 == NOT_SET)
            *pswt_2_swir1 = pswt_2_swir1_l47_default;

        if (*pswt_2_swir2 == NOT_SET)
            *pswt_2_swir2 = pswt_2_swir2_l47_default;
    }

    if (*percent_slope_high == NOT_SET)
        *percent_slope_high = percent_slope_high_default;
    if (*percent_slope_moderate == NOT_SET)
        *percent_slope_moderate = percent_slope_moderate_default;
    if (*percent_slope_wetland == NOT_SET)
        *percent_slope_wetland = percent_slope_wetland_default;
    if (*percent_slope_low == NOT_SET)
        *percent_slope_low = percent_slope_low_default;

    if (*hillshade == NOT_SET)
        *hillshade = hillshade_default;


    /* ---------- Validate the parameters ---------- */
    if ((*wigt < 0.0) || (*wigt > 2.0))
    {
        ERROR_MESSAGE ("WIGT is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*awgt < -2.0) || (*awgt > 2.0))
    {
        ERROR_MESSAGE ("AWGT is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*pswt_1_mndwi < -2.0) || (*pswt_1_mndwi > 2.0))
    {
        ERROR_MESSAGE ("PSWT_1_MNDWI is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if (*pswt_1_nir < 0)
    {
        ERROR_MESSAGE ("PSWT_1_NIR is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if (*pswt_1_swir1 < 0)
    {
        ERROR_MESSAGE ("PSWT_1_SWIR1 is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*pswt_1_ndvi < 0.0) || (*pswt_1_ndvi > 2.0))
    {
        ERROR_MESSAGE ("PSWT_1_NDVI is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }


    if ((*pswt_2_mndwi < -2.0) || (*pswt_2_mndwi > 2.0))
    {
        ERROR_MESSAGE ("PSWT_2_MNDWI is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if (*pswt_2_blue < 0)
    {
        ERROR_MESSAGE ("PSWT_2_BLUE is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if (*pswt_2_nir < 0)
    {
        ERROR_MESSAGE ("PSWT_2_NIR is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if (*pswt_2_swir1 < 0)
    {
        ERROR_MESSAGE ("PSWT_2_SWIR1 is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if (*pswt_2_swir2 < 0)
    {
        ERROR_MESSAGE ("PSWT_2_SWIR2 is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*percent_slope_high < 0.0) || (*percent_slope_high > 100.0))
    {
        ERROR_MESSAGE ("Percent Slope high is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*percent_slope_moderate < 0.0) || (*percent_slope_moderate > 100.0))
    {
        ERROR_MESSAGE ("Percent Slope moderate is out of range\n\n", 
            MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*percent_slope_wetland < 0.0) || (*percent_slope_wetland > 100.0))
    {
        ERROR_MESSAGE ("Percent Slope wetland is out of range\n\n", 
            MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*percent_slope_low < 0.0) || (*percent_slope_low > 100.0))
    {
        ERROR_MESSAGE ("Percent Slope low is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*hillshade < 0) || (*hillshade > 255))
    {
        ERROR_MESSAGE ("Hillshade threshold is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    return SUCCESS;
}
