#-----------------------------------------------------------------------------
# Makefile
#
# For building dynamic-surface-water-extent.
#-----------------------------------------------------------------------------
.PHONY: all install clean

# Inherit from upper-level make.config
TOP = ../..
include $(TOP)/make.config

#-----------------------------------------------------------------------------
# Set up compile options
CC = gcc
RM = rm -f
EXTRA = -Wall $(EXTRA_OPTIONS)

# Define the include files
INC = build_slope_band.h const.h dswe.h get_args.h input.h output.h utilities.h

# Define the source code and object files
SRC = \
      utilities.c         \
      get_args.c          \
      input.c             \
      output.c            \
      build_slope_band.c  \
      dswe.c
OBJ = $(SRC:.c=.o)

# Define include paths
INCDIR  = -I. -I$(ESPAINC) -I$(XML2INC)
NCFLAGS = $(EXTRA) $(INCDIR)

# Define the object libraries and paths
EXLIB = -L$(ESPALIB) -l_espa_raw_binary -l_espa_common \
        -L$(XML2LIB) -lxml2 \
        -L$(LZMALIB) -llzma \
        -L$(ZLIBLIB) -lz
MATHLIB = -lm
LOADLIB = $(EXLIB) $(MATHLIB)

# Define the executable
EXE = dswe

#-----------------------------------------------------------------------------
all: $(EXE)

$(EXE): $(OBJ) $(INC)
	$(CC) $(EXTRA) -o $(EXE) $(OBJ) $(LOADLIB)

#-----------------------------------------------------------------------------
install:
	install -d $(link_path)
	install -d $(dswe_bin_install_path)
	install -m 755 $(EXE) $(dswe_bin_install_path)
	ln -sf $(dswe_link_source_path)/$(EXE) $(link_path)/$(EXE)

#-----------------------------------------------------------------------------
clean:
	$(RM) *.o $(EXE)

#-----------------------------------------------------------------------------
$(OBJ): $(INC)

.c.o:
	$(CC) $(NCFLAGS) -c $<

