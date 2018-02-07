
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>


#include "const.h"
#include "cfmask_water_detection.h"
#include "utilities.h"
#include "get_args.h"


/*****************************************************************************
  NAME:  version

  PURPOSE:  Prints the version information for this application.

  RETURN VALUE:  None
*****************************************************************************/
void
version ()
{
    printf("%s version %s\n", CFWD_APP_NAME, CFWD_VERSION);
}


/*****************************************************************************
  NAME:  usage

  PURPOSE:  Displays the help/usage to the terminal.

  RETURN VALUE:  None
*****************************************************************************/
void
usage ()
{
    version();

    printf("CFmask based Water Detection\n"
           "Determines and adds Water Extent to the QA Band.\n\n");
    printf("usage: cfmask_water_detection"
           " --xml <input_xml_filename> [--help]\n\n");
    printf ("where the following parameters are required:\n");
    printf ("    --xml: Name of the input XML file which contains the top of"
            " atmosphere\n"
            "           files output from TOA processing in raw binary "
            "(envi) format\n\n");

    printf("where the following parameters are optional:\n");
    printf("    --verbose: Should intermediate messages be printed? (default"
           " is false)\n\n");

    printf("    --help will print this usage statement\n\n");
    printf("Example: cfmask_water_detection"
           " --xml LE07_L1TP_013033_20130330_20160908_01_T2.xml\n");
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
    int argc,          /* I: number of cmd-line args */
    char *argv[],      /* I: string of cmd-line args */
    char **xml_infile, /* O: input XML filename */
    bool *verbose_flag /* O: verbose messaging */
)
{
    int c;
    int option_index;
    char msg[256];
    int tmp_verbose_flag = false;

    struct option long_options[] = {
        /* These options provide values */
        {"xml", required_argument, 0, 'x'},

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
        ERROR_MESSAGE("Missing required command line arguments\n\n",
                      MODULE_NAME);
        usage();
        return ERROR;
    }

    /* loop through all the cmd-line options */
    opterr = 0; /* turn off getopt_long error msgs as we'll print our own */
    while (1)
    {
        c = getopt_long(argc, argv, "", long_options, &option_index);
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
            usage();
            exit(EXIT_SUCCESS);
            break;

        case 'v':
            version();
            exit(EXIT_SUCCESS);
            break;

        case 'x':
            *xml_infile = strdup(optarg);
            break;

        case '?':
        default:
            snprintf(msg, sizeof(msg),
                     "Unknown option %s\n\n", argv[optind - 1]);
            ERROR_MESSAGE(msg, MODULE_NAME);
            usage();
            return ERROR;
            break;
        }
    }

    /* Grab the boolean command line options */
    if (tmp_verbose_flag)
        *verbose_flag = true;
    else
        *verbose_flag = false;

    /* ---------- Validate the parameters ---------- */
    /* Make sure the XML was specified */
    if (*xml_infile == NULL)
    {
        ERROR_MESSAGE("XML input file is a required command line"
                      " argument\n\n", MODULE_NAME);
        usage();
        return ERROR;
    }

    return SUCCESS;
}
