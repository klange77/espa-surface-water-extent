# Script to generate maps using DSWE Prototype 3 Version 1 (P3V6)
#
# Foundational script written by Angria Baruah Written 10/01/14
# Modifed on 10/02/14 by John W. Jones to include the full DSWE model
#     (P2V6 specs) along with an additoinal new test to create P3V1.
# Modified on 11/13/14 by John W. Jones to use and output floating point MNDWI.
# Modfied on 01/20/15 by John W. Jones to incorporate cfmask cloud/cloud
#     shadow and percent slope "corrections"; to produce both diagnostic
#     output and ESPA provisional prodcut scaled data; to add documentation in
#     the form of comments and finally, to replace as many arcpy "PLUS"
#     operations as possible to make the code a little more efficient.
#
# Variable Declarations
#
# Input (data) variables
# B1 = Landsat TM or ETM band 1
# B2 = Landsat TM or ETM band 2
# B3 = Landsat TM or ETM band 3
# B4 = Landsat TM or ETM band 4
# B5 = Landsat TM or ETM band 5
# B7 = Landsat TM or ETM band 7
# perslp = percent slope calculated in ArcMAP from DEM. This file
#          (name perslp.img) should be placed in the ancillary directory of
#          the study area.
#
# Variables calculated within the program upon which decisions are made
#
# MNDWI = Modified Normalized Difference Wetness Index
# MBSRV = Multiband Spectral Relationship Visible
# MBSRN = Multiband Spectral Relationship Near-infrared
# AWEsh = Automated raster Extent Shadow
# PSW1 = Partial Surface Water 1
# PSW2 = Partial Surface Water 2
#
# Defaults are provided for the following thresholds / variable names
#
# MNDWI for first water test / WIGT
# AWEsh / AWGT
# MNDWI for PSW1 / PSW1_MNDWI
# MNDWI for PSW2 / PSW2_MNDWI
# PSW1 test band 4 / PSW1b4
# PSW1 test band 5 / PSW1b5
# PSw2 test band 4 / PSW2b4
# PSW2 test band 7 / PSW2b7
# Percent slope / PS


import os
import arcpy
from arcpy import env
from arcpy.sa import *
import csv


# Specify the user defined variables
Scale_Factor = 0.0001
WIGT = 0.0123
AWGT = 0
PSW1b4 = 1500
PSW1b5 = 1000
PSW1_MNDWI = -0.5
PSW2b4 = 1700
PSW2b7 = 650
PSW2_MNDWI = -0.5
AWE_param1 = 2.5
AWE_param2 = -1.5
AWE_param3 = -0.25
PS = 9.0
output_dir = 'G:/CA_P4342_R34/DSWE_P3_V3'


# Specify the path for input data
input_dir = 'G:/CA_P4342_R34/Bands/Band_2'
path_band_1 = 'G:/CA_P4342_R34/Bands/Band_1'
path_band_3 = 'G:/CA_P4342_R34/Bands/Band_3'
path_band_4 = 'G:/CA_P4342_R34/Bands/Band_4'
path_band_5 = 'G:/CA_P4342_R34/Bands/Band_5'
path_band_7 = 'G:/CA_P4342_R34/Bands/Band_7'
path_cfmask = 'G:/CA_P4342_R34/Bands/cfmask'
path_dem = 'G:/CA_P4342_R34/ancillary'
path_remap = 'G:/CA_P4342_R34/ancillary'


# Set the overwrite output environment
env.overwriteOutput = True

# Check out the ArcGIS Spatial Analyst extension license
arcpy.CheckOutExtension('spatial')

# Set the input environment for the Band_2 rasters
env.workspace = input_dir

# Get the list of Band_2 rasters from input environment
inputRaster = arcpy.ListRasters("*", "img")


