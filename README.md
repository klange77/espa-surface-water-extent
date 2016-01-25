## ESPA Surface Water Extent - Release Notes
Release Date: March XX, 2016

See git tag [2016_Mar]

This project contains application source code for producing Surface Water Extent products.

## Implemented Algorithms

### DSWE - Dynamic Surface Water Extent (Algorithm)
* Implemented in C
* Only supports Landsat 4-7, as prototype products that are under evaluation
* This software is based on an algorithm developed by John W. Jones, and Michael J. Starbuck
* See folder <b>not-validated-prototype-dswe</b> for the Landsat 4, 5, and 7 version.

### CFmask Based Water Detection (Algorithm)
* Implemented in C
* Supports Landsat 4-7, as well as Landsat 8.
* Implemented to support Level 2 Quality Band consistency with previous CFmask
  products.
* This software is based on the Matlab code developed by Zhe Zhu, and
  Curtis E. Woodcock
  * Zhu, Z. and Woodcock, C. E., Object-based cloud and cloud shadow detection in Landsat imagery, Remote Sensing of Environment (2012), doi:10.1016/j.rse.2011.10.028 
  * More information from the algorithm developers can be found [here](https://github.com/prs021/fmask).


## Release Notes
Please see the Wiki pages or Release Tags for notes related to past versions.

- Added a CFmask based water detection algorithm.
- Updated DSWE ENVI headers and XML

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

## More Information
This project is provided by the US Geological Survey (USGS) Earth Resources
Observation and Science (EROS) Land Satellite Data Systems (LSDS) Science
Research and Development (LSRD) Project. For questions regarding products
produced by this source code, please contact the Landsat Contact Us page and
specify USGS CDR/ECV in the "Regarding" section.
https://landsat.usgs.gov/contactus.php 
