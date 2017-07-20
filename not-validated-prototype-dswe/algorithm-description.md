
This is the algorithm description derived from the prototype implementation
provided by the authors and subsequent conversations and emails.


==============================================================================
Algorithm Authors:

    John W. Jones
    Research Geographer
    Eastern Geographic Science Center
    U.S. Geological Survey
    email: jwjones@usgs.gov

    Michael J. Starbuck
    Physical Scientist
    Earth Resources Observation and Science Center
    U.S. Geological Survey
    email: mstarbuck@usgs.gov


==============================================================================
Algorithm Description - Overview:

    The algorithm relies on a series of relatively simple and efficient water
    detection tests, each with their own output code for a "positive" test
    result or 0 for a negative test result.  The result is a 5 digit output
    value in the range 00000 to 11111, where each digit corresponds to a
    specific test.

    These test results are then further refined (recoded) to the following
    values:

          0 -> Not Water
          1 -> Water - High Confidence
          2 -> Water - Moderate Confidence
          3 -> Potential Wetland 
          4 -> Low Confidence Water or Wetland 
          9 -> Cloud, Cloud Shadow, or Snow
        255 -> Fill (no data)

    The algorithm provides an output of 3 bands.  

    The first band represents the Raw DSWE (recoded values 0, 1, 2, 3, 4, 9, 
    and 255).

    The second band represents the Raw DSWE with filtering applied for 
    Percent Slope, Hillshade, Cloud, Cloud Shadow, and Snow (recoded values 0, 
    1, 2, 3, 9, and 255).

    Percent-Slope is utilized to remove locations where the terrain is too
    sloped to hold water.  Any values that meet this criteria are recoded to
    a value of 0.  Similarly, any locations where the terrain is too shaded
    is recoded to a value of 0.

    The third band is a mask band with the following mask values:

    0 - No mask applied
    2 - Cloud shadow mask applied
    3 - Snow mask applied
    4 - Cloud mask applied
    10 - Percent slope mask applied
    20 - Hillshade mask applied

    Multiple mask reasons can apply for each pixel in the mask band.  Mask
    values are added.  For example, a pixel masked for both snow and hillshade 
    will have a value of 23. 

==============================================================================
Algorithm Description - Inputs:

    The primary source of the input is Surface Reflectance derived from L1T
    products.  Specifically the Blue, Green, Red, NIR, SWIR1, and SWIR2
    Surface Reflectance bands, along with the pixel QA mask band, are used 
    as input.

    A DEM is also used to generate internal Percent-Slope and Hillshade bands
    for the required slope and hillshade filtering.

