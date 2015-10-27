#------------------------------------------------------------------------------
# Makefile
#
# Simple makefile for building and installing land-surface-temperature
# applications.
#------------------------------------------------------------------------------
.PHONY: check-environment all install clean all-script install-script clean-script all-dswe install-dswe clean-dswe

include make.config

MAKEFILE_NAME = Makefile

DIR_DSWE = not-validated-prototype-dswe

all: all-script all-dswe

install: check-environment install-script install-dswe

clean: clean-script clean-dswe

#------------------------------------------------------------------------------
all-script:
	echo "make all in scripts"; \
        (cd scripts; $(MAKE) all -f $(MAKEFILE_NAME));

install-script: check-environment
	echo "make install in scripts"; \
        (cd scripts; $(MAKE) install -f $(MAKEFILE_NAME));

clean-script:
	echo "make clean in scripts"; \
        (cd scripts; $(MAKE) clean -f $(MAKEFILE_NAME));

#------------------------------------------------------------------------------
all-dswe: all-script
	echo "make all in not-validated-prototype-dswe"; \
        (cd $(DIR_DSWE); $(MAKE) all -f $(MAKEFILE_NAME));

install-dswe: check-environment install-script
	echo "make install in not-validated-prototype-dswe"; \
        (cd $(DIR_DSWE); $(MAKE) install -f $(MAKEFILE_NAME));

clean-dswe: clean-script
	echo "make clean in not-validated-prototype-dswe"; \
        (cd $(DIR_DSWE); $(MAKE) clean -f $(MAKEFILE_NAME));

check-environment:
ifndef PREFIX
    $(error Environment variable PREFIX is not defined)
endif

