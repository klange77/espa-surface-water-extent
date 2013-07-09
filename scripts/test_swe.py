#! /usr/bin/env python

import sys
from do_water_extent import Swe

ERROR = 1
SUCCESS = 0

if __name__ == "__main__":
    status = Swe().runSwe(metafile="/data1/sguo/LT50450302001272LGS01_MTL.txt", reflectance="/data1/sguo/lndsr.LT50450302001272LGS01.hdf", usebin=True)
    if status != SUCCESS:
        print "Error running surface water extent"
        sys.exit(ERROR)

    print "Success running surface water extent"
    sys.exit(SUCCESS)

