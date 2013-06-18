#! /usr/bin/env python
import sys
import os
import re
import commands
import datetime
from optparse import OptionParser
from create_dem import SceneDEM

ERROR = 1
SUCCESS = 0

############################################################################
# Description: logIt logs the information to the logfile (if valid) or to
# stdout if the logfile is None.
#
# Inputs:
#   msg - message to be printed/logged
#   log_handler - log file handler; if None then print to stdout
#
# Returns: nothing
#
# Notes:
############################################################################
def logIt (msg, log_handler):
    if log_handler == None:
        print msg
    else:
        log_handler.write (msg + '\n')


#############################################################################
# Created on January 29, 2013 by Song Guo, USGS/EROS
# Created Python script to run the scene-based DEM and scene-based surface
# water algorithms in one shot, including checking processing status.
##
# Usage: do_water_extent.py --help prints the help message
############################################################################
class Swe():

    def __init__(self):
        pass


    ########################################################################
    # Description: runSwe will use the parameters passed for the input/output
    # files, logfile, and usebin.  If input/output files are None (i.e. not
    # specified) then the command-line parameters will be parsed for this
    # information.  The scene-based DEM and snow cover applications are then
    # executed on the specified input files.  If a log file was specified,
    # then the output from each application will be logged to that file.
    #
    # Inputs:
    #   metafile - name of the Landsat metadata file to be processed
    #   toa_infile - name of the input TOA reflectance HDF file to be processed
    #   btemp_infile - name of the input brightness temp HDF file to
    #       be processed
    #   sca_outfile - name of the output snow cover HDF file
    #   logfile - name of the logfile for logging information; if None then
    #       the output will be written to stdout
    #   usebin - this specifies if the DEM and SCE exes reside in the $BIN
    #       directory; if None then the DEM and SCA exes are expected to be in
    #       the PATH
    #
    # Returns:
    #     ERROR - error running the DEM and SCA applications
    #     SUCCESS - successful processing
    #
    # Notes:
    #   1. The script obtains the path of the metadata file and changes
    #      directory to that path for running the scene-based DEM and SCA
    #      code.  If the metafile directory is not writable, then this script
    #      exits with an error.
    #   2. If the metadata file is not specified and the information is going
    #      to be grabbed from the command line, then it's assumed all the
    #      parameters will be pulled from the command line.
    #   3. Currently the raw binary products are automatically written in
    #      addition to the HDF-EOS product for the snow cover algorithm.
    #      The raw binary products will be used for downstream processing,
    #      but only the output HDF-EOS product should be delivered to the
    #      general public.
    #######################################################################
    def runSwe (self, metafile=None, reflectance=None, mgt=None, 
                mlt1=None, mlt2=None, b4t1=None, b4t2=None, b5t1=None, 
                b5t2=None, per_slope=None, write_binary=None, use_fmask=None, 
                verbose=None, logfile=None, usebin=None):
        # if no parameters were passed then get the info from the
        # command line, then get the command line argument for the metadata file
        parser = OptionParser()

        if metafile == None:
            parser.add_option ("-f", "--metafile", type="string",
                dest="metafile",
                help="name of Landsat MTL file", metavar="FILE")
        if reflectance == None:
            parser.add_option ("-r", "--reflectance", type="string",
                dest="reflectance",
                help="name of SR or TOA reflectance HDF file", metavar="FILE")
        if mgt == None:
            parser.add_option("--mgt", dest="mgt",
                      action="store", default=0.123,
                      help="mgt value")
        if mlt1 == None:
            parser.add_option("--mlt1", dest="mlt1",
                      action="store", default=-0.5,
                      help="mlt1 value")
        if mlt2 == None:
            parser.add_option("--mlt2", dest="mlt2",
                      action="store", default=-0.4,
                      help="mlt2 value")
        if b4t1 == None:
            parser.add_option("--b4t1", dest="b4t1",
                      action="store", default=1500,
                      help="b4t1 value")
        if b4t2 == None:
            parser.add_option("--b4t2", dest="b4t2",
                      action="store", default=1500,
                      help="b4t2 value")
        if b5t1 == None:
            parser.add_option("--b5t1", dest="b5t1",
                      action="store", default=1000,
                      help="b5t1 value")
        if b5t2 == None:
            parser.add_option("--b5t2", dest="b5t2",
                      action="store", default=1700,
                      help="b5t2 value")
        if per_slope == None:
            parser.add_option("--per_slope", dest="per_slope",
                      action="store", default=3.0,
                      help="percent slope value")
        if usebin == None:
            parser.add_option ("--usebin", dest="usebin", default=False,
                action="store_true",
                help="use BIN environment variable as the location of DEM and SCA apps")
        if logfile == None:
            parser.add_option ("-l", "--logfile", type="string", dest="logfile",
                help="name of optional log file", metavar="FILE")
        if write_binary == None:
            parser.add_option("-b", "--write_binary",
                  action="store_false", dest="write_binary", default=True,
                  help="don't output results to binary file")
        if use_fmask == None:
            parser.add_option("-u", "--use_fmask",
                  action="store_false", dest="use_fmask", default=True,
                  help="use fmask cloud/shadow results")
        if verbose == None:
            parser.add_option("-q", "--quiet",
                  action="store_false", dest="verbose", default=True,
                  help="don't print status messages to stdout")

            (options, args) = parser.parse_args()
    
        # validate the command-line options
        if usebin == None:
            usebin = options.usebin          # should $BIN directory be used
        if logfile == None:
            logfile = options.logfile        # name of the log file
        if write_binary == None:
            write_binary = options.write_binary
        if use_fmask == None:
            use_fmask = options.use_fmask
        if verbose == None:
            verbose = options.verbose
        if mgt == None:
            mgt = options.mgt
        if mlt1 == None:
            mlt1 = options.mlt1 
        if mlt2 == None:
            mlt2 = options.mlt2 
        if b4t1 == None:
            b4t1 = options.b4t1 
        if b4t2 == None:
            b4t2 = options.b4t2 
        if b5t1 == None:
            b5t1 = options.b5t1 
        if b5t2 == None:
            b5t2 = options.b5t2 
        if per_slope == None:
            per_slope = options.per_slope 

            print 'mgt=%f\n' % mgt 

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
                parser.error ("missing input reflectance file command-line argument");
                return ERROR
                       
        # open the log file if it exists; use line buffering for the output
        log_handler = None
        if logfile != None:
            log_handler = open (logfile, 'w', buffering=1)
        msg = 'SWE processing of LEDAPS reflectance file: %s' % reflectance
        logIt (msg, log_handler)
        
        # should we expect the DEM and SWE applications to be in the PATH or
        # in the BIN directory?
        if usebin:
            # get the BIN dir environment variable
            bin_dir = os.environ.get('BIN')
            bin_dir = bin_dir + '/'
            msg = 'BIN environment variable: %s' % bin_dir
            logIt (msg, log_handler)
        else:
            # don't use a path to the DEM/SWE applications
            bin_dir = ""
            msg = 'DEM and SWE executables expected to be in the PATH'
            logIt (msg, log_handler)
        
        # make sure the metadata file exists
        if not os.path.isfile(metafile):
            msg = "Error: metadata file does not exist or is not accessible: " + metafile
            logIt (msg, log_handler)
            return ERROR

        # use the base metadata filename and not the full path.
        base_metafile = os.path.basename (metafile)
        msg = 'Processing metadata file: %s' % base_metafile
        logIt (msg, log_handler)
        
        # get the path of the MTL file and change directory to that location
        # for running this script.  save the current working directory for
        # return to upon error or when processing is complete.  Note: use
        # abspath to handle the case when the filepath is just the filename
        # and doesn't really include a file path (i.e. the current working
        # directory).
        mydir = os.getcwd()
        metadir = os.path.dirname (os.path.abspath (metafile))
        if not os.access(metadir, os.W_OK):
            msg = 'Path of metadata file is not writable: %s.  Script needs write access to the metadata directory.' % metadir
            logIt (msg, log_handler)
            return ERROR
        msg = 'Changing directories for snow cover processing: %s' % metadir
        logIt (msg, log_handler)
        os.chdir (metadir)

        # instantiate the SceneDEM class for use and create the scene-based
        # DEM for use with the snow cover
        dem = SceneDEM ()
        status = dem.createDEM (base_metafile, logfile, log_handler, usebin)
        if status != SUCCESS:
            msg = 'Error creating scene-based DEM.  Processing will terminate.'
            logIt (msg, log_handler)
            os.chdir (mydir)
            return ERROR

        # run water extent algorithm, checking the return status of each module.
        # exit if any errors occur.
        cmdstr = "%sscene_based_swe --reflectance=%s --dem=%s --mgt=%f --mlt1=%f --mlt2=%f --b4t1=%d --b4t2=%d --b5t1=%d --b5t2=%d --per_slope=%f --write_binary --use_fmask --verbose" % (bin_dir, reflectance, dem.scene_dem_envi, mgt, mlt1, mlt2, b4t1, b4t2, b5t1, b5t2, per_slope)
#        cmdstr = "%sscene_based_swe --reflectance=%s --dem=%s --write_binary --use_fmask --verbose")
        print 'Executing: scene_based_sca command: %s' % cmdstr
        (status, output) = commands.getstatusoutput (cmdstr)
        logIt (output, log_handler)
        exit_code = status >> 8
        if exit_code != 0:
            msg = 'Error running scene_based_sca.  Processing will terminate.'
            logIt (msg, log_handler)
            os.chdir (mydir)
            return ERROR
        
        # successful completion.  return to the original directory.
        os.chdir (mydir)
        msg = 'Completion of scene based snow cover.'
        logIt (msg, log_handler)
        if logfile != None:
            log_handler.close()
        return SUCCESS

######end of Swe class######

if __name__ == "__main__":
    sys.exit (Swe().runSwe())
