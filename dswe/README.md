## Dynamic Surface Water Extent Version 2.2 - Release Notes

Release Date: November 2017

See git tag [dswe-v2.2]

This application produces Surface Water Extent products for Landsat 4, 5, 7, and 8.

## Product Descriptions
See the [Dynamic Surface Water Extent Product Guide](https://remotesensing.usgs.gov/ecv/document/provisional_dswe_README_v10.pdf) for more information about the DSWE product.

## Release Notes
* Updated version numbers
* Update percent slope and hillshade thresholds 
* Changed "raw" band filename to "intrpd"
* Renamed "not-validate-prototype-dswe" directory to "dswe"

## Installation

### Dependencies
* ESPA raw binary libraries, tools, and its dependencies, found here [espa-product-formatter](https://github.com/USGS-EROS/espa-product-formatter)

### Environment Variables
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

### Build Steps
* Clone the repository and replace the defaulted version(master) with this
  version of the software
```
git clone https://github.com/USGS-EROS/espa-surface-water-extent.git
cd espa-surface-water-extent
git checkout dswe-version_<version>
```
* Build and install the application specific software
```
make all-dswe
make install-dswe
```

## Usage
See `surface_water_extent.py --help` for command line details.<br>
See `surface_water_extent.py --xml <xml_file> --help` for command line details specific to the Landsat 4, 5, 7, and 8 application.  When the XML file specified is for an Landsat 4, 5, 7, or 8 scene.<br>
See `dswe --help` for command line details when the above wrapper script is not called.

### Environment Variables
* PATH - May need to be updated to include the following
  - `$PREFIX/bin`

### Data Processing Requirements
This version of the DSWE application requires the input products to be in the ESPA internal file format.

The following input data are required to generate the surface water extent products:
* Surface Reflectance
* Product Formatter 
* Pixel QA 
* Elevation

The Surface Reflectance, Product Formatter, and Pixel QA products can be generated using the software found in our [espa-surface-reflectance](https://github.com/USGS-EROS/espa-surface-reflectance), [espa-product-formatter](https://github.com/USGS-EROS/espa-product-formatter), and [espa-cloud-masking](https://github.com/USGS-EROS/espa-cloud-masking) projects.  They can also be generated through our ondemand processing system [ESPA](https://espa.cr.usgs.gov).  If using that system, be sure to select the ENVI output format.

The Elevation data is required to be in the same projection and physical data file size as the Pixel QA and Surface Reflectance products.

This surface water extent product is <b>NOT</b> currently available in the [ESPA](https://espa.cr.usgs.gov) processing system.

### Data Postprocessing
After compiling the [espa-product-formatter](https://github.com/USGS-EROS/espa-product-formatter) libraries and tools, the `convert_espa_to_gtif` and `convert_espa_to_hdf` command-line tools can be used to convert the ESPA internal file format to HDF or GeoTIFF.  Otherwise the data will remain in the ESPA internal file format, which includes each band in the ENVI file format (i.e. raw binary file with associated ENVI header file) and an overall XML metadata file.

## More Information
This project is provided by the US Geological Survey (USGS) Earth Resources
Observation and Science (EROS) Land Satellite Data Systems (LSDS) Science
Research and Development (LSRD) Project. For questions regarding products
produced by this source code, please contact the Landsat Contact page and
specify USGS CDR/ECV in the "Subject" section.
https://landsat.usgs.gov/contact
