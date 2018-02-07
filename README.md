## ESPA Surface Water Extent - Release Notes
Release Date: February 2018

See git tag [espa-v2.27.0]

This project contains application source code for producing Surface Water Extent products.

## Implemented Algorithms

### DSWE - Dynamic Surface Water Extent (Algorithm)
* Implemented in C
* Supports Landsat 4-8, as prototype products that are under evaluation
* This software is based on an algorithm developed by John W. Jones, and Michael J. Starbuck
* See [DSWE](dswe/README.md) in [dswe](dswe) for more information

### CFmask Based Water Detection (Algorithm)
* Implemented in C
* Supports Landsat 4-8
* Implemented to support Level 2 Quality Band consistency with previous CFmask products.
* This software is based on the Matlab code developed by Zhe Zhu, and Curtis E. Woodcock
  * Zhu, Z. and Woodcock, C. E., Object-based cloud and cloud shadow detection in Landsat imagery, Remote Sensing of Environment (2012), doi:10.1016/j.rse.2011.10.028 
  * More information from the algorithm developers can be found [here](https://github.com/prs021/fmask)
* Also See [CFBWD](cfmask-based-water-detection/README.md) in [cfmask-based-water-detection](cfmask-based-water-detection) for more information


## Release Notes
* Updated version numbers
* Write percent slope band as an integer and scale it
* Remove pre-collections software 

## Installation Notes

### Installation of Specific Algorithms
Please see the installation instructions within the algorithm sub-directory.

### Installation of All Algorithms

#### Dependencies
* ESPA raw binary libraries, tools, and it's dependencies, found here [espa-product-formatter](https://github.com/USGS-EROS/espa-product-formatter)

#### Environment Variables
* Required for building this software
```
export PREFIX="path_to_Installation_Directory"
export XML2INC="path_to_LIBXML2_include_files"
export XML2LIB="path_to_LIBXML2_libraries_for_linking"
export LZMALIB="path_to_LZMA_libraries_for_linking"
export ZLIBLIB="path_to_ZLIB_libraries_for_linking"
export ESPAINC="path_to_ESPA_PRODUCT_FORMATTER_include_files"
export ESPALIB="path_to_ESPA_PRODUCT_FORMATTER_libraries_for_linking"
```

#### Build Steps
* Clone the repository and replace the defaulted version(master) with this
  version of the software
```
git clone https://github.com/USGS-EROS/espa-surface-water-extent.git
cd espa-surface-water-extent
git checkout <version>
```
* Build and install the application specific software
```
make
make install
```

## Usage
See the algorithm specific sub-directories for details on usage.

### Support Information
This project is unsupported software provided by the U.S. Geological Survey (USGS) Earth Resources Observation and Science (EROS) Land Satellite Data Systems (LSDS) Project. For questions regarding products produced by this source code, please contact us at custserv@usgs.gov.

### Disclaimer
This software is preliminary or provisional and is subject to revision. It is being provided to meet the need for timely best science. The software has not received final approval by the U.S. Geological Survey (USGS). No warranty, expressed or implied, is made by the USGS or the U.S. Government as to the functionality of the software and related material nor shall the fact of release constitute any such warranty. The software is provided on the condition that neither the USGS nor the U.S. Government shall be held liable for any damages resulting from the authorized or unauthorized use of the software.
