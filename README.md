## Surface Water Extent Version 1.0.3 Release Notes
Release Date: May 13, 2015

This project contains application source code for producing Surface Water Extent products.  It currently only supports Landsat 4-7, as prototype output products that are under evaluation.  Algorithms for specific sensors, where warranted, are located in sensor specific sub-directories.  See the sensor specific sub-directories for more details and usage examples.

This project is hosted by the US Geological Survey (USGS) Earth Resources Observation and Science (EROS) Land Satellite Data Systems (LSDS) Science Research and Development (LSRD) Project. For questions regarding this source code, please contact the Landsat Contact Us page and specify USGS CDR/ECV in the "Regarding" section. https://landsat.usgs.gov/contactus.php 


### Downloads
Surface Water Extent source code
```
    git clone https://github.com/USGS-EROS/espa-surface-water-extent.git
```

For current proto-type version, see git tag [dswe-version_1.0.3]

### Installation
  * Install dependent libraries: ESPA product formatter (https://github.com/USGS-EROS/espa-product-formatter)

  * Set up environment variables: Can create an environment shell file or add the following to your bash shell.  For C shell, use 'setenv VAR "directory"'.
```
    export PREFIX="path_to_directory_for_build_data"
```

  * Download: (from GitHub USGS-EROS/espa-surface-water-extent project)

  * Build/Install: The following build will create an executable file under $PREFIX/bin: dswe (tested in Linux with the gcc compiler). It will also copy the Python scripts for running Surface Water Extent into the $PREFIX/bin directory.
```
make
make install
make clean
```

### Dependencies
  * ESPA raw binary and ESPA common libraries from ESPA product formatter and associated dependencies
  * XML2 library

### Data Preprocessing
This version of the spectral indices application requires the input Landsat products to be in the ESPA internal file format.

### Data Postprocessing
After compiling the ESPA product formatter raw\_binary libraries and tools, the convert\_espa\_to\_gtif and convert\_espa\_to\_hdf command-line tools can be used to convert the ESPA internal file format to HDF or GeoTIFF.  Otherwise the data will remain in the ESPA internal file format, which includes each band in the ENVI file format (i.e. raw binary file with associated ENVI header file) and an overall XML metadata file.

### Associated Scripts

### Verification Data

### User Manual

### Product Guide

## Changes From Previous Version
#### Created on May 13th, 2015 - USGS EROS
  * Initial proto-type release of the software for analysis