# Get bands 1,3,4,5 and 7
for raster in inputRaster:

    # Define file names
    B1 = path_band_1 + '\%s_band_1.img' % (raster)[0:25]
    B3 = path_band_3 + '\%s_band_3.img' % (raster)[0:25]
    B4 = path_band_4 + '\%s_band_4.img' % (raster)[0:25]
    B5 = path_band_5 + '\%s_band_5.img' % (raster)[0:25]
    B7 = path_band_7 + '\%s_band_7.img' % (raster)[0:25]
    CF = path_cfmask + '\%s_cfmask.img' % (raster)[0:25]
    perslp = path_dem + '\perslp.img'
    espa_recode = path_remap + '\ESPA_recode.rmp'

    out_name = 'DSWE_P3_V3'
    # outfile = output_dir + out_name + str(threshold) + '.csv'

    # Step 1: Rescale Band2 and Band5
    RB2 = Times(raster, Scale_Factor)
    RB5 = Times(B5, Scale_Factor)

    # Step 2 Subtract Band 5 from Band 2
    outMinus = Minus(RB2, RB5)

    # Step 3 of MNDWI: Add Band 2 with Band 5
    outPlus = Plus(RB2, RB5)

    # Step 4 of MNDWI: Calculate the band ratio
    MNDWI = Divide(outMinus, outPlus)
    # MNDWI.save("G:/CA_P4342_R34/MNDWI/MNDWI_%s.img"%"_".join(raster.split('_'))[0:25])

    # Perform base MNDWI test
    con_MNDWI1 = Con(MNDWI > WIGT, 1, 0)
    # MNDWI1.save("G:/CA_P4342_R34/Algorithm/Results/Condition/Con_MNDWI/MNDWI1_%s.img"%"_".join(raster.split('_'))[0:25])

    # Step 1 of MBSR: Visible bands
    MBSV = Plus(raster, B3)

    # Step 2 of MBSR: NIR bands
    MBSRN = Plus(B4, B5)

    # Perform MBSR test
    con_MBSR = Con(MBSV > MBSRN, 10, 0)

    # Calculate AWEsh values
    # ERDAS version of equation is:
    # $n1_lndsr(1)  +  2.5 *  $n1_lndsr(2)  -  1.5  *  ($n1_lndsr(4)  +  $n1_lndsr(5))  -  0.25  *  $n1_lndsr(7)
    # Runs in arcpy as:
    AWE_temp1 = Times(raster, AWE_param1)
    AWE_temp2 = Plus(B4, B5)
    AWE_temp3 = Times(AWE_temp2, AWE_param2)
    AWE_temp4 = Times(B7, AWE_param3)
    AWE_temp7 = B1 + AWE_temp1 + AWE_temp3 + AWE_temp4
    con_AWEsh = Con(AWE_temp7 > 0, 100, 0)
    # con_AWEsh.save("G:/CA_P4342_R34/Algorithm/Results/Condition/Con_AWEsh/con_AWEsh_%s.img"%"_".join(raster.split('_'))[0:25])

    # Filter the MNDWI raster with a condition for PSW1
    con_PSW1_MNDWI = Con((MNDWI > PSW1_MNDWI), 1, 0)
    # con_PSW1_MNDWI.save("G:/CA_P4342_R34/Algorithm/Results/Condition/Con_MNDWI1/con_PSW1_MNDWI_%s.img"%"_".join(raster.split('_'))[0:25])

    # Create a PSW1 conditional raster from Band_4
    con_PSW1b4 = Con(Raster(B4) < PSW1b4, 1, 0)
    # con_PSW1b4.save("G:/CA_P4342_R34/Algorithm/Results/Condition/Con_B4/con_PSW1b4_%s.img"%"_".join(raster.split('_'))[0:25])

    # Create a PSW1 conditional raster from Band_5
    con_PSW1b5 = Con(Raster(B5) < PSW1b5, 1, 0)
    # con_PSW1b5.save("G:/CA_P4342_R34/Algorithm/Results/Condition/Con_B5/con_PSW1b5_%s.img"%"_".join(raster.split('_'))[0:25])

    # Perform PSW1 test
    PSW1a = Times(con_MNDWI1, con_PSW1b4)
    PSW1b = Times(PSW1a, con_PSW1b5)
    PSW1 = Times(PSW1b, 1000)
    # PSW1.save("G:/CA_P4342_R34/Algorithm/Results/PSW1/PSW1_%s.img"%"_".join(raster.split('_'))[0:25])

    # Filter the MNDWI raster with a condition for PSW2
    con_PSW2_MNDWI = Con((MNDWI > PSW2_MNDWI), 1, 0)
    # con_PSW2_MNDWI.save("G:/CA_P4342_R34/Algorithm/Results/Condition/Con_MNDWI1/con_PSW2_MNDWI_%s.img"%"_".join(raster.split('_'))[0:25])

    # Create a PSW2 conditional raster from Band_4
    con_PSW2b4 = Con(Raster(B4) < PSW2b4, 1, 0)
    # con_PSW2b4.save("G:/CA_P4342_R34/Algorithm/Results/Condition/Con_B4/con_PSW2b4_%s.img"%"_".join(raster.split('_'))[0:25])

    # Create a PSW2 conditional raster from Band_7
    con_PSW2b7 = Con(Raster(B7) < PSW2b7, 1, 0)
    # con_PSW2b7.save("G:/CA_P4342_R34/Algorithm/Results/Condition/Con_B7/con_PSW2b7_%s.img"%"_".join(raster.split('_'))[0:25])

    # Perform PSW2 test
    PSW2a = Times(con_PSW2_MNDWI, con_PSW2b4)
    PSW2b = Times(PSW2a, con_PSW2b7)
    PSW2 = Times(PSW2b, 10000)

    # Create diagnostic composite output map
    # finmapa = Plus(con_MNDWI1,con_MBSR)
    # finmapb = Plus(finmapa, con_AWEsh)
    # finmapc = Plus(finmapb, PSW1)
    # finmapd = Plus(finmapc, PSW2)
    finmapd = con_MNDWI1 + con_MBSR + con_AWEsh + PSW1 + PSW2
    finmape = Con(Raster(CF) == 2, 11999, finmapd)
    finmapf = Con(Raster(CF) == 4, 11999, finmape)
    # finmapf.save(output_dir +  "\\" + out_name + "cs" + "_%s.img"%"_".join(raster.split('_'))[0:25])

    # create ESPA provisinal uncorrected/full output
    espamap = ReclassByASCIIFile(finmapd, espa_recode)
    espamap.save(output_dir + "\\" + out_name + "espa" + "_%s.img" % "_".join(raster.split('_'))[0:25])
    # create ESPA provisional cloud/shadow "corrected" output
    espamapcs = ReclassByASCIIFile(finmapf, espa_recode)
    espamapcs.save(output_dir + "\\" + out_name + "espa_cs" + "_%s.img" % "_".join(raster.split('_'))[0:25])
    # create ESPA provisional cloud/shadow/slope "correted" output
    espamapcsp = Con(Raster(perslp) >= PS, 0, espamapcs)
    espamapcsp.save(output_dir + "\\" + out_name + "espa_csp" + "_%s.img" % "_".join(raster.split('_'))[0:25])
