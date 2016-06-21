#-----------------------------------------------------------------------------
# Makefile
#
# Simple makefile for building and installing land-surface-temperature
# applications.
#-----------------------------------------------------------------------------
.PHONY: check-environment all install clean all-script install-script clean-script all-dswe install-dswe clean-dswe all-cfwd install-cfwd clean-cfwd rpms dswe-rpm

include make.config

DIR_DSWE = not-validated-prototype-dswe
DIR_CFWD = cfmask-based-water-detection

#-----------------------------------------------------------------------------
all: all-script all-dswe all-cfwd

install: check-environment install-script install-dswe install-cfwd

clean: clean-script clean-dswe clean-cfwd

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
	echo "make all in not-validated-prototype-dswe"; \
        (cd $(DIR_DSWE); $(MAKE) all);

install-dswe: check-environment
	echo "make install in not-validated-prototype-dswe"; \
        (cd $(DIR_DSWE); $(MAKE) install);

clean-dswe:
	echo "make clean in not-validated-prototype-dswe"; \
        (cd $(DIR_DSWE); $(MAKE) clean);

#-----------------------------------------------------------------------------
all-cfwd:
	echo "make all in cfmask-based-water-detection"; \
        (cd $(DIR_CFWD); $(MAKE) all);

install-cfwd: check-environment
	echo "make install in cfmask-based-water-detection"; \
        (cd $(DIR_CFWD); $(MAKE) install);

clean-cfwd:
	echo "make clean in cfmask-based-water-detection"; \
        (cd $(DIR_CFWD); $(MAKE) clean);

#-----------------------------------------------------------------------------
rpms: dswe-rpm

dswe-rpm:
	rpmbuild -bb --clean RPM_spec_files/RPM-DSWE.spec

#-----------------------------------------------------------------------------
check-environment:
ifndef PREFIX
    $(error Environment variable PREFIX is not defined)
endif

