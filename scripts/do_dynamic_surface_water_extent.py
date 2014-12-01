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

    # Optional parameters
    default_wigt = '0.015'
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

    default_pswt = '0.0'
    parser.add_argument('--pswt',
                        action='store', dest='pswt', default=default_pswt,
                        help=("Partial Surface Water"
                              " Threshold between -2.00 and 2.00"
                              " (default value is {0})".format(default_pswt)))

    default_pswnt = '1500'
    parser.add_argument('--pswnt',
                        action='store', dest='pswnt', default=default_pswnt,
                        help=("Partial Surface Water NIR"
                              " Threshold between 0 and data maximum value"
                              " (default value is {0})".format(default_pswnt)))

    default_pswst = '1000'
    parser.add_argument('--pswst',
                        action='store', dest='pswst', default=default_pswst,
                        help=("Partial Surface Water SWIR1"
                              " Threshold between 0 and data maximum value"
                              " (default value is {0})".format(default_pswst)))

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
    cmd = ['dswe', '--xml', args.xml_filename,
           '--wigt', args.wigt,
           '--awgt', args.awgt,
           '--pswt', args.pswt,
           '--pswnt', args.pswnt,
           '--pswst', args.pswst]

    if args.verbose:
        cmd.append('--verbose')

    # Convert to a string
    cmd = ' '.join(cmd)

    logger.info("Executing [{0}]".format(cmd))

    try:
        execute_cmd(cmd)
    except Exception, e:
        logger.fatal(str(e))
        return EXIT_FAILURE

    return EXIT_SUCCESS
