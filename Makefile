#-----------------------------------------------------------------------------
# Makefile
#
# Simple makefile for building and installing land-surface-temperature
# applications.
#-----------------------------------------------------------------------------
.PHONY: check-environment all install clean all-script install-script clean-script all-dswe install-dswe clean-dswe

include make.config

DIR_DSWE = not-validated-prototype-dswe

all: all-script all-dswe

install: check-environment install-script install-dswe

clean: clean-script clean-dswe

#-----------------------------------------------------------------------------
all-script:
	echo "make all in scripts"; \
        (cd scripts; $(MAKE) all);

install-script: check-environment
	echo "make install in scripts"; \
        (cd scripts; $(MAKE) install);

clean-script:
	echo "make clean in scripts"; \
        (cd scripts; $(MAKE) clean);

#-----------------------------------------------------------------------------
all-dswe: all-script
	echo "make all in not-validated-prototype-dswe"; \
        (cd $(DIR_DSWE); $(MAKE) all);

install-dswe: check-environment install-script
	echo "make install in not-validated-prototype-dswe"; \
        (cd $(DIR_DSWE); $(MAKE) install);

clean-dswe: clean-script
	echo "make clean in not-validated-prototype-dswe"; \
        (cd $(DIR_DSWE); $(MAKE) clean);

check-environment:
ifndef PREFIX
    $(error Environment variable PREFIX is not defined)
endif

