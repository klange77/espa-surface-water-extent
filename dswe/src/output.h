
#ifndef OUTPUT_H
#define OUTPUT_H


#include <stdbool.h>
#include <stdint.h>

#include "espa_metadata.h"

#include "const.h"


int
add_dswe_band_product
(
    char *xml_filename,
    bool use_toa_flag,
    char *product_name,
    char *band_name,
    char *short_name,
    char *long_name,
    int min_range,
    int max_range,
    int add_class,
    int add_bitmap,
    uint8_t *data
);


int
add_test_band_product
(
    char *xml_filename,
    bool use_toa_flag,
    char *product_name,
    char *band_name,
    char *short_name,
    char *long_name,
    int min_range,
    int max_range,
    int16_t *data
);


int
add_ps_band_product
(
    char *xml_filename,
    bool use_toa_flag,
    char *product_name,
    char *band_name,
    char *short_name,
    char *long_name,
    int min_range,
    int max_range,
    int16_t *data
);


#endif /* OUTPUT_H */
