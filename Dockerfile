# =======================================================
#   Binary Build Layer
# =======================================================
FROM usgseros/espa-surface-reflectance:docker-devel-3.0rc1.dev1 as builder
LABEL maintainer="USGS EROS LSRD http://eros.usgs.gov" \
      description="ESPA executables for generating surface water extent products"
USER root

WORKDIR ${SRC_DIR}
COPY . ${SRC_DIR}

# `  Install the product-formatter applications
RUN cd ${SRC_DIR}/scripts \
    && make BUILD_STATIC=yes ENABLE_THREADING=yes \
    && make install \
    && cd ${SRC_DIR}/dswe \
    && make BUILD_STATIC=yes ENABLE_THREADING=yes \
    && make install \
    && cd ${SRC_DIR}/cfmask-based-water-detection \
    && make BUILD_STATIC=yes ENABLE_THREADING=yes \
    && make install \
    && cd ${SRC_DIR} \
    && rm -rf *
USER espadev