==============================================================================
Algorithm Description - Detailed:


    NOTE: Keep in mind during the processing of the Raw DSWE band, the output
          is filtered for fill data and those values are set to 255.


    Raw DSWE -> Output:

         1) Calculate Modified Normalized Difference Wetness Index (MNDWI)
            from the Green and SWIR1 bands.

            mndwi = (Green - SWIR1) / (Green + SWIR1)

         2) Calculate Multi-band Spectral Relationship Visible (MBSRV) from
            the Green and Red bands.

            mbsrv = Green + Red

         3) Calculate Multi-band Spectral Relationship Near-Infrared (MBSRN)
            from the NIR and SWIR1 bands.

            mbsrn = NIR + SWIR1

         4) Calculate Normalized Difference Vegetation Index (NDVI)
            from the NIR and Red bands.

            ndvi = (NIR - Red) / (NIR + Red)

         5) Calculate Automated Water Extent Shadow (AWEsh) from the Blue,
            Green, and SWIR2 bands, along with MBSRN.

            awesh = (Blue
                     + (2.5 * Green)
                     + (-1.5 * mbsrn)
                     + (-0.25 * SWIR2))

         6) Perform the first test by comparing the MNDWI to a Wetness Index
            threshold, where the threshold ranges from 0.0 to 2.0 and is
            defaulted to a value of 0.0124.

            if (mndwi > 0.0124) set the ones digit  (Example 00001)

         7) Perform the second test by comparing the MBSRV and MBSRN values to
            each other.

            if (mbsrv > mbsrn) set the tens digit  (Example 00010)

         8) Perform the third test by comparing AWEsh to an Automated Water
            Extent Shadow threshold, where the threshold ranges from -2.0 to
            2.0 and is defaulted to a value of 0.0.

            if (awesh > 0.0) set the hundreds digit  (Example 00100)

         9) Perform the fourth test by comparing the MNDWI and NDVI along with 
            the NIR and SWIR1 bands to the following thresholds. 

            - Partial Surface Water Test-1 threshold, where the threshold 
              ranges from -2.0 to 2.0 and is defaulted to a value of -0.44.
            - Partial Surface Water Test-1 SWIR1 threshold, where the threshold
              ranges from 0 to data maximum and is defaulted to a value of 900.
            - Partial Surface Water Test-1 NIR threshold, where the threshold 
              ranges from 0 to data maximum and is defaulted to a value of 1500.
            - Partial Surface Water Test-1 NDVI threshold, where the threshold 
              ranges from 0 to 2.0 and is defaulted to a value of 0.7.

            if (mndwi > -0.44
                && SWIR1 < 9000
                && NIR < 1500
                && NDVI < 0.7) set the thousands digit  (Example 01000)

         10) Perform the fifth test by comparing the MNDWI along with the Blue,
            NIR, SWIR1, and SWIR2 bands to the following thresholds.

            - Partial Surface Water Test-2 threshold, where the threshold 
              ranges from -2.0 to 2.0 and is defaulted to a value of -0.5.
            - Partial Surface Water Test-2 Blue threshold, where the threshold 
              ranges from 0 to data maximum and is defaulted to a value of 1000.
            - Partial Surface Water Test-2 NIR threshold, where the threshold 
              ranges from 0 to data maximum and is defaulted to a value of 2500.
            - Partial Surface Water Test-2 SWIR1 threshold, where the threshold
              ranges from 0 to data maximum and is defaulted to a value of 3000.
            - Partial Surface Water Test-2 SWIR2 threshold, where the threshold
              ranges from 0 to data maximum and is defaulted to a value of 1000.
            
            if (mndwi > -0.5
                && Blue < 1000
                && SWIR1 < 3000
                && SWIR2 < 1000
                && NIR < 2500) set the ten-thousands digit  (Example 10000)

        11) Recode the results from the previous steps using the following
            ranges and values.

            00000 : 0 (Not Water)
            00001 : 0
            00010 : 0
            00100 : 0
            01000 : 0

            01111 : 1 (Water - High Confidence)
            10111 : 1
            11011 : 1
            11101 : 1
            11110 : 1
            11111 : 1

            00111 : 2 (Water - Moderate Confidence)
            01011 : 2
            01101 : 2
            01110 : 2
            10011 : 2
            10101 : 2
            10110 : 2
            11001 : 2
            11010 : 2
            11100 : 2

            11000 : 3 (Potential Wetland)

            00011 : 4 (Low Confidence Water or Wetland)
            00101 : 4
            00110 : 4
            01001 : 4
            01010 : 4
            01100 : 4
            10000 : 4
            10001 : 4
            10010 : 4
            10100 : 4

        12) Output the Raw DSWE


    Raw DSWE -> Percent-Slope -> Hillshade -> Cloud, Cloud Shadow, Snow 
             -> Output:

         1) Build a Percent-Slope band from the DEM source.

         2) Build a Hillshade band from the DEM source.

         3) Define Percent-Slope thresholds (range 0.0 - 100.0):

            percent_slope_high (default 30.0)
            percent_slope_moderate (default 30.0)
            percent_slope_wetland (default 20.0)
            percent_slope_low (default 10.0)

         4) Perform the following tests by comparing the Percent-Slope band to 
            the Percent-Slope thresholds:

            if (percent-slope >= percent_slope_high) and the Raw DSWE is High
                Confidence Water (1), set the filtered Raw DSWE to a recoded
                value of 0, otherwise set it to Raw DSWE
            if (percent-slope >= percent_slope_moderate) and the Raw DSWE is 
                Moderate Confidence Water (2), set the filtered Raw DSWE to a 
                recoded value of 0, otherwise set it to Raw DSWE
            if (percent-slope >= percent_slope_wetland) and the Raw DSWE is 
                Potential Wetland (3), set the filtered Raw DSWE to a recoded
                value of 0, otherwise set it to Raw DSWE
            if (percent-slope >= percent_slope_low) and the Raw DSWE is Low 
                Confidence Water or Wetland (4), set the filtered Raw DSWE to 
                a recoded value of 0, otherwise set it to Raw DSWE

         5) Define Hillshade threshold (range 0 - 255) with default 110.

         6) If the filtered Raw DSWE was not recoded to a value of 0, perform 
            the following test by comparing the Hillshade band to the hillshade
            threshold: 

            if (hillshade <= hillshade threshold), set the filtered Raw DSWE to
                a recoded value of 0, otherwise set it to Raw DSWE

         7) Perform a test by comparing the Pixel QA (quality/mask) band to the
           Cloud, Snow, and Cloud Shadow values.

            if (the Pixel QA cloud, cloud shadow, and/or snow bit is set)
                set the filtered Raw DSWE to a recoded value of 9, otherwise 
                leave it alone

         8) Output the Percent-Slope, Hillshade, Cloud, Cloud Shadow, and Snow
            filtered Raw DSWE.


    Mask Output:

         1) Initialize Mask Output to 0

         2) Add the contribution of the pixel QA mask values as follows:

            if (pixel QA cloud shadow bit is set)
                mask_output = mask_output + 2 
            if (pixel QA snow bit is set)
                mask_output = mask_output + 3 
            if (pixel QA cloud bit is set)
                mask_output = mask_output + 4 

         3) Add the contribution of the percent slope mask values as follows:

            if (any percent slope test caused the Raw DSWE to be recoded to 0)
                mask_output = mask_output + 10 
                
         4) Add the contribution of the hillshade mask values as follows:

            if (the hillshade test caused the Raw DSWE to be recoded to 0)
                mask_output = mask_output + 20 

         5) Output the Mask 

