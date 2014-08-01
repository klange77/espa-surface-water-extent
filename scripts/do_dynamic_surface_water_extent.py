#! /usr/bin/env python

'''
Created on January 29, 2013 by Song Guo, USGS/EROS

License:
  "NASA Open Source Agreement 1.3"

Description:
  Run the scene-based surface water algorithm, including checking processing
  status.

Usage:
  do_surface_water_extent.py --help prints the help message
'''

import sys
import os
import re
import subprocess
import datetime
from optparse import OptionParser

from espa_constants import *
from espa_logging import log

class Swe():
    '''
    Defines the class object for performing surface water extent processing
    '''

    def __init__(self):
        pass

    def runSwe (self, metafile=None, reflectance=None, dem=None,
                mgt=None, mlt1=None, mlt2=None, b4lt1=None, b4lt2=None,
                b5lt1=None, b5lt2=None, per_shade=None, write_binary=None,
                use_ledaps_mask=None, use_zeven_thorne=None, verbose=None,
                logfile=None, usebin=None):
        """
        Description:
          runSwe will use the parameters passed for the input/output files,
          logfile, and usebin.  If input/output files are None (i.e. not
          specified) then the command-line parameters will be parsed for this
          information.  The scene-based DEM and SWE applications are then
          executed on the specified input files.  If a log file was specified,
          then the output from each application will be logged to that file.

        Inputs:
          metafile - name of the Landsat metadata file to be processed
          reflectance - name of the input TOA reflectance HDF file to be
              processed
          dem - name of an ENVI formatted DEM file
          mgt, mlt1, mlt2, b4lt1, b4lt2, b5lt1, b5lt2, per_shade -
              these are threashold values used in the SWE algorithm
          logfile - name of the logfile for logging information; if None then
              the output will be written to stdout
          usebin - this specifies if the DEM and SCE exes reside in the $BIN
              directory; if None then the DEM and SCA exes are expected to be
              in the PATH

        Returns:
            ERROR - error running the DEM or SWE application
            SUCCESS - successful processing

        Notes:
          1. The script obtains the path of the metadata file and changes
             directory to that path for running the scene-based DEM and SCA
             code.  If the metafile directory is not writable, then this
             script exits with an error.
          2. If the metadata file is not specified and the information is
             going to be grabbed from the command line, then it's assumed all
             the parameters will be pulled from the command line.
          3. Currently the raw binary products are automatically written in
             addition to the HDF-EOS product for the SWE algorithm.  The raw
             binary products will be used for downstream processing, but only
             the output HDF-EOS product should be delivered to the general
             public.
        """

        # define a parser to read arguments from the command line
        parser = OptionParser()

        # specify the command line agruments to read
        parser.add_option ('-f', '--metafile', type='string', dest='metafile',
            help="name of Landsat MTL file", metavar='FILE')

        parser.add_option ('-r', '--reflectance', type='string',
            dest='reflectance',
            help="name of SR or TOA reflectance HDF file", metavar='FILE')

        parser.add_option ('--dem', type='string',
            dest='dem',
            help="name of an ENVI formatted DEM file", metavar='FILE')

        parser.add_option('--mgt', dest='mgt',
            action='store', default=0.123, help="mgt value")

        parser.add_option('--mlt1', dest='mlt1',
            action='store', default=-0.5, help="mlt1 value")

        parser.add_option('--mlt2', dest='mlt2',
            action='store', default=-0.4, help="mlt2 value")

        parser.add_option('--b4lt1', dest='b4lt1',
            action='store', default=1500, help="b4lt1 value")

        parser.add_option('--b4lt2', dest='b4lt2',
            action='store', default=1500, help="b4lt2 value")

        parser.add_option('--b5lt1', dest='b5lt1',
            action='store', default=1000, help="b5lt1 value")

        parser.add_option('--b5lt2', dest='b5lt2',
            action='store', default=1700, help="b5lt2 value")

        parser.add_option('--per_shade', dest='per_shade',
            action='store', default=3.0, help="percent shade value")

        parser.add_option ('--usebin', dest='usebin', default=False,
            action='store_true',
            help="use BIN environment variable to find DEM and SCA apps")

        parser.add_option ('-l', '--logfile', type='string', dest='logfile',
            help="name of optional log file", metavar='FILE')

        parser.add_option('-b', '--write_binary',
            action='store_true', dest='write_binary', default=True,
            help="don't output results to binary file")

        parser.add_option('-u', '--use_ledaps_mask',
            action='store_false', dest='use_ledaps_mask', default=False,
            help="use ledaps cloud/shadow results")

        parser.add_option('-z', '--use_zeven_thorne', action='store_false',
            dest='use_zeven_thorne', default=False,
            help="use Zevenburgen Thorne slope algorithm")

        parser.add_option('-q', '--quiet', action='store_true', dest='verbose',
            default=True, help="don't print status messages to stdout")

        # read any command line arguments
        (options, args) = parser.parse_args()
    
        # if not provided through the method arguments grab the value from the
        # command line options which will be populated with the default value
        # if specified

        if usebin == None:
            usebin = options.usebin

        if logfile == None:
            logfile = options.logfile

        if write_binary == None:
            write_binary = options.write_binary

        if use_ledaps_mask == None:
            use_ledaps_mask = options.use_ledaps_mask

        if use_zeven_thorne == None:
            use_zeven_thorne = options.use_zeven_thorne

        if verbose == None:
            verbose = options.verbose

        if mgt == None:
            mgt = options.mgt

        if mlt1 == None:
            mlt1 = options.mlt1 

        if mlt2 == None:
            mlt2 = options.mlt2 

        if b4lt1 == None:
            b4lt1 = options.b4lt1 

        if b4lt2 == None:
            b4lt2 = options.b4lt2 

        if b5lt1 == None:
            b5lt1 = options.b5lt1 

        if b5lt2 == None:
            b5lt2 = options.b5lt2 

        if per_shade == None:
            per_shade = options.per_shade 

        ##### Verify that required parameters are present #####

        # metadata file
        if metafile == None:
            metafile = options.metafile
            if metafile == None:
                parser.error ("missing metafile command-line argument");
                return ERROR

        # SR or TOA reflectance file
        if reflectance == None:
            reflectance = options.reflectance
            if reflectance == None:
                parser.error ("missing input reflectance file command-line" \
                    " argument");
                return ERROR
                       
        # ENVI DEM file
        if dem == None:
            dem = options.dem
            if dem == None:
                parser.error ("missing input dem file command-line" \
                    " argument");
                return ERROR
                       
        # let the world know what we are processing
        msg = "SWE processing of LEDAPS reflectance file: %s" % reflectance
        log (msg)
        
        # should we expect the DEM and SWE applications to be in the PATH or
        # in the BIN directory?
        if usebin:
            # get the BIN dir environment variable
            bin_dir = os.environ.get('BIN') + '/'
            msg = "BIN environment variable: %s" % bin_dir
            log (msg)
        else:
            # don't use a path to the DEM/SWE applications
            bin_dir = ''
            msg = "DEM and SWE executables expected to be in the PATH"
            log (msg)
        
        # make sure the metadata file exists
        if not os.path.isfile(metafile):
            msg = "Error: metadata file does not exist or is not" \
                " accessible: " + metafile
            log (msg)
            return ERROR

        # use the base metadata filename and not the full path.
        base_metafile = os.path.basename (metafile)
        msg = "Processing metadata file: %s" % base_metafile
        log (msg)
        
        # get the path of the MTL file and change directory to that location
        # for running this script.  save the current working directory for
        # return to upon error or when processing is complete.  Note: use
        # abspath to handle the case when the filepath is just the filename
        # and doesn't really include a file path (i.e. the current working
        # directory).
        mydir = os.getcwd()
        metadir = os.path.dirname (os.path.abspath (metafile))
        if not os.access(metadir, os.W_OK):
            msg = "Path of metadata file is not writable: %s.  Script needs" \
                " write access to the metadata directory." % metadir
            log (msg)
            return ERROR

        msg = "Changing directories for SWE processing: %s" % metadir
        log (msg)
        os.chdir (metadir)

        # run SWE algorithm, checking the return status of each module.
        # exit if any errors occur.
        cmd = ['%sscene_based_swe' % bin_dir,
               '--reflectance=%s' % reflectance,
               '--dem=%s' % dem,
               '--mgt=%f' % mgt,
               '--mlt1=%f' % mlt1,
               '--mlt2=%f' % mlt2,
               '--b4lt1=%d' % b4lt1,
               '--b4lt2=%d' % b4lt2,
               '--b5lt1=%d' % b5lt1,
               '--b5lt2=%d' % b5lt2,
               "--per_shade=%f" % per_shade,
               "--verbose"]

        if write_binary:
            cmd = cmd + ['--write_binary']

        print "Executing: scene_based_swe command: %s" % ' '.join(cmd)
        try:
            (output) = subprocess.check_output (cmd)
        except subprocess.CalledProcessError, e:
            msg = "Error running scene_based_swe.  Processing will terminate."
            log (msg)
            os.chdir (mydir)
            return ERROR

        log (output)
            
        # successful completion.  return to the original directory.
        os.chdir (mydir)
        msg = "Completion of scene based surface water extent."
        log (msg)

        return SUCCESS
    #### End of runSwe ####
#### End of Swe class ####


if __name__ == '__main__':
    return_value = Swe().runSwe()

    if (return_value != SUCCESS):
        sys.exit (EXIT_FAILURE)
    else:
        sys.exit (EXIT_SUCCESS)

