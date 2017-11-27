#-----------------------------------------------------------------------------
# Makefile
#
# Simple makefile for building and installing land-surface-temperature
# applications.
#-----------------------------------------------------------------------------
.PHONY: check-environment all install clean all-script install-script clean-script all-dswe install-dswe clean-dswe all-cfbwd install-cfbwd clean-cfbwd

include make.config

DIR_DSWE = dswe
DIR_CFWD = cfmask-based-water-detection

#-----------------------------------------------------------------------------
all: all-script all-dswe all-cfbwd

install: check-environment install-script install-dswe install-cfbwd

clean: clean-script clean-dswe clean-cfbwd

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
all-dswe:
	echo "make all in dswe"; \
        (cd $(DIR_DSWE); $(MAKE) all);

install-dswe: check-environment
	echo "make install in dswe"; \
        (cd $(DIR_DSWE); $(MAKE) install);

clean-dswe:
	echo "make clean in dswe"; \
        (cd $(DIR_DSWE); $(MAKE) clean);

#-----------------------------------------------------------------------------
all-cfbwd:
	echo "make all in cfmask-based-water-detection"; \
        (cd $(DIR_CFWD); $(MAKE) all);

install-cfbwd: check-environment
	echo "make install in cfmask-based-water-detection"; \
        (cd $(DIR_CFWD); $(MAKE) install);

clean-cfbwd:
	echo "make clean in cfmask-based-water-detection"; \
        (cd $(DIR_CFWD); $(MAKE) clean);

#-----------------------------------------------------------------------------
check-environment:
ifndef PREFIX
    $(error Environment variable PREFIX is not defined)
endif

