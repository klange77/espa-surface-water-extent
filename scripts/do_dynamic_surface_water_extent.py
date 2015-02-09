#! /usr/bin/env python

'''
    FILE: do_dynamic_surface_water_extent.py

    PURPOSE: Master script for running scene-based surface water algorithm.

    USAGE: See 'do_dynamic_surface_water_extent.py --help'

    LICENSE: NASA Open Source Agreement 1.3

    HISTORY:

    Date              Programmer               Reason
    ----------------  ------------------------ -------------------------------
    Jan/2013          Song Guo                 Initial Implementation
    Dec/2014          Ron Dilley               Complete Re-write
'''

import os
import sys
import commands
import logging
from argparse import ArgumentParser

from espa_constants import EXIT_SUCCESS
from espa_constants import EXIT_FAILURE


def execute_cmd(cmd):
    '''
    Description:
      Execute a command line and return the terminal output or raise an
      exception

    Returns:
        output - The stdout and/or stderr from the executed command.
    '''

    output = ''

    (status, output) = commands.getstatusoutput(cmd)

    if status < 0:
        message = "Application terminated by signal [%s]" % cmd
        if len(output) > 0:
            message = ' Stdout/Stderr is: '.join([message, output])
        raise Exception(message)

    if status != 0:
        message = "Application failed to execute [%s]" % cmd
        if len(output) > 0:
            message = ' Stdout/Stderr is: '.join([message, output])
        raise Exception(message)

    if os.WEXITSTATUS(status) != 0:
        message = "Application [%s] returned error code [%d]" \
                  % (cmd, os.WEXITSTATUS(status))
        if len(output) > 0:
            message = ' Stdout/Stderr is: '.join([message, output])
        raise Exception(message)

    return output


if __name__ == '__main__':
    '''
    Description:
        Execute the main code.
    '''

    # Create a command line argument parser
    description = ("Build the command line and then kick-off the Dynamic"
                   " Surface Water Extent application")
    parser = ArgumentParser(description=description)

    # ---- Add parameters ----
    # Required parameters
    parser.add_argument('--xml',
                        action='store', dest='xml_filename', required=True,
                        help="The XML metadata file to use")

    parser.add_argument('--dem',
                        action='store', dest='dem_filename', required=True,
                        help="The DEM metadata file to use")

    # Optional parameters
    default_wigt = '0.0123'
    parser.add_argument('--wigt',
                        action='store', dest='wigt', default=default_wigt,
                        help=("Modified Normalized Difference Wetness Index"
                              " Threshold between 0.00 and 2.00"
                              " (default value is {0})".format(default_wigt)))

    default_awgt = '0.0'
    parser.add_argument('--awgt',
                        action='store', dest='awgt', default=default_awgt,
                        help=("Automated Water Extent Shadow"
                              " Threshold between -2.00 and 2.00"
                              " (default value is {0})".format(default_awgt)))

    default_pswt_1 = '-0.05'
    parser.add_argument('--pswt_1',
                        action='store', dest='pswt_1',
                        default=default_pswt_1,
                        help=("Partial Surface Water Test-1"
                              " Threshold between -2.00 and 2.00"
                              " (default value is {0})"
                              .format(default_pswt_1)))
    default_pswt_2 = '-0.05'
    parser.add_argument('--pswt_2',
                        action='store', dest='pswt_2',
                        default=default_pswt_2,
                        help=("Partial Surface Water Test-2"
                              " Threshold between -2.00 and 2.00"
                              " (default value is {0})"
                              .format(default_pswt_2)))

    default_pswnt_1 = '1500'
    parser.add_argument('--pswnt_1',
                        action='store', dest='pswnt_1',
                        default=default_pswnt_1,
                        help=("Partial Surface Water Test-1 NIR"
                              " Threshold between 0 and data maximum value"
                              " (default value is {0})"
                              .format(default_pswnt_1)))
    default_pswnt_2 = '1700'
    parser.add_argument('--pswnt_2',
                        action='store', dest='pswnt_2',
                        default=default_pswnt_2,
                        help=("Partial Surface Water Test-2 NIR"
                              " Threshold between 0 and data maximum value"
                              " (default value is {0})"
                              .format(default_pswnt_2)))

    default_pswst_1 = '1000'
    parser.add_argument('--pswst_1',
                        action='store', dest='pswst_1',
                        default=default_pswst_1,
                        help=("Partial Surface Water Test-1 SWIR1"
                              " Threshold between 0 and data maximum value"
                              " (default value is {0})"
                              .format(default_pswst_1)))
    default_pswst_2 = '650'
    parser.add_argument('--pswst_2',
                        action='store', dest='pswst_2',
                        default=default_pswst_2,
                        help=("Partial Surface Water Test-2 SWIR2"
                              " Threshold between 0 and data maximum value"
                              " (default value is {0})"
                              .format(default_pswst_2)))

    default_percent_slope = '9.0'
    parser.add_argument('--percent_slope',
                        action='store', dest='percent_slope',
                        default=default_percent_slope,
                        help=("Percent Slope"
                              " Threshold between 0.0 and 100.0"
                              " (default value is {0})"
                              .format(default_percent_slope)))

    parser.add_argument('--verbose',
                        action='store_true', dest='verbose', default=False,
                        help=("Should intermediate messages be printed?"
                              " (default value is False)"))

    # Parse the command line parameters
    args = parser.parse_args()

    # setup the default logger format and level. log to STDOUT.
    logging.basicConfig(format=('%(asctime)s.%(msecs)03d %(process)d'
                                ' %(levelname)-8s'
                                ' %(filename)s:%(lineno)d:'
                                '%(funcName)s -- %(message)s'),
                        datefmt='%Y-%m-%d %H:%M:%S',
                        level=logging.INFO)

    # get the logger
    logger = logging.getLogger(__name__)

    # Build the command line
    cmd = ['dswe',
           '--xml', args.xml_filename,
           '--dem', args.dem_filename,
           # These include parameters are temporary
           '--include-tests', '--include-ps',
           '--wigt', args.wigt,
           '--awgt', args.awgt,
           '--pswt_1', args.pswt_1,
           '--pswt_2', args.pswt_2,
           '--pswnt_1', args.pswnt_1,
           '--pswnt_2', args.pswnt_2,
           '--pswst_1', args.pswst_1,
           '--pswst_2', args.pswst_2,
           '--percent-slope', args.percent_slope]

    if args.verbose:
        cmd.append('--verbose')

    # Convert to a string
    cmd = ' '.join(cmd)

    logger.info("Executing [{0}]".format(cmd))

    output = ''
    try:
        output = execute_cmd(cmd)

    except Exception, e:
        logger.exception("Error running DSWE.  Processing will terminate.")
        sys.exit(EXIT_FAILURE)

    finally:
        if len(output) > 0:
            logger.info("STDOUT/STDERR Follows: {0}".format(output))

    logger.info("Completion of DSWE")
    sys.exit(EXIT_SUCCESS)
